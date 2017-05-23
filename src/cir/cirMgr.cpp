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
	createVar(_F);
	createVar(_G);
	createVar(_dupF);
	createVar(_dupG);

	/********************/
	// tie variables
	/*******************/
	tiePi(_F, _G);
	tieGate(_F -> getGateByName("g1"), _dupF -> getGateByName("g1"));
	tieGate(_F -> getGateByName("g2"), _dupF -> getGateByName("g2"));
	tiePi(_dupF, _dupG);
	//tiePi(_F, _dupF);
	//tiePi(_F, _dupG);

	addToSolver(_F);
        buildVarMap(_F);
	addToSolver(_G);
        buildVarMap(_G);
	addXorConstraint(_F, _G);
	addErrorConstraint(_F, 1);
	/********************/
	// mark onset clause 
	/*******************/
	numClauses = getNumClauses();
	markOnsetClause(numClauses);

	addToSolver(_dupF);
        buildVarMap(_dupF);
	addToSolver(_dupG);
        buildVarMap(_dupG);
	addXorConstraint(_dupF, _dupG);
	addErrorConstraint(_dupF, 0);
	/********************/
	// mark onset clause 
	/*******************/
	numClauses = getNumClauses();
	markOffsetClause(numClauses);

	bool isSat = solve();
	cout << (isSat ? "SAT" : "UNSAT") << endl;
	//_G -> reportGateAll();
	//_dupG -> reportGateAll();
	//_F -> reportNetList();
	//_F -> addToSolver(_s);
}

//CirNet* out = 0;
/*
CirNet*
CirMgr::miterCkt(CirNet* f, CirNet* g)
{
        ofstream out("miter.v", ios::out);
        out << "module top(";

}*/
