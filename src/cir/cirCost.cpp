// File name [ cirCost.cpp ]
#include <iostream>
#include <algorithm>

#include "cir/cirMgr.h"

#define CUT_SIZE 40

using namespace std;

bool sortGateByWeight(CirGate* a, CirGate* b)
{
	return a -> getWeight() < b -> getWeight();
}

void
CirMgr::sortCandidate()
{
	_sortedCandGate.resize(_candNameList.size());
	for( unsigned i = 0; i < _candNameList.size(); ++i ) {
      assert(_F -> getGateByName(_candNameList[i]));
		_sortedCandGate[i] = _F -> getGateByName(_candNameList[i]);
	}
	std::sort(_sortedCandGate.begin(), _sortedCandGate.end(), sortGateByWeight);
}

void
CirMgr::createVar4CostSolver()
{
	vec<Lit> clause; clause.clear();
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		Var v = _costSolver -> newVar();
		_sortedCandGate[i] -> setCostVar(v);

		// block all zero's here !!!
		Lit l = mkLit(v, false);
		clause.push(l);
		//_costVar2Gate.insert( std::pair<Var, CirGate*>(v, _sortedCandGate[i]) );
      if( _debug ) {
		   std::cout << "gate: " << _sortedCandGate[i] -> getName() << ", cost Var: " << _sortedCandGate[i] -> getCostVar() << std::endl;
      }
	}

	_costSolver -> addClause(clause);
}

void
CirMgr::addCostConstraint(unsigned cost)
{
	vec<Lit> clause;	
	cout << "adding cost constraint: <= " << cost << endl;
	std::vector<unsigned> indices;
	indices.push_back(0);
	unsigned currCost = _sortedCandGate[0] -> getWeight();
	while( 1 ) {
		while( currCost <= cost ) {
			if( indices.back() == _sortedCandGate.size() - 1 ) break;
			indices.push_back(indices.back() + 1);
			//std::cout << "pushing back: " << indices.back() << std::endl;
			currCost += _sortedCandGate[indices.back()] -> getWeight();
			//std::cout << "currCost: " << currCost << std::endl;
			if( indices.back() == _sortedCandGate.size() - 1 ) break;
		}
		if( currCost <= cost ) {
			assert(indices.back() == _sortedCandGate.size() - 1);
			if( indices.size() == 1 ) return;
			updateIndices(indices, currCost);
		}
		// add constraint
		else {
			std::cout << "adding: ";
			for( unsigned i = 0; i < indices.size(); ++i ) std::cout << indices[i] << " ";
			std::cout << std::endl;

			clause.clear();
			//std::cout << "pushing: ";
			for( unsigned i = 0; i < indices.size(); ++i ) {
				//std::cout << _sortedCandGate[indices[i]] -> getCostVar() << " ";
				Lit l = mkLit(_sortedCandGate[indices[i]] -> getCostVar(), true);
				clause.push(l);
			}
			//std::cout << std::endl;
			//std::cout << "clause size: " << clause.size() << std::endl;
			_costSolver -> addClause(clause);
			updateIndices(indices, currCost);
		}	
	}
}

// increase indices.back() by 1, and maintain
void
CirMgr::updateIndices(std::vector<unsigned>& indices, unsigned& currCost)
{
	//check if indices.back() == _sortedCandGate.size() - 1	
	if( indices.back() == _sortedCandGate.size() - 1 ) {
		currCost -= _sortedCandGate[indices.back()] -> getWeight();
		indices.pop_back();
		updateIndices(indices, currCost);
		return;
	}
	currCost -= _sortedCandGate[indices.back()] -> getWeight();
	++indices.back();
	currCost += _sortedCandGate[indices.back()] -> getWeight();
}

bool
CirMgr::getCut(idxVec& cutIdx, bool zeroFirst)
{
	cutIdx.clear(); //cutIdx.reserve(_sortedCandGate.size());
	if(_costSolver -> solve(zeroFirst)) {
		//cout << "get an assignment ..." << endl;
		for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
			Var v = _sortedCandGate[i] -> getCostVar();
			if( _costSolver -> getAssignment(v) ) cutIdx.push_back(i);
		}
/*
		if( cutIdx.size() > CUT_SIZE ) {
			idxVec tmpCut; tmpCut.reserve(CUT_SIZE);
			for( unsigned i = 0; i < CUT_SIZE; ++i ) tmpCut.push_back(cutIdx[i]);
			cutIdx.swap(tmpCut);
			tmpCut.clear();
		}
		assert(cutIdx.size() <= CUT_SIZE);
*/
		return true;
	}
	else {
		//cout << "no valid assignment ..." << endl;
		return false;
	}
}

