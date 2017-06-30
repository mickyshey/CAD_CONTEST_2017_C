// File name [ cirMgr.h ]
#ifndef CIRMGR_H
#define CIRMGR_H

#include <iostream>
#include <string>
#include <map>
#include <cstring>

#include "cir/cirNet.h"
//#include "minisat/Solver.h"
#include "sat/sat.h"
#include "reader.h"

using namespace std;

typedef map<Var, CirGate*> VarMap;

enum VAR_GROUP
{
    LOCAL_ON,
    LOCAL_OFF,
    COMMON,
    NONE
};

class CirMgr
{
public:
	explicit CirMgr(const string& name = ""): 
			_name(name),
			_F(NULL),
			_G(NULL),
			_dupF(NULL),
			_dupG(NULL),
			_patch(NULL) {
			_candNameList.clear();
			_isClauseOn.clear();
			_s = new SatSolverV; 
			_costSolver = new SatSolverV; }//_s -> init(); }
	~CirMgr() {}//{ delete _F; delete _G; delete _patch; }

//	info
	void setName(const string& n ) 	{ _name = n; }
	CirNet* getF() 						{ return _F; }
	CirNet* getG() 						{ return _G; }
	CirNet* getPath() 					{ return _patch; }
	unsigned getNumClauses() const 	{ return _s -> getNumClauses(); }
	unsigned getNumVars() const		{ return _s -> nVars(); }

//	in cirParse.cpp
	bool readVerilog(const string& filename, bool isF);
	bool removeComment(const string& inFileName, const string& outFileName) const;
    bool addWeight(const string& fileName);

//	in cirDup.cpp
	CirNet* dupNet(CirNet* n) const;
	CirNet* connectXor(CirNet* f, CirNet* g);	// we shouldn't create the whole XOR net, instead, we simply add the XOR cnf constraint to the solver

// in cirMgr.cpp
	void test();
	void createMux4Candidates();
    void writeToPatch(const string& fileName);
    void writeToOut(const string& fileName);

//	in cirReport.cpp
	void reportCandList() const;
	void reportSortedCand() const;
	void reportMuxAssignment() const;

//	in cirSat.cpp
	bool solve() { return _s -> solve(); }
	void createVar(CirNet* n) const { n -> createVar(_s); }
	void addToSolver(CirNet* n) const { n -> addToSolver(_s); }
	void tieGate(CirGate* g1, CirGate* g2);
	void tiePi(CirNet* f, CirNet* g);					// _F = _G, _dupF = _dupG
	void tieConst(CirNet* f, CirNet* g);
	void addXorConstraint(CirNet* f, CirNet* g);		// _F ^ _G, _dupF ^ _dupG
	void addErrorConstraint(CirNet* n, bool val);		// _F(t = 0), _dupF(t = 1)
	void markOnsetClause(const ClauseId& cid);
	void markOffsetClause(const ClauseId& cid);
    void buildVarMap(CirNet* ntk);
    CirNet* getItp();
    CirNet* buildItp(const string& fileName);
    void retrieveProof(Reader& rdr, vector<unsigned>& clausePos, vector<ClauseId>& usedClause);
	bool proveEQ(CirNet* f, CirNet* g);


// in cirCost.cpp
	void sortCandidate();
	void createVar4CostSolver(bool setMgr);
	void addCostConstraint(unsigned cost);
	bool getMuxAssignment();
	void updateIndices(std::vector<unsigned>& indices, unsigned& currCost);
	void addBlockingAssignment(const std::vector<bool>& assign);
	unsigned getTotalCost();
	void addAllBlockings();
	void tieGateWithAssignment(const std::vector<bool>& assign);
	void restoreTiedGates();

//      in cirProof.cpp
        void checkPo();
        void genProofModel(SatSolverV& solver);

private:
	string 								_name;
	CirNet*								_F;
	CirNet*								_G;
	CirNet*								_dupF;
	CirNet*								_dupG;
	CirNet*								_patch;
	SatSolverV*							_s;
	SatSolverV*							_costSolver;
        CirNet*                                                         _out;
	vector<string>						_candNameList;
	vector<CirGate*>					_sortedCandGate;
	vector<bool>						_muxAssignment;
	map<Var, CirGate*>	                _costVar2Gate;

	vector<bool>						_isClauseOn;
	vector<bool>						_isClauseOnDup;
        vector<VAR_GROUP>                                       _varGroup;
        VarMap                                                  _var2Gate;
	std::vector<std::vector<bool>>		_blockingClauses;
	// for restore
	std::vector<Var>					_varsDup;
};

#endif
