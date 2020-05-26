/**
 *  @note This file is part of MABE, https://github.com/mercere99/MABE2
 *  @copyright Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  @date 2020.
 *
 *  @file  MovePopulation.h
 *  @brief Module to move organisms from one population to another (optionally clearing the destination)
 */

#ifndef MABE_SCHEMA_MOVE_POPULATION_H
#define MABE_SCHEMA_MOVE_POPULATION_H

#include "../core/Module.h"

namespace mabe {

  /// Add elite selection with the current population.
  class MovePopulation : public Module {
  private:
    int from_id = 1;       ///< Which population are we moving from?
    int to_id = 0;         ///< Which population are we moving to?
    bool reset_to = true;  ///< Should we reset_to the 'to' population before moving in?

  public:
    MovePopulation(mabe::MABE & control,
           const std::string & name="MovePopulation",
           const std::string & desc="Module to move organisms to a new population",
           int _from_id=0, int _to_id=1, bool _reset_to=true)
      : Module(control, name, desc), from_id(_from_id), to_id(_to_id), reset_to(_reset_to)
    {
      SetManageMod(true);         ///< Mark this module as a population  module.
    }

    void SetupConfig() override {
      LinkPop(from_id, "from_pop", "Population to move organisms from.");
      LinkPop(to_id, "to_pop", "Population to move organisms into.");
      LinkVar(reset_to, "reset_to", "Should we erase organisms at the destination?");
    }

    void OnUpdate(size_t update) override {
      Population & from_pop = control.GetPopulation(from_id);
      Population & to_pop = control.GetPopulation(to_id);

      // Setup an iterator to point to the "to" population.
      OrgPosition it_to;

      // Clear out the "to" population and before moving the new populaiton in.
      if (reset_to) {
        control.EmptyPop(to_pop, from_pop.GetSize());  // Clear out the population.
        it_to = to_pop.begin();                        // Start at the beginning of population.
      }

      // Otherwise append the from population to the end of the to population.
      else {
        size_t old_pop_size = to_pop.GetSize();
        control.ResizePop(to_pop, old_pop_size + from_pop.GetSize());
        it_to = to_pop.begin() + old_pop_size;
      }

      // Move the next generation to the main population.
      for (OrgPosition it_from = from_pop.begin(); it_from != from_pop.end(); ++it_from, ++it_to) {
        if (it_from.IsOccupied()) control.MoveOrg(it_from, it_to);
      }

      // Clear out the next generation
      control.EmptyPop(from_pop, 0);
    }
  };

  MABE_REGISTER_MODULE(MovePopulation, "Move organisms from one populaiton to another.");
}

#endif
