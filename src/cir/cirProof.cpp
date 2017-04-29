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
    SatSolver solver;
    
    solver.initialize();
    genProofModel(solver);
}

void
CirMgr::genProofModel(Solver& solver)
{
    varI1F.clear();
    varI1G.clear();
    varI2F.clear();
    varI2G.clear();
    assert(_F->_piList.size() == _G->_piList.size());
}
