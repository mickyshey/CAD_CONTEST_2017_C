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
	_dupF = dupNet(_F);
	_dupG = dupNet(_G);
	createVar(_F);
	createVar(_G);
	createVar(_dupF);
	createVar(_dupG);
	tiePi(_F, _G);
	tiePi(_F, _dupF);
	tiePi(_F, _dupG);
	addToSolver(_F);
	addToSolver(_G);
	addToSolver(_dupF);
	addToSolver(_dupG);
	addXorConstraint(_F, _G);
	addXorConstraint(_dupF, _dupG);
	addErrorConstraint(_F, 1);
	addErrorConstraint(_dupF, 0);
	bool isSat = solve();
	cout << (isSat ? "SAT" : "UNSAT") << endl;
	//_G -> reportGateAll();
	//_dupG -> reportGateAll();
	//_F -> reportNetList();
	//_F -> addToSolver(_s);
}

CirNet* out = 0;
/*
CirNet*
CirMgr::miterCkt(CirNet* f, CirNet* g)
{
        ofstream out("miter.v", ios::out);
        out << "module top(";

}*/
