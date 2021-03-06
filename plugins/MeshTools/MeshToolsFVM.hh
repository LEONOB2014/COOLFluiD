// Copyright (C) 2012 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef COOLFluiD_MeshToolsFVM_hh
#define COOLFluiD_MeshToolsFVM_hh

//////////////////////////////////////////////////////////////////////////////

#include "Environment/ModuleRegister.hh"

//////////////////////////////////////////////////////////////////////////////

namespace COOLFluiD {

  namespace MeshTools {

//////////////////////////////////////////////////////////////////////////////

/**
 * This class defines the Module MeshToolsFVM
 */
class MeshToolsFVMModule : public Environment::ModuleRegister<MeshToolsFVMModule> {
public:

  /**
   * Static function that returns the module name.
   * Must be implemented for the ModuleRegister template
   * @return name of the module
   */
  static std::string getModuleName()
  {
    return "MeshToolsFVM";
  }

  /**
   * Static function that returns the description of the module.
   * Must be implemented for the ModuleRegister template
   * @return descripton of the module
   */
  static std::string getModuleDescription()
  {
    return "This module implements computation of aerodynamic coefficients with bindings to FVM methods.";
  }

}; // end MeshToolsFVMModule

//////////////////////////////////////////////////////////////////////////////

  } // namespace MeshTools

} // namespace COOLFluiD

//////////////////////////////////////////////////////////////////////////////

#endif // COOLFLUID_MeshToolsFVM_hh

