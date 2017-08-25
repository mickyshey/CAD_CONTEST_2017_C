// File name [ cirMgr.cpp ]
#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cmath>
#include <bitset>
#include <unordered_set>
#include <ctime>

#include "cir/cirMgr.h"
#include "util/parse.h";

using namespace std;


void
CirMgr::formulaRM(CirNet* f, CirNet* g, int solver) 
{
    if(solver == 0) {

        createVar(f);
        createVar(g);
        std::cout << "# vars in solver: " << _s -> nVars() << endl;

        tiePi(f, g);
        if(f -> getName() != "top") {
            tiePi(_F, f);
            tiePi(_F, g);
        }

        addToSolver(f);
        addToSolver(g);
        addXorConstraint(f, g, 0);
    } else if(solver == 1) {
        // create var for candidate first (modify later?)
        for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
            Var v = _rmSolver -> newVar();
            f -> getGateByName(_sortedCandGate[i] -> getName()) -> setRMVar(v);
        }
        cout << endl;
        f -> createVar(_rmSolver, 2);
        g -> createVar(_rmSolver, 2);
        std::cout << "# vars in RMsolver: " << _rmSolver -> nVars() << endl;

        tiePi(f, g, 2);

        unsigned numClauses = _rmSolver -> getNumClauses();
        assert( numClauses == 0 );
        f -> addToSolver(_rmSolver, 2);
        g -> addToSolver(_rmSolver, 2);
        addXorConstraint(f, g, 2);
    }
}

void
CirMgr::markMultiOnsetClause(const ClauseId& cid, const string& name)
{
    unsigned cSize = getNumClauses();
    assert(cid < (int)cSize);
    if(_rmOnset[name].size() < cSize) {
        _rmOnset[name].resize(cSize, false);
    }
    _rmOnset[name][cid] = true;
}

void
CirMgr::markMultiOffsetClause(const ClauseId& cid, const string& name)
{
    unsigned cSize = getNumClauses();
    assert(cid < (int)cSize);
    if(_rmOnset[name].size() < cSize) {
        _rmOnset[name].resize(cSize, false);
    }
    _rmOnset[name][cid] = false;
}
