/**
 *  @note This file is part of MABE, https://github.com/mercere99/MABE2
 *  @copyright Copyright (C) Michigan State University, MIT Software license;
 * see doc/LICENSE.md
 *  @date 2019-2022.
 *
 *  @file  SymbiontOrg.hpp
 *  @brief An endosymbiont organism that can live in a host.
 *  @note Status: ALPHA
 */

#ifndef MABE_SYMBIONT_ORGANISM_H
#define MABE_SYMBIONT_ORGANISM_H

#include "../core/MABE.hpp"
#include "../core/Organism.hpp"
#include "../core/OrganismManager.hpp"

namespace mabe {

const int POTATOES = 3;

class SymbiontOrg : public OrganismTemplate<SymbiontOrg> {
protected:
  bool has_host = false;

public:
  SymbiontOrg(OrganismManager<SymbiontOrg> &_manager)
      : OrganismTemplate<SymbiontOrg>(_manager) {}
  SymbiontOrg(const SymbiontOrg &) = default;
  SymbiontOrg(SymbiontOrg &&) = default;
  ~SymbiontOrg() { ; }

  struct ManagerData : public Organism::ManagerData {
  };

  std::string ToString() const override {
    return "Symbiont";
  }

  size_t Mutate(emp::Random &) override {
    return 0;
  }

  bool TrySetHost() {
    bool r = !has_host;
    has_host = true;
    return r;
  }
};

MABE_REGISTER_ORG_TYPE(
    SymbiontOrg,
    "Organism that is an endosymbiont living in a host.");
} // namespace mabe

#endif
