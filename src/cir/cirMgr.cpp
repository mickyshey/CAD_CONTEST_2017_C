// File name [ cirMgr.cpp ]
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cassert>
#include <cmath>

#include "cir/cirMgr.h"

using namespace std;

void
CirMgr::test()
{
	//if( proveEQ( _F, _G ) ) std::cout << "EQ " << std::endl;
	//else std::cout << "nonEQ" << std::endl;
	//return ;

    _dupF = dupNet(_F);
    _dupG = dupNet(_G);
	
    // cerr << "### debug report _F ###" << endl;
     //_F -> reportNetList();
		//_G -> reportNetList();
    // cerr << "### debug report _dupF ###" << endl;
     //_dupF -> reportNetList();
		//_dupG -> reportNetList();

	/********************/
	// tie variables
	/*******************/

	//createMux4Candidates();
	//assert(_F -> getPiNum() == _dupF -> getPiNum() + _candNameList.size());
	//sortCandidate();

	//std::cout << "after create Mux " << std::endl;
	//_F -> reportNetList();	
    //cerr << "### debug report sorted _candList ###" << endl;
	//reportSortedCand();

	std::vector<bool> assign(_candNameList.size(), false);
	_blockingClauses.push_back(assign);
	assert(_blockingClauses.size() == 1);
//	modified to test
/*
	assign[4] = true;
	_blockingClauses.push_back(assign);
	assign[4] = false; assign[6] = true;
	_blockingClauses.push_back(assign);
	assign[5] = true;
	_blockingClauses.push_back(assign);
	assign[5] = false; assign[3] = true;
	_blockingClauses.push_back(assign);
	assign[3] = false; assign[6] = false;
	assign[4] = true; assign[5] = true;
	_blockingClauses.push_back(assign);
*/	
// end of modification
	//unsigned totalCost = getTotalCost();
	//if(getMuxAssignment()) reportMuxAssignment();

	// Var should be created right here !!
    createVar(_F);
	const GateList& topo = _F -> buildTopoList();
	for( unsigned i = 0; i < topo.size(); ++i )
		std::cout << topo[i] -> getName() << "(" << topo[i] -> getVar() << ") ";
	std::cout << std::endl;
    createVar(_G);
	const GateList& topo1 = _G -> buildTopoList();
	for( unsigned i = 0; i < topo1.size(); ++i )
		std::cout << topo1[i] -> getName() << "(" << topo1[i] -> getVar() << ") ";
	std::cout << std::endl;
    createVar(_dupF);
	const GateList& topo2 = _dupF -> buildTopoList();
	for( unsigned i = 0; i < topo2.size(); ++i )
		std::cout << topo2[i] -> getName() << "(" << topo2[i] -> getVar() << ") ";
	std::cout << std::endl;
    createVar(_dupG);
	const GateList& topo3 = _dupG -> buildTopoList();
	for( unsigned i = 0; i < topo3.size(); ++i )
		std::cout << topo3[i] -> getName() << "(" << topo3[i] -> getVar() << ") ";
	std::cout << std::endl;
	
	//assert(_F -> getPiNum() == _G -> getPiNum() + _candNameList.size());
	assert(_F -> getPiNum() == _dupF -> getPiNum());
	tieConst(_F, _G);
	tiePi(_F, _G);
	//assert(_dupF -> getPiNum() == _dupG -> getPiNum());
	//tiePi(_dupF, _dupG);
	tiePi(_F, _dupF);
	tieConst(_F, _dupF);
	tiePi(_F, _dupG);
	tieConst(_F, _dupG);
	for( unsigned i = 0; i < topo.size(); ++i )
		std::cout << topo[i] -> getName() << "(" << topo[i] -> getVar() << ") ";
	std::cout << std::endl;
	for( unsigned i = 0; i < topo1.size(); ++i )
		std::cout << topo1[i] -> getName() << "(" << topo1[i] -> getVar() << ") ";
	std::cout << std::endl;
	for( unsigned i = 0; i < topo2.size(); ++i )
		std::cout << topo2[i] -> getName() << "(" << topo2[i] -> getVar() << ") ";
	std::cout << std::endl;
	for( unsigned i = 0; i < topo3.size(); ++i )
		std::cout << topo3[i] -> getName() << "(" << topo3[i] -> getVar() << ") ";
	std::cout << std::endl;

	unsigned numClauses = getNumClauses();
	assert(numClauses == 0);
	addToSolver(_F);
    buildVarMap(_F);
	addToSolver(_G);
    buildVarMap(_G);
	addXorConstraint(_F, _G);
	addErrorConstraint(_F, 0);

	/********************/
	// mark onset clause 
	/*******************/

	for( unsigned i = numClauses; i < getNumClauses(); ++i ) markOnsetClause(i);
	assert(_isClauseOn.size() == getNumClauses());
	std::cout << "after markON" << std::endl;
	for( unsigned i = 0; i < _isClauseOn.size(); ++i ) {
		std::cout << _isClauseOn[i];
	}
	std::cout << std::endl;

	numClauses = getNumClauses();
	addToSolver(_dupF);
    buildVarMap(_dupF);
	addToSolver(_dupG);
    buildVarMap(_dupG);
	addXorConstraint(_dupF, _dupG);
	addErrorConstraint(_dupF, 1);

	/********************/
	// mark offset clause 
	/*******************/

	for( unsigned i = numClauses; i < getNumClauses(); ++i) markOffsetClause(i);
	assert(_isClauseOn.size() == getNumClauses());
	std::cout << "after markOFF" << std::endl;
	for( unsigned i = 0; i < _isClauseOn.size(); ++i ) {
		std::cout << _isClauseOn[i];
	}
	std::cout << std::endl;

	bool isSat = solve();
	cout << (isSat ? "SAT" : "UNSAT") << endl;
	_patch = getItp();
    //_patch -> reportGateAll();
	_patch -> reportNetList();


	std::cout << "after patching..." << std::endl;
	std::cout << "_F: " << std::endl;
	_F -> reportNetList();
	std::cout << "_G: " << std::endl;
	_G -> reportNetList();
	_s->reset();
	createVar(_F);
	createVar(_G);
	tiePi(_F, _G);
	addToSolver(_F);
	addToSolver(_G);
	addXorConstraint(_F, _G);
	bool eqCheck = solve();
	cout << (eqCheck ? "SAT" : "UNSAT") << endl;
	//_G -> reportGateAll();
	//_dupG -> reportGateAll();
	//_F -> reportNetList();
	//_F -> addToSolver(_s);

	//tieGate(_F -> getGateByName("g1"), _dupF -> getGateByName("g1"));
	//tieGate(_F -> getGateByName("g2"), _dupF -> getGateByName("g2"));
/*
	unsigned count = 0;
	while( 1 ) {
		++count;
		_costSolver -> reset();
		if( count == 1) createVar4CostSolver(true);
		else createVar4CostSolver(false);
		// get an assignment, then call tieGate

		addCostConstraint(std::floor(totalCost/_sortedCandGate.size()) * count);
		//addCostConstraint(std::floor(totalCost/3) * count);
		addAllBlockings();
		while( getMuxAssignment() ) {
			std::cout << "current assignment: ";
			reportMuxAssignment();
			std::cout << std::endl;
			addBlockingAssignment(_muxAssignment);
			_blockingClauses.push_back(_muxAssignment);
			_s -> reset();
			createVar(_F);
			createVar(_G);
			createVar(_dupF);
			createVar(_dupG);
			// tie gate with '1' assignment
			tieGateWithAssignment(_muxAssignment);
			addToSolver(_F);
			addToSolver(_G);
			addXorConstraint(_F, _G);
			addErrorConstraint(_F, 1);
		
			addToSolver(_dupF);
			addToSolver(_dupG);
			addXorConstraint(_dupF, _dupG);
			addErrorConstraint(_dupF, 0);

			_s -> simplify();
			_s -> assumeRelease();
			for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
				CirGate* sel = _F -> getPi(_G -> getPiNum() + i);
				//std::cout << "sel name: " << sel -> getName() << std::endl;
				if( _muxAssignment[i] ) _s -> assumeProperty(sel -> getVar(), false);
				else _s -> assumeProperty(sel -> getVar(), true);
			}
			bool isSat = _s -> assump_solve();
			cout << (isSat ? "SAT" : "UNSAT") << endl;
			
			if( !isSat ) return;
			else {
				// restore the tied Gate
				restoreTiedGates();
			}
		}
	}
*/
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
    ofs << "endmodule";
    ofs.close();
}

void
CirMgr::writeToOut(const string& fileName)
{
    std::ofstream ofs(fileName.c_str());
    size_t max = _F -> getPiNum() - 1;
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
