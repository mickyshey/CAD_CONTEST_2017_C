/****************************************************************************
  FileName     [ sat.cpp ]
  PackageName  [ sat ]
  Synopsis     [ Define miniSat solver interface functions ]
  Author       [ Chung-Yang (Ric) Huang, Cheng-Yin Wu ]
  Copyright    [ Copyleft(c) 2010-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef SAT_C
#define SAT_C

#include "sat.h"
#include <cmath>

//SatSolverV::SatSolverV(const V3Ntk* const ntk ): _ntk(ntk)
SatSolverV::SatSolverV()
{
   _solver = new SolverV();
   _solver->proof = new Proof(); 
   _assump.clear();
   _curVar = 0; _solver->newVar(); ++_curVar;
   //_ntkData = new vector<Var> [ntk->getNetSize()];
   //for(uint32_t i = 0; i < ntk->getNetSize(); ++i ) _ntkData[i].clear();
}

SatSolverV::~SatSolverV() 
{
   delete _solver;  
   assumeRelease();
   //for(uint32_t i = 0; i < _ntk->getNetSize(); ++i ) _ntkData[i].clear();
   //delete [] _ntkData;
}

void SatSolverV::reset()
{
   delete _solver; 
   _solver = new SolverV();
   _solver->proof = new Proof(); 
   _assump.clear();
   _curVar = 0; _solver->newVar(); ++_curVar;
   //_ntkData = new vector<Var> [_ntk->getNetSize()];
   //for(uint32_t i = 0; i < _ntk->getNetSize(); ++i ) _ntkData[i].clear();
}

void SatSolverV::assumeRelease() { _assump.clear(); }

void SatSolverV::assumeProperty(const Var& var, const bool& inv)
{
   _assump.push(mkLit(var, inv));
}

void SatSolverV::assertProperty(const size_t& var, const bool& invert)
{
   //_solver->addUnit(mkLit(getOriVar(var), invert ^ isNegFormula(var)));
}

void SatSolverV::addUnitCNF(Var var, bool val)
{
  _solver->addUnit(mkLit(var, !val));
}

void SatSolverV::addEqCNF(Var var0, Var var1, bool inv)
{
  Lit lit0 = mkLit(var0, false);
  Lit lit1 = mkLit(var1, inv);
  _solver->addBinary( lit0, ~lit1);
  _solver->addBinary(~lit0,  lit1);
}

void SatSolverV::addAndCNF(Var out, Var in0, bool inv0, Var in1, bool inv1)
{
  Lit outLit = mkLit(out, false);
  Lit in0Lit = mkLit(in0, inv0);
  Lit in1Lit = mkLit(in1, inv1);
  // out = in0 & in1
  // clause (in0 + ~out)
  _solver->addBinary(in0Lit, ~outLit);
  // clause (in1 + ~out)
  _solver->addBinary(in1Lit, ~outLit);
  // clause (~in0 + ~in1 + out)
  _solver->addTernary(~in0Lit, ~in1Lit, outLit);
}

void SatSolverV::addOrCNF(Var out, Var in0, bool inv0, Var in1, bool inv1)
{
  Lit outLit = mkLit(out, false);
  Lit in0Lit = mkLit(in0, inv0);
  Lit in1Lit = mkLit(in1, inv1);
  // out = in0 | in1
  // clause (~in0 + out)
  _solver->addBinary(~in0Lit, outLit);
  // clause (~in1 + out)
  _solver->addBinary(~in1Lit, outLit);
  // clause (in0 + in1 + ~out)
  _solver->addTernary(in0Lit, in1Lit, ~outLit);
}

void SatSolverV::addOrCNF(Var out, const vector<Var>& Xors)
{
	assert(Xors.size() > 1);
	vec<Lit> longCla;		longCla.clear();
	vec<Lit> shortCla; 	shortCla.clear();
	Lit outLit = mkLit(out, false);
	longCla.push(~outLit);
	for( unsigned i = 0; i < Xors.size(); ++i ) {
		shortCla.push(outLit);
		// var of Xors should have no inv
		Lit inLit = mkLit(Xors[i], false);
		shortCla.push(~inLit);
		//assert(_solver -> value(inLit) == l_Undef);
		assert(shortCla.size() == 2);
		//std::cout << "[" << i << "] short: " << var(shortCla[0]) << "(" << sign(shortCla[0]) << ") + " << var(shortCla[1]) << "(" << sign(shortCla[1]) << ")" << std::endl;
		_solver -> addClause(shortCla);
		shortCla.clear();
		longCla.push(inLit);
	}
	_solver -> addClause(longCla);
/*
	std::cout << "long: ";
	for( unsigned i = 0; i < longCla.size(); ++i ) {
		std::cout << var(longCla[i]) << "(" << sign(longCla[i]) << ") ";
	}
	std::cout << std::endl;
*/
	longCla.clear();
}

