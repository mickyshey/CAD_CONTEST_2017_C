// File name [ CirSat.cpp ]

#include <cassert>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <iomanip>

#include "cir/cirMgr.h"
#include "util/parse.h"

using namespace std;

unsigned CirMgr::_patchGateCount = 0;

void
CirMgr::createVarAll() const
{
   _s -> reset();
	// IMPORTANT !! we first create var for candidates
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		Var v = _s -> newVar();
		_sortedCandGate[i] -> setVar(v);
	}
    createVar(_F);
    createVar(_G);
    createVar(_dupF);
    createVar(_dupG);
}

void
CirNet::createVar(SatSolverV* s, int solver) const
{
	assert(solver <= 2);
	totGateList();
	for( unsigned i = 0; i < _totGateList.size(); ++i ) {
		// IMPORTANT !! we first create var for candidates
		/* if( _totGateList[i] -> getVar() ) continue; */
		if( _totGateList[i] -> getWeight() ) continue;
		Var v = s -> newVar();
		if( solver == 0 ) _totGateList[i] -> setVar(v);
		else if( solver == 1 ) _totGateList[i] -> setCandVar(v);
		else if( solver == 2 ) _totGateList[i] -> setRMVar(v);
	}
/*
	buildTopoList();
	for( unsigned i = 0; i < _topoList.size(); ++i ) {
		Var v = s -> newVar();
		if( solver == 0 ) _topoList[i] -> setVar(v);
		else if( solver == 1 ) _topoList[i] -> setCandVar(v);
	}
*/
}

void
CirNet::addToSolver(SatSolverV* s, int solver) const
{
	totGateList();
	for( unsigned i = 0; i < _totGateList.size(); ++i ) {
		_totGateList[i] -> addToSolver(s, solver);
	}
/*
	buildTopoList();
	for( unsigned i = 0; i < _topoList.size(); ++i ) {
		//cout << "adding " << _topoList[i] -> getName() << endl;
		_topoList[i] -> addToSolver(s, solver);
		//std::cout << "curr # clauses: " << s -> getNumClauses() << std::endl;
	}
*/
}

void
CirMgr::buildVarMap(CirNet* ntk) {
    // map PI var
    for(int i = 0; i < ntk->getPiNum(); i++) {
        CirGate* g = ntk->getPi(i);
			Var v = g -> getVar();
			if( _var2Gate.find(v) == _var2Gate.end() ) _var2Gate.insert(std::pair<Var, CirGate*>(v, g));
    }
    // map PO var
    for(int i = 0; i < ntk->getPoNum(); i++) {
        CirGate* g = ntk->getPo(i);
			Var v = g -> getVar();
			if( _var2Gate.find(v) == _var2Gate.end() ) _var2Gate.insert(std::pair<Var, CirGate*>(v, g));
    }
    // map gate var
    for(int i = 0; i < ntk->getGateNum(); i++) {
        CirGate* g = ntk->getGate(i);
			Var v = g -> getVar();
			if( _var2Gate.find(v) == _var2Gate.end() ) _var2Gate.insert(std::pair<Var, CirGate*>(v, g));
    }

}

void
CirMgr::buildVarMap(CirGate* g ) {
	Var v = g -> getVar();
	_var2Gate.insert(std::pair<Var, CirGate*>(v, g));
}

void
CirMgr::tieGate(CirGate* g1, CirGate* g2)
{
	assert(g1 -> getName() == g2 -> getName());
	g2 -> setVar(g1 -> getVar());
}

