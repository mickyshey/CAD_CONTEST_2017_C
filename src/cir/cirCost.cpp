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
		_sortedCandGate[i] = _F -> getGateByName(_candNameList[i]);
	}
	std::sort(_sortedCandGate.begin(), _sortedCandGate.end(), sortGateByWeight);
}

void
CirMgr::createVar4CostSolver(bool setMgr)
{
	if( setMgr ) _varsDup.resize(_sortedCandGate.size());
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		Var v = _costSolver -> newVar();
		_sortedCandGate[i] -> setCostVar(v);
		if( setMgr ) _varsDup[i] = v;
		_costVar2Gate.insert( std::pair<Var, CirGate*>(v, _sortedCandGate[i]) );
      if( _debug ) {
		   std::cout << "gate: " << _sortedCandGate[i] -> getName() << ", cost Var: " << _sortedCandGate[i] -> getCostVar() << std::endl;
      }
	}
	_muxAssignment.resize(_sortedCandGate.size());
}

void
CirMgr::addCostConstraint(unsigned cost)
{
	vec<Lit> clause;	
	cout << "adding cost constraint: <= " << cost << endl;
/*
	for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
		unsigned sum = 0;
		for( unsigned j = i; j < _sortedCandGate.size(); ++j ) {
			sum += _sortedCandGate[j] -> getWeight();
			cout << "current sum: " << sum << endl;
			if( sum > cost ) {
				// add constraint to solver
				cout << "exceed cost, add constraint ..." << endl;
				unsigned last = j;
				while( last < _sortedCandGate.size() ) {
					cout << "add constraint: " << i << " ~ " << (j - 1) << " plus: " << last << endl;
					clause.clear();
					// add all var befor the breaking point
					for( unsigned k = i; k <= j - 1; ++k ) {
						Lit l = mkLit(_sortedCandGate[k] -> getCostVar(), false);
						//cout << "var: " << _sortedCandGate[k] -> getCostVar() << ", lit: " << l << endl;
						clause.push(~l);	
					}
					Lit l = mkLit(_sortedCandGate[last] -> getCostVar(), false);
					clause.push(~l);
					_costSolver -> addClause(clause);
					++last;
				}
				break;
			}
			if( j == _sortedCandGate.size() - 1 ) {
				// no more combination of candidates could exceed the cost, return
				cout << "no more combinations could exceed the cost ..." << endl;
				return;
			}
		}
	}
*/
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
CirMgr::getMuxAssignment()
{
	assert(_muxAssignment.size() == _sortedCandGate.size());
	if(_costSolver -> solve()) {
		//cout << "get an assignment ..." << endl;
		for( unsigned i = 0; i < _sortedCandGate.size(); ++i ) {
			Var v = _sortedCandGate[i] -> getCostVar();
			//std::cout << "for var: " << v << ", assignment: " << _costSolver -> getDataValue(v) << std::endl;
			//_muxAssignment[i] = _costSolver -> getDataValue(_sortedCandGate[i] -> getCostVar());
			//std::cout << "for var: " << v << ", assignment: " << _costSolver -> getAssignment(v) << std::endl;
			_muxAssignment[i] = _costSolver -> getAssignment(_sortedCandGate[i] -> getCostVar());
		}
		return true;
	}
	else {
		//cout << "no valid assignment ..." << endl;
		return false;
	}
}

void
CirMgr::addBlockingAssignment(const std::vector<bool>& assign)
{
	assert(assign.size() == _sortedCandGate.size());
	std::cout << "add blocking assignment: ";
	for( unsigned i = 0; i < assign.size(); ++i )
		std::cout << assign[i] << " ";
	std::cout << std::endl;
	vec<Lit> clause; clause.clear();
	for( unsigned i = 0; i < assign.size(); ++i ) {
		Lit l = mkLit(_sortedCandGate[i] -> getCostVar(), false);
		if( assign[i] ) clause.push(~l);
		else clause.push(l);
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

void
CirMgr::addAllBlockings()
{
	for( unsigned i = 0; i < _blockingClauses.size(); ++i ) {
		addBlockingAssignment(_blockingClauses[i]);
	}
}
