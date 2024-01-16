/**
 *  @note This file is part of MABE, https://github.com/mercere99/MABE2
 *  @copyright Copyright (C) Michigan State University, MIT Software license;
 * see doc/LICENSE.md
 *  @date 2019-2022.
 *
 *  @file  HostOrg.hpp
 *  @brief An organism capable of hosting symbionts.
 *  @note  Status: ALPHA
 */

#ifndef MABE_HOST_ORGANISM_H
#define MABE_HOST_ORGANISM_H

#include "../core/EmptyOrganism.hpp"
#include "../core/MABE.hpp"
#include "../core/Organism.hpp"
#include "../core/OrganismManager.hpp"

#include "SymbiontOrg.hpp"

namespace mabe {

class HostOrg : public OrganismTemplate<HostOrg> {
protected:
  emp::vector<OrgPosition> symbionts;

public:
  HostOrg(OrganismManager<HostOrg> &_manager)
      : OrganismTemplate<HostOrg>(_manager) {}
  HostOrg(const HostOrg &x) : OrganismTemplate<HostOrg>(x){};
  HostOrg(HostOrg &&x) : OrganismTemplate<HostOrg>(x){};
  ~HostOrg() {
    // Kill the symbionts when the host dies
    // The symbiont population should use FreeListPlacement to reuse these slots
    for (OrgPosition sym : symbionts) {
      GetManager().GetControl().ClearOrgAt(sym);
    }
  }

  struct ManagerData : public Organism::ManagerData {
    std::string sym_pop;
  };

  std::string ToString() const override {
    return emp::to_string("Host with ", symbionts.size(), " symbionts");
  }

  size_t Mutate(emp::Random &) override { return 0; }

  void AddSymbiont(OrgPosition sym) { symbionts.push_back(sym); }

  emp::vector<OrgPosition> &GetSymbionts() { return symbionts; }

  void Initialize(emp::Random &random) override {
    Randomize(random);
    // Attempt to find and capture a host-less symbiont
    Collection sym_col =
        GetManager().GetControl().ToCollection(SharedData().sym_pop);
    for (auto it = sym_col.begin(); it != sym_col.end(); it++) {
      if (!it->IsEmpty()) {
        if (auto sym = dynamic_cast<SymbiontOrg *>(&*it)) {
          if (sym->TrySetHost()) {
            symbionts.push_back(it.AsPosition());
            break;
          }
        }
      }
    }
  }

  /// Setup this organism type to be able to load from config.
  void SetupConfig() override {
    GetManager().LinkVar(SharedData().sym_pop, "sym_pop",
                         "The Collection that this hosts' symbionts live in.");
  }
};

MABE_REGISTER_ORG_TYPE(HostOrg, "Organism capable of hosting symbionts.");
} // namespace mabe

#endif
