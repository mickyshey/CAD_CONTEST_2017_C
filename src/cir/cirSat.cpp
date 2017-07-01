// File name [ CirSat.cpp ]

#include <cassert>
#include <queue>
#include <set>

#include "cir/cirMgr.h"
#include "util/parse.h"

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
			Var v = g -> getVar();
			if( _var2Gate.find(v) == _var2Gate.end() ) _var2Gate.insert({v, g});
    }
    // map PO var
    for(int i = 0; i < ntk->getPoNum(); i++) {
        CirGate* g = ntk->getPo(i);
			Var v = g -> getVar();
			if( _var2Gate.find(v) == _var2Gate.end() ) _var2Gate.insert({v, g});
    }
    // map gate var
    for(int i = 0; i < ntk->getGateNum(); i++) {
        CirGate* g = ntk->getGate(i);
			Var v = g -> getVar();
			if( _var2Gate.find(v) == _var2Gate.end() ) _var2Gate.insert({v, g});
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
		//std::cout << "f: " << fPi -> getName() << "(" << fPi -> getVar() << ")" << std::endl;
		//std::cout << "before tie: " << std::endl;
		//std::cout << "g: " << gPi -> getName() << "(" << gPi -> getVar() << ")" << std::endl;
		gPi -> setVar(fPi -> getVar());
		//std::cout << "after tie: " << std::endl;
		//std::cout << "g: " << gPi -> getName() << "(" << gPi -> getVar() << ")" << std::endl;
	}
}

void
CirMgr::tieConst(CirNet* f, CirNet* g)
{
	g -> getConst(1) -> setVar(f -> getConst(1) -> getVar());
	g -> getConst(0) -> setVar(f -> getConst(0) -> getVar());
}

void
CirMgr::addXorConstraint(CirNet* f, CirNet* g)
{
	assert(f -> getPoNum() == g -> getPoNum());
	vector<Var> Xors;
	for( unsigned i = 0; i < f -> getPoNum(); ++i ) {
		CirGate* fPo = f -> getPo(i);
		CirGate* gPo = g -> getGateByName(fPo -> getName());
		std::cout << "XORing: " << fPo -> getName() << "(" << fPo << ") and " << gPo -> getName() << "(" << gPo << ")" << std::endl;
		//CirGate* gPo = g -> getPo(i);
		Var v = _s -> newVar();
		_s -> addXorCNF(v, fPo -> getVar(), false, gPo -> getVar(), false);			// POs should not have bubbles !?
		// should i record these vars ? for later purpose: make assumption
		
		// we first assert all Xors to be 1
		// NO !! we should add an OR gate

	// for debugging, only one PO
		//_s -> addUnitCNF(v, 1);


	// if real case is more than two POs
		Xors.push_back(v);
	}
	// first assume only two outputs
	assert(Xors.size() == 2);
	Var out = _s -> newVar();
	_s -> addOrCNF(out, Xors[0], false, Xors[1], false);
	_s -> addUnitCNF(out, 1);
}

