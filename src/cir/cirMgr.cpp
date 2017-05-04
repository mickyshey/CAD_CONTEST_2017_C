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
	//_dupF = dupNet(_F);
	//_dupF -> reportGateAll();
	//_F -> reportGateAll();
	//_F -> reportNetList();
	_s -> init();
	_F -> addToSolver(_s);
}

CirNet* out = 0;
/*
CirNet*
CirMgr::miterCkt(CirNet* f, CirNet* g)
{
        ofstream out("miter.v", ios::out);
        out << "module top(";

}*/