void
CirMgr::addBlockingCut(idxVec& cutIdx, bool isSat)
{
/*
	for( unsigned i = 0 ; i < cutIdx.size(); ++i ) {
		for( unsigned j = i + 1; j < cutIdx.size(); ++j ) {
			assert(cutIdx[i] < cutIdx[j]);
		}
	}
*/
	vec<Lit> clause; clause.clear();
	
	//std::cout << "adding: ";
	if( isSat ) {
		unsigned idx = 0;
		for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
			if( i == (idx >= cutIdx.size() ? _sortedCandGate.size() : cutIdx[idx]) ) { ++idx; continue; }
			Var v = _sortedCandGate[i] -> getCostVar();
			//std::cout << _sortedCandGate[i] -> getName() << " ";
			Lit l = mkLit(v, false);
			clause.push(l);
		}
		assert(cutIdx.size() + clause.size() == _sortedCandGate.size());
	}
	else {
		for( unsigned i = 0; i < cutIdx.size(); ++i ) {
			Var v = _sortedCandGate[cutIdx[i]] -> getCostVar();
			//std::cout << "!" << _sortedCandGate[cutIdx[i]] -> getName() << " ";
			Lit l = mkLit(v, true);
			clause.push(l);
			//clause.push(~l);
		}
		assert(cutIdx.size() == clause.size());
	}

	//std::cout << std::endl;

	_costSolver -> addClause(clause);

}

void
CirMgr::tieGateWithAssignment(const std::vector<bool>& assign)
{
	for( unsigned i = 0; i < _muxAssignment.size(); ++i ) {
		// call tieGate
		if( _muxAssignment[i] ) {
			string name = _sortedCandGate[i] -> getName();
			//std::cout << "tie gate: " << name << std::endl;
			tieGate(_F -> getGateByName(name), _dupF -> getGateByName(name));
		}
	}
}

void
CirMgr::restoreTiedGates()
{
	for( unsigned i = 0; i < _muxAssignment.size(); ++i ) {
		if( _muxAssignment[i] ) {
			assert(_sortedCandGate[i] -> getVar() != _varsDup[i]);
			_sortedCandGate[i] -> setVar(_varsDup[i]);
		}
	}
}

unsigned
CirMgr::getTotalCost()
{
	unsigned cost = 0;
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		cost += _sortedCandGate[i] -> getWeight();
	}
	return cost;
}

unsigned
CirMgr::getCost(idxVec& cutIdx)
{
	unsigned cost = 0;
	for( unsigned i = 0; i < cutIdx.size(); ++i ) {
		cost += _sortedCandGate[cutIdx[i]] -> getWeight();
	}
	return cost;
}

void
CirMgr::addAllBlockings()
{
/*
	for( unsigned i = 0; i < _blockingClauses.size(); ++i ) {
		addBlockingAssignment(_blockingClauses[i]);
	}
*/
}

void
CirMgr::cutTest(idxVec& cutIdx)
{
	std::cout << "making assumption: " << std::endl;
	unsigned idx = 0;
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		Var v = _sortedCandGate[i] -> getCostVar();
		if( i == (idx >= cutIdx.size() ? _sortedCandGate.size() : cutIdx[idx]) ) {
			++idx;
			std::cout << _sortedCandGate[i] -> getName() << " ";
			_costSolver -> assumeProperty(v, false);
		}
		else {
			_costSolver -> assumeProperty(v, true);
		}
	}
	std::cout << std::endl;
	bool cutSAT = _costSolver -> assump_solve();
	std::cout << "cutSAT: " << (cutSAT ? "SAT" : "UNSAT") << std::endl;
	if( cutSAT ) {
		for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
			Var v = _sortedCandGate[i] -> getCostVar();
			if( _costSolver -> getAssignment(v) )
				std::cout << _sortedCandGate[i] -> getName() << " ";
		}
		std::cout << std::endl;
	}
	
}
