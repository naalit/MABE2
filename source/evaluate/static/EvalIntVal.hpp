/**
 *  @note This file is part of MABE, https://github.com/mercere99/MABE2
 *  @copyright Copyright (C) Michigan State University, MIT Software license;
 * see doc/LICENSE.md
 *  @date 2019-2021.
 *
 *  @file  EvalIntVal.hpp
 *  @brief Evaluator for symbiotic organisms with interaction values.
 */

#ifndef MABE_EVAL_INT_VAL_H
#define MABE_EVAL_INT_VAL_H

#include "../../core/MABE.hpp"
#include "../../core/Module.hpp"
#include "../../orgs/HostOrg.hpp"

namespace mabe {

class EvalIntVal : public Module {
private:
  RequiredTrait<double> int_val_trait{this, "int_val",
                                      "Trait for interaction value."};
  SharedTrait<double> points_trait{this, "points", "Trait to use to give an organism resources."};
  double synergy = 5.0;
  double points_per_update = 100.0;

public:
  EvalIntVal(mabe::MABE &control, const std::string &name = "EvalIntVal",
             const std::string &desc =
                 "Evaluator for symbiotic organisms with interaction values.")
      : Module(control, name, desc) {
    SetEvaluateMod(true);
  }
  ~EvalIntVal() {}

  // Setup member functions associated with this class.
  static void InitType(emplode::TypeInfo &info) {
    info.AddMemberFunction(
        "EVAL",
        [](EvalIntVal &mod, Collection list) {
          mod.Evaluate(list);
          return 0;
        },
        "Allocate resources based on interaction values.");
  }

  void SetupConfig() override {
    LinkVar(synergy, "synergy",
            "Amount symbiont's returned resources should be multiplied by.");
    LinkVar(points_per_update, "points_per_update",
            "Amount of points to distribute to each host each update.");
  }

  void AddPoints(Organism &org, double points) {
    points_trait.Get(org) += points;
  }

  void Evaluate(Collection orgs) {
    emp_assert(control.GetNumPopulations() >= 1);

    // Loop through the population and evaluate each organism.
    mabe::Collection alive_collect(orgs.GetAlive());
    for (Organism &host : alive_collect) {

      // Receive resources from the world and distribute them to the symbionts
      double resources = points_per_update;
      double host_int_val = int_val_trait.Get(host);
      HostOrg *host_org = dynamic_cast<HostOrg *>(&host);
      if (host_org && !host_org->GetSymbionts().empty()) {
        auto &symbionts = host_org->GetSymbionts();
        double per_sym = resources / symbionts.size();
        for (auto sym_pos : symbionts) {
          // First the host decides how many resources to donate to the sym, or
          // it spends resources on defense
          double host_resources = per_sym;
          double donation = 0;
          if (host_int_val < 0) {
            // Spend resources on defense
            host_resources -= -host_int_val * per_sym;
          } else {
            // Donate to the symbiont
            donation = host_int_val * per_sym;
            host_resources -= donation;
          }

          // Then the symbiont decides whether to steal from the host
          Organism &sym = *sym_pos;

          double sym_int_val = int_val_trait.Get(sym);
          double sym_resources = 0;
          if (sym_int_val < 0) {
            // Attempt to steal from the host
            // If host_int_val >= sym_int_val, the host successfully defends itself
            if (sym_int_val < host_int_val) {
              double stolen =
                  (std::min(host_int_val, 0.0) - sym_int_val) * host_resources;
              host_resources -= stolen;
              sym_resources = donation + stolen;
            }
          } else {
            // Split the donation between sym and host according to sym int value
            host_resources += donation * sym_int_val * synergy;
            sym_resources = donation * (1 - sym_int_val);
          }

          AddPoints(sym, sym_resources);
          AddPoints(host, host_resources);
        }
      } else {
        double spent = resources * std::abs(host_int_val);
        AddPoints(host, resources - spent);
      }
    }
  }
};

MABE_REGISTER_MODULE(
    EvalIntVal, "Evaluator for symbiotic organisms with interaction values.");
} // namespace mabe

#endif
