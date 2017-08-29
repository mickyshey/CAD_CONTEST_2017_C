#include <cassert>

#include "cirMgr.h"

using namespace std;

VarMap varI1F;
VarMap varI1G;
VarMap varI2F;
VarMap varI2G;

void
CirMgr::checkPo()
{
/*
    SatSolverV solver;
    
    genProofModel(solver);
*/
}

void
CirMgr::genProofModel(SatSolverV& solver)
{
/*
    varI1F.clear();
    varI1G.clear();
    varI2F.clear();
    varI2G.clear();
    assert(_F->_piList.size() == _G->_piList.size());
*/
}

void
CirMgr::checkValidPatch()
{
   // connect patch back to _F
   // connect patch_po to error, assuming single error
   CirGate* patchPo = _patch -> getPo(0);
   assert(patchPo -> getFanoutSize() == 0);
   assert(_F -> getError(0) -> getFaninSize() == 0);
   patchPo -> pushBackFanout(CirGateV(_F -> getError(0), false));
   _F -> getError(0) -> pushBackFanin(CirGateV(patchPo, false));
   // connect patch_pi to corresponding candidates
   for( unsigned i = 0; i < _patch -> getPiNum(); ++i ) {
      CirGate* patchPi = _patch -> getPi(i);
      CirGate* target = _F -> getGateByName(patchPi -> getName());
      assert(target);
      assert(patchPi -> getFaninSize() == 0);
      patchPi -> pushBackFanin(CirGateV(target,false));
      target -> pushBackFanout(CirGateV(patchPi, false));
   }

	_s -> reset();
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		Var v = _s -> newVar();
		_sortedCandGate[i] -> setVar(v);
	}
	createVar(_F);
	createVar(_G);
	tiePi(_F, _G);
	addToSolver(_F);
	addToSolver(_G);
	addXorConstraint(_F, _G);
	_s -> simplify();
	bool eqCheck = solve();
	cout << (eqCheck ? "SAT" : "UNSAT") << endl;
	
	
}
