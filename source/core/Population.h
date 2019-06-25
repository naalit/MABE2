/**
 *  @note This file is part of MABE, https://github.com/mercere99/MABE2
 *  @copyright Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  @date 2019
 *
 *  @file  Population.h
 *  @brief Container for a group of arbitrary MABE organisms.
 *
 *  Organisms in MABE are stored in Population objects.
 *  A single position in a Population object is described by a Population::Position.
 */

#ifndef MABE_POPULATION_H
#define MABE_POPULATION_H

#include <string>

#include "base/Ptr.h"
#include "base/vector.h"

#include "Organism.h"

namespace mabe {

  class Population {
  private:
    std::string name;                      ///< Unique name for this population.
    int id;                                ///< Position in world of this population.
    emp::vector<emp::Ptr<Organism>> orgs;  ///< Info on all organisms in this population.

  public:
    class Iterator {
    private:
      emp::Ptr<Population> pop_ptr;
      size_t pos;
      bool skip_empty;

    public:
      Iterator(emp::Ptr<Population> _pop, size_t _pos=0) : pop_ptr(_pop), pos(_pos) { }
      Iterator(const Iterator & _in) : pop_ptr(_in.pop_ptr), pos(_in.pos) { }
      Iterator & operator=(const Iterator & _in) = default;

      // Shortcuts to retrieve information from the population.
      const std::string & PopName() const { return pop_ptr->name; }
      int PopID() const { return pop_ptr->id; }
      size_t PopSize() const { emp_assert(pop_ptr); return pop_ptr->orgs.size(); }      
      emp::Ptr<Organism> OrgPtr() { emp_assert(pop_ptr); return pop_ptr->orgs[pos]; }
      emp::Ptr<const Organism> OrgPtr() const { emp_assert(pop_ptr); return pop_ptr->orgs[pos]; }

      /// IS the pointed-to cell occupied?
      bool IsOccupied() const { return !OrgPtr().IsNull(); }

      /// If on empty cell, advance iterator to next non-null position (or the end)
      void ToOccupied() { while (pos < PopSize() && OrgPtr().IsNull()) ++pos; }

      /// Advance iterator to the next non-empty cell in the world.
      Iterator & operator++() {
        ++pos;
        ToOccupied();
        return *this;
      }

      /// Backup iterator to the previos non-empty cell in the world.
      Iterator & operator--() {
        --pos;
        while (pos < PopSize() && OrgPtr().IsNull()) --pos;
        return *this;
      }

      /// Iterator comparisons (iterators from different populations have no ordinal relationship).
      bool operator==(const Iterator& in) const { return pop_ptr == in.pop_ptr && pos == in.pos; }
      bool operator!=(const Iterator& in) const { return pop_ptr != in.pop_ptr || pos != in.pos; }
      bool operator< (const Iterator& in) const { return pop_ptr == in.pop_ptr && pos <  in.pos; }
      bool operator<=(const Iterator& in) const { return pop_ptr == in.pop_ptr && pos <= in.pos; }
      bool operator> (const Iterator& in) const { return pop_ptr == in.pop_ptr && pos >  in.pos; }
      bool operator>=(const Iterator& in) const { return pop_ptr == in.pop_ptr && pos >= in.pos; }

      /// Return a reference to the organism pointed to by this iterator; may advance iterator.
      Organism & operator*() { ToOccupied(); return *(OrgPtr()); }

      /// Return a const reference to the organism pointed to by this iterator.
      /// Note that since this version is const, it will NOT advance the iterator.
      const Organism & operator*() const { emp_assert(IsOccupied()); return *(OrgPtr()); }

      /// Is this iterator pointing to a valid cell in the world?
      operator bool() const { IsOccupied(); return pos < PopSize(); }

      /// Return an iterator pointing to the first occupied cell in the world.
      Iterator begin() { return Iterator(pop_ptr, 0); }

      /// Return a const iterator pointing to the first occupied cell in the world.
      const Iterator begin() const { return Iterator(pop_ptr, 0); }

      /// Return an iterator pointing to just past the end of the world.
      Iterator end() { return Iterator(pop_ptr, PopSize()); }

      /// Return a const iterator pointing to just past the end of the world.
      const Iterator end() const { return Iterator(pop_ptr, PopSize()); }
    };

    Population(const std::string & in_name, int in_id=0) : name(in_name), id(in_id) { }
    Population(const Population & in_pop) : name(in_pop.name + "_copy"), orgs(in_pop.orgs.size()) {
      for (size_t i = 0; i < orgs.size(); i++) {
        orgs[i] = in_pop.orgs[i]->Clone();
      }
    }
    Population(Population &&) = default;

    ~Population() { for (auto x : orgs) x.Delete(); }

    const std::string & GetName() const noexcept { return name; }
    int GetID() const noexcept { return id; }

    void SetID(int in_id) noexcept { id = in_id; }
  };

}

#endif