// make the var in _piList equal
void
CirMgr::tiePi(CirNet* f, CirNet* g, int solver)
{
	assert(solver <= 2);
	//assert(f -> getPiNum() == g -> getPiNum() + _candNameList.size());
	// we should get pi from g instead of f
	for( unsigned i = 0; i < g -> getPiNum(); ++i ) {
		CirGate* fPi = f -> getPi(i);
		CirGate* gPi = g -> getGateByName(fPi -> getName());
		//std::cout << "f: " << fPi -> getName() << "(" << fPi -> getVar() << ")" << std::endl;
		//std::cout << "before tie: " << std::endl;
		//std::cout << "g: " << gPi -> getName() << "(" << gPi -> getVar() << ")" << std::endl;
		if( solver == 0 ) gPi -> setVar(fPi -> getVar());
		else if( solver == 1 ) gPi -> setCandVar(fPi -> getCandVar());
		else if( solver == 2 ) gPi -> setRMVar(fPi -> getRMVar());
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
CirMgr::addXorConstraint(CirNet* f, CirNet* g, int solver)
{
	assert(solver <= 2);
	//assert(f -> getPoNum() == g -> getPoNum());
// test
	if( f -> getPoNum() == 1 ) {
		CirGate* fPo = f -> getPo(0);
		CirGate* gPo = g -> getGateByName(fPo -> getName());
		if( solver == 0 ) {
		    Var v = _s -> newVar();
            _s -> addXorCNF(v, fPo -> getVar(), false, gPo -> getVar(), false);			// POs should not have bubbles !?
		    _s -> addUnitCNF(v, 1);
            _rmOut.push_back(v);
		}
		else if( solver == 1 ) {
		    Var v = _candSolver -> newVar();
		    _candSolver -> addXorCNF(v, fPo -> getCandVar(), false, gPo -> getCandVar(), false);			// POs should not have bubbles !?
		    _candSolver -> addUnitCNF(v, 1);
		}
        else if( solver == 2 ) {
            Var v = _s -> newVar();
            _rmSolver -> addXorCNF(v, fPo -> getVar(), false, gPo -> getVar(), false);
            _rmSolver -> addUnitCNF(v, 0);
        }
		return;
	}
// end of test
	vector<Var> Xors;
	for( unsigned i = 0; i < f -> getPoNum(); ++i ) {
		CirGate* fPo = f -> getPo(i);
		CirGate* gPo = g -> getGateByName(fPo -> getName());
		//std::cout << "XORing: " << fPo -> getName() << "(" << fPo << ") and " << gPo -> getName() << "(" << gPo << ")" << std::endl;
		Var v;
		if( solver == 0 ) {
		    v = _s -> newVar();
		    _s -> addXorCNF(v, fPo -> getVar(), false, gPo -> getVar(), false);			// POs should not have bubbles !?
		}
		else if( solver == 1 ) {
		    v = _candSolver -> newVar();
		    _candSolver -> addXorCNF(v, fPo -> getCandVar(), false, gPo -> getCandVar(), false);			// POs should not have bubbles !?
		}
        else if( solver == 2 ) {
		    v = _rmSolver -> newVar();
		    _rmSolver -> addXorCNF(v, fPo -> getVar(), false, gPo -> getVar(), false);			// POs should not have bubbles !?
		}
		// we first assert all Xors to be 1
		// NO !! we should add an OR gate
		// record these vars
		Xors.push_back(v);
	}
	assert(Xors.size() == f -> getPoNum());
	Var out;
	if( solver == 0 ) {
	    out = _s -> newVar();
	    _s -> addOrCNF(out, Xors);
	    _s -> addUnitCNF(out, 1);
        _rmOut.push_back(out);
	}
	else if( solver == 1 ) {
	    out = _candSolver -> newVar();
        _candSolver -> addOrCNF(out, Xors);
	    _candSolver -> addUnitCNF(out, 1);
	}
    else if( solver == 2 ) {
        out = _rmSolver -> newVar();
        _rmSolver -> addOrCNF(out, Xors);
        _rmSolver -> addUnitCNF(out, 0);
    }
}

// for single error only
void
CirMgr::addErrorConstraint(CirNet* n, bool val, int solver)
{
	assert(solver <= 1);
	assert(n -> getErrorNum() == 1);
	CirGate* g = n -> getError(0);
	if( solver == 0 ) _s -> addUnitCNF(g -> getVar(), val);
	else if( solver == 1 ) _candSolver -> addUnitCNF(g -> getCandVar(), val);
}

void
CirMgr::addConstConstraint(CirNet* n, int solver)
{
	assert(solver <= 1);
	if( solver == 0 ) {
	_s -> addUnitCNF(n -> getConst(0) -> getVar(), 0);
	_s -> addUnitCNF(n -> getConst(1) -> getVar(), 1);
	}
	else if( solver == 1 ) {
	_candSolver -> addUnitCNF(n -> getConst(0) -> getCandVar(), 0);
	_candSolver -> addUnitCNF(n -> getConst(1) -> getCandVar(), 1);
	}
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
CirMgr::getItp(const string& name)
{
    string proofName = "itp.tmp";
    _s->_solver->proof->save(proofName.c_str());
    
    // modified for multiple errors...
    _isClauseOn.clear();
    _isClauseOn = _rmOnset[name];
    assert(_isClauseOn.size() == _rmOnset[name].size());
    // modified for multiple errors...
    CirNet* itp = buildItp(proofName);
    
    unlink(proofName.c_str());
    return itp;
}

CirNet*
CirMgr::buildItp(const string& fileName)
{
    Reader rdr;

    map<ClauseId, CirGateV> claItpLookUp;
    vector<ClauseId> usedClause;
    vector<unsigned> clausePos;
    
    // tmp var
    CirNet* ntk = new CirNet("patch");
    CirGateV g;
    CirGateV g1;
    CirGateV g2;
    int i, cid, tmp, idx, tmp_cid;
	string wireName = "w";
    unordered_set<CirGate*> commonGate;
    unordered_map<std::string, CirGate*> createdPi;

	ntk -> createConst(0);
	ntk -> createConst(1);
    CirGateV CONST0 = CirGateV(ntk -> getConst(0), false);
    //CirGateV CONST1 = CirGateV(ntk -> getConst(0), true);
    CirGateV CONST1 = CirGateV(ntk -> getConst(1), false);

    rdr.open(fileName.c_str());
    retrieveProof(rdr, clausePos, usedClause);
    //for(size_t n = 0; n < _isClauseOn.size(); ++n) cerr << _isClauseOn[n];
    //cerr << endl;
    //for(size_t i = 0; i < _isClauseOnDup.size(); ++i) cerr << _isClauseOnDup[i];
    //cerr << endl;
    //cerr << "usedClause: " << usedClause.size();
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

                   // modified by mlllk
                   // make patch totally indepedent with _F
                   std::string piName = _var2Gate.find(idx >> 1) -> second -> getName();
                   if( createdPi.find(piName) == createdPi.end() ) {
                      // the createdPi type is Gate_Buf, which can be changed afterwards
                      CirGate* patchPi = ntk -> createGate(Gate_Pi, piName);
                      // clone the key info 
                      patchPi -> swapRemovedGateAndCloneBestGateName(_var2Gate.find(idx >> 1) -> second);
                      createdPi.insert({piName, patchPi});
                      g = CirGateV(patchPi, false);
                   }
                   else g = CirGateV(createdPi.at(piName), false);
                    /* g = CirGateV((_var2Gate.find(idx >> 1))->second, false); */
                   // end of modification
                    g1 = g;

                    commonGate.insert(g.getGate());
                    //g1 = (_var2Gate.find(idx >> 1))->second;
                    if((idx & 1) == 1) {

                        //std::cout << "need to flip inv: " << std::endl;
                        //std::cout << g.getGate() << "   " << g1.getGate() << std::endl;
                        std::string name = wireName + myToString(_patchGateCount);
                        _patchGateCount++;
                        g = CirGateV(ntk->createGate(Gate_Inv, name), false);
                        /* std::cout << "creating inv: " << g.getGate() -> getName() << std::endl; */

                        // modified by mlllk
                        g.getGate()->pushBackFanin(g1);
                        /* for(int j = 0; j < g1.getGate() -> getFanoutSize(); j++) { */
                        /*     //cout << j << " " << g1.getGate() -> getFanout(j) -> getName() << endl; */
                        /*     CirGateV tmpG = CirGateV(g1.getGate() -> getFanout(j)); */
                        /*     //cout << "tmpG: " << tmpG.getGate() << endl; */
                        /*     g.getGate() -> pushBackFanout(tmpG); */
                        /* } */
                        /* g1.getGate() -> clearFanout(); */
                        g1.getGate() -> pushBackFanout(g);
                        /* assert(g1.getGate()->getFanoutSize() == 1); */
                        g1 = g;
                        // end of modification

					    // g.flipInv();
						// g1.flipInv();
                    }
                    while(1) {
                        tmp = rdr.get64();
                        if(tmp == 0) break;
                        idx += tmp;
                        if(_varGroup[idx >> 1] == COMMON) {

                            // modified by mlllk
                            // make patch totally indepedent with _F
                            std::string piName = _var2Gate.find(idx >> 1) -> second -> getName();
                            if( createdPi.find(piName) == createdPi.end() ) {
                               // the createdPi type is Gate_Buf, which can be changed afterwards
                               CirGate* patchPi = ntk -> createGate(Gate_Pi, piName);
                               patchPi -> swapRemovedGateAndCloneBestGateName(_var2Gate.find(idx >> 1) -> second);
                               createdPi.insert({piName, patchPi});
                               g2 = CirGateV(patchPi, false);
                            }
                            else g2 = CirGateV(createdPi.at(piName), false);
                            /* g2 = CirGateV((_var2Gate.find(idx >> 1))->second, false); */
                            // end of modification

                            commonGate.insert(g2.getGate());
                            if((idx & 1) == 1) {
                                
                                //std::cout << "need to flip g2" << endl;
                                std::string name = wireName + myToString(_patchGateCount);
                                _patchGateCount++;
                                CirGateV invG = CirGateV(ntk->createGate(Gate_Inv, name), false);
                                /* std::cout << "creating inv: " << invG.getGate() -> getName() << std::endl; */ 

                        // modified by mlllk
                                invG.getGate()->pushBackFanin(g2);
                                /* for(int j = 0; j < g2.getGate() -> getFanoutSize(); j++) { */
                                /*     CirGateV tmpG = CirGateV(g2.getGate() -> getFanout(j)); */
                                /*     invG.getGate() -> pushBackFanout(tmpG); */
                                /* } */
                                /* g2.getGate() -> clearFanout(); */
                                g2.getGate() -> pushBackFanout(invG);
                                /* assert(g2.getGate()->getFanoutSize() == 1); */
                                g2 = invG;
                         // end of modification
							    // g2.flipInv();

                            }
                            // or
                            //cerr << "created OR gate w(" << w << ")"  << endl; // for debug
							std::string name = wireName + myToString(_patchGateCount);
                            _patchGateCount++;
                            g = CirGateV(ntk->createGate(Gate_Or, name), false);
                            g.getGate()->pushBackFanin(g1);
                            g.getGate()->pushBackFanin(g2);
                            g1.getGate() -> pushBackFanout(g);
                            g2.getGate() -> pushBackFanout(g);
                            //wireGate.insert(g);
                            g1 = g;
                        }
                    }
                } else {
                    g = CONST0;
                }
                claItpLookUp[cid] = g;
            } else {
                claItpLookUp[cid] = CONST1;
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
                        if((g1 == CONST1) || (g2 == CONST1)) {
                            g = CONST1;
                            g1 = g;
                        } else if(g1 == CONST0) {
                            g = g2;
                            g1 = g;
                        } else if(g2 == CONST0) {
                            g = g1; // wtf in SoCV here???
							g1 = g;
                        } else {
                            // or
                            //cerr << "created OR gate w(" << w << ")"  << endl; // for debug
							std::string name = wireName + myToString(_patchGateCount);
							_patchGateCount++;
                            g = CirGateV(ntk->createGate(Gate_Or, name), false);
                            g.getGate()->pushBackFanin(g1);
                            g.getGate()->pushBackFanin(g2);
							g1.getGate() -> pushBackFanout(g);
							g2.getGate() -> pushBackFanout(g);
                            //wireGate.insert(g);
                            g1 = g;
                        }
                    } else { // build AND gate
                        if((g1 == CONST0) || (g2 == CONST0)) {
                            g = CONST0;
                            g1 = g;
                        } else if(g1 == CONST1) {
                            g = g2;
                            g1 = g;
                        } else if(g2 == CONST1) {
                            g = g1;
							g1 = g;
                        } else {
                            // and
                            //cerr << "created AND gate w(" << w << ")"  << endl; // for debug
							std::string name = wireName + myToString(_patchGateCount);
							_patchGateCount++;
                            g = CirGateV(ntk->createGate(Gate_And, name), false);
                            g.getGate()->pushBackFanin(g1);
                            g.getGate()->pushBackFanin(g2);
							g1.getGate() -> pushBackFanout(g);
							g2.getGate() -> pushBackFanout(g);
                            //wireGate.insert(g);
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
    ntk -> pushBackPOList(g.getGate()); // add po to _poList
	//cout << "po of the itp circuit: " << g.getGate()->getName() << endl;
	//cout << "reporting ITP..." << endl;
	//GateList topoList = ntk->buildTopoList(); // construct _topoList
    //for( unsigned i = 0; i < ntk -> getGateNum(); ++i ) {
    //    cerr << ntk -> getGate(i) -> getName() << endl;
    //}
/*
    std::cout << "before modification" << std::endl;
    ntk -> reportPi();
    std::cout << "patch before modification" << std::endl;
    ntk -> reportNetList();
*/
   for(std::unordered_set<CirGate*>::iterator it = commonGate.begin(); it != commonGate.end(); ++it) {
      // modified by mlllk
      // add external inv to fit the smallest weight during 'removeInvBuf()'
      CirGate* g = *it;

      if( g -> getBestGateName().size() ) {
         std::string target = g -> getBestGateName();
         // delete g in _piList
         ntk -> deletePI(g);
         // create a dummy gate for this newly patch pi
         CirGate* dummy = ntk -> createGate(Gate_Pi, target);
         // count the inv number, wrong method !!!
         unsigned invCount = searchInvCount(g -> getBestGateName(), g -> getName());
         /* for( unsigned i = 0; i < g -> getRemovedGateSize(); ++i ) { */
         /*    std::pair<GateType, std::string> typeNamePair = g -> getTypeNamePair(i); */
         /*    std::cout << "type: " << typeNamePair.first << std::endl; */
         /*    std::cout << "name: " << typeNamePair.second << std::endl; */
         /*    if( typeNamePair.first == Gate_Inv ) { */
         /*       ++invCount; */
         /*    } */
         /*    if( typeNamePair.second == g -> getBestGateName() ) break; */
         /* } */
         if( invCount % 2 ) {
            // insert inv for gate named 'wX'
            for( unsigned i = 0; i < g -> getFanoutSize(); ++i ) {
               CirGate* fanout = g -> getFanout(i);
               /* if( fanout -> getName()[0] != 'w' ) continue; */
               assert(fanout -> getName()[0] == 'w');
               /* std::cout << fanout -> getName() << std::endl; */
               unsigned idx = (fanout -> getFanin(0) == g ? 0 : 1);
               assert(fanout -> getFanin(idx) == g);
               // insert inv between fanout and g
               std::string name = wireName + myToString(_patchGateCount);
               _patchGateCount++;
               CirGate* inv = ntk -> createGate(Gate_Inv, name + "_modified");
               inv -> pushBackFanout(CirGateV(fanout, false));
               fanout -> setFanin(CirGateV(inv, false), idx);
               /* inv -> pushBackFanin(CirGateV(g, false)); */
               /* g -> setFanout(CirGateV(inv, false), i); */
               // connect to dummy
               inv -> pushBackFanin(CirGateV(dummy, false));
               dummy -> pushBackFanout(CirGateV(inv, false));
            }
         }
         else {
            // modify the connection between g/fanout
            for( unsigned i = 0; i < g -> getFanoutSize(); ++i ) {
               CirGate* fanout = g -> getFanout(i);
               assert(fanout -> getName()[0] == 'w');
               unsigned idx = (fanout -> getFanin(0) == g ? 0 : 1);
               assert(fanout -> getFanin(idx) == g);
               // connect to dummy
               fanout -> setFanin(CirGateV(dummy, false), idx);
               dummy -> pushBackFanout(CirGateV(fanout, false));
            }
         }
      delete g;
      }
      /* else ntk -> pushBackPIList(*it); */
         // end of modification
   }

    //cout << endl;
    // FIXME: paste patch should be done outside this function
    /* CirGate* po = _F->getError(0); */
	/* //std::cout << "itp out: " << g -> getName() << std::endl; */
	/* unsigned gSize = g.getGate()->getFanoutSize(); */
    /* g.getGate()->setFanoutSize(gSize + 1); */
	/* g.getGate()->setFanout(CirGateV(po), gSize); */
	/* po->setFaninSize(1); */
	/* po->setFanin(g, 0); */
    // below are some old code may be deleted 08/22
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
            //cerr << (_isClauseOn[cid] ? "A":"B") << " ";
            //cerr << "R" << cid << ": ";
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

void
CirMgr::initCandSolver()
{
	assert(_F); assert(_dupF); assert(_G); assert(_dupG);
	// INPORTANT !! we first create var for candidates
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		Var v = _candSolver -> newVar();
		_sortedCandGate[i] -> setCandVar(v);
	}
	_F -> createVar(_candSolver, 1); _G -> createVar(_candSolver, 1);
	_dupF -> createVar(_candSolver, 1); _dupG -> createVar(_candSolver, 1);
	std::cout << "# vars in cand solver: " << _candSolver -> nVars() << std::endl;
}

void
CirMgr::setUpImpVar()
{
	Lit impLit, FLit, dupFLit;
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		Var v = _candSolver -> newVar();
		_sortedCandGate[i] -> setImpVar(v);
		impLit = mkLit(v, false);
		FLit = mkLit(_sortedCandGate[i] -> getCandVar(), false);
		dupFLit = mkLit(_dupF -> getGateByName(_sortedCandGate[i] -> getName()) -> getCandVar(), false);
		_candSolver -> addTernary(~impLit, ~FLit, dupFLit);
		_candSolver -> addTernary(~impLit, FLit, ~dupFLit);
	}
	//std::cout << "curr # clauses: " << _candSolver -> getNumClauses() << std::endl;
}

void
CirMgr::addAllToCandSolver()
{
	_F -> addToSolver(_candSolver, 1); _G -> addToSolver(_candSolver, 1);
	_dupF -> addToSolver(_candSolver, 1); _dupG -> addToSolver(_candSolver, 1);
	//std::cout << "curr # clauses: " << _candSolver -> getNumClauses() << std::endl;
}

void
CirMgr::assumeCut(vector<Lit>& Lit_vec_origin)
{
   Var v;
	Lit_vec_origin.clear();
	_candSolver -> assumeRelease();

// TODO
   v = _F -> getGateByName("g1") -> getImpVar();
	assert(v);
	_candSolver -> assumeProperty(v, false);
   Lit_vec_origin.push_back(mkLit(v, false));
   v = _F -> getGateByName("g2") -> getImpVar();
	assert(v);
	_candSolver -> assumeProperty(v, false);
   Lit_vec_origin.push_back(mkLit(v, false));
}

void
CirMgr::assumeCut(idxVec& cutIdx, std::vector<Lit>& Lit_vec_origin)
{
   Var v;
	Lit_vec_origin.clear();
	_candSolver -> assumeRelease();

	for( unsigned i = 0;  i < cutIdx.size(); ++i ) {
		//std::cout << "assuming " << _sortedCandGate[cutIdx[i]] -> getName() << std::endl;
		v = _sortedCandGate[cutIdx[i]] -> getImpVar(); assert(v);
		_candSolver -> assumeProperty(v, false);
		Lit_vec_origin.push_back(mkLit(v, false));
	}
}

void
CirMgr::setUpCandSolver()
{
	tiePi(_F, _G, 1);
	tiePi(_dupF, _dupG, 1);
	setUpImpVar();
	addAllToCandSolver();
	addXorConstraint(_F, _G, 1);
	//addConstConstraint(_F, 1);
	//addConstConstraint(_G, 1);
	addErrorConstraint(_F, 0, 1);
	addXorConstraint(_dupF, _dupG, 1);
	//addConstConstraint(_dupF, 1);
	//addConstConstraint(_dupG, 1);
	addErrorConstraint(_dupF, 1, 1);
}

void
CirMgr::generatePatch()
{
    clock_t start = clock();
    createVarAll();
    if( _debug ) {
        std::cout << "var of each gate: " << std::endl;
        const GateList& topo = _F -> totGateList();
        //const GateList& topo = _F -> buildTopoList();
        for( unsigned i = 0; i < topo.size(); ++i )
            std::cout << topo[i] -> getName() << "(" << topo[i] -> getVar() << ") ";
        std::cout << std::endl;
        const GateList& topo1 = _G -> totGateList();
        for( unsigned i = 0; i < topo1.size(); ++i )
            std::cout << topo1[i] -> getName() << "(" << topo1[i] -> getVar() << ") ";
        std::cout << std::endl;
        const GateList& topo2 = _dupF -> totGateList();
        for( unsigned i = 0; i < topo2.size(); ++i )
            std::cout << topo2[i] -> getName() << "(" << topo2[i] -> getVar() << ") ";
        std::cout << std::endl;
        const GateList& topo3 = _dupG -> totGateList();
        for( unsigned i = 0; i < topo3.size(); ++i )
            std::cout << topo3[i] -> getName() << "(" << topo3[i] -> getVar() << ") ";
        std::cout << std::endl;
    }
    std::cout << "# vars in patch solver: " << _s -> nVars() << std::endl;
	
	assert(_F -> getPiNum() == _dupF -> getPiNum());
	tiePi(_F, _G);
	assert(_dupF -> getPiNum() == _dupG -> getPiNum());
	//tiePi(_dupF, _dupG);

	// for PI test
	tiePi(_F, _dupF);
	tiePi(_F, _dupG);
	for( unsigned i = 0;  i < _F -> getPiNum(); ++i ) {
		buildVarMap(_F -> getPi(i));
	}


// TODO
/*
	tieGate(_F -> getGateByName("g1"), _dupF -> getGateByName("g1"));
	tieGate(_F -> getGateByName("g2"), _dupF -> getGateByName("g2"));
	tieGate(_F -> getGateByName("g3"), _dupF -> getGateByName("g3"));
	tieGate(_F -> getGateByName("y1"), _dupF -> getGateByName("y1"));
   // remember to clear _var2Gate
   _var2Gate.clear();
	buildVarMap(_F -> getGateByName("g1"));
	buildVarMap(_F -> getGateByName("g2"));
	buildVarMap(_F -> getGateByName("g3"));
	buildVarMap(_F -> getGateByName("y1"));
*/

	if( _debug ) {
		std::cout << "report VarMap: " << std::endl;
		VarMap::iterator it;
		for( it = _var2Gate.begin(); it != _var2Gate.end(); ++it ) {
			std::cout << "Var: " << it -> first << ", name: " << it -> second -> getName() << ", " << it -> second << std::endl;
		}
	}

	unsigned numClauses = getNumClauses();
	assert(numClauses == 0);
	addToSolver(_F);
	addToSolver(_G);

	addXorConstraint(_F, _G);
	//addConstConstraint(_F);
	//addConstConstraint(_G);
	addErrorConstraint(_F, 0);

	/********************/
	// mark onset clause 
	/*******************/
	for( unsigned i = numClauses; i < getNumClauses(); ++i ) markOnsetClause(i);
	assert(_isClauseOn.size() == getNumClauses());
	if( _debug ) {
		std::cout << "after markON" << std::endl;
		for( unsigned i = 0; i < _isClauseOn.size(); ++i ) {
			std::cout << _isClauseOn[i];
		}
		std::cout << std::endl;
	}

	numClauses = getNumClauses();
	addToSolver(_dupF);
	addToSolver(_dupG);
	addXorConstraint(_dupF, _dupG);
	//addConstConstraint(_dupF);
	//addConstConstraint(_dupG);
	addErrorConstraint(_dupF, 1);

	/********************/
	// mark offset clause 
	/*******************/
	for( unsigned i = numClauses; i < getNumClauses(); ++i) markOffsetClause(i);
	assert(_isClauseOn.size() == getNumClauses());
	if( _debug ) {
		std::cout << "after markOFF" << std::endl;
		for( unsigned i = 0; i < _isClauseOn.size(); ++i ) {
			std::cout << _isClauseOn[i];
		}
		std::cout << std::endl;
	}
	std::cout << "# clauses in patch solver: " << _s -> getNumClauses() << std::endl;

	_s -> simplify();
	bool isSat = solve();
	cout << (isSat ? "SAT" : "UNSAT") << endl;
	std::cout << "time: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
	if( isSat ) return;
   std::cout << "generating patch ..." << std::endl;
	_patch = getItp();
	if( _debug ) {
		std::cout << "report patch: " << std::endl;
		_patch -> reportNetList();
		std::cout << "after patching..." << std::endl;
		std::cout << "_F: " << std::endl;
		_F -> reportNetList();
		std::cout << "_G: " << std::endl;
		_G -> reportNetList();
	}
	std::cout << "time: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
/*
// verify patch validity
	_s->reset();
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
*/
}

void
CirMgr::generatePatch(idxVec& cutIdx)
{
	clock_t start = clock();
   createVarAll();
	if( _debug ) {
		std::cout << "var of each gate: " << std::endl;
		const GateList& topo = _F -> totGateList();
		for( unsigned i = 0; i < topo.size(); ++i )
			std::cout << topo[i] -> getName() << "(" << topo[i] -> getVar() << ") ";
		std::cout << std::endl;
		const GateList& topo1 = _G -> totGateList();
		for( unsigned i = 0; i < topo1.size(); ++i )
			std::cout << topo1[i] -> getName() << "(" << topo1[i] -> getVar() << ") ";
		std::cout << std::endl;
		const GateList& topo2 = _dupF -> totGateList();
		for( unsigned i = 0; i < topo2.size(); ++i )
			std::cout << topo2[i] -> getName() << "(" << topo2[i] -> getVar() << ") ";
		std::cout << std::endl;
		const GateList& topo3 = _dupG -> totGateList();
		for( unsigned i = 0; i < topo3.size(); ++i )
			std::cout << topo3[i] -> getName() << "(" << topo3[i] -> getVar() << ") ";
		std::cout << std::endl;
	}
	std::cout << "# vars in patch solver: " << _s -> nVars() << std::endl;
	
// TODO
	// remember to clear _var2Gate
	_var2Gate.clear();
	for( unsigned i = 0; i < cutIdx.size(); ++i ) {
		std::string name = _sortedCandGate[cutIdx[i]] -> getName();
		tieGate(_F -> getGateByName(name), _dupF -> getGateByName(name));
		buildVarMap(_F -> getGateByName(name));
	}

/*
	tieGate(_F -> getGateByName("a"), _dupF -> getGateByName("a"));
	tieGate(_F -> getGateByName("b"), _dupF -> getGateByName("b"));
	tieGate(_F -> getGateByName("c"), _dupF -> getGateByName("c"));
	_var2Gate.clear();
	buildVarMap(_F -> getGateByName("a"));
	buildVarMap(_F -> getGateByName("b"));
	buildVarMap(_F -> getGateByName("c"));
*/
	assert(_F -> getPiNum() == _dupF -> getPiNum());
	tiePi(_F, _G);
	assert(_dupF -> getPiNum() == _dupG -> getPiNum());
	tiePi(_dupF, _dupG);

	if( _debug ) {
		std::cout << "report VarMap: " << std::endl;
		VarMap::iterator it;
		for( it = _var2Gate.begin(); it != _var2Gate.end(); ++it ) {
			std::cout << "Var: " << it -> first << ", name: " << it -> second -> getName() << ", " << it -> second << std::endl;
		}
	}

	unsigned numClauses = getNumClauses();
	assert(numClauses == 0);
	addToSolver(_F);
	addToSolver(_G);

	addXorConstraint(_F, _G);
	//addConstConstraint(_F);
	//addConstConstraint(_G);
	addErrorConstraint(_F, 0);

	/********************/
	// mark onset clause 
	/*******************/
	for( unsigned i = numClauses; i < getNumClauses(); ++i ) markOnsetClause(i);
	assert(_isClauseOn.size() == getNumClauses());
	if( _debug ) {
		std::cout << "after markON" << std::endl;
		for( unsigned i = 0; i < _isClauseOn.size(); ++i ) {
			std::cout << _isClauseOn[i];
		}
		std::cout << std::endl;
	}

	numClauses = getNumClauses();
	addToSolver(_dupF);
	addToSolver(_dupG);
	addXorConstraint(_dupF, _dupG);
	//addConstConstraint(_dupF);
	//addConstConstraint(_dupG);
	addErrorConstraint(_dupF, 1);

	/********************/
	// mark offset clause 
	/*******************/
	for( unsigned i = numClauses; i < getNumClauses(); ++i) markOffsetClause(i);
	assert(_isClauseOn.size() == getNumClauses());
	if( _debug ) {
		std::cout << "after markOFF" << std::endl;
		for( unsigned i = 0; i < _isClauseOn.size(); ++i ) {
			std::cout << _isClauseOn[i];
		}
		std::cout << std::endl;
	}
	std::cout << "# clauses in patch solver: " << _s -> getNumClauses() << std::endl;

	_s -> simplify();
	bool isSat = solve();
	cout << (isSat ? "SAT" : "UNSAT") << endl;
	std::cout << "time: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
	if( isSat ) return;
   std::cout << "generating patch ..." << std::endl;
	_patch = getItp();
	/* if( _debug ) { */
		/* std::cout << "report patch: " << std::endl; */
		/* _patch -> reportNetList(); */
      /* std::cout << "PI of patch: " << std::endl; */
      /* _patch -> reportPi(); */
		/* std::cout << "after patching..." << std::endl; */
		/* std::cout << "_F: " << std::endl; */
		/* _F -> reportNetList(); */
		/* std::cout << "_G: " << std::endl; */
		/* _G -> reportNetList(); */
	/* } */
	std::cout << "time: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;

// verify patch validity
	/* std::cerr << "checking patch validity ..." << std::endl; */
	/* _s->reset(); */
	/* // IMPORTANT !! we first create var for candidates */
	/* for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) { */
	/* 	Var v = _s -> newVar(); */
	/* 	_sortedCandGate[i] -> setVar(v); */
	/* } */
	/* createVar(_F); */
	/* createVar(_G); */
	/* tiePi(_F, _G); */
    /* cout << "# cluases: " << _s -> getNumClauses() << endl; */ 
	/* addToSolver(_F); */
	/* addToSolver(_G); */
    /* cout << "# cluases: " << _s -> getNumClauses() << endl; */ 
	/* addXorConstraint(_F, _G); */
   /* //addConstConstraint(_F); */
   /* //addConstConstraint(_G); */
	/* _s -> simplify(); */
	/* bool eqCheck = solve(); */
	/* cout << (eqCheck ? "SAT" : "UNSAT") << endl; */

}

void
CirMgr::UNSATGeneralizationWithUNSATCore(idxVec& cutIdx, std::vector<Lit>& Lit_vec_origin, idxVec& generalizedCut)
{
	generalizedCut.clear();

	std::vector<Lit> Lit_vec_new;
	Lit_vec_new.resize(cutIdx.size());

	assert(cutIdx.size() == Lit_vec_origin.size());
	assert(cutIdx.size() == Lit_vec_new.size());
	for( unsigned i = 0; i < cutIdx.size(); ++i ) {
		Lit_vec_new[i] = Lit(0);
	}

	if( _debug ) {
		std::cout << "conflict size: " << _candSolver -> _solver -> conflict.size() << std::endl;
		for( unsigned i = 0; i < _candSolver -> _solver -> conflict.size(); ++i ) {
			std::cout << var(_candSolver -> _solver -> conflict[i]) << " ";
		}
		std::cout << std::endl;
	}
	
	// find which lit is in unsat core and generalize the cube

 	for (unsigned i = 0; i < Lit_vec_origin.size(); ++i) {
 	  for (int j = 0; j < _candSolver -> _solver -> conflict.size(); ++j) {
 	      if (var(Lit_vec_origin[i]) == var(_candSolver -> _solver -> conflict[j])) {
 	        Lit_vec_new[i] = Lit_vec_origin[i];
 	        break;
			}
 	  }
 	}


	for( unsigned i = 0; i < Lit_vec_new.size(); ++i ) {
		if( Lit_vec_new[i] == Lit(0) ) continue;
		generalizedCut.push_back(cutIdx[i]);
	}

	if( _debug ) {
		std::cout << "generalized cut: " << std::endl;
		for( unsigned i = 0; i < generalizedCut.size(); ++i ) {
			std::cout << _sortedCandGate[generalizedCut[i]] -> getName() << std::endl;
		}
	}

}

void
CirMgr::SATGeneralization(idxVec& generalizedCut)
{
/*
	generalizedCut.clear();
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		Var v = _sortedCandGate[i] -> getCandVar();
		Var dupV = (_dupF -> getGateByName(_sortedCandGate[i] -> getName())) -> getCandVar();
		if( _candSolver -> getAssignment(v) == _candSolver -> getAssignment(dupV) ) {
			generalizedCut.push_back(i);
		}
	}
	return;	
*/
	// more aggressive method
	clock_t start = clock();
	generalizedCut.clear();
	unsigned count = 0;
	unsigned loopCount = 0;
	while( 1 ) {
		++loopCount;
		//std::cout << "loop count: " << loopCount << std::endl;
		if( loopCount > 8 ) {
			//std::cout << "final generalized cut size: " << generalizedCut.size() << std::endl;
			//std::cout << "time for SAT gen: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::flush;
			return;
		}
		bool assignSAT = true;
		idxVec tmpCut;
		_candSolver -> assumeRelease();
		for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
			Var v = _sortedCandGate[i] -> getCandVar();
			Var dupV = (_dupF -> getGateByName(_sortedCandGate[i] -> getName())) -> getCandVar();
			if( _candSolver -> getAssignment(v) == _candSolver -> getAssignment(dupV) ) {
				tmpCut.push_back(i);
				Var impVar = _sortedCandGate[i] -> getImpVar();
				_candSolver -> assumeProperty(impVar, false);
			}
		}
		assert(tmpCut.size() >= generalizedCut.size());
		//std::cout << "size of generalized cut: " << generalizedCut.size() << std::endl;
		//std::cout << "size of tmp cut: " << tmpCut.size() << std::endl;
		if( generalizedCut.size() == tmpCut.size() ) {
			++count;
			//std::cout << "count: " << count << std::endl;
			if( count >= 30 ) {
				std::cout << "final generalized cut size: " << generalizedCut.size() << std::endl;
				std::cout << "time for SAT gen: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::flush;
				//std::cout << "\r" << setw(50) << "\rtime for SAT gen: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::flush;
				return;
			}
		}	
		else {
			count = 0;
			generalizedCut.swap(tmpCut);
			tmpCut.clear();
		}
		
		// block the current assignment
		vec<Lit> clause;
		for( unsigned i = 0; i < _F -> getPiNum(); ++i ) {
			Var v = _F -> getPi(i) -> getCandVar();
			Var dupV = (_dupF -> getGateByName(_F -> getPi(i) -> getName())) -> getCandVar();
			clause.push(mkLit(v, _candSolver -> getAssignment(v)));
			clause.push(mkLit(dupV, _candSolver -> getAssignment(dupV)));
		}
		assert(clause.size() == (_F -> getPiNum()) * 2);
		_candSolver -> addClause(clause);
		clock_t solveStart = clock();
		assignSAT = _candSolver -> assump_solve();
		//std::cout << "time for solve: " << (double)(clock() - solveStart) / CLOCKS_PER_SEC << std::endl;
		if( !assignSAT ) {
			//std::cout << "no more SAT assignment ..." << std::endl;
			//std::cout << "final generalized cut size: " << generalizedCut.size() << std::endl;
			return;
		}
	}
}

void
CirMgr::getCutWithDecisionOrdered(bool zeroFirst, unsigned& bestCost)
{
	idxVec cutIdx;
   idxVec generalizedCut;
	vector<Lit> Lit_vec_origin;
	bool candSAT;
	clock_t startTime = clock();
	while( 1 ) {
		if( (double)(clock() - startTime) / CLOCKS_PER_SEC >= 60 ) {
			// some case still can't find a valid cut in 600s, should set a time limit to change cut generalization method
			std::cout << "\ntime out ..."  << std::endl;
			break;
		}
		if( !getCut(cutIdx, zeroFirst) ) {
			std::cout << "\nall solution space explored ..." << std::endl;
			_allExplored = true;
			break;
		}
		if( _debug ) {	
			std::cout << "current cut: " << std::endl;
			for( unsigned i = 0; i < cutIdx.size(); ++i )
				std::cout << _sortedCandGate[cutIdx[i]] -> getName() << " ";
			std::cout << std::endl;
		}
		assumeCut(cutIdx, Lit_vec_origin);
		_candSolver -> simplify();
		clock_t candStart = clock();
		candSAT = _candSolver -> assump_solve();
		//std::cout << "\r" << setw(50) << "\rtime for solving: " << (double)(clock() - candStart) / CLOCKS_PER_SEC << std::flush; 
		if( candSAT ) {
			//std::cout << "SAT" << "\r" << std::flush;
			SATGeneralization(generalizedCut);			

			/************************/
			// assert everything in cutIdx is in generalizedCut
			for( unsigned i = 0; i < cutIdx.size(); ++i ) {
				bool isFound = false;
				for( unsigned j = 0; j < generalizedCut.size(); ++j ) {
					if( cutIdx[i] == generalizedCut[j] ) { isFound = true; break; }
				}	
				assert(isFound);
			}
			// end of assertion
			/************************/
			if( _debug ) {
				std::cout << "SATgeneralized cut: " << std::endl;
				unsigned idx = 0;
				for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
					if( i == (idx >= generalizedCut.size() ? _sortedCandGate.size() : generalizedCut[idx]) ) { ++idx; continue; }
					std::cout << _sortedCandGate[i] -> getName() << " ";
				}
/*
				for( unsigned i = 0; i < generalizedCut.size(); ++i )
					std::cout << _sortedCandGate[generalizedCut[i]] -> getName() << " ";
*/
				std::cout << std::endl;
			}
			addBlockingCut(generalizedCut, true);
		}
		else {
			//std::cout << "UNSAT" << "\r" << std::flush;
	      UNSATGeneralizationWithUNSATCore(cutIdx, Lit_vec_origin, generalizedCut);

			/************************/
			// assert everything in generalizedCut is in cutIdx
			for( unsigned i = 0; i < generalizedCut.size(); ++i ) {
				bool isFound = false;
				for( unsigned j = 0; j < cutIdx.size(); ++j ) {
					if( generalizedCut[i] == cutIdx[j] ) { isFound = true; break; }
				}	
				assert(isFound);
			}
			// end of assertion
			/************************/
			if( _debug ) {
				std::cout << "UNSATgeneralized cut: " << std::endl;
				for( unsigned i = 0; i < generalizedCut.size(); ++i )
					std::cout << _sortedCandGate[generalizedCut[i]] -> getName() << " ";
				std::cout << std::endl;
			}

			unsigned currCost = getCost(generalizedCut);
			//std::cout << "bestCost: " << bestCost << std::endl;
			std::cout << "\r" << setw(50) << "\rcurrCost: " << currCost << std::flush;
			//std::cout << std::endl << "currCost: " << currCost << std::endl;
			addBlockingCut(generalizedCut, false);
			if( currCost < bestCost ) {
				_bestCut.swap(generalizedCut);
				bestCost = currCost;
				assert(getCost(_bestCut) == bestCost);
			}
		}
	}
}

unsigned
CirMgr::searchInvCount(const std::string& bestName, const std::string& gateName)
{
   // count the inv on the path from bestName to gatename in _FF
   CirGate* g = _FF -> getGateByName(gateName); assert(g);
   CirGate* best = _FF -> getGateByName(bestName); assert(best);
   unsigned invCount = 0;
   searchInvCountRec(best, g, invCount);
   return invCount;
}

void
CirMgr::searchInvCountRec(CirGate* g, CirGate* target, unsigned& invCount)
{
   if( g == target ) return;
   assert(g -> getType() == Gate_Inv || g -> getType() == Gate_Buf);
   if( g -> getType() == Gate_Inv ) ++invCount;
   assert(g -> getFaninSize() == 1);
   searchInvCountRec(g -> getFanin(0), target, invCount);
}
