// File name [ cirGate.cpp ]
#include "cir/cirGate.h" 
#include <cassert>

using namespace std;

unsigned CirGate::_globalRef = 0;

/*************************
		CirGate
*************************/
/*************************
		CirConstGate
*************************/
const GateType
CirConstGate::getType() const
{
	return Gate_Const;
}

void
CirConstGate::report() const
{
	if( _name == "1'b0" ) cout << "[Const0]";
	else cout << "[Const1]";
}

void
CirConstGate::addToSolver(SatSolver* s) const
{
	if( _name == "1'b0" ) s -> addUnitCNF(_var, false);
	else s -> addUnitCNF(_var, true);
}
/*************************
		CirPiGate
*************************/
const GateType
CirPiGate::getType() const
{
	return Gate_Pi;
}

void
CirPiGate::report() const
{
	cout << "[Pi]: " << _name;
}

void
CirPiGate::addToSolver(SatSolver* s) const
{}
/*************************
		CirPoGate
*************************/
const GateType
CirPoGate::getType() const
{
	return Gate_Po;
}

void
CirPoGate::report() const
{
	cout << "[Po]: " << _name;
}

void
CirPoGate::addToSolver(SatSolver* s) const
{
	assert(_in.size() == 1);
	s -> addEqCNF(_var, getFaninVar(0), isFaninInv(0));	
}
/*************************
		CirBufGate
*************************/
const GateType
CirBufGate::getType() const
{
	return Gate_Buf;
}

void
CirBufGate::report() const
{
	cout << "[Buf]: " << _name;
}

void
CirBufGate::addToSolver(SatSolver* s) const
{
	assert(_in.size() == 1);
	s -> addEqCNF(_var, getFaninVar(0), isFaninInv(0));
}
/*************************
		CirInvGate
*************************/
const GateType
CirInvGate::getType() const
{
	return Gate_Inv;
}

void
CirInvGate::report() const
{
	cout << "[Inv]: " << _name;
}

void
CirInvGate::addToSolver(SatSolver* s) const
{
	assert(_in.size() == 1);
	s -> addEqCNF(_var, getFaninVar(0), !isFaninInv(0));
}
/*************************
		CirAndGate
*************************/
const GateType
CirAndGate::getType() const
{
	return Gate_And;
}

void
CirAndGate::report() const
{
	cout << "[And]: " << _name;
}

void
CirAndGate::addToSolver(SatSolver* s) const
{
/*	Lit outLit = mkLit(_id, false);
	vec<Lit> c;
	c.push(outLit);
	for( unsigned i = 0; i < _in.size(); ++i ) {
		Lit inLit = mkLit(getFaninId(0), isFaninInv(0));
		c.push(~inLit);
		// (in[i] + ~out)
		s -> addClause(inLit, ~outLit);	
	}
	// (out + ~in[0] + ~in[1] + ...)
	s -> addClause(c);
*/
	assert(_in.size() == 2);
	s -> addAndCNF(_var, getFaninVar(0), isFaninInv(0), getFaninVar(1), isFaninInv(1));	
}
/*************************
		CirOrGate
*************************/
const GateType
CirOrGate::getType() const
{
	return Gate_Or;
}

void
CirOrGate::report() const
{
	cout << "[Or]: " << _name;
}

void
CirOrGate::addToSolver(SatSolver* s) const
{
/*
	Lit outLit = mkLit(_id, false);
	vec<Lit> c;
	c.push(~outLit);
	for( unsigned i = 0; i < _in.size(); ++i ) {
		Lit inLit = mkLit(getFaninId(0), isFaninInv(0));
		c.push(inLit);
		// (~in[i] + out)
		s -> addClause(~inLit, outLit);	
	}
	// (~out + in[0] + in[1] + ...)
	s -> addClause(c);
*/
	assert(_in.size() == 2);
	s -> addOrCNF(_var, getFaninVar(0), isFaninInv(0), getFaninVar(1), isFaninInv(1));
}
/*************************
		CirNandGate
*************************/
const GateType
CirNandGate::getType() const
{
	return Gate_Nand;
}

void
CirNandGate::report() const
{
	cout << "[Nand]: " << _name;
}

void
CirNandGate::addToSolver(SatSolver* s) const
{
	assert(_in.size() == 2);
	s -> addOrCNF(_var, getFaninVar(0), !isFaninInv(0), getFaninVar(1), !isFaninInv(1));
}
/*************************
		CirNorGate
*************************/
const GateType
CirNorGate::getType() const
{
	return Gate_Nor;
}

void
CirNorGate::report() const
{
	cout << "[Nor]: " << _name;
}

void
CirNorGate::addToSolver(SatSolver* s) const
{
	assert(_in.size() == 2);
	s -> addAndCNF(_var, getFaninVar(0), !isFaninInv(0), getFaninVar(1), !isFaninInv(1));
}
/*************************
		CirXorGate
*************************/
const GateType
CirXorGate::getType() const
{
	return Gate_Xor;
}

void
CirXorGate::report() const
{
	cout << "[Xor]: " << _name;
}

void
CirXorGate::addToSolver(SatSolver* s) const
{
	assert(_in.size() == 2);
	s -> addXorCNF(_var, getFaninVar(0), isFaninInv(0), getFaninVar(1), isFaninInv(1));
}
/*************************
		CirXnorGate
*************************/
const GateType
CirXnorGate::getType() const
{
	return Gate_Xnor;
}

void
CirXnorGate::report() const
{
	cout << "[Xnor]: " << _name;
}

void
CirXnorGate::addToSolver(SatSolver* s) const
{
	assert(_in.size() == 2);
	s -> addXnorCNF(_var, getFaninVar(0), isFaninInv(0), getFaninVar(1), isFaninInv(1));
}
/*************************
		CirErrorGate
*************************/
const GateType
CirErrorGate::getType() const
{
	return Gate_Error;
}

void
CirErrorGate::report() const
{
	cout << "[Error]: " << _name;
}

void
CirErrorGate::addToSolver(SatSolver* s) const
{}
