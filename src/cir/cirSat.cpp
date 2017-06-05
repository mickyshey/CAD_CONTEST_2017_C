// File name [ CirSat.cpp ]

#include <cassert>
#include <queue>

#include "cir/cirMgr.h"
#include "cir/reader.h"

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
CirMgr::buildVarMap(CirNet* ntk) {
    // map PI var
    for(int i = 0; i < ntk->getPiNum(); i++) {
        CirGate* g = ntk->getPi(i);
        _var2Gate[g->getVar()] = g;
    }
    // map PO var
    for(int i = 0; i < ntk->getPoNum(); i++) {
        CirGate* g = ntk->getPo(i);
        _var2Gate[g->getVar()] = g;
    }
    // map gate var
    for(int i = 0; i < ntk->getGateNum(); i++) {
        CirGate* g = ntk->getGate(i);
        _var2Gate[g->getVar()] = g;
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
	assert(f -> getPiNum() == g -> getPiNum());
	for( unsigned i = 0; i < f -> getPiNum(); ++i ) {
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

CirNet*
CirMgr::getItp()
{
    string proofName = "itp.tmp";
    _s->proof->save(proofName.c_str());
    
    CirNet* itp = buildItp(proofName);
    
    return itp;
}

CirNet*
CirMgr::buildItp(const string& fileName)
{
    Reader rdr;

    map<ClauseId, CirGate*> claItpLookUp;
    vector<ClauseId> usedClause;
    vector<unsigned> clausePos;
    
    // tmp var
    CirNet* ntk;
    CirGate* g;
    CirGate* g1;
    CirGate* g2;
    int i, cid, tmp, idx, tmp_cid;

    ntk->createConst(0);
    ntk->createConst(1);

    rdr.open(fileName.c_str());
    retrieveProof(rdr, clausePos, usedClause);

    for(i = 0; i < (int)usedClause.size(); i++) {
        cid = usedClause[i];
        rdr.seek(clausePos[cid]);
        tmp = rdr.get64();
        if((tmp & 1) == 0) {
            // root clause
            if(_isClauseOnDup[cid]) {
                idx = tmp >> 1;
                while(_varGroup[idx >> 1] != COMMON) {
                    tmp = rdr.get64();
                    if(tmp == 0) break;
                    idx += tmp;
                }

                if(_varGroup[idx >> 1] == COMMON) {
                    g = (_var2Gate.find(idx >> 1))->second;
                    g1 = (_var2Gate.find(idx >> 1))->second;
                    if((idx & 1) == 1) {
                        /*for(int j = 0; j = g->getFanoutSize(); j++) {
                            CirGateV tmpG = g->getFanout(j);
                            tmpG = tmpG.flipInv();
                            g->setFanout(tmp, j);
                        }*/
                        for(int j = 0; j = g1->getFanoutSize(); j++) {
                            CirGateV tmpG = CirGateV(g1->getFanout(j));
                            tmpG.flipInv();
                            g1->setFanout(tmpG, j);
                        }
                    }
                    while(1) {
                        tmp = rdr.get64();
                        if(tmp == 0) break;
                        idx += tmp;
                        if(_varGroup[idx >> 1] == COMMON) {
                            g2 = (_var2Gate.find(idx >> 1))->second;
                            if((idx & 1) == 1) {
                                for(int j = 0; j = g2->getFanoutSize(); j++) {
                                    CirGateV tmpG = CirGateV(g2->getFanout(j));
                                    tmpG.flipInv();
                                    g2->setFanout(tmpG, j);
                                }
                            }
                            // or
                            g = ntk->createGate(Gate_Or);
                            g->pushBackFanin(CirGateV(g1));
                            g->pushBackFanin(CirGateV(g2));
                            g1 = g;
                        }
                    }
                } else {
                    g = ntk->getConst(0);
                }
                claItpLookUp[cid] = g;
            } else {
                claItpLookUp[cid] = ntk->getConst(1);
            }
        }  else {
            // derive clause
            tmp_cid = cid - (tmp >> 1);
            g = (claItpLookUp.find(tmp_cid))->second;
            g1 = (claItpLookUp.find(tmp_cid))->second;
            while(1) {
                idx = rdr.get64();
                if(idx == 0) break;
                idx--;
                // Var is idx
                tmp_cid = cid - rdr.get64();
                g2 = (claItpLookUp.find(tmp_cid))->second;
                if(g1 != g2) {
                    if(_varGroup[idx] == LOCAL_ON) {
                        if((g1 == ntk->getConst(1)) || (g2 == ntk->getConst(0))) {
                            g = ntk->getConst(1);
                            g1 = g;
                        } else if(g1 == ntk->getConst(0)) {
                            g = g2;
                            g1 = g;
                        } else if(g2 == ntk->getConst(0)) {
                            g = g1; // wtf in SoCV here???
                        } else {
                            // or
                            g = ntk->createGate(Gate_Or);
                            g->pushBackFanin(CirGateV(g1));
                            g->pushBackFanin(CirGateV(g2));
                            g1 = g;
                        }
                    } else { // build AND gate
                        if((g1 == ntk->getConst(0)) || (g2 == ntk->getConst(0))) {
                            g = ntk->getConst(0);
                            g1 = g;
                        } else if(g1 == ntk->getConst(1)) {
                            g = g2;
                            g1 = g;
                        } else if(g2 == ntk->getConst(0)) {
                            g = g1;
                        } else {
                            // and
                            g = ntk->createGate(Gate_And);
                            g->pushBackFanin(CirGateV(g1));
                            g->pushBackFanin(CirGateV(g2));
                            g1 = g;
                        }
                    }
                }
            }
            claItpLookUp[cid] = g;
        }
    }

    cid = usedClause[usedClause.size() - 1];
    g = claItpLookUp[cid]; // needed??

    return ntk;
}

void
CirMgr::retrieveProof( Reader& rdr, vector<unsigned>& clausePos, vector<ClauseId>& usedClause )
{
    unsigned tmp, cid, idx, tmp_cid, root_cid;
    
    // clear all
    clausePos.clear();
    usedClause.clear();
    _isClauseOnDup.clear();
    _varGroup.clear();
    _varGroup.resize(_s->nVars(), NONE);
    assert((int)_isClauseOn.size() == getNumClauses());

    // generate clausePos
    rdr.seek(0);
    root_cid = 0;
    for(unsigned pos = 0; (tmp = rdr.get64()) != RDR_EOF; pos = rdr.Current_Pos()) {
        cid = clausePos.size();
        clausePos.push_back(pos);
        if((tmp & 1) == 0){ // root clause
            _isClauseOnDup.push_back(_isClauseOn[root_cid]);
            idx = tmp >> 1;
            if(_isClauseOn[root_cid]) {
                if(_varGroup[idx >> 1] == NONE) _varGroup[idx >> 1] = LOCAL_ON;
                else if(_varGroup[idx >> 1] == LOCAL_OFF) _varGroup[idx >> 1] = COMMON;
            }
            else {
                if(_varGroup[idx >> 1] == NONE) _varGroup[idx >> 1] = LOCAL_OFF;
                else if(_varGroup[idx >> 1] == LOCAL_ON) _varGroup[idx >> 1] = COMMON;
            }
            while(1) {
                tmp = rdr.get64();
                if( tmp == 0 ) break;
                idx += tmp;
                if(_isClauseOn[root_cid]) {
                    if(_varGroup[idx >> 1] == NONE) _varGroup[idx >> 1] = LOCAL_ON;
                    else if(_varGroup[idx >> 1] == LOCAL_OFF) _varGroup[idx >> 1] = COMMON;
                }
                else{
                    if(_varGroup[idx >> 1] == NONE) _varGroup[idx >> 1] = LOCAL_OFF;
                    else if(_varGroup[idx >> 1] == LOCAL_ON) _varGroup[idx >> 1] = COMMON;
                }
            }
            ++root_cid;
        } else {
            _isClauseOnDup.push_back(false);
            idx = 0;
            while(1) {
                tmp = rdr.get64();
                if(tmp == 0) break;
                idx = 1;
                tmp = rdr.get64();
            }
            if(idx == 0) {
                clausePos.pop_back(); // clause deleted
                _isClauseOnDup.pop_back();
            }
        }
    }
    // generate usedClause
    priority_queue<unsigned> clause_queue;
    vector<bool> in_queue;
    in_queue.resize(clausePos.size());
    for(unsigned i = 0; i < in_queue.size(); i++) in_queue[i] = false;
    in_queue[in_queue.size() - 1] = true;
    clause_queue.push(clausePos.size() - 1); // Push root empty clause
    while(clause_queue.size() != 0) {
        cid = clause_queue.top();
        clause_queue.pop();

        rdr.seek( clausePos[cid] );

        tmp = rdr.get64();
        if((tmp & 1) == 0) continue; // root clause

        // eles derive clause
        tmp_cid = cid - (tmp >> 1);
        if( !in_queue[tmp_cid] ) {
            in_queue[tmp_cid] = true;
            clause_queue.push(tmp_cid);
        }
        while(1) {
            tmp = rdr.get64();
            if(tmp == 0) break;
            tmp_cid = cid - rdr.get64();
            if(!in_queue[tmp_cid]) {
                in_queue[tmp_cid] = true;
                clause_queue.push(tmp_cid);
            }
        }
    }
    for(unsigned i = 0; i < in_queue.size(); i++) {
        if(in_queue[i]) usedClause.push_back(i);
    }
}

