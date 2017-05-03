// File name [ CirSat.cpp ]

#include "cir/cirNet.h"

using namespace std;

void
CirNet::addToSolver(SatSolver* s) const
{
	buildTopoList();
	for( unsigned i = 0; i < _topoList.size(); ++i ) {
		CirGate* g = _topoList[i];
		g -> setVar(s -> newVar());
		//cout << "adding " << _topoList[i] -> getName() << endl;
		_topoList[i] -> addToSolver(s);
	}
}