void
CirMgr::addXorCheck(CirNet* f, CirNet* g, CirNet* p)
{
	assert(f -> getPoNum() == g -> getPoNum());
	vector<Var> Xors;
	for( unsigned i = 0; i < f -> getPoNum(); ++i ) {
		CirGate* fPo = f -> getPo(i);
		CirGate* gPo = g -> getGateByName(fPo -> getName());
		std::cout << "XORing: " << fPo -> getName() << "(" << fPo << ") and " << gPo -> getName() << "(" << gPo << ")" << std::endl;
		//CirGate* gPo = g -> getPo(i);
		Var v = _s -> newVar();
		_s -> addXorCNF(v, fPo -> getVar(), false, gPo -> getVar(), false);			// POs should not have bubbles !?
		// should i record these vars ? for later purpose: make assumption
		
		// we first assert all Xors to be 1
		// NO !! we should add an OR gate

	// for debugging, only one PO
		//_s -> addUnitCNF(v, 1);


	// if real case is more than two POs
		Xors.push_back(v);
	}
	// first assume only two outputs
	assert(Xors.size() == 2);
	Var out = _s -> newVar();
	_s -> addOrCNF(out, Xors[0], false, Xors[1], false);
	_s -> addUnitCNF(out, 1);
    Var po = p->getPo(0)->getVar();
    Var out1 = _s -> newVar();
    _s -> addXorCNF(out1, out, false, po, false);
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
    _s->_solver->proof->save(proofName.c_str());
    
    CirNet* itp = buildItp(proofName);
    
    unlink(proofName.c_str());
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
    CirNet* ntk = new CirNet;
    CirGate* g;
    CirGate* g1;
    CirGate* g2;
    int i, cid, tmp, idx, tmp_cid, w;
	string wireName = "w";
    unsigned curFoSize, curFiSize;
    // set<CirGate*> wireGate;

    ntk->createConst(0);
    ntk->createConst(1);

    rdr.open(fileName.c_str());
    retrieveProof(rdr, clausePos, usedClause);
    //for(size_t n = 0; n < _isClauseOn.size(); ++n) cerr << _isClauseOn[n];
    //cerr << endl;
    //for(size_t i = 0; i < _isClauseOnDup.size(); ++i) cerr << _isClauseOnDup[i];
    //cerr << endl;
    //cerr << "usedClause: ";
    //for( unsigned i = 0; i < usedClause.size(); ++i )cerr  << usedClause[i] << ' ';
    //cerr << endl;
/*
    // MY DEBUGGING
    cerr << "*********** START DEBUGGING **************" << endl;
    for(i = 0; i < (int)usedClause.size() ; i++) {
        cid = usedClause[i];
        rdr.seek( clausePos[ cid ] );
        tmp = rdr.get64();
		if((tmp & 1) == 0) {
		    //Root Clause
			cerr << cid;
			if( _isClauseOnDup[ cid ] ) cerr << " (A): ";
			else cerr << " (B): ";
			idx = tmp >> 1;
			if ( _varGroup[idx >> 1] == COMMON )
			    cerr << (idx&1) << '_' << (_var2Gate.find(idx >> 1)->second)->getName() << ' ';
			else
				cerr << (idx&1) << '_' << (idx>>1) << ' ';
			while(1){
				tmp = rdr.get64();
				if( tmp == 0 ) break;
				idx += tmp;
				if ( _varGroup[idx >> 1] == COMMON )
				    cerr << (idx&1) << '_' << (_var2Gate.find(idx >> 1)->second)->getName() << ' ';
				else
				    cerr << (idx&1) << '_' << (idx>>1) << ' ';
			}
			cerr << endl;
		} else {
			//Derived Clause
			cerr << cid << ": ";
			tmp_cid = cid - (tmp >> 1);
			cerr << tmp_cid << ' ';
			while(1) {
				idx = rdr.get64();
				if( idx == 0 ) break;
				idx--;
				if ( _varGroup[idx] == COMMON )
			        cerr << '(' << (_var2Gate.find(idx)->second)->getName() << ") ";
				else if(_varGroup[idx] == LOCAL_ON)
				    cerr << '(' << idx << "/A) ";
				else if(_varGroup[idx] == LOCAL_OFF)
				    cerr << '(' << idx << "/B) ";
				//Var is idx
				tmp_cid = cid - rdr.get64();
				cerr << tmp_cid << ' ';
			}
			cerr << endl;
		}
	}
    cerr << "*********** END DEBUGGING **************" << endl;
	// END OF MY DEBUGGING
*/
	w = 0;
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
                    ntk -> pushBackPIList(g); // push BaseNode into ntk _piList
                    if((idx & 1) == 1) {
                        for(int j = 0; j < g -> getFanoutSize(); j++) {
                            CirGateV tmpG = CirGateV(g -> getFanout(j));
                            tmpG.flipInv();
                            g -> setFanout(tmpG, j);
                        }
                    }
                    while(1) {
                        tmp = rdr.get64();
                        if(tmp == 0) break;
                        idx += tmp;
                        if(_varGroup[idx >> 1] == COMMON) {
                            g2 = (_var2Gate.find(idx >> 1))->second;
                            ntk -> pushBackPIList(g2); // push BaseNode into ntk _piList
                            if((idx & 1) == 1) {
                                for(int j = 0; j < g2 -> getFanoutSize(); j++) {
                                    CirGateV tmpG = CirGateV(g2 -> getFanout(j));
                                    tmpG.flipInv();
                                    g2 -> setFanout(tmpG, j);
                                }
                            }
                            // or
                            cerr << "created OR gate w(" << w << ")"  << endl; // for debug
							std::string name = wireName + myToString(w);
							w++;
                            g = ntk->createGate(Gate_Or, name);
                            g -> pushBackFanin(CirGateV(g1));
                            g -> pushBackFanin(CirGateV(g2));
                            // maintain the corresponding fanout of g1, g2
                            g1 -> pushBackFanout(CirGateV(g));
                            g2 -> pushBackFanout(CirGateV(g));
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
                        if((g1 == ntk->getConst(1)) || (g2 == ntk->getConst(1))) {
                            g = ntk->getConst(1);
                            g1 = g;
                        } else if(g1 == ntk->getConst(0)) {
                            g = g2;
                            g1 = g;
                        } else if(g2 == ntk->getConst(0)) {
                            g = g1; // wtf in SoCV here???
                        } else {
                            // or
                            cerr << "created OR gate w(" << w << ")"  << endl; // for debug
							std::string name = wireName + myToString(w);
							w++;
                            g = ntk->createGate(Gate_Or, name);
                            g -> pushBackFanin(CirGateV(g1));
                            g -> pushBackFanin(CirGateV(g2));
                            // maintain corresponding fanout of g1, g2
                            g1 -> pushBackFanout(CirGateV(g));
                            g2 -> pushBackFanout(CirGateV(g));
                            g1 = g;
                        }
                    } else { // build AND gate
                        if((g1 == ntk->getConst(0)) || (g2 == ntk->getConst(0))) {
                            g = ntk->getConst(0);
                            g1 = g;
                        } else if(g1 == ntk->getConst(1)) {
                            g = g2;
                            g1 = g;
                        } else if(g2 == ntk->getConst(1)) {
                            g = g1;
                        } else {
                            // and
                            cerr << "created AND gate w(" << w << ")"  << endl; // for debug
							std::string name = wireName + myToString(w);
							w++;
                            g = ntk->createGate(Gate_And, name);
                            g -> pushBackFanin(CirGateV(g1));
                            g -> pushBackFanin(CirGateV(g2));
                            // maintain corresponding fanout of g1, g2
                            g1 -> pushBackFanout(CirGateV(g));
                            g2 -> pushBackFanout(CirGateV(g));
                            g1 = g;
                        }
                    }
                }
            }
            claItpLookUp[cid] = g;
        }
    }

    cid = usedClause[usedClause.size() - 1];
    g = claItpLookUp[cid];
    // IMPORTANT!! when a new net created, every PI/PO/Gate/Topo lists shoud be mantained carefully.
    ntk -> pushBackPOList(g); // add po to _poList

    // FIXME: paste patch should be done outside this function
    CirGate* po = _F->getError(0);
	unsigned gSize = g->getFanoutSize();
    g->setFanoutSize(gSize + 1);
	g->setFanout(CirGateV(po), gSize);
	po->setFaninSize(1);
	po->setFanin(CirGateV(g), 0);
    
    /*
    CirGate* po = _F -> getError(0);
    for(size_t i = 0; i < po->getFanoutSize(); i++) {
        g->setFanout(CirGateV(po->getFanout(i)), i);
    }
	for(size_t i = 0; i < po->getFanoutSize(); i++) {
		CirGate* fo = po->getFanout(i);
		curFiSize = fo->getFaninSize();
		fo->setFaninSize(curFiSize + 1);
		fo->setFanin(CirGateV(g), curFiSize);
	}
    */

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
            // debug
            // cerr << (_isClauseOn[cid] ? "A":"B") << " ";
            // cerr << "R" << cid << ": ";
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
    // debug
    //cerr << "retrive... _varGroup" << endl;
    //for(size_t n = 0; n < _varGroup.size(); ++n) cerr << _varGroup[n];
    //cerr << endl;
}

bool
CirMgr::proveEQ(CirNet* f, CirNet* g)
{
	_s -> reset();
	assert(getNumClauses() == 0);
	createVar(f);
	createVar(g);
	tiePi(f, g);
	addToSolver(f);
	addToSolver(g);
	addXorConstraint(f, g);
	return !solve();
}

