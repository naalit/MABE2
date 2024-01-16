/**
 *  @note This file is part of MABE, https://github.com/mercere99/MABE2
 *  @copyright Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  @date 2021-2022.
 *
 *  @file  FreeListPlacement.h
 *  @brief Attempt to replace any empty cells in the population, adding to the end if none are available.
 */

#ifndef MABE_FREE_LIST_PLACEMENT_H
#define MABE_FREE_LIST_PLACEMENT_H

#include "../core/MABE.hpp"
#include "../core/Module.hpp"

namespace mabe {

  class FreeListPlacement : public Module {
  private:
    Collection target_collect; ///< Collection of populations to manage

  public:
    FreeListPlacement(mabe::MABE & control,
                    const std::string & name="FreeListPlacement",
                    const std::string & desc="Grow population only after replacing all free cells")
      : Module(control, name, desc), target_collect(control.GetPopulation(0))
    {
      SetPlacementMod(true);
    }
    ~FreeListPlacement() { }

    /// Set up variables for configuration file
    void SetupConfig() override {
      LinkCollection(target_collect, "target", "Population(s) to manage.");
    }

    /// Set birth and inject functions for the specified populations
    void SetupModule() override {
      for(size_t pop_id = 0; pop_id < control.GetNumPopulations(); ++pop_id){
        Population& pop = control.GetPopulation(pop_id);
        if(target_collect.HasPopulation(pop)){
          pop.SetPlaceBirthFun( 
            [this, &pop](Organism & /*org*/, OrgPosition ppos) {
              return PlaceInject(pop);
            }
          );
          pop.SetPlaceInjectFun( 
            [this, &pop](Organism & /*org*/){
              return PlaceInject(pop);
            }
          );
        }
      }
    }

    /// Manually inject an organism. Also used for PlaceBirth, since the parent position is irrelevant.
    OrgPosition PlaceInject(Population & target_pop) {
      if (target_collect.HasPopulation(target_pop)) { // If population is monitored...
        // Find an empty slot
        for (auto it = target_pop.begin(); it != target_pop.end(); it++) {
            if (it->IsEmpty()) {
                return it.AsPosition();
            }
        }
        // If no empty slots, add new position
        return control.PushEmpty(target_pop);
      }
      // Otherwise, don't find a legal place!
      return OrgPosition();      
    }

  };

  MABE_REGISTER_MODULE(FreeListPlacement, "Grow population only after replacing all free cells");
}

#endif
