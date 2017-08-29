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

	_s -> reset();
	// create OFF set
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		Var v = _s -> newVar();
		_sortedCandGate[i] -> setVar(v);
	}
	createVar(_F);
	createVar(_G);
	tiePi(_F, _G);
    cout << "# cluases: " << _s -> getNumClauses() << endl; 
	addToSolver(_F);
	addToSolver(_G);
    cout << "# cluases: " << _s -> getNumClauses() << endl; 
	addXorConstraint(_F, _G);
   //addConstConstraint(_F);
   //addConstConstraint(_G);
	_s -> simplify();
	bool eqCheck = solve();
	cout << (eqCheck ? "SAT" : "UNSAT") << endl;
	
	
}
