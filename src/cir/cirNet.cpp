// File name [ cirNet.cpp ]

#include <iostream>

#include "cir/cirNet.h"

using namespace std;

CirNet::~CirNet()
{
	if( _const0 ) delete _const0;
	if( _const1 ) delete _const1;
	for( unsigned i = 0; i < _gateList.size(); ++i )
		delete _gateList[i];
	for( unsigned i = 0; i < _piList.size(); ++i )
		delete _piList[i];
	for( unsigned i = 0; i < _poList.size(); ++i )
		delete _poList[i];
	for( unsigned i = 0; i < _errorList.size(); ++i )
		delete _errorList[i];
}

void
CirNet::init()
{
	if( _const0 ) delete _const0;
	if( _const1 ) delete _const1;
	for( unsigned i = 0; i < _gateList.size(); ++i )
		delete _gateList[i];
	for( unsigned i = 0; i < _piList.size(); ++i )
		delete _piList[i];
	for( unsigned i = 0; i < _poList.size(); ++i )
		delete _poList[i];
	for( unsigned i = 0; i < _errorList.size(); ++i )
		delete _errorList[i];
	_gateList.clear();
	_piList.clear();
	_poList.clear();
	_errorList.clear();
	_name2GateMap.clear();
}

CirGate*
CirNet::createGate(const GateType& t, const string& name, unsigned id)
{
	if( t == Gate_Const ) {							
		CirConstGate* g = new CirConstGate(name, id);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Pi ) {
		CirPiGate* g = new CirPiGate(name, id);
		_piList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Po ) {							
		CirPoGate* g = new CirPoGate(name, id);
		_poList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Buf ) {
		CirBufGate* g = new CirBufGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Inv ) {
		CirInvGate* g = new CirInvGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_And ) {
		CirAndGate* g = new CirAndGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Or ) {
		CirOrGate* g = new CirOrGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Nand ) {
		CirNandGate* g = new CirNandGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Nor ) {
		CirNorGate* g = new CirNorGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Xor ) {
		CirXorGate* g = new CirXorGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Xnor ) {
		CirXnorGate* g = new CirXnorGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Error ) {
		CirErrorGate* g = new CirErrorGate(name, id);
		_errorList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	assert(0);
	return NULL;
}

void
CirNet::createConst(bool val)
{
	if( val ) {
		_const1 = createGate(Gate_Const, "1'b1", 1);
	}
	else _const0 = createGate(Gate_Const, "1'b0", 0);
}

const GateList&
CirNet::buildTopoList() const
{
	CirGate::incRef();
	_topoList.clear();
	_const0 -> setToRef();
	_const1 -> setToRef();
	_topoList.push_back(_const0);		// always 0 at 0, 1 at 1
	_topoList.push_back(_const1);
	for( unsigned i = 0; i < _poList.size(); ++i )
		buildTopoListRec(_poList[i]);
	return _topoList;
}

void
CirNet::buildTopoListRec(CirGate* g) const
{
	if( g -> isRef() ) return;
	g -> setToRef();
	for( unsigned i = 0; i < g -> getFaninSize(); ++i )
		buildTopoListRec(g -> getFanin(i));
	_topoList.push_back(g);
}

void
CirNet::sweep()
{
    /*
        cout << "// Sweeping..." << endl;
        for(size_t i = 0; i < _topoList.size(); i++) {
            CirGate* g = _topoList[i];
            GateType type = g.getType();
            if(type != Gate_Buf && type != Gate_Inv) {
                for(unsigned j = 0; j < g -> getFaninSize(); j++) {
                    bool isInv = false;
                    CirGate* in = g -> getFanin(j);

                }
            }
        }
    */
}
