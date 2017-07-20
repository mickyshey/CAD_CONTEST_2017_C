// File name [ cirSim.cpp ]

#include <iostream>
#include <vector>
#include <bitset>
#include <set>

#include "cir/cirMgr.h"

void
CirMgr::knownSim(CirNet* g, CirNet* f) const
{
	assert(g -> getPiNum() == f -> getPiNum()); assert(g -> getPoNum() == f -> getPoNum());
	// tie PI simV
	for( unsigned i = 0; i < f -> getPiNum(); ++i ) {
		g -> getPi(i) -> setSimV(f -> getPi(i) -> getSimV());
	}
	g -> knownSim();
}

void
CirMgr::analyzeVec(std::vector<size_t>& t_1, std::vector<size_t>& t_0, unsigned startIdx)
{
	unsigned endIdx = (startIdx + CUT_CAND >= _sortedCandGate.size() ? _sortedCandGate.size() : startIdx + CUT_CAND);
	std::cout << "checking cand: " << startIdx << " ~ " << endIdx << std::endl;
	// analyze the controlling value of the error's partner, assuming one error only
	assert(_F -> getError(0) -> getFanoutSize() == 1 );
	CirGate* errorOut = _F -> getError(0) -> getFanout(0);
	assert(errorOut); assert(errorOut -> getFaninSize() == 2);
	GateType t = errorOut -> getType();
	std::pair<CirGate*, bool> partner;
	if( t == Gate_And || t == Gate_Nand ) {
		partner.first = (errorOut -> getFanin(0) == _F -> getError(0) ? errorOut -> getFanin(1) : errorOut -> getFanin(0));
		partner.second = false;
	}
	else if( t == Gate_Or || t == Gate_Nor ) {
		partner.first = (errorOut -> getFanin(0) == _F -> getError(0) ? errorOut -> getFanin(1) : errorOut -> getFanin(0));
		partner.second = true;
	}
	else {
		partner.first = NULL; partner.second = false;
	}
	for( unsigned i = 0; i < 32; ++i ) {
		bool goodVec = true;
		size_t filter = 0x1 << i;
		//std::cout << "filter: " << std::bitset<32>(filter) << std::endl;
		// check if the gate of the error fanout gets a controlling value from another fanin
		if( partner.first ) {
			if( (bool)(partner.first -> getSimV() & filter) && partner.second ) continue;
		}
		for( unsigned j = 0; j < _F -> getPoNum(); ++j ) {
			if( (_F -> getPo(j) -> getSimV() & filter) ^ (_G -> getPo(j) -> getSimV() & filter) ) { goodVec = false; break; }
		}
		std::cout << "[" << i << "]: " << goodVec << ", error: " << (bool)(_F -> getError(0) -> getSimV() & filter) << std::endl;
		// construct assignment for each candidate (assume all)
		size_t assignment = 0;
		for( unsigned j = startIdx; j < endIdx; ++j ) {		// startIdx, endIdx is the idx of _sortedCandGate
			if( _sortedCandGate[j] -> getSimV() & filter ) assignment |= (0x1 << j);
		}
		std::cout << "assignment: " << std::bitset<32>(assignment) << std::endl;
		if( goodVec ) {
			// assume single error
			if( _F -> getError(0) -> getSimV() & filter ) t_1.push_back(assignment);
			else t_0.push_back(assignment);
		}
		else {
			if( _F -> getError(0) -> getSimV() & filter ) t_0.push_back(assignment);
			else t_1.push_back(assignment);
		}
	}
}

bool
CirMgr::checkValidCut(std::vector<size_t>& t_1, std::vector<size_t>& t_0)
{
	std::set<size_t> hash;
	// insert all t_1 into hash
	std::cout << "t1 size: " << t_1.size() << std::endl;
	for( unsigned i = 0; i < t_1.size(); ++i ) {
		//std::cout << std::bitset<32>(t_1[i]) << std::endl;
		//if( hash.find(t_1[i]) != hash.end() ) std::cout << "already exists" << std::endl;
		hash.insert(t_1[i]);
	}
	std::cout << "hash size: " << hash.size() << std::endl;
	// check if any assignment in t_0 found in hash
	for( std::set<size_t>::iterator it = hash.begin(); it != hash.end(); ++it ) std::cout << std::bitset<32>(*it) << std::endl;
	std::cout << "t_0: " << std::endl;
	for( unsigned i = 0; i < t_0.size(); ++i ) {
		std::cout << std::bitset<32>(t_0[i]) << std::endl;
		if( hash.find(t_0[i]) != hash.end() ) return false;
	}
	return true;
}