void SatSolverV::addAndCNF(Var out, const vector<Var>& rms)
{
	assert(rms.size() > 1);
	vec<Lit> longCla;		longCla.clear();
	vec<Lit> shortCla; 	shortCla.clear();
	Lit outLit = mkLit(out, false);
	longCla.push(outLit);
	for( unsigned i = 0; i < rms.size(); ++i ) {
		shortCla.push(~outLit);
		// var of rms should have no inv
		Lit inLit = mkLit(rms[i], false);
		shortCla.push(inLit);
		//assert(_solver -> value(inLit) == l_Undef);
		assert(shortCla.size() == 2);
		//std::cout << "[" << i << "] short: " << var(shortCla[0]) << "(" << sign(shortCla[0]) << ") + " << var(shortCla[1]) << "(" << sign(shortCla[1]) << ")" << std::endl;
		_solver -> addClause(shortCla);
		shortCla.clear();
		longCla.push(~inLit);
	}
	_solver -> addClause(longCla);

	//std::cout << "long: ";
	for( unsigned i = 0; i < longCla.size(); ++i ) {
		//std::cout << var(longCla[i]) << "(" << sign(longCla[i]) << ") ";
	}
	std::cout << std::endl;

	longCla.clear();
}

void SatSolverV::addXorCNF(Var out, Var in0, bool inv0, Var in1, bool inv1)
{
  Lit outLit = mkLit(out, false);
  Lit in0Lit = mkLit(in0, inv0);
  Lit in1Lit = mkLit(in1, inv1);
  // out = in0 ^ in1
  // clause (in0 + in1 + ~out)
  _solver->addTernary( in0Lit,  in1Lit, ~outLit);
  // clause (in0 + ~in1 + out)
  _solver->addTernary( in0Lit, ~in1Lit,  outLit);
  // clause (~in0 + in1 + out)
  _solver->addTernary(~in0Lit,  in1Lit,  outLit);
  // clause (~in0 + ~in1 + ~out)
  _solver->addTernary(~in0Lit, ~in1Lit, ~outLit);
}

void SatSolverV::addXnorCNF(Var out, Var in0, bool inv0, Var in1, bool inv1)
{
  Lit outLit = mkLit(out, false);
  Lit in0Lit = mkLit(in0, inv0);
  Lit in1Lit = mkLit(in1, inv1);
  // out = ~(in0 ^ in1)
  // clause (in0 + in1 + out)
  _solver->addTernary( in0Lit,  in1Lit,  outLit);
  // clause (in0 + ~in1 + ~out)
  _solver->addTernary( in0Lit, ~in1Lit, ~outLit);
  // clause (~in0 + in1 + ~out)
  _solver->addTernary(~in0Lit,  in1Lit, ~outLit);
  // clause (~in0 + ~in1 + out)
  _solver->addTernary(~in0Lit, ~in1Lit,  outLit);
}

