/**
 *  @note This file is part of MABE, https://github.com/mercere99/MABE2
 *  @copyright Copyright (C) Michigan State University, MIT Software license; see doc/LICENSE.md
 *  @date 2019-2020.
 *
 *  @file  MABE.cc
 *  @brief Dynamic MABE executable that can be fully configured from a file.
 */

#include <iostream>

// Empirical tools
#include "config/ArgManager.h"
#include "tools/BitVector.h"
#include "tools/Random.h"

// Framework
#include "../source/core/MABE.h"
#include "../source/core/EmptyOrganism.h"

// Include the full set of available modules.
#include "../source/modules.h"

int main(int argc, char* argv[])
{
  // Build the MABE controller, passing in the command-line arguments.
  mabe::MABE control(argc, argv);
  control.SetupEmpty<mabe::EmptyOrganismManager>();

  // [ If you build your own version of this file, add required modules here... ]

  // Setup all of the modules; read in the config file.
  if (control.Setup() == false) return 0;

  // Start the run! 
  control.DoRun(1000);
}