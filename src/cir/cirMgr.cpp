// File name [ cirMgr.cpp ]
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <cmath>

#include "cir/cirMgr.h"

using namespace std;

void
CirMgr::test()
{
	_dupF = dupNet(_F);
	_dupG = dupNet(_G);
	createMux4Candidates();
	assert(_F -> getPiNum() == _dupF -> getPiNum() + _candNameList.size());
	sortCandidate();
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
	unsigned totalCost = getTotalCost();
	//if(getMuxAssignment()) reportMuxAssignment();

	assert(_F -> getPiNum() == _G -> getPiNum() + _candNameList.size());
	tiePi(_F, _G);
	assert(_dupF -> getPiNum() == _dupG -> getPiNum());
	tiePi(_dupF, _dupG);


	//tieGate(_F -> getGateByName("g1"), _dupF -> getGateByName("g1"));
	//tieGate(_F -> getGateByName("g2"), _dupF -> getGateByName("g2"));
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
