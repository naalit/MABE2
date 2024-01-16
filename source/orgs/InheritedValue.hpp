/**
 *  @note This file is part of MABE, https://github.com/mercere99/MABE2
 *  @copyright Copyright (C) Michigan State University, MIT Software license;
 * see doc/LICENSE.md
 *  @date 2019-2022.
 *
 *  @file  InheritedValue.hpp
 *  @brief Manages an inherited `double`-valued trait for all organisms.
 *  @note  Status: ALPHA
 */

#ifndef MABE_INHERITED_VALUE_GENOME_H
#define MABE_INHERITED_VALUE_GENOME_H

#include "../core/MABE.hpp"
#include "../core/Module.hpp"

namespace mabe {

class InheritedValue : public Module {
private:
  GeneratedTrait<double> value_trait{this, "value",
                                       "Trait for the inherited value."};
  double mut_rate;
  double mut_size;
  double min_value;
  double max_value;

public:
  InheritedValue(mabe::MABE &control, const std::string &name = "InheritedValue",
               const std::string &desc =
                   "Manages an inherited `double`-valued trait for all organisms.",
               double min_value = -1.0, double max_value = 1.0,
               double mut_rate = 1.0, double mut_size = 0.002)
      : Module(control, name, desc), min_value(min_value), max_value(max_value), mut_rate(mut_rate), mut_size(mut_size) {}
  ~InheritedValue() {}

  // Setup member functions associated with this class.
  static void InitType(emplode::TypeInfo &info) {}

  // Randomize inherited value on inject
  void OnInjectReady(Organism &org, Population &) override {
    value_trait.Get(org) = control.GetRandom().GetDouble(min_value, max_value);
  }

  // Mutate inherited value when an organism is mutated
  // TODO why didn't OnMutate() work for this
  void OnOffspringReady(Organism &org, OrgPosition, Population &) override {
    emp::Random &random = control.GetRandom();
    if (random.GetDouble(0.0, 1.0) <= mut_rate) {
      double val = value_trait.Get(org);
      val += random.GetNormal(0.0, mut_size);
      val = std::clamp(val, min_value, max_value);
      value_trait.Get(org) = val;
    }
  }

  void SetupConfig() override {
    LinkVar(mut_rate, "mut_rate",
            "Probability of mutating inherited value on reproduction.");
    LinkVar(mut_size, "mut_size",
            "Standard deviation of the size of inherited value mutations.");
    LinkVar(min_value, "min_value",
            "Minimum value for the inherited value.");
    LinkVar(max_value, "max_value",
            "Maximum value for the inherited value.");
  }
};

MABE_REGISTER_MODULE(InheritedValue,
                     "Manages an inherited `double`-valued trait for all organisms.");
} // namespace mabe

#endif
