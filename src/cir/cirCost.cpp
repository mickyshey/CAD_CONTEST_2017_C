// File name [ cirCost.cpp ]
#include <iostream>
#include <algorithm>

#include "cir/cirMgr.h"

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
CirMgr::getCut(idxVec& cutIdx)
{
	cutIdx.clear(); //cutIdx.reserve(_sortedCandGate.size());
	if(_costSolver -> solve()) {
		cout << "get an assignment ..." << endl;
		for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
			Var v = _sortedCandGate[i] -> getCostVar();
			//std::cout << "for var: " << v << ", assignment: " << _costSolver -> getDataValue(v) << std::endl;
			//_muxAssignment[i] = _costSolver -> getDataValue(_sortedCandGate[i] -> getCostVar());
			//std::cout << "for var: " << v << ", assignment: " << _costSolver -> getAssignment(v) << std::endl;
			if( _costSolver -> getAssignment(v) ) cutIdx.push_back(i);
		}
		return true;
	}
	else {
		cout << "no valid assignment ..." << endl;
		return false;
	}
}

void
CirMgr::addBlockingCut(idxVec& cutIdx, bool isSat)
{
	for( unsigned i = 0 ; i < cutIdx.size(); ++i ) {
		for( unsigned j = i + 1; j < cutIdx.size(); ++j ) {
			assert(cutIdx[i] < cutIdx[j]);
		}
	}

	vec<Lit> clause; clause.clear();
	if( isSat ) {
		unsigned idx = 0;
		for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
			if( i == cutIdx[idx] ) { ++idx; continue; }
			std::cout << "adding: " << i << std::endl;
			Var v = _sortedCandGate[i] -> getCostVar();
			Lit l = mkLit(v, false);
			clause.push(l);
		}
	}
	else {
		for( unsigned i = 0; i < cutIdx.size(); ++i ) {
			Var v = _sortedCandGate[cutIdx[i]] -> getCostVar();
			Lit l = mkLit(v, false);
			clause.push(~l);
		}
	}
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
