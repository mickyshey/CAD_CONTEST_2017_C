// File name [ CirSat.cpp ]

#include "cir/cirMgr.h"

using namespace std;

void
CirNet::createVar(SatSolverV* s) const
{
	buildTopoList();
	for( unsigned i = 0; i < _topoList.size(); ++i ) {
		Var v = s -> newVar();
		_topoList[i] -> setVar(v);
	}
}

void
CirNet::addToSolver(SatSolverV* s) const
{
	buildTopoList();
	for( unsigned i = 0; i < _topoList.size(); ++i ) {
		//cout << "adding " << _topoList[i] -> getName() << endl;
		_topoList[i] -> addToSolver(s);
	}
}

void
CirMgr::tieGate(CirGate* g1, CirGate* g2)
{
	assert(g1 -> getName() == g2 -> getName());
	g2 -> setVar(g1 -> getVar());
}

// make the var in _piList equal
void
CirMgr::tiePi(CirNet* f, CirNet* g)
{
	//assert(f -> getPiNum() == g -> getPiNum() + _candNameList.size());
	// we should get pi from g instead of f
	for( unsigned i = 0; i < g -> getPiNum(); ++i ) {
		CirGate* fPi = f -> getPi(i);
		CirGate* gPi = g -> getGateByName(fPi -> getName());
		gPi -> setVar(fPi -> getVar());
	}
}

void
CirMgr::addXorConstraint(CirNet* f, CirNet* g)
{
	assert(f -> getPoNum() == g -> getPoNum());
	for( unsigned i = 0; i < f -> getPoNum(); ++i ) {
		CirGate* fPo = f -> getPo(i);
		CirGate* gPo = g -> getPo(i);
		Var v = _s -> newVar();
		_s -> addXorCNF(v, fPo -> getVar(), false, gPo -> getVar(), false);			// POs should not have bubbles !?
		// should i record these vars ? for later purpose: make assumption
		
		// we first assert all Xors to be 1
		_s -> addUnitCNF(v, 1);
	}
}

// for single error only
void
CirMgr::addErrorConstraint(CirNet* n, bool val)
{
	assert(n -> getErrorNum());
	CirGate* g = n -> getError(0);
	_s -> addUnitCNF(g -> getVar(), val);
}

void
CirMgr::markOnsetClause(const ClauseId& cid)
{
  unsigned cSize = getNumClauses();
  assert(cid < (int)cSize);
  if(_isClauseOn.size() < cSize) {
    _isClauseOn.resize(cSize, false);
  }
  _isClauseOn[cid] = true;
}

void
CirMgr::markOffsetClause(const ClauseId& cid)
{
  unsigned cSize = getNumClauses();
  assert(cid < (int)cSize);
  if(_isClauseOn.size() < cSize) {
    _isClauseOn.resize(cSize, false);
  }
  _isClauseOn[cid] = false;
}
