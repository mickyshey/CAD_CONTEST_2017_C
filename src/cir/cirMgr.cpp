// File name [ cirMgr.cpp ]
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>

#include "cir/cirMgr.h"

using namespace std;

void
CirMgr::test()
{
	unsigned numClauses;

	reportCandList();
	_dupF = dupNet(_F);
	_dupG = dupNet(_G);

	createMux4Candidates();

	assert(_F -> getPiNum() == _dupF -> getPiNum() + _candNameList.size());
	_F -> reportNetList();

	createVar(_F);
	createVar(_G);
	createVar(_dupF);
	createVar(_dupG);
	/********************/
	// tie variables
	/*******************/
/*
	assert(_F -> getPiNum() == _G -> getPiNum() + _candNameList.size());
	tiePi(_F, _G);
	tieGate(_F -> getGateByName("g1"), _dupF -> getGateByName("g1"));
	tieGate(_F -> getGateByName("g2"), _dupF -> getGateByName("g2"));
	assert(_dupF -> getPiNum() == _dupG -> getPiNum());
	tiePi(_dupF, _dupG);
	//tiePi(_F, _dupF);
	//tiePi(_F, _dupG);

	numClauses = getNumClauses();
	addToSolver(_F);
	addToSolver(_G);
	addXorConstraint(_F, _G);
	addErrorConstraint(_F, 1);
*/
	/********************/
	// mark onset clause 
	/*******************/
/*
	for( unsigned i = numClauses; i < getNumClauses(); ++i ) markOnsetClause(i);

	numClauses = getNumClauses();
	addToSolver(_dupF);
	addToSolver(_dupG);
	addXorConstraint(_dupF, _dupG);
	addErrorConstraint(_dupF, 0);
*/
	/********************/
	// mark onset clause 
	/*******************/
/*
	for( unsigned i = numClauses; i < getNumClauses(); ++i) markOffsetClause(i);

	bool isSat = solve();
	cout << (isSat ? "SAT" : "UNSAT") << endl;
	//_G -> reportGateAll();
	//_dupG -> reportGateAll();
	//_F -> reportNetList();
	//_F -> addToSolver(_s);
*/
}

void
CirMgr::createMux4Candidates()
{
	assert(_F && _G && _dupF && _dupG && _candNameList.size());
	for( unsigned i = 0; i < _candNameList.size(); ++i ) {
		const string& name = _candNameList[i];
		cout << "muxing: " << name << endl;
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