void SatSolverV::addMuxCNF(Var out, Var in0, bool inv0, Var in1, bool inv1, Var sel, bool invSel)
{
	Lit outLit = mkLit(out, false);
	Lit in0Lit = mkLit(in0, inv0);
	Lit in1Lit = mkLit(in1, inv1);
	Lit selLit = mkLit(sel, invSel);
	// out = (s ? in1 : in0);
	// clause (s + ~in0 + out)
	_solver -> addTernary( selLit, ~in0Lit,  outLit);
	// clause (s + in0 + ~out)
	_solver -> addTernary( selLit,  in0Lit, ~outLit);
	// clause (~s + in1 + ~out)
	_solver -> addTernary(~selLit,  in1Lit, ~outLit);
	// clause (~s + ~in1 + out)
	_solver -> addTernary(~selLit, ~in1Lit,  outLit);
}
/*
void SatSolverV::assumeProperty(const V3NetId& id, const bool& invert, const uint32_t& depth)
{
   assert(_ntk->validNetId(id)); assert( 1 == _ntk->getNetWidth(id) );
   const Var var = getVerifyData(id, depth); assert(var);
   _assump.push(mkLit(var, invert ^ isV3NetInverted(id)));
}
*/
/*
void SatSolverV::assertProperty(const V3NetId& id, const bool& invert, const uint32_t& depth)
{
   assert(_ntk->validNetId(id)); assert( 1 == _ntk->getNetWidth(id) );
   const Var var = getVerifyData(id, depth); assert(var);
   _solver->addUnit(mkLit(var, invert ^ isV3NetInverted(id)));
}
*/
const bool SatSolverV::simplify() { return _solver->simplifyDB(); }

const bool SatSolverV::solve(bool zeroFirst)
{
   _solver->solve(zeroFirst);
   return _solver->okay();
}

const bool SatSolverV::assump_solve(bool zeroFirst)
{
   bool result = _solver->solve(_assump, zeroFirst);
   return result;
}
/*
const V3BitVecX SatSolverV::getDataValue(const V3NetId& id, const uint32_t& depth) const 
{
   Var var = getVerifyData(id, depth); assert(var);
   uint32_t i, width = _ntk->getNetWidth(id);
   V3BitVecX value(width);
   if(isV3NetInverted(id)) {
      for(i = 0; i < width; ++i)
         if(l_True == _solver->model[var+i]) value.set0(i);
         else value.set1(i);
   }
   else {
      for(i = 0; i < width; ++i)
         if(l_True == _solver->model[var+i]) value.set1(i);
         else value.set0(i);
   }
   return value;
}
*/
const bool SatSolverV::getDataValue(const size_t& var) const 
{
   return (isNegFormula(var)) ^ (l_True == _solver->model[getOriVar(var)] );
}

