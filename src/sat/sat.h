/****************************************************************************
  FileName     [ sat.h ]
  PackageName  [ sat ]
  Synopsis     [ Define miniSat solver interface functions ]
  Author       [ Chung-Yang (Ric) Huang, Cheng-Yin Wu ]
  Copyright    [ Copyleft(c) 2010-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef SAT_H
#define SAT_H

#include <cassert>
#include <iostream>
#include <vector>
#include "SolverV.h"

using namespace std;

class CirMgr;

/********** MiniSAT_Solver **********/
class SatSolverV
{
   friend class CirMgr;
   public : 
      //SatSolverV(const V3Ntk* const);
	  SatSolverV();
      ~SatSolverV();

      void reset();
      void assumeRelease();
      void assumeProperty(const Var& var, const bool& inv);
      void assertProperty(const size_t& var, const bool& invert);
      //void assumeProperty(const V3NetId& id, const bool& invert, const uint32_t& depth);
      //void assertProperty(const V3NetId& id, const bool& invert, const uint32_t& depth);
      const bool simplify();
      const bool solve(bool zeroFirst = true);
      const bool assump_solve(bool zeroFirst = true);
      int getNumClauses() const { return _solver->nRootCla(); }
      int nVars() const { return _solver -> nVars(); }

      Var newVar();

      // functions for adding clauses
      void addUnit(Lit lit0) { _solver->addUnit(lit0); }
      void addBinary(Lit lit0, Lit lit1) { _solver->addBinary(lit0, lit1); }
      void addTernary(Lit lit0, Lit lit1, Lit lit2) { _solver->addTernary(lit0, lit1, lit2); }
      void addClause(const vec<Lit>& clause) { _solver->addClause(clause); }
      void addUnitCNF(Var var, bool val);
      void addEqCNF(Var var0, Var var1, bool inv);
      void addAndCNF(Var out, Var in0, bool inv0, Var in1, bool inv1);
      void addAndCNF(Var out, const vector<Var>& rms);
      void addOrCNF(Var out, Var in0, bool inv0, Var in1, bool inv1);
      void addOrCNF(Var out, const vector<Var>& Xors);
      void addXorCNF(Var out, Var in0, bool inv0, Var in1, bool inv1);
      void addXnorCNF(Var out, Var in0, bool inv0, Var in1, bool inv1);
      void addMuxCNF(Var out, Var in0, bool inv0, Var in1, bool inv1, Var sel, bool invSel);

      // Network to Solver Functions
      //const size_t getFormula(const V3NetId&, const uint32_t&);
      //const V3BitVecX getDataValue(const V3NetId&, const uint32_t&) const;
      const bool getDataValue(const size_t& var) const;
      const bool getAssignment(const Var& v) const;
      // Variable Interface Functions
      inline const size_t reserveFormula() { return getPosVar(newVar()); }
      inline const bool isNegFormula(const size_t& v) const { return (v & 1ul); }
      inline const size_t getNegFormula(const size_t& v) const { return (v ^ 1ul); }

      // Gate Formula to Solver Functions 
      //void add_FALSE_Formula(const V3NetId&, const uint32_t&);
      //void add_PI_Formula(const V3NetId&, const uint32_t&);
      //void add_FF_Formula(const V3NetId&, const uint32_t&);
      //void add_AND_Formula(const V3NetId&, const uint32_t&);

      //void addBoundedVerifyData(const V3NetId&, const uint32_t&);
      //const bool existVerifyData(const V3NetId&, const uint32_t&);
      //void resizeNtkData(const uint32_t& num);

   private : 
      //const Var getVerifyData(const V3NetId&, const uint32_t&) const;
      //void addBoundedVerifyDataRecursively(const V3NetId&, const uint32_t&);

      inline const Var getOriVar(const size_t& v) const { return (Var)(v >> 1ul); }
      inline const size_t getPosVar(const Var& v) const { return (((size_t)v) << 1ul); }
      inline const size_t getNegVar(const Var& v) const { return ((getPosVar(v)) | 1ul); }

      SolverV            *_solver;    // Pointer to a Minisat solver
      Var                 _curVar;    // Variable currently
      vec<Lit>            _assump;    // Assumption List for assumption solve
      //const V3Ntk* const  _ntk;       // Network Under Verification
      //vector<Var>*        _ntkData;   // Mapping between V3NetId and Solver Data

};

#endif  // SAT_H

