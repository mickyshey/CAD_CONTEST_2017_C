// File name [ cirDup.cpp ]
#include <iostream>

#include "cir/cirMgr.h"

using namespace std;

// the duplicated network stores the same name of each gate
// which can used to reach the gate of the original net from name2GateMap
CirNet*
CirMgr::dupNet(CirNet* n) const
{
	CirNet* newNet = new CirNet;
	newNet -> setName(n -> getName() + "_dup");
	newNet -> createConst(0);
	newNet -> createConst(1);
    GateList totalList = n -> totGateList();
    for( unsigned i = 2; i < totalList.size(); ++i ) {
        CirGate* g = totalList[i];
        CirGate* dupG = newNet -> createGate(g -> getType(), g -> getName(), g -> getId());
         dupG -> reserveFaninSize(g -> getFaninSize());
        for( unsigned j = 0; j < g -> getFaninSize(); ++j ) {
            CirGate* in = g -> getFanin(j);
            CirGate* dupIn = newNet -> getGateByName(in -> getName());
            dupG -> pushBackFanin(CirGateV(dupIn, g -> isFaninInv(j)));
				// we do not maintain the bubble of fanout, still ok so far ...
            dupIn -> pushBackFanout(CirGateV(dupG, false));
        }
    }
    /*
    for( unsigned i = 2; i < topoList.size(); ++i ) {			// 0 for const0, 1 for const1
		CirGate* g = topoList[i];
		CirGate* dupG = newNet -> createGate(g -> getType(), g -> getName(), g -> getId());		// duplicate name and id, also maintain every lists
		for( unsigned j = 0; j < g -> getFaninSize(); ++j ) {
			CirGate* in = g -> getFanin(j);	
			CirGate* dupIn = newNet -> getGateByName(in -> getName());
			dupG -> pushBackFanin(CirGateV(dupIn, false));		// _in still remains order
			dupIn -> pushBackFanout(CirGateV(dupG,false));		// _out will not remain order
		}
	}*/
	return newNet;
}

void
CirMgr::miterNet(CirNet* src, CirNet* target)
{
    GateList totalList = src -> totGateList();
    CirGate* dupG;
    for( unsigned i = 2; i < totalList.size(); ++i ) {
        CirGate* g = totalList[i];
        if(g -> getType() == Gate_Pi) {
            //cerr << g -> getType() << "  " << g -> getName() << endl;
            dupG = target -> getGateByName(g -> getName());
            //cerr << dupG -> getType() << "  " << dupG -> getName() << endl;
        } else {
            dupG = target -> createGate(g -> getType(), g -> getName(), g -> getId());
            dupG -> reserveFaninSize(g -> getFaninSize());
        }
        for( unsigned j = 0; j < g -> getFaninSize(); ++j ) {
            CirGate* in = g -> getFanin(j);
            CirGate* dupIn = target -> getGateByName(in -> getName());
            dupG -> pushBackFanin(CirGateV(dupIn, g -> isFaninInv(j)));
			// we do not maintain the bubble of fanout, still ok so far ...
            dupIn -> pushBackFanout(CirGateV(dupG, false));
        }
    }
    // itp do not create Gate_Po type
    for( unsigned i = 0; i < src -> getPoNum(); ++i ) {
        // cerr << src -> getPo(i) -> getName() << " po name\n";
        CirGate* g = src -> getPo(i);
        CirGate* dupG = target -> getGateByName(g -> getName());
        target -> pushBackPOList(dupG);
    }
    
    /*
    for( unsigned i = 0; i < src -> getPiNum(); ++i ) {
        CirGate* g = src -> getPi(i);
        target -> pushBackPIList(g);
    }
    */
}

CirNet*
CirMgr::connectXor(CirNet* f, CirNet* g)
{
/*
	assert(f -> getPoNum() == g -> getPoNum());
	CirNet* fdup = dupNet(f);
	CirNet* gdup = dupNet(g);
	CirNet* newNet = new CirNet;
	newNet -> setName(f -> getName() + " ^ " + g -> getName());
	newNet -> createConst(0);
	newNet -> createConst(1);
	for( unsigned i = 0; i < fdup -> getPoNum(); ++i ) {
		CirGate* fPo = fdup -> getPo(i) -> getFanin(0);				// get the fanin0 of the po, not po itself
		CirGate* gPo = gdup -> getPo(i) -> getFanin(0);
		CirGate* xorGate = newNet -> createGate(Gate_Xor, fPo -> getName() + " ^ " + gPo -> getName());
		xorGate -> pushBackFanin(CirGateV(fPo, false));
		xorGate -> pushBackFanin(CirGateV(gPo, false));
		fPo -> pushBackFanout(CirGateV(xorGate, false));
		gPo -> pushBackFanout(CirGateV(xorGate, false));
		CirGate* po = newNet -> createGate(Gate_Po, xorGate -> getName() + "_po");
		po -> pushBackFanin(CirGateV(xorGate, false));
		xorGate -> pushBackFanout(CirGateV(po, false));
	}
	assert(newNet -> getPoNum() == f -> getPoNum());
	// should record error, pi
	return NULL;
*/
}