const bool SatSolverV::getAssignment(const Var& v) const
{
	return (l_True == _solver -> model[v]);
}
/*
const size_t SatSolverV::getFormula(const V3NetId& id, const uint32_t& depth)
{
   Var var = getVerifyData(id, depth); assert(var);
   assert(!isNegFormula(getPosVar(var)));
   return (isV3NetInverted(id) ? getNegVar(var) : getPosVar(var));
}
*/
/*
void SatSolverV::resizeNtkData(const uint32_t& num)
{
   vector<Var>* tmp = new vector<Var>[_ntk->getNetSize()];
   for(uint32_t i = 0, j = (_ntk->getNetSize()-num); i < j; ++i)
      tmp[i] = _ntkData[i];
   delete [] _ntkData;
   _ntkData = tmp;
}
*/
Var SatSolverV::newVar() {
   Var cur_var = _curVar;
   _solver->newVar();
   _curVar++; 
   return cur_var;
}
/*
const Var SatSolverV::getVerifyData(const V3NetId& id, const uint32_t& depth) const
{
   assert(_ntk->validNetId(id));
   if ( depth >= _ntkData[getV3NetIndex(id)].size() ) return 0;
   else return _ntkData[getV3NetIndex(id)][depth];
}
*/
/*
void SatSolverV::add_FALSE_Formula(const V3NetId& out, const uint32_t& depth)
{
   const uint32_t index = getV3NetIndex(out); 
   assert( depth == _ntkData[index].size());
   _ntkData[index].push_back(newVar()); 
   _solver->addUnit(mkLit(_ntkData[index].back(), true));
}

void SatSolverV::add_PI_Formula(const V3NetId& out, const uint32_t& depth)
{
   const uint32_t index = getV3NetIndex(out); 
   assert( depth == _ntkData[index].size());
   _ntkData[index].push_back(newVar()); 
}

void SatSolverV::add_FF_Formula(const V3NetId& out, const uint32_t& depth) 
{
   const uint32_t index = getV3NetIndex(out);
   assert( depth == _ntkData[index].size());

   if(depth) {
      // Build FF I/O Relation
      const V3NetId in1 = _ntk->getInputNetId(out,0); assert(_ntk->validNetId(in1));
      const Var var1 = getVerifyData(in1, depth-1); assert(var1);

      if(isV3NetInverted(in1)) {
         // a <-> b
         _ntkData[index].push_back(newVar());
         Lit a = mkLit(_ntkData[index].back());
         Lit b = mkLit(var1,true);				// b = ~var1
         vec<Lit> lits; lits.clear();
         lits.push(~a); lits.push( b); _solver->addClause(lits); lits.clear();
         lits.push( a); lits.push(~b); _solver->addClause(lits); lits.clear();
      }
      else _ntkData[index].push_back(var1);		// do not create a eq CNF, instead, make fanin var be my var
   }
   else {  // Timeframe 0
      _ntkData[index].push_back(newVar());
   }
}

void SatSolverV::add_AND_Formula(const V3NetId& out, const uint32_t& depth)
{
   const uint32_t index = getV3NetIndex(out); 
   assert( depth == _ntkData[index].size());
   _ntkData[index].push_back(newVar()); 

   const Var& var = _ntkData[index].back();
   // Build AND I/O Relation
   const V3NetId in1 = _ntk->getInputNetId(out,0); assert(_ntk->validNetId(in1));
   const V3NetId in2 = _ntk->getInputNetId(out,1); assert(_ntk->validNetId(in2));
   const Var var1 = getVerifyData(in1,depth); assert(var1);
   const Var var2 = getVerifyData(in2,depth); assert(var2);

   Lit y = mkLit(var);
   Lit a = mkLit(var1, isV3NetInverted(in1));
   Lit b = mkLit(var2, isV3NetInverted(in2));

   vec<Lit> lits; lits.clear();
   lits.push(a); lits.push(~y); _solver->addClause(lits); lits.clear();
   lits.push(b); lits.push(~y); _solver->addClause(lits); lits.clear();
   lits.push(~a); lits.push(~b); lits.push(y); _solver->addClause(lits); lits.clear();
}

void SatSolverV::addBoundedVerifyData(const V3NetId& id, const uint32_t& depth)
{
   if( existVerifyData( id, depth) ) return;
   addBoundedVerifyDataRecursively(id,depth);
}

void SatSolverV::addBoundedVerifyDataRecursively(const V3NetId& id, const uint32_t& depth)
{
   assert( _ntk->validNetId(id) );  
   if( existVerifyData(id,depth) ) return;
   const V3GateType type = _ntk->getGateType(id); assert(type < V3_XD);
   if( V3_PIO >= type ) add_PI_Formula(id,depth);
   else if( V3_FF == type ) {
      if(depth) { addBoundedVerifyDataRecursively(_ntk->getInputNetId(id,0), depth-1); }
      add_FF_Formula(id, depth);
   }
   else if(AIG_FALSE >= type) {
      if(AIG_NODE == type) {
         addBoundedVerifyDataRecursively(_ntk->getInputNetId(id,0), depth);
         addBoundedVerifyDataRecursively(_ntk->getInputNetId(id,1), depth);
         add_AND_Formula(id,depth);
      }
      else {
         assert(AIG_FALSE == type);
         add_FALSE_Formula(id,depth);
      }
   }
   else {
      assert(0);
   }
}

const bool SatSolverV::existVerifyData(const V3NetId& id, const uint32_t& depth)
{
   return getVerifyData(id, depth);
}
*/
#endif
