// File name [ cirMgr.cpp ]
#include <iostream>
#include <iomanip>
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
CirMgr::test()
{

   // modified by mlllk
   // for checking patch validity
   _FF = dupNet(_F);
   for( unsigned i = 0; i < _candNameList.size(); ++i ) {
      _candFF.push_back(_candNameList[i]);
   }
   // end of modification

	removeInvBuf();
	removeCandFromFanoutCone(true); // removePo = true

    _dupF = dupNet(_F);
    _dupG = dupNet(_G);
	
	if( _debug ) {
		std::cout << "report F: " << std::endl;
		_F -> reportNetList();
		std::cout << "report G: " << std::endl;
		_G -> reportNetList();
		std::cout << "report _dupF: " << std::endl;
		_dupF -> reportNetList();
		std::cout << "report _dupG: " << std::endl;
		_dupG -> reportNetList();
	}


	sortCandidate();	// sort by increasing weight

	if( _debug ) {
		std::cout << "report sortedCand: " << std::endl;
		reportSortedCand();
	}



/*************************************/
// simulation based cut generalization
/*************************************/

	initCandSolver();
	setUpCandSolver();
   reduceCandidates();
   std::cout << "biggest weight in cand: " << _sortedCandGate.back() -> getWeight() << std::endl;
	createVar4CostSolver();

	assignmentVec t_1, t_0;
	idxVec candIdx;
	idxVec cutIdx;
   idxVec generalizedCut;
/*
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i )
		std::cout << _sortedCandGate[i] -> getCandVar() << " ";
	std::cout << std::endl;
*/
/*
	randSim(_F);
	knownSim(_G, _F);
	if( _debug ) {
		std::cout << "F simV: " << std::endl;
		_F -> reportSimV();
		std::cout << "G simV: " << std::endl;
		_G -> reportSimV();
	}
	unsigned startIdx = 0;
	addCandIdx(candIdx, startIdx, 10);
	analyzeVec(t_1, t_0, candIdx);
	std::cout << "potential cut size: " << candIdx.size() << std::endl;
	if( _debug ) {
		std::cout << "t1 size: " << t_1.size() << std::endl;
		for( unsigned i = 0 ; i < t_1.size(); ++i ) 
			std::cout << std::bitset<32>(t_1[i]) << std::endl;
		std::cout << "t0 size: " << t_0.size() << std::endl;
		for( unsigned i = 0 ; i < t_0.size(); ++i ) 
			std::cout << std::bitset<32>(t_0[i]) << std::endl;
	}
	if( checkValidCut(t_1, t_0) ) {
		std::cout << "valid cut" << std::endl;
		generalizeCut(t_1, t_0, candIdx, cutIdx);
		std::cout << "generalized Cut: " << std::endl;
		for( unsigned i = 0; i < cutIdx.size(); ++i )
			std::cout << _sortedCandGate[i] -> getName() << std::endl;
	}
	else std::cout << "invalid cut" << std::endl;
*/
/*************************************/

	vector<Lit> Lit_vec_origin;
	bool candSAT;
/*************************************/
// cutSolver cut generalization
/*************************************/
	unsigned bestCost = (_bestCut.size() ? getCost(_bestCut) : 1e10);	
   std::cout << "initialized cost: " << bestCost << std::endl;

	// tiePI
/*
	cutIdx.clear();
   for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		if( _sortedCandGate[i] -> getType() == Gate_Pi ) 
			cutIdx.push_back(i);
	}
	assumeCut(cutIdx, Lit_vec_origin);
	_candSolver -> simplify();
	candSAT = _candSolver -> assump_solve();
	assert(!candSAT);
	std::cout << "cost for tiePi: " << getCost(cutIdx) << std::endl;
	UNSATGeneralizationWithUNSATCore(cutIdx, Lit_vec_origin, generalizedCut);
	addBlockingCut(generalizedCut, false);
	for( unsigned i = 0; i < generalizedCut.size(); ++i ) {
		std::cout << _sortedCandGate[generalizedCut[i]] -> getName() << " ";
	}
	std::cout << std::endl;
	_bestCut.swap(generalizedCut);
	bestCost = getCost(_bestCut);
	std::cout << "cost for tiePi: " << bestCost << std::endl;
*/
/******************/
// for test only
/*
	cutIdx.clear();
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		std::string name = _sortedCandGate[i] -> getName();
		if( name == "g124" || name == "g135" || name == "n321" || name == "n331" )
			cutIdx.push_back(i);
	}
	cutTest(cutIdx);
	return;
*/
/******************/

	std::cout << "size of cand: " << _sortedCandGate.size() << std::endl;
   unsigned loopCount = 0;
   while( 1 ) {
      if( loopCount >= 2 ) break;
      if( _allExplored ) break;
      getCutWithDecisionOrdered(true, bestCost);	// zeroFirst
		std::cout << std::endl << "curr bestCost: " << bestCost << std::endl;
		if( _allExplored ) break;
      getCutWithDecisionOrdered(false, bestCost);	// oneFirst
		std::cout << std::endl << "curr bestCost: " << bestCost << std::endl;
      ++loopCount;
   }
   std::cout << "bestCost: " << getCost(_bestCut) << std::endl;
   generatePatch(_bestCut);
   /* checkValidPatch(); */
   return;

/*************************************/
}

