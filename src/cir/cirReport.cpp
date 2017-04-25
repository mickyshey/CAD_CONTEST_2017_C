// File name [ cirReport.cpp ]
#include <iostream>

#include "cir/cirMgr.h"

using namespace std;

void
CirNet::reportGateAll() const
{
	cout << "PI:" << endl;
	for( unsigned i = 0; i < _piList.size(); ++i )
		cout << "[" << _piList[i] -> getName() << "], id: " << _piList[i] -> getId() << " ";
	cout << endl << endl;
	cout << "Error:" << endl;
	for( unsigned i = 0; i < _errorList.size(); ++i )
		cout << "[" << _errorList[i] -> getName() << "], id: " << _errorList[i] -> getId() << " ";
	cout << endl << endl;
	cout << "PO:" << endl;
	for( unsigned i = 0; i < _poList.size(); ++i )
		cout << "[" << _poList[i] -> getName() << "], id: " << _poList[i] -> getId() << ", in0: " << _poList[i] -> getFanin(0) -> getName() << endl;
	cout << endl;
	cout << "Gates:" << endl;
	for( unsigned i = 0; i < _gateList.size(); ++i ) {
		if( _gateList[i] -> getType() == Gate_Buf || _gateList[i] -> getType() == Gate_Inv )
			cout << "[" << _gateList[i] -> getName() << "], id: " << _gateList[i] -> getId() << ", in0: " << _gateList[i] -> getFanin(0) -> getName() << endl;
		else {
			CirGate* g = _gateList[i];
			cout << "[" << g -> getName() << "], id: " << g -> getId();
			for( unsigned j = 0; j < g -> getFaninSize(); ++j )
				cout << ", in" << j << ": " << g -> getFanin(j) -> getName();
			cout << endl;
		}
	}
	cout << endl;
}

void
CirNet::reportNetList() const
{
	CirGate::incRef();
	for( unsigned i = 0; i < _poList.size(); ++i )
		reportNetListRec(_poList[i], 0);
}

void
CirNet::reportNetListRec(CirGate* g, unsigned level) const
{
	for( unsigned i = 0; i < level; ++i ) cout << "  ";
	cout << "[" << level << "]";
	g -> report();
	if( g -> isRef() ) { cout << " (*)" << endl; return; }
	cout << endl;
	g -> setToRef();
	for( unsigned i = 0; i < g -> getFaninSize(); ++i )
		reportNetListRec(g -> getFanin(i), level + 1);
}

void
CirNet::reportTopoList() const
{
	for( unsigned i = 0; i < _topoList.size(); ++i ) {
		CirGate* g = _topoList[i];
		cout << g -> getName() << endl;
	}
}
