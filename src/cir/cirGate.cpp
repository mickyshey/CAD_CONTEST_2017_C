// File name [ cirGate.cpp ]
#include "cir/cirGate.h" 
#include <cassert>
#include <cstdlib>

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
CirConstGate::addToSolver(SatSolverV* s, int solver) const
{
   assert(solver <= 2);
	if( _name == "1'b0" ) {
      if( solver == 0 ) s -> addUnitCNF(_var, false);
      else if( solver == 1 ) s -> addUnitCNF(_candVar, false);
      else if( solver == 2 ) s -> addUnitCNF(_rmVar, false);
   }
	else {
      if( solver == 0 ) s -> addUnitCNF(_var, true);
      else if( solver == 1 ) s -> addUnitCNF(_candVar, true);
      else if( solver == 2 ) s -> addUnitCNF(_rmVar, true);
   }
}

void
CirConstGate::simulate() {
   if( _name == "1'b0" ) _simV = 0;
   else _simV = -1;
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
CirPiGate::addToSolver(SatSolverV* s, int solver) const
{
   assert(solver <= 2);
   if(_in.size() == 0) return;
   if( solver == 0 ) s -> addEqCNF(_var, getFaninVar(0), isFaninInv(0));
   else if( solver == 1 ) s -> addEqCNF(_candVar, getFaninCandVar(0), isFaninInv(0));
   else if( solver == 2 ) s -> addEqCNF(_rmVar, getFaninRMVar(0), isFaninInv(0));
}

void
CirPiGate::simulate() {
   for( unsigned i = 0; i < 32; ++i ) {
      _simV = (_simV << 1) | (rand() & 1);
   }
}
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
CirPoGate::addToSolver(SatSolverV* s, int solver) const
{
    assert(solver <= 2);
    assert(_in.size() == 1);
    if( solver == 0 ) s -> addEqCNF(_var, getFaninVar(0), isFaninInv(0));	
    else if( solver == 1 ) s -> addEqCNF(_candVar, getFaninCandVar(0), isFaninInv(0));
    else if( solver == 2 ) s -> addEqCNF(_rmVar, getFaninRMVar(0), isFaninInv(0));
}

void
CirPoGate::simulate() {
   assert(_in.size() == 1);
   _simV = (isFaninInv(0) ? ~getFaninSimV(0) : getFaninSimV(0));
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
CirBufGate::addToSolver(SatSolverV* s, int solver) const
{
    assert(solver <= 2);
    assert(_in.size() == 1);
    if( solver == 0 ) s -> addEqCNF(_var, getFaninVar(0), isFaninInv(0));
    else if( solver == 1 ) s -> addEqCNF(_candVar, getFaninCandVar(0), isFaninInv(0));
    else if( solver == 2 ) s -> addEqCNF(_rmVar, getFaninRMVar(0), isFaninInv(0));
}

void
CirBufGate::simulate() {
   assert(_in.size() == 1);
   _simV = (isFaninInv(0) ? ~getFaninSimV(0) : getFaninSimV(0));
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
CirInvGate::addToSolver(SatSolverV* s, int solver) const
{
    assert(solver <= 2);
    assert(_in.size() == 1);
    if( solver == 0 ) s -> addEqCNF(_var, getFaninVar(0), !isFaninInv(0));
    else if ( solver == 1 ) s -> addEqCNF(_candVar, getFaninCandVar(0), !isFaninInv(0));
    else if ( solver == 2 ) s -> addEqCNF(_rmVar, getFaninRMVar(0), !isFaninInv(0));
}

void
CirInvGate::simulate() {
   assert(_in.size() == 1);
   _simV = (isFaninInv(0) ? getFaninSimV(0) : ~getFaninSimV(0));
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
CirAndGate::addToSolver(SatSolverV* s, int solver) const
{
    assert(solver <= 2);
/*	
    Lit outLit = mkLit(_id, false);
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
    if( solver == 0 ) s -> addAndCNF(_var, getFaninVar(0), isFaninInv(0), getFaninVar(1), isFaninInv(1));	
    else if( solver == 1 ) s -> addAndCNF(_candVar, getFaninCandVar(0), isFaninInv(0), getFaninCandVar(1), isFaninInv(1));	
    else if( solver == 2 ) s -> addAndCNF(_rmVar, getFaninRMVar(0), isFaninInv(0), getFaninRMVar(1), isFaninInv(1));	
}

void
CirAndGate::simulate() {
   assert(_in.size() == 2);
   size_t sim0 = (isFaninInv(0) ? ~getFaninSimV(0) : getFaninSimV(0));
   size_t sim1 = (isFaninInv(1) ? ~getFaninSimV(1) : getFaninSimV(1));
   _simV = sim0 & sim1;
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
CirOrGate::addToSolver(SatSolverV* s,int solver) const
{
    assert(solver <= 2);
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
    if( solver == 0 ) s -> addOrCNF(_var, getFaninVar(0), isFaninInv(0), getFaninVar(1), isFaninInv(1));
    else if( solver == 1 ) s -> addOrCNF(_candVar, getFaninCandVar(0), isFaninInv(0), getFaninCandVar(1), isFaninInv(1));
    else if( solver == 2 ) s -> addOrCNF(_rmVar, getFaninRMVar(0), isFaninInv(0), getFaninRMVar(1), isFaninInv(1));
}

void
CirOrGate::simulate() {
   assert(_in.size() == 2);
   size_t sim0 = (isFaninInv(0) ? ~getFaninSimV(0) : getFaninSimV(0));
   size_t sim1 = (isFaninInv(1) ? ~getFaninSimV(1) : getFaninSimV(1));
   _simV = sim0 | sim1;
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
CirNandGate::addToSolver(SatSolverV* s, int solver) const
{
    assert(solver <= 2);
    assert(_in.size() == 2);
    if( solver == 0 ) s -> addOrCNF(_var, getFaninVar(0), !isFaninInv(0), getFaninVar(1), !isFaninInv(1));
    else if( solver == 1 ) s -> addOrCNF(_candVar, getFaninCandVar(0), !isFaninInv(0), getFaninCandVar(1), !isFaninInv(1));
    else if( solver == 2 ) s -> addOrCNF(_rmVar, getFaninRMVar(0), !isFaninInv(0), getFaninRMVar(1), !isFaninInv(1));
}

void
CirNandGate::simulate() {
   assert(_in.size() == 2);
   size_t sim0 = (isFaninInv(0) ? ~getFaninSimV(0) : getFaninSimV(0));
   size_t sim1 = (isFaninInv(1) ? ~getFaninSimV(1) : getFaninSimV(1));
   _simV = ~(sim0 & sim1);
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
CirNorGate::addToSolver(SatSolverV* s, int solver) const
{
    assert(solver <= 2);
    assert(_in.size() == 2);
    if( solver == 0 ) s -> addAndCNF(_var, getFaninVar(0), !isFaninInv(0), getFaninVar(1), !isFaninInv(1));
    else if( solver == 1 ) s -> addAndCNF(_candVar, getFaninCandVar(0), !isFaninInv(0), getFaninCandVar(1), !isFaninInv(1));
    else if( solver == 2 ) s -> addAndCNF(_rmVar, getFaninRMVar(0), !isFaninInv(0), getFaninRMVar(1), !isFaninInv(1));
}

void
CirNorGate::simulate() {
   assert(_in.size() == 2);
   size_t sim0 = (isFaninInv(0) ? ~getFaninSimV(0) : getFaninSimV(0));
   size_t sim1 = (isFaninInv(1) ? ~getFaninSimV(1) : getFaninSimV(1));
   _simV = ~(sim0 | sim1);
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
CirXorGate::addToSolver(SatSolverV* s, int solver) const
{
    assert(solver <= 2);
    assert(_in.size() == 2);
    if( solver == 0 ) s -> addXorCNF(_var, getFaninVar(0), isFaninInv(0), getFaninVar(1), isFaninInv(1));
    else if( solver == 1 ) s -> addXorCNF(_candVar, getFaninCandVar(0), isFaninInv(0), getFaninCandVar(1), isFaninInv(1));
    else if( solver == 2 ) s -> addXorCNF(_rmVar, getFaninRMVar(0), isFaninInv(0), getFaninRMVar(1), isFaninInv(1));
}

void
CirXorGate::simulate() {
   assert(_in.size() == 2);
   size_t sim0 = (isFaninInv(0) ? ~getFaninSimV(0) : getFaninSimV(0));
   size_t sim1 = (isFaninInv(1) ? ~getFaninSimV(1) : getFaninSimV(1));
   _simV = sim0 ^ sim1;
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
CirXnorGate::addToSolver(SatSolverV* s, int solver) const
{
    assert(solver <= 2);
    assert(_in.size() == 2);
    if( solver == 0 ) s -> addXnorCNF(_var, getFaninVar(0), isFaninInv(0), getFaninVar(1), isFaninInv(1));
    else if( solver == 1 ) s -> addXnorCNF(_candVar, getFaninCandVar(0), isFaninInv(0), getFaninCandVar(1), isFaninInv(1));
    else if( solver == 2 ) s -> addXnorCNF(_rmVar, getFaninRMVar(0), isFaninInv(0), getFaninRMVar(1), isFaninInv(1));
}

void
CirXnorGate::simulate() {
   assert(_in.size() == 2);
   size_t sim0 = (isFaninInv(0) ? ~getFaninSimV(0) : getFaninSimV(0));
   size_t sim1 = (isFaninInv(1) ? ~getFaninSimV(1) : getFaninSimV(1));
   _simV = ~(sim0 ^ sim1);
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
CirErrorGate::addToSolver(SatSolverV* s, int solver) const
{
    assert(solver <= 2);
    assert(_in.size() < 2);
    if(_in.size() == 0) return;
    if( solver == 0 ) s -> addEqCNF(_var, getFaninVar(0), isFaninInv(0));
    else if( solver == 1 ) s -> addEqCNF(_candVar, getFaninCandVar(0), isFaninInv(0));
    else if( solver == 2 ) s -> addEqCNF(_rmVar, getFaninRMVar(0), isFaninInv(0));
}

void
CirErrorGate::simulate() {
   for( unsigned i = 0; i < 32; ++i ) {
      _simV = (_simV << 1) | (rand() & 1);
   }
}
/*************************
		CirMuxGate
*************************/
const GateType
CirMuxGate::getType() const
{
	return Gate_Mux;
}

void
CirMuxGate::report() const
{
	cout << "[Mux]: " << _name;
}

void
CirMuxGate::addToSolver(SatSolverV* s, int solver) const
{
	// TODO
    assert(solver <= 2);
    assert(_in.size() == 3);
    if( solver == 0 ) s -> addMuxCNF(_var, getFaninVar(0), isFaninInv(0), getFaninVar(1), isFaninInv(1), getFaninVar(2), isFaninInv(2));	
    else if( solver == 1 ) s -> addMuxCNF(_candVar, getFaninCandVar(0), isFaninInv(0), getFaninCandVar(1), isFaninInv(1), getFaninCandVar(2), isFaninInv(2));	
    else if( solver == 2 ) s -> addMuxCNF(_rmVar, getFaninRMVar(0), isFaninInv(0), getFaninRMVar(1), isFaninInv(1), getFaninRMVar(2), isFaninInv(2));	
}

void
CirMuxGate::simulate() {

}