void
CirMgr::testRM()
{
    // modified by mlllk
    // for checking patch validity
    _FF = dupNet(_F);
    for( unsigned i = 0; i < _candNameList.size(); ++i ) {
        _candFF.push_back(_candNameList[i]);
    }
    // end of modification
    removeInvBuf();
    removeCandFromFanoutCone();

    sortCandidate();	// sort by increasing weight

	if( _debug ) {
		std::cout << "report sortedCand: " << std::endl;
		reportSortedCand();
	}

    _s -> reset();
 
    // map var to gate in _F
    _var2Gate.clear();

    unsigned numClauses = _s -> getNumClauses();
    assert( numClauses == 0 );

    _rmOut.clear(); // clear the output var of RM
    // create var for candidate first (modify later?)
    for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
        Var v = _s -> newVar();
        _sortedCandGate[i] -> setVar(v);
    }

    formulaRM(_F, _G, 0);
    
    for( unsigned i = 0; i < _F -> getPiNum(); ++i ) {
        std::string name = _F -> getPi(i) -> getName();
        buildVarMap(_F -> getGateByName(name));
    }

    if(_debug) {
        for( auto it = _var2Gate.begin(); it != _var2Gate.end(); ++it ) {
            cerr << " " << it -> first << ":" << it -> second;
        }
        cerr << endl;
    }

    // add error constraints for the first iteration
    vector<bool> rv; // recitify vector
    for( unsigned i = 0; i < _F -> getErrorNum(); ++i ) {
        CirGate* t = _F -> getError(i);
        Var v = t -> getVar();
        cout << "add error " << t -> getName() << "(" << v << ")" << "...\n";
        _s -> addUnitCNF(v, 0);
        //_s -> assumeProperty(v, 1);
        rv.push_back(false);
    }
    // mark all errors onset
    for( unsigned i = 0; i < _F -> getErrorNum(); ++i ) {
        CirGate* t = _F -> getError(i);
        for( unsigned j = numClauses; j < _s -> getNumClauses(); ++j ) {
            markMultiOnsetClause(j, t -> getName());
        }
    }
    numClauses = _s -> getNumClauses(); // update numClauses
    _rvset.push_back(rv);
    
    if(_debug) {
        // print rv0
        for( unsigned i = 0; i < _rvset.size(); ++i ) {
            vector<bool> tmp = _rvset[i];
            for( unsigned j = 0; j < tmp.size(); ++j ) {
                cout << tmp[j] << " ";
            }
            cout << endl;
        }
    }

    // set up RM solver
    _rmSolver -> reset();
    formulaRM(_F, _G, 1);
    _rmSolver -> simplify();
    
    // start rm loop
    int count = 0;
    while(1) {
        cout << "loop " << count << endl;
        ++count;
        //if( count == 3 ) break;
        std::cout << "# clauses in _s: " << _s -> getNumClauses() << endl;
        //bool result = _s -> assump_solve();
        _s -> simplify();
        bool result = _s -> solve();
        rv.clear(); // clear the previous rv
        if(!result) {
            cerr << "UNSAT: ";
            /*
            vector<bool> internal = _rvset.back();
            for( unsigned i = 0; i < internal.size(); ++i ) {
                cout << internal[i] << " ";
            }
            cout << endl;
            */
            _patch = new CirNet;
            _patch -> setName("patch");
            for( unsigned i = 0; i < _F -> getErrorNum(); ++i ) {
                CirNet* tmpPatch = new CirNet;
                tmpPatch = getItp(_F -> getError(i) -> getName());
                //tmpPatch -> reportPi();
                if( _patch -> getPiNum() == 0 ) {
                    for( unsigned j = 0; j < tmpPatch -> getPiNum(); ++j ) {
                        CirGate* g = tmpPatch -> getPi(j);
                        CirGate* dupG = _patch -> createGate(g -> getType(), g -> getName(), g -> getId());
                    }
                }
                //_patch -> reportPi();
                //tmpPatch -> reportNetList();
                miterNet(tmpPatch, _patch);
            }
            break;
        } else {
            cout << "SAT" << endl;
            cout << "ouput SAT assignment of RM: ";
            for(unsigned i = 0; i < _rmOut.size(); ++i ) {
                std::cout << _s -> getAssignment(_rmOut[i]) << " ";
            }
            cout << endl;
            _rmSolver -> assumeRelease();
            // get sat assignment of input value
            cout << "assume pi: ";
            for( unsigned i = 0; i < _F -> getPiNum(); ++i ) {
                Var v = _F -> getPi(i) -> getRMVar();
                Var sv = _F -> getPi(i) -> getVar(); // v and sv will be the same idx
                bool val = _s -> getAssignment(sv);
                cout << val;
                _rmSolver -> assumeProperty(v, !val);
            }
            cout << endl;
            bool assumeResult = _rmSolver -> assump_solve();
            cout << (assumeResult ? "assumpSAT" : "assumpUNSAT") << endl;
            if(!assumeResult) {
                break;
            } else {
                CirNet* dupF = dupNet(_F);
                CirNet* dupG = dupNet(_G);
                formulaRM(dupF, dupG, 0);
                
                assert(dupF -> getPiNum() == _F -> getPiNum());
                assert(dupG -> getPiNum() == _G -> getPiNum());
                for(unsigned k = 0; k < dupF -> getPiNum(); ++k) {
                     CirGate* g = dupF -> getPi(k);
                     CirGate* g_old = _F -> getGateByName(g -> getName());
                     CirGate* f = dupG -> getGateByName(g -> getName());
                     CirGate* f_old = _G -> getGateByName(g -> getName());
                     //cerr << g -> getName() << "(" << g -> getVar() << ") " << g_old -> getName() << "(" << g_old -> getVar() << endl;
                     assert(g -> getVar() == g_old -> getVar());
                     assert(g_old -> getVar() == f -> getVar());
                     assert(g_old -> getVar() == f_old -> getVar());
                }
                /*
                std::cout << "var of each gate: " << std::endl;
                const GateList& topo = _F -> totGateList();
                for( unsigned i = 0; i < topo.size(); ++i )
                    std::cout << topo[i] -> getName() << "(" << topo[i] -> getVar() << ") ";
                std::cout << std::endl;
                const GateList& topo1 = _G -> totGateList();
                for( unsigned i = 0; i < topo1.size(); ++i )
                    std::cout << topo1[i] -> getName() << "(" << topo1[i] -> getVar() << ") ";
                std::cout << std::endl;
                const GateList& topo2 = dupF -> totGateList();
                for( unsigned i = 0; i < topo2.size(); ++i )
                    std::cout << topo2[i] -> getName() << "(" << topo2[i] -> getVar() << ") ";
                std::cout << std::endl;
                const GateList& topo3 = dupG -> totGateList();
                for( unsigned i = 0; i < topo3.size(); ++i )
                    std::cout << topo3[i] -> getName() << "(" << topo3[i] -> getVar() << ") ";
                std::cout << std::endl;
                */
                // get sat assignment of error
                for( unsigned i = 0; i < _F -> getErrorNum(); ++i ) {
                   CirGate* t = _F -> getError(i);
                   Var v = t -> getRMVar();
                   bool val = _rmSolver -> getAssignment(v);
                   cout << t -> getName() << "(" << val << ") ";
                   CirGate* t_new = dupF -> getGateByName(t -> getName());
                   Var v_new = t_new -> getVar();
                   _s -> addUnitCNF(v_new, val);
                   rv.push_back(val);
                }
                _rvset.push_back(rv);
                cout << endl;
                // mark on/offset clauses
                for( unsigned i = 0; i < _F -> getErrorNum(); ++i ) {
                    CirGate* t = _F -> getError(i);
                    Var v = t -> getRMVar();
                    bool val = _rmSolver -> getAssignment(v);
                    for( unsigned j = numClauses; j < _s -> getNumClauses(); ++j ) {
                        if( val == 0 ) {
                            markMultiOnsetClause(j, t -> getName());
                        } else if( val == 1 ) {
                            markMultiOffsetClause(j, t -> getName());
                        }
                    }
                }
                numClauses = _s -> getNumClauses();
                /*
                // add error constraint with t = v*
                for( unsigned i = 0; i < dupF -> getErrorNum(); ++i ) {
                    CirGate* t = dupF -> getError(i);
                    cout << "add error " << t -> getName() << " var: " << t -> getVar() << " w/ "  << rv[i] << "...\n";
                    Var v = t -> getVar();
                    _s -> addUnitCNF(v, rv[i]);
                }
                */
                //cout << "dupF: \n";
                //dupF -> reportGateAll();
                //_s -> assumeRelease();
                //Var conjuntionV = _s -> newVar();
                //_s -> addAndCNF(conjuntionV, _rmOut);
                //_s -> assumeProperty(conjuntionV, 0);
                _rcset.push_back(dupF);
                _rcset.push_back(dupG);
            }
        }
    }
    return;
    
}

