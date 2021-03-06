#include "NEQ.hh"
#include "Euler3DNEQRhoivtTv.hh"
#include "Common/NotImplementedException.hh"
#include "Environment/ObjectProvider.hh"
#include "Framework/PhysicalChemicalLibrary.hh"

//////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace COOLFluiD::Framework;
using namespace COOLFluiD::Common;
using namespace COOLFluiD::Physics::NavierStokes;

//////////////////////////////////////////////////////////////////////////////

namespace COOLFluiD {

  namespace Physics {

    namespace NEQ {

//////////////////////////////////////////////////////////////////////////////

Environment::ObjectProvider<Euler3DNEQRhoivtTv, ConvectiveVarSet, NEQModule, 1>
euler3DNEQRhoivtTvProvider("Euler3DNEQRhoivtTv");

//////////////////////////////////////////////////////////////////////////////
      
Euler3DNEQRhoivtTv::Euler3DNEQRhoivtTv(Common::SafePtr<BaseTerm> term) :
  Euler3DNEQRhoivt(term),
  _tvDim(),
  _moleculesIDs()
{
  const CFuint nbSpecies = getModel()->getNbScalarVars(0);
  const CFuint nbTv = getModel()->getNbScalarVars(1);
  _tvDim.resize(nbTv);
  
  vector<std::string> names(nbSpecies +4 + nbTv);
  for (CFuint ie = 0; ie < nbSpecies; ++ie) {
    names[ie] = "rho" + StringOps::to_str(ie);
  }
  names[nbSpecies]     = "u";
  names[nbSpecies + 1] = "v";
  names[nbSpecies + 2] = "w";
  names[nbSpecies + 3] = "T";
  
  const CFuint startTv = nbSpecies + 4; 
  for (CFuint ie = 0; ie < nbTv; ++ie) {
    names[startTv + ie] = "Tv" + StringOps::to_str(ie);
  }
  
  setVarNames(names);
}

//////////////////////////////////////////////////////////////////////////////

Euler3DNEQRhoivtTv::~Euler3DNEQRhoivtTv()
{
}

//////////////////////////////////////////////////////////////////////////////

void Euler3DNEQRhoivtTv::computeEigenValuesVectors(RealMatrix& rightTv,
					       RealMatrix& leftTv,
					       RealVector& eValues,
					       const RealVector& normal)
{
  throw Common::NotImplementedException (FromHere(),"Euler3DNEQRhoivtTv::computeEigenValuesVectors()");
}

//////////////////////////////////////////////////////////////////////////////

CFuint Euler3DNEQRhoivtTv::getBlockSeparator() const
{
  return Framework::PhysicalModelStack::getActive()->getNbEq();
}

//////////////////////////////////////////////////////////////////////////////

void Euler3DNEQRhoivtTv::computeJacobians()
{
  throw Common::NotImplementedException (FromHere(),"Euler3DNEQRhoivtTv::computeJacobians()");
}


//////////////////////////////////////////////////////////////////////////////

void Euler3DNEQRhoivtTv::splitJacobian(RealMatrix& jacobPlus,
				    RealMatrix& jacobMin,
				    RealVector& eValues,
				    const RealVector& normal)
{
  throw Common::NotImplementedException (FromHere(),"Euler3DNEQRhoivtTv::splitJacobian()");
}

//////////////////////////////////////////////////////////////////////////////

void Euler3DNEQRhoivtTv::setThermodynamics(CFreal rho, 
					   const State& state, 
					   RealVector& data)
{ 
  const CFuint nbSpecies = getModel()->getNbScalarVars(0);
  const RealVector& refData = getModel()->getReferencePhysicalData();
  CFreal rhodim = rho*refData[EulerTerm::RHO];
  CFreal T = state[getTempID(nbSpecies)];
  CFreal Tdim = T*refData[EulerTerm::T];
  
  const CFuint nbTv = getModel()->getNbScalarVars(1);
  const CFuint firstTv = getModel()->getFirstScalarVar(1);
  const CFuint startTv = nbSpecies + 4;
  
  for (CFuint ie = 0; ie < nbTv; ++ie) {
    _tvDim[ie] = state[startTv + ie]*refData[EulerTerm::T];
  }
  
  CFreal pdim = _library->pressure(rhodim, Tdim, &_tvDim[0]);
  CFreal p = pdim/refData[EulerTerm::P];
  
  // unused //  const EquationSubSysDescriptor& eqSS = PhysicalModelStack::getActive()->getEquationSubSysDescriptor();
  // unused //  const CFuint iEqSS = eqSS.getEqSS();
  // unused //  const CFuint nbEqSS = eqSS.getTotalNbEqSS();
  
  data[EulerTerm::P] = p;
  data[EulerTerm::T] = T;
  data[EulerTerm::RHO] = rho;
  
  if (!_skipEnergyData) {
    _library->setDensityEnthalpyEnergy(Tdim, _tvDim, pdim,_dhe,_extraData);

    const CFuint nbTe = _library->getNbTe();
    const CFuint nbTvH = nbTv - nbTe;

    // data stores the moleculare vibrational energy multiplied 
    // by the molecules mass fractions
    if (nbTvH != 0) {
       for (CFuint ie = 0; ie < nbTvH; ++ie) {
           data[firstTv + ie] = _dhe[3 + ie]/refData[EulerTerm::H]; 
       } 
    }
 
    if (nbTe == 1) {
      data[firstTv + nbTvH] = _dhe[3 + nbTvH]/refData[EulerTerm::H];
    }
    
    _library->frozenGammaAndSoundSpeed(Tdim, pdim, rhodim,
				       data[EulerTerm::GAMMA],
				       data[EulerTerm::A], &_tvDim);
    
    const CFreal V2 = data[EulerTerm::V]*data[EulerTerm::V];
    data[EulerTerm::H] = _dhe[1]/refData[EulerTerm::H] + 0.5*V2;
    data[EulerTerm::E] = _dhe[2]/refData[EulerTerm::H] + 0.5*V2;
  }
}

//////////////////////////////////////////////////////////////////////////////

void Euler3DNEQRhoivtTv::computeStateFromPhysicalData(const RealVector& data,
						 State& state)
{
  throw Common::NotImplementedException (FromHere(),"Euler3DNEQRhoivtTv::computeStateFromPhysicalData()");
}

//////////////////////////////////////////////////////////////////////////////

void Euler3DNEQRhoivtTv::setDimensionalValues(const State& state,
					      RealVector& result)
{
  // first call the parent
  Euler3DNEQRhoivt::setDimensionalValues(state,result);
  
  const RealVector& refData = getModel()->getReferencePhysicalData();
  const CFuint nbSpecies = getModel()->getNbScalarVars(0);
  const CFuint nbTv = getModel()->getNbScalarVars(1);
  const CFuint startTv = nbSpecies + 4;
  for (CFuint ie = 0; ie < nbTv; ++ie) {
    result[startTv + ie] = state[startTv + ie]*refData[EulerTerm::T];
  }
}
      
//////////////////////////////////////////////////////////////////////////////

void Euler3DNEQRhoivtTv::setAdimensionalValues(const Framework::State& state,
					       RealVector& result)
{
  // first call the parent
  Euler3DNEQRhoivt::setDimensionalValues(state,result);
  
  const RealVector& refData = getModel()->getReferencePhysicalData();
  const CFuint nbSpecies = getModel()->getNbScalarVars(0);
  const CFuint nbTv = getModel()->getNbScalarVars(1);
  const CFuint startTv = nbSpecies + 4;
  for (CFuint ie = 0; ie < nbTv; ++ie) {
    result[startTv + ie] = state[startTv + ie]/refData[EulerTerm::T];
  }
}

//////////////////////////////////////////////////////////////////////////////

void Euler3DNEQRhoivtTv::setDimensionalValuesPlusExtraValues
(const State& state, RealVector& result,
 RealVector& extra)
{
  Euler3DNEQRhoivtTv::setDimensionalValues(state,result);
  
  const RealVector& refData = getModel()->getReferencePhysicalData();
  const CFuint nbSpecies = getModel()->getNbScalarVars(0);
  
  // Set the mixture density (sum of the partial densities)
  CFreal rho = 0.0;
  for (CFuint ie = 0; ie < nbSpecies; ++ie) {
    rho += state[ie]; // state[ie] is partial density
    _ye[ie] = state[ie];
  } 
  _ye /= rho;
    
  // set the current species fractions in the thermodynamic library
  // this has to be done before computing any other thermodynamic quantity !!! 
  _library->setSpeciesFractions(_ye);
  
  const CFreal u = result[nbSpecies];
  const CFreal v = result[nbSpecies+1];
  const CFreal w = result[nbSpecies+2];
  const CFreal V2 = u*u + v*v + w*w;
  CFreal rhodim = rho*refData[EulerTerm::RHO];
  CFreal Tdim = state[nbSpecies + 3]*refData[EulerTerm::T];
  
  // set the vibrational temperature
  const CFuint nbTv = getModel()->getNbScalarVars(1);
  const CFuint startTv = nbSpecies + 4;
  for (CFuint ie = 0; ie < nbTv; ++ie) {
    result[startTv + ie] = state[startTv + ie]*refData[EulerTerm::T];
    _tvDim[ie] = result[startTv + ie];
  }
  
  CFreal pdim = _library->pressure(rhodim, Tdim, &_tvDim[0]);
  
  _library->setDensityEnthalpyEnergy(Tdim,_tvDim,pdim, _dhe);
  
  CFreal gamma = 0.0;
  CFreal a = 0.0;
  _library->frozenGammaAndSoundSpeed(Tdim,pdim,rhodim, gamma, a, &_tvDim);
  
  extra.resize(4);
  extra[0] = rho*refData[EulerTerm::RHO];
  extra[1] = _dhe[1] + 0.5*V2;
  extra[2] = sqrt(V2)/a;
  extra[3] = pdim;
}

//////////////////////////////////////////////////////////////////////////////

void Euler3DNEQRhoivtTv::setup()
{
  Euler3DNEQRhoivt::setup();
  // set the IDs for the molecules
  _library->setMoleculesIDs(_moleculesIDs);
}

//////////////////////////////////////////////////////////////////////////////

void Euler3DNEQRhoivtTv::computePressureDerivatives(const Framework::State& state, 
						    RealVector& dp)
{
  const CFuint nbSpecies = getModel()->getNbScalarVars(0);
  const CFuint TID = nbSpecies + 3;
  const CFuint start = (_library->presenceElectron()) ? 1 : 0;
  
  dp[TID] = 0.0;
  for (CFuint  i = start; i < nbSpecies; ++i) {
    dp[i] = _Rspecies[i]*state[TID];
    dp[TID] += _Rspecies[i]*state[i];
  }
  
  
  if (_library->presenceElectron()) {
    const CFuint TeID = (_library->getNbTe() == 0) ? 
      TID + 1 + _library->getElectrEnergyID() : state.size() - 1;
    dp[0] = _Rspecies[0]*state[TeID];
    dp[TeID] = _Rspecies[0]*state[0];
  }
}

//////////////////////////////////////////////////////////////////////////////

    } // namespace NEQ

  } // namespace Physics

} // namespace COOLFluiD

//////////////////////////////////////////////////////////////////////////////
