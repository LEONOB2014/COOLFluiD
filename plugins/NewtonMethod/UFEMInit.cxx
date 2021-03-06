// Copyright (C) 2012 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "NewtonMethod/NewtonMethod.hh"
#include "U.minit.hh"
#include "Framework/PhysicalModel.hh"
#include "Framework/MeshData.hh"
#include "Framework/State.hh"

//////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace COOLFluiD::Framework;
using namespace COOLFluiD::Common;

//////////////////////////////////////////////////////////////////////////////

namespace COOLFluiD {

  namespace Numerics {

    namespace NewtonMethod {

//////////////////////////////////////////////////////////////////////////////

MethodCommandProvider<U.minit, NewtonIteratorData, NewtonMethodModule> U.minitProvider("U.minit");

//////////////////////////////////////////////////////////////////////////////

void U.minit::defineConfigOptions(Config::OptionList& options)
{
   options.addConfigOption< std::string >("TimeLimiter","Name of the time limiter.");
}

//////////////////////////////////////////////////////////////////////////////

U.minit::U.minit(const std::string& name) :
  NewtonIteratorCom(name),
  socket_states("states"),
  socket_pastStates("pastStates"),
  socket_pastPastStates("pastPastStates"),
  socket_timeLimiter("timeLimiter")
{

  addConfigOptionsTo(this);

  _timeLimiterStr = "MinMod";
  setParameter("TimeLimiter",&_timeLimiterStr);


}

//////////////////////////////////////////////////////////////////////////////

void U.minit::configure ( Config::ConfigArgs& args )
{
  NewtonIteratorCom::configure(args);

  _timeLimiter =
  Environment::Factory<FiniteVolume::TimeLimiter>::getInstance().
    getProvider(_timeLimiterStr)->create(_timeLimiterStr);

  configureNested ( _timeLimiter.getPtr(), args );

  cf_assert(_timeLimiter.isNotNull());

}

//////////////////////////////////////////////////////////////////////////////

void U.minit::setup()
{
  NewtonIteratorCom::setup();

  DataHandle < Framework::State*, Framework::GLOBAL > states = socket_states.getDataHandle();
  DataHandle<CFreal> timeLimiter = socket_timeLimiter.getDataHandle();

  const CFuint nbStates = states.size();
  const CFuint nbEqs = PhysicalModelStack::getActive()->getNbEq();

  timeLimiter.resize(nbStates*nbEqs);

  timeLimiter = 1.;

}

//////////////////////////////////////////////////////////////////////////////

void U.minit::execute()
{

  CFAUTOTRACE;

  DataHandle < Framework::State*, Framework::GLOBAL > states = socket_states.getDataHandle();
  DataHandle<State*> pastStates = socket_pastStates.getDataHandle();
  DataHandle<State*> pastPastStates = socket_pastPastStates.getDataHandle();

  DataHandle<CFreal> timeLimiter = socket_timeLimiter.getDataHandle();

  const CFreal timestep = SubSystemStatusStack::getActive()->getDT();
  const CFreal previousTimestep = SubSystemStatusStack::getActive()->getPreviousDT();
  const CFuint nbStates = states.size();
  const CFuint nbEqs = PhysicalModelStack::getActive()->getNbEq();

if(SubSystemStatusStack::getActive()->getNbIter() < 2){
  for(CFuint iState=0; iState < nbStates; iState++)
  {
    for (CFuint iEq = 0; iEq < nbEqs; ++iEq)
    {
      const CFreal slopeNEW = (*states[iState])[iEq] - (*pastStates[iState])[iEq] / timestep;
      const CFreal slopeOLD = (*pastStates[iState])[iEq] - (*pastPastStates[iState])[iEq] / previousTimestep;

      timeLimiter(iState, iEq, nbEqs) = _timeLimiter->computeTimeLimiterValue(slopeOLD,slopeNEW);
    }
  }
}





}

//////////////////////////////////////////////////////////////////////////////

vector<SafePtr<BaseDataSocketSink> >
U.minit::needsSockets()
{
  vector<SafePtr<BaseDataSocketSink> > result;

  result.push_back(&socket_states);
  result.push_back(&socket_pastStates);
  result.push_back(&socket_pastPastStates);

  return result;
}

//////////////////////////////////////////////////////////////////////////////

vector<SafePtr<BaseDataSocketSource> >
U.minit::providesSockets()
{
  vector<SafePtr<BaseDataSocketSource> > result;

  result.push_back(&socket_timeLimiter);

  return result;
}


//////////////////////////////////////////////////////////////////////////////

    } // namespace NewtonMethod

  } // namespace Numerics

} // namespace COOLFluiD