void
CirMgr::createMux4Candidates()
{
	assert(_F && _G && _dupF && _dupG && _candNameList.size());
	for( unsigned i = 0; i < _candNameList.size(); ++i ) {
		const string& name = _candNameList[i];
		//cout << "muxing: " << name << endl;
		CirGate* g_F = _F -> getGateByName(name);
		CirGate* g_dupF = _dupF -> getGateByName(name);
		assert(g_F -> getName() == g_dupF -> getName());
		// the created mux gate should be inserted to _F
		CirGate* mux = _F -> createMux(g_F, g_dupF);
	}
}

//CirNet* out = 0;
/*
CirNet*
CirMgr::miterCkt(CirNet* f, CirNet* g)
{
        ofstream out("miter.v", ios::out);
        out << "module top(";

}*/

void
CirMgr::removeInvBuf()
{
	_F -> removeInvBuf(_candNameList);

	
	// _G could be stored as aig, and utilize fraig to simplify netlist
	std::vector<std::string> tmp;
	_G -> removeInvBuf(tmp);
}

void
CirMgr::removeCandFromFanoutCone(bool removePo)
{
	std::unordered_set<std::string> nameHash;
   GateList tmpPoList;
	for( unsigned i = 0; i < _candNameList.size(); ++i ) {
		nameHash.insert(_candNameList[i]);
	}

	// we first deal with single error, directly remove all candidates in fanout cone
	//std::cout << "report fanout cone of error: " << std::endl;
	//reportFanoutCone(_F -> getError(0));

   /* std::cout << "before:" << std::endl; */
   /* _F -> reportPo(); */
	CirGate::incRef();
	removeCandFromFanoutConeRec(_F -> getError(0), nameHash, tmpPoList);
   if( removePo ) {
      _G -> maintainPoList(tmpPoList);
      _F -> swapPoList(tmpPoList);
   }
   /* std::cout << "after:" << std::endl; */
   /* _F -> reportPo(); */
	
	std::vector<std::string> tmpList;
	tmpList.reserve(nameHash.size());
	std::unordered_set<std::string>::iterator it;
	for( it = nameHash.begin(); it != nameHash.end(); ++it ) tmpList.push_back(*it);
	nameHash.clear();
	_candNameList.swap(tmpList);
	tmpList.clear();
}

