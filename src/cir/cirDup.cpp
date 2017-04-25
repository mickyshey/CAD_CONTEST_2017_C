// File name [ cirDup.cpp ]
#include <iostream>

#include "cir/cirMgr.h"

using namespace std;

CirNet*
CirMgr::dupNet(CirNet* n) const
{
	CirNet* newNet = new CirNet;
	newNet -> setName(n -> getName() + "_dup");
	newNet -> createConst(0);
	newNet -> createConst(1);
	GateList topoList = n -> buildTopoList();
	for( unsigned i = 2; i < topoList.size(); ++i ) {			// 0 for const0, 1 for const1
		CirGate* g = topoList[i];
		CirGate* dupG = newNet -> createGate(g -> getType(), g -> getName(), g -> getId());
		for( unsigned j = 0; j < g -> getFaninSize(); ++j ) {
			CirGate* in = g -> getFanin(j);	
			CirGate* dupIn = newNet -> getGateByName(in -> getName());
			dupG -> pushBackFanin(CirGateV(dupIn, false));		// _in still remains order
			dupIn -> pushBackFanout(CirGateV(dupG,false));		// _out will not remain order
		}
	}
	return newNet;
}
