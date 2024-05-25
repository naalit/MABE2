/**
 *  @note This file is part of MABE, https://github.com/mercere99/MABE2
 *  @copyright Copyright (C) Michigan State University, MIT Software license;
 * see doc/LICENSE.md
 *  @date 2019-2022.
 *
 *  @file  SymManager.hpp
 *  @brief Handles vertical and horizontal transmission for symbionts.
 *  @note  Status: ALPHA
 */

#ifndef MABE_SYM_MANAGER_H
#define MABE_SYM_MANAGER_H

#include "../core/MABE.hpp"
#include "../core/Module.hpp"
#include "HostOrg.hpp"

namespace mabe {

class SymManager : public Module {
private:
  RequiredTrait<double> points_trait{this, "points",
                                     "Trait representing organism points."};
  double sym_vert_trans_points = 0.0;
  double sym_vert_trans_prob = 0.7;

public:
  SymManager(mabe::MABE &control, const std::string &name = "SymManager",
             const std::string &desc = "Handles vertical and horizontal transmission for symbionts.",
             double sym_vert_trans_points = 0.0)
      : Module(control, name, desc),
        sym_vert_trans_points(sym_vert_trans_points) {}
  ~SymManager() {}

  static void InitType(emplode::TypeInfo &info) {}

  void OnOffspringReady(Organism &org, OrgPosition parent,
                        Population &) override {
    // Do vertical transmission if the organisms involved are hosts
    if (auto host_new = dynamic_cast<HostOrg *>(&org))
      if (auto host_old = dynamic_cast<HostOrg *>(&*parent)) {
        for (OrgPosition pos : host_old->GetSymbionts()) {
          if (points_trait.Get(*pos) >= sym_vert_trans_points &&
              control.GetRandom().P(sym_vert_trans_prob)) {
            points_trait.Get(*pos) -= sym_vert_trans_points;
            dynamic_cast<SymbiontOrg *>(&*pos)->SetVerticalTransmission(true);
            Collection new_symbionts =
                control.Replicate(pos, pos->GetPopulation());
            for (auto sym = new_symbionts.begin(); sym != new_symbionts.end();
                 sym++) {
              if (sym.IsOccupied() && host_new->AddSymbiont(sym))
                dynamic_cast<SymbiontOrg *>(&*sym)->TrySetHost(host_new);
            }
            dynamic_cast<SymbiontOrg *>(&*pos)->SetVerticalTransmission(false);
          }
        }
      }
  }

  void BeforePlacement(Organism &org, OrgPosition org_pos,
                       OrgPosition parent) override {
    // Horizontal transmission
    if (auto sym_new = dynamic_cast<SymbiontOrg *>(&org)) {
      if (!sym_new->IsFromVerticalTransmission() && parent.IsOccupied()) {
        if (auto sym_old = dynamic_cast<SymbiontOrg *>(&*parent)) {
          // We're doing horizontal transmission, so the symbiont needs to find
          // a host or it will die
          auto phost = sym_old->GetHost();
          if (!phost.has_value()) {
            std::cerr << "no host" << std::endl;
          } else {
            auto phost_pos = (*phost)->GetPopulation().PositionOf(**phost);
            auto new_host_pos = phost_pos->Pop().FindNeighbor(*phost_pos);
            if (new_host_pos.IsOccupied()) {
              if (auto new_host = dynamic_cast<HostOrg *>(&*new_host_pos))
                if (new_host->AddSymbiont(org_pos)) {
                  sym_new->TrySetHost(new_host);
                }
            }
          }
        }
      }
    }
  }

  void OnPlacement(OrgPosition pos) override {
    // Kill symbionts that can't find hosts, but only after the initial
    // placement where hosts pick symbionts
    if (control.GetUpdate() > 0) {
      if (auto sym = dynamic_cast<SymbiontOrg *>(&*pos)) {
        if (!sym->IsFromVerticalTransmission() && !sym->GetHost().has_value()) {
          control.ClearOrgAt(pos);
        }
      }
    }
  }

  void SetupConfig() override {
    LinkVar(sym_vert_trans_points, "sym_vert_trans_points",
            "Number of points required for symbionts (if present) to "
            "vertically transmit.");
    LinkVar(sym_vert_trans_prob, "sym_vert_trans_prob",
            "Probability that symbionts will vertically transmit.");
  }
};

MABE_REGISTER_MODULE(SymManager, "Handles vertical and horizontal transmission for symbionts.");
} // namespace mabe

#endif
