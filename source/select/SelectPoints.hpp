/**
 *  @note This file is part of MABE, https://github.com/mercere99/MABE2
 *  @copyright Copyright (C) Michigan State University, MIT Software license;
 * see doc/LICENSE.md
 *  @date 2019-2022.
 *
 *  @file  SelectPoints.hpp
 *  @brief A selector that has organisms pay a certain number of points to
 * reproduce.
 *  @note  Status: ALPHA
 */

#ifndef MABE_SELECT_POINTS_H
#define MABE_SELECT_POINTS_H

#include "../core/MABE.hpp"
#include "../core/Module.hpp"
#include "../orgs/HostOrg.hpp"

namespace mabe {

class SelectPoints : public Module {
private:
  RequiredTrait<double> points_trait{this, "points",
                                     "Trait representing organism points."};
  double points_threshold;
  double sym_vert_trans_points;

  Collection Select(Population &select_pop, Population &birth_pop) {
    Collection placement_list;
    for (auto it = select_pop.begin(); it != select_pop.end(); it++) {
      if (it.IsOccupied()) {
        double points = points_trait.Get(*it);
        if (points >= points_threshold) {
          points_trait.Get(*it) -= points_threshold;
          Collection orgs = control.Replicate(it.AsPosition(), birth_pop);

          // Do vertical transmission if the organisms involved are hosts
          // (this is here because symbionts also require points to vertically transmit)
          for (Organism &org : orgs) {
            if (auto host_new = dynamic_cast<HostOrg *>(&org))
              if (auto host_old = dynamic_cast<HostOrg *>(&*it)) {
                for (OrgPosition pos : host_old->GetSymbionts()) {
                  if (points_trait.Get(*pos) >= sym_vert_trans_points) {
                    points_trait.Get(*pos) -= sym_vert_trans_points;
                    Collection new_symbionts =
                        control.Replicate(pos, pos->GetPopulation());
                    for (auto sym = new_symbionts.begin();
                         sym != new_symbionts.end(); sym++) {
                      host_new->AddSymbiont(sym);
                    }
                  }
                }
              }
          }

          placement_list += orgs;
        }
      }
    }
    return placement_list;
  }

public:
  SelectPoints(mabe::MABE &control, const std::string &name = "SelectPoints",
               const std::string &desc =
                   "Has organisms pay a certain number of points to reproduce.",
               double points_threshold = 100,
               double sym_vert_trans_points = 100)
      : Module(control, name, desc), points_threshold(points_threshold),
        sym_vert_trans_points(sym_vert_trans_points) {
    SetSelectMod(true);
  }
  ~SelectPoints() {}

  // Setup member functions associated with this class.
  static void InitType(emplode::TypeInfo &info) {
    info.AddMemberFunction(
        "SELECT",
        [](SelectPoints &mod, Population &from, Population &to) {
          return mod.Select(from, to);
        },
        "Perform points-based selection on the provided organisms.");
  }

  void SetupConfig() override {
    LinkVar(points_threshold, "points_threshold",
            "Number of points required to reproduce.");
    LinkVar(sym_vert_trans_points, "sym_vert_trans_points",
            "Number of points required for symbionts (if present) to "
            "vertically transmit.");
  }
};

MABE_REGISTER_MODULE(
    SelectPoints, "Has organisms pay a certain number of points to reproduce.");
} // namespace mabe

#endif