void
CirMgr::removeCandFromFanoutConeRec(CirGate* g, std::unordered_set<std::string>& nameHash, GateList& tmpPoList)
{
	if( g -> isRef() ) return;
	g -> setToRef();
	if( nameHash.find(g -> getName()) != nameHash.end() ) {
		//std::cout << "removing " << g -> getName() << std::endl;
		g -> setWeight(0);
		nameHash.erase(nameHash.find(g -> getName()));
	}

   // detect PO
   if( g -> getType() == Gate_Po ) {
      tmpPoList.push_back(g);
   }

	for( unsigned i = 0; i < g -> getFanoutSize(); ++i )
		removeCandFromFanoutConeRec(g -> getFanout(i), nameHash, tmpPoList);
}

void
CirMgr::reduceCandidates()
{
   unsigned size = _sortedCandGate.size();
   if( size < 100 ) return;
	vector<Lit> Lit_vec_origin;
	idxVec tmpCut, generalizedCut;
   unsigned cutSizePerRound = _sortedCandGate.size() / 20;
   std::cout << "size of cand: " << size << std::endl;
   std::cout << "size per round: " << cutSizePerRound << std::endl;
	unsigned cutCount = 0;
	while( 1 ) {
		for( unsigned i = cutCount * cutSizePerRound; i < (cutCount + 1) * cutSizePerRound; ++i ) {
			tmpCut.push_back(i);
		}
		/* std::cout << "tmp size: " << tmpCut.size() << std::endl; */
		assert(tmpCut.size() == (cutCount + 1) * cutSizePerRound);
		assumeCut(tmpCut, Lit_vec_origin);
		_candSolver -> simplify();
		bool candSAT = _candSolver -> assump_solve();
		if( candSAT ) {
			/* SATGeneralization(generalizedCut); */			
			/* addBlockingCut(generalizedCut, true); */
			++cutCount;
		}
		else {
	      UNSATGeneralizationWithUNSATCore(tmpCut, Lit_vec_origin, generalizedCut);
			/* addBlockingCut(generalizedCut, false); */
			_bestCut.swap(generalizedCut);

         std::cout << "before optional addition: " << tmpCut.size() << std::endl;
         // push the same weighted gate into _sortedCandGate (optional)
         unsigned weight = _sortedCandGate[tmpCut.back()] -> getWeight();
         unsigned idx = (cutCount + 1) * cutSizePerRound;
         while( 1 ) {
            if( _sortedCandGate[idx] -> getWeight() != weight ) break;
            tmpCut.push_back(idx);
            ++idx;
         }
         std::cout << "after optional addition: " << tmpCut.size() << std::endl;

         // clear all other weights
         for( unsigned i = idx; i < _sortedCandGate.size(); ++i ) {
            _sortedCandGate[i] -> setWeight(0);
         }

         // swap the tmpCut with _sortedCandGate
         GateList tmpList;
         for( unsigned i = 0; i < tmpCut.size(); ++i ) {
            tmpList.push_back(_sortedCandGate[i]);   
            //tmpList.push_back(_sortedCandGate[tmpCut[i]]);   
         }
         _sortedCandGate.swap(tmpList);
         tmpList.clear();
			break;
		}
	}
}

