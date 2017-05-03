// File name [ cirMgr.cpp ]
#include <iostream>
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
