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

	removeInvBuf();
	removeCandFromFanoutCone();

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

	createVar4CostSolver();


/*************************************/
// simulation based cut generalization
/*************************************/

	initCandSolver();
	setUpCandSolver();
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
	unsigned bestCost = 1e10;	

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
      if( loopCount >= 1 ) break;
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
   return;

/*************************************/
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
CirMgr::removeCandFromFanoutCone()
{
	std::unordered_set<std::string> nameHash;
	for( unsigned i = 0; i < _candNameList.size(); ++i ) {
		nameHash.insert(_candNameList[i]);
	}

	// we first deal with single error, directly remove all candidates in fanout cone
	//std::cout << "report fanout cone of error: " << std::endl;
	//reportFanoutCone(_F -> getError(0));

	CirGate::incRef();
	removeCandFromFanoutConeRec(_F -> getError(0), nameHash);
	
	std::vector<std::string> tmpList;
	tmpList.reserve(nameHash.size());
	std::unordered_set<std::string>::iterator it;
	for( it = nameHash.begin(); it != nameHash.end(); ++it ) tmpList.push_back(*it);
	nameHash.clear();
	_candNameList.swap(tmpList);
	tmpList.clear();
}

void
CirMgr::removeCandFromFanoutConeRec(CirGate* g, std::unordered_set<std::string>& nameHash)
{
	if( g -> isRef() ) return;
	g -> setToRef();
	if( nameHash.find(g -> getName()) != nameHash.end() ) {
		//std::cout << "removing " << g -> getName() << std::endl;
		g -> setWeight(0);
		nameHash.erase(nameHash.find(g -> getName()));
	}
	for( unsigned i = 0; i < g -> getFanoutSize(); ++i )
		removeCandFromFanoutConeRec(g -> getFanout(i), nameHash);
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
    for( unsigned i = 0; i < _patch -> getPiNum(); ++i ) { // FIXME: PI is the valid cut we chose
        ofs << _patch -> getPi(i) -> getName() << (i == max ? ");" : ", ");
    }
    ofs << endl;
    // write input
    ofs << "input ";
    for( unsigned i = 0; i < _patch -> getPiNum(); ++i ) { // FIXME
        ofs << _patch -> getPi(i) -> getName() << (i == max ? ";" : ", ");
    }
    ofs << endl;
    // write output
    ofs << "output " << _patch -> getPo(0) -> getName() << ";" << endl;
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
    ofs << "." << _patch -> getPo(0) -> getName() << "(" << _F -> getError(0) -> getName() << "), ";
    max = _patch -> getPiNum() - 1;
    for(unsigned i = 0; i < _patch -> getPiNum(); ++i ) {
        ofs << "." << _patch -> getPi(i) -> getName() << "(" << _patch -> getPi(i)  -> getName() << ")" << (i == max ? ");" : ", ");
    }
    ofs << endl;
    ofs << "endmodule";
    ofs.close();
}