void
CirMgr::writeToPatch(const string& fileName)
{
    std::ofstream ofs(fileName.c_str());
    size_t max = _patch -> getPiNum() - 1;
    // write header
    ofs << "module patch (";
    for( unsigned i = 0; i < _patch -> getPoNum(); ++i ) {
        ofs << _patch -> getPo(i) -> getName() << ", ";
    }
    for( unsigned i = 0; i < _patch -> getPiNum(); ++i ) { 
        ofs << _patch -> getPi(i) -> getName() << (i == max ? ");" : ", ");
    }
    ofs << endl;
    // write input
    ofs << "input ";
    for( unsigned i = 0; i < _patch -> getPiNum(); ++i ) {
        ofs << _patch -> getPi(i) -> getName() << (i == max ? ";" : ", ");
    }
    ofs << endl;
    // write output
    max = _patch -> getPoNum() - 1;
    ofs << "output ";
    for( unsigned i = 0; i < _patch -> getPoNum(); ++i ) {
        ofs << _patch -> getPo(i) -> getName() << (i == max ? ";" : ", ");
    }
    ofs << endl;
    // write wire
    max = _patch -> getGateNum() - 1;
    ofs << "wire ";
    for( unsigned i = 0; i < _patch -> getGateNum(); ++i ) {
        ofs << _patch -> getGate(i) -> getName() << (i == max ? ";" : ", ");
    }
    ofs << endl;
    // write gate
    //GateList topoList = _patch -> buildTopoList();
    for( unsigned i = 0; i < _patch -> getGateNum(); ++i ) {
       CirGate* g = _patch -> getGate(i);
       switch( g -> getType() ) {
           case Gate_And:
               ofs << "and (" << g -> getName() << ", ";
               ofs << (g->isFaninInv(0) ? "!":"") << g -> getFanin(0) -> getName() << ", " << (g->isFaninInv(1) ? "!":"") << g -> getFanin(1) -> getName() << ");" << endl;
               break;
           case Gate_Buf:
               ofs << "buf(" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ");" << endl;
               break;
           case Gate_Inv:
               ofs << "not(" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ");" << endl;
               break;
           case Gate_Or:
               ofs << "or (" << g -> getName() << ", ";
               ofs << (g->isFaninInv(0) ? "!":"") <<  g -> getFanin(0) -> getName() << ", " << (g->isFaninInv(1) ? "!":"") <<  g -> getFanin(1) -> getName() << ");" << endl;
               break;
           case Gate_Nand:
               ofs << "nand (" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ", " << g -> getFanin(1) -> getName() << ");" << endl;
               break;
           case Gate_Nor:
               ofs << "nor (" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ", " << g -> getFanin(1) -> getName() << ");" << endl;
               break;
           case Gate_Xnor:
               ofs << "xnor (" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ", " << g -> getFanin(1) -> getName() << ");" << endl;
               break;
           case Gate_Xor:
               ofs << "xor (" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ", " << g -> getFanin(1) -> getName() << ");" << endl;
               break;
           default:
               break;
       }
    }
    ofs << "endmodule";
    ofs.close();
}

