// File name [ cirReport.cpp ]
#include <iostream>
#include <bitset>

#include "cir/cirMgr.h"

using namespace std;

void
CirNet::reportGateAll() const
{
	cout << "PI:" << endl;
	for( unsigned i = 0; i < _piList.size(); ++i )
		cout << "[" << _piList[i] -> getName() << "], id: " << _piList[i] -> getId() << ", var: " << _piList[i] -> getVar() << " ";
	cout << endl << endl;
	cout << "Error:" << endl;
	for( unsigned i = 0; i < _errorList.size(); ++i )
		cout << "[" << _errorList[i] -> getName() << "], id: " << _errorList[i] -> getId() << ", var: " << _errorList[i] -> getVar() << " ";
	cout << endl << endl;
	cout << "PO:" << endl;
	for( unsigned i = 0; i < _poList.size(); ++i )
		cout << "[" << _poList[i] -> getName() << "], id: " << _poList[i] -> getId() << ", var: " << _poList[i] -> getVar() << ", in0: " << _poList[i] -> getFanin(0) -> getName() << endl;
	cout << endl;
	cout << "Gates:" << endl;
	for( unsigned i = 0; i < _gateList.size(); ++i ) {
        if( _gateList[i] -> getType() == Gate_Buf || _gateList[i] -> getType() == Gate_Inv )
			cout << "[" << _gateList[i] -> getName() << "], id: " << _gateList[i] -> getId() << ", var: " << _gateList[i] -> getVar() << ", in0: " << _gateList[i] -> getFanin(0) -> getName() << endl;
		else {
			CirGate* g = _gateList[i];
			cout << "[" << g -> getName() << "], id: " << g -> getId() << ", var: " << _gateList[i] -> getVar() << ", in0: " << g -> getFanin(0) -> getName() << ", in1: " << g -> getFanin(1) -> getName() << endl;
			/*for( unsigned j = 0; j < g -> getFaninSize(); ++j )
				cout << ", in" << j << ": " << g -> getFanin(j) -> getName();
			cout << endl;*/
		}
	}
	cout << endl;
}

void
CirNet::reportSimV() const
{
	cout << "PI:" << endl;
	for( unsigned i = 0; i < _piList.size(); ++i )
		cout << "[" << _piList[i] -> getName() << "], id: " << _piList[i] -> getId() << ", simV: " << std::bitset<32>(_piList[i] -> getSimV()) << std::endl;
	std::cout << std::endl;
	cout << "Error:" << endl;
	for( unsigned i = 0; i < _errorList.size(); ++i )
		cout << "[" << _errorList[i] -> getName() << "], id: " << _errorList[i] -> getId() << ", simV: " << std::bitset<32>(_errorList[i] -> getSimV()) << std::endl;
	std::cout << std::endl;
	cout << "PO:" << endl;
	for( unsigned i = 0; i < _poList.size(); ++i )
		cout << "[" << _poList[i] -> getName() << "], id: " << _poList[i] -> getId() << ", simV " << std::bitset<32>(_poList[i] -> getSimV()) << ", in0: " << _poList[i] -> getFanin(0) -> getName() << ", cp: " << _poList[i] -> isFaninInv(0) << endl;
	cout << endl;
	cout << "Gates:" << endl;
	for( unsigned i = 0; i < _gateList.size(); ++i ) {
		if( _gateList[i] -> getType() == Gate_Buf || _gateList[i] -> getType() == Gate_Inv )
			cout << "[" << _gateList[i] -> getName() << "], id: " << _gateList[i] -> getId() << ", simV: " << std::bitset<32>(_gateList[i] -> getSimV()) << ", in0: " << _gateList[i] -> getFanin(0) -> getName() << ", cp: " << _gateList[i] -> isFaninInv(0) << endl;
		else {
			CirGate* g = _gateList[i];
			cout << "[" << g -> getName() << "], id: " << g -> getId() << ", simV: " << std::bitset<32>(_gateList[i] -> getSimV()) << ", in0: " << g -> getFanin(0) -> getName() << ", cp: " << g -> isFaninInv(0) << ", in1: " << g -> getFanin(1) -> getName() << ", cp: " << g -> isFaninInv(1) << endl;
			/*for( unsigned j = 0; j < g -> getFaninSize(); ++j )
				cout << ", in" << j << ": " << g -> getFanin(j) -> getName();
			cout << endl;*/
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
	// print out inv
	std::cout << ", fanout size: " << g -> getFanoutSize();
	for( unsigned i = 0; i < g -> getFaninSize(); ++i ) {
		std::cout << ", inv" << i << ": " << g -> isFaninInv(i) << " ";
	}
	// print out address
	//std::cout << ", " << g;
    // print out var
    std::cout << ", " << g -> getVar();
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

void
CirMgr::reportCandList() const
{
	for( unsigned i = 0; i < _candNameList.size(); ++i ) {
		cout << _candNameList[i] << "(" << _F -> getGateByName(_candNameList[i]) -> getWeight() << ") ";
	}
	cout << endl;
}

void
CirMgr::reportSortedCand() const
{
	assert(_sortedCandGate.size() == _candNameList.size());
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		cout << _sortedCandGate[i] -> getName() << "(" << _sortedCandGate[i] -> getWeight() << ") ";
	}
	cout << endl;
}

void
CirMgr::reportMuxAssignment() const
{
	assert(_muxAssignment.size() == _sortedCandGate.size());
	for( unsigned i = 0; i < _muxAssignment.size(); ++i ) {
		cout << _sortedCandGate[i] -> getName() << "[" << _muxAssignment[i] << "] ";
	}
	cout << endl;
}

void
CirMgr::reportFanoutCone(CirGate* g) const
{
	CirGate::incRef();
	reportFanoutConeRec(g, 0);
}

void
CirMgr::reportFanoutConeRec(CirGate* g, unsigned level) const
{
	for( unsigned i = 0; i < level; ++i ) cout << "  ";
	cout << "[" << level << "]";
	g -> report();
	// print out inv
/*
	std::cout << ", fanout size: " << g -> getFanoutSize();
	for( unsigned i = 0; i < g -> getFaninSize(); ++i ) {
		std::cout << ", inv" << i << ": " << g -> isFaninInv(i) << " ";
	}
*/
	// print out address
	//std::cout << ", " << g;
	if( g -> isRef() ) { cout << " (*)" << endl; return; }
	cout << endl;
	g -> setToRef();
	for( unsigned i = 0; i < g -> getFanoutSize(); ++i )
		reportFanoutConeRec(g -> getFanout(i), level + 1);
	
}

void
CirNet::reportPi() const
{
   for( unsigned i = 0; i < getPiNum(); ++i ) {
      std::cout << getPi(i) -> getName() << "  ";
   }
   std::cout << std::endl;
}

void
CirNet::reportPo() const
{
   for( unsigned i = 0; i < getPoNum(); ++i ) {
      std::cout << getPo(i) -> getName() << "  ";
   }
   std::cout << std::endl;
}