void
CirMgr::writeToOut(const string& fileName, const string& inpuFile)
{
    std::ofstream ofs(fileName.c_str());
    std::ifstream ifs(inpuFile.c_str()); // read the original F.v
    std::string str;
    std::vector<string> tokens;
    size_t max = _F -> getPiNum() - 1;
    if(ifs.fail()) cerr << "fail to read F.v!!" << endl;
    while(!ifs.eof()) {
        str = readUntil(ifs, ';');
        tokens = split(str, ";\t\n");
        if(!tokens[0].compare("endmodule")) break;
        ofs << str;
    }
    ofs << endl;
    ifs.close();
    /*
    // write header
    ofs << "module top (";
    for( unsigned i = 0; i < _F -> getPoNum(); ++i ) {
        ofs << _F -> getPo(i) -> getName() << ", ";
    }
    for( unsigned i = 0; i < _F -> getPiNum(); ++i ) {
        ofs << _F -> getPi(i) -> getName() << (i == max ? ");" : ", ");
    }
    ofs << endl;
    // write input
    ofs << "input ";
    for( unsigned i = 0; i < _F -> getPiNum(); ++i ) {
        ofs << _F -> getPi(i) -> getName() << (i == max ? ";" : ", ");
    }
    ofs << endl;
    // write output
    ofs << "output ";
    max = _F -> getPoNum() - 1;
    for( unsigned i = 0; i < _F -> getPoNum(); ++i ) {
        ofs << _F -> getPo(i) -> getName() << (i == max ? ";" : ", ");
    }
    ofs << endl;
    // write wire
    ofs << "wire ";
    max = _F -> getGateNum() - 1;
    for( unsigned i = 0; i < _F -> getGateNum(); ++i ) {
        ofs << _F -> getGate(i) -> getName() << (i == max ? ";" : ", ");
    }
    ofs << endl;
    // write error
    ofs << "wire ";
    max = _F -> getErrorNum() - 1;
    for( unsigned i = 0; i < _F -> getErrorNum(); ++i ) {
        ofs << _F -> getError(i) -> getName() << (i == max ? ";" : ", ");
    }
    ofs << endl;
    // write gate list
    GateList totList = _F -> totGateList();
    for( unsigned i = 0; i < totList.size(); ++i ) {
        CirGate* g = totList[i];
        switch( g -> getType() ) {
            case Gate_And:
                ofs << "and (" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ", " << g -> getFanin(1) -> getName() << ");" << endl;
                break;
            case Gate_Buf:
                ofs << "buf(" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ");" << endl;
                break;
            case Gate_Inv:
                ofs << "not(" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ");" << endl;
                break;
            case Gate_Or:
                ofs << "or (" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ", " << g -> getFanin(1) -> getName() << ");" << endl;
                break;
            case Gate_Nand:
                ofs << "nand (" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ", " << g -> getFanin(1) -> getName() << ");" << endl;
                break;
            case Gate_Nor:
                ofs << "nor (" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ", " << g -> getFanin(1) -> getName() << ");" << endl;
                break;
            case Gate_Xnor:
                ofs << "xnor (" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ", " << g -> getFanin(1) -> getName() << ");" << endl;
                break;
            case Gate_Xor:
                ofs << "xor (" << g -> getName() << ", " << g -> getFanin(0) -> getName() << ", " << g -> getFanin(1) -> getName() << ");" << endl;
                break;
            default:
                break;
        }
    }
    */
    // write patch
    // FIXME: only single error, so only one patch...
    ofs << "patch p0 (";
    for( unsigned i = 0; i < _patch -> getPoNum(); ++i ) {
        ofs << "." << _patch -> getPo(i) -> getName() << "(" << _F -> getError(i) -> getName() << "), ";
    }
    //ofs << "." << _patch -> getPo(0) -> getName() << "(" << _F -> getError(0) -> getName() << "), ";
    max = _patch -> getPiNum() - 1;
    for(unsigned i = 0; i < _patch -> getPiNum(); ++i ) {
        ofs << "." << _patch -> getPi(i) -> getName() << "(" << _patch -> getPi(i)  -> getName() << ")" << (i == max ? ");" : ", ");
    }
    ofs << endl;
    ofs << "endmodule";
    ofs.close();
}
