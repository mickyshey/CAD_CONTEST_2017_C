// File name [ cirMgr.h ]
#ifndef CIRMGR_H
#define CIRMGR_H

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <cstring>
#include <ctime>

#include "cir/cirNet.h"
//#include "minisat/Solver.h"
#include "sat/sat.h"
#include "reader.h"

#define CUT_CAND 10     // grab 10 candidate at a time

using namespace std;

typedef std::unordered_map<Var, CirGate*> VarMap;
typedef std::vector<size_t> assignmentVec;
typedef std::vector<unsigned> idxVec;
typedef std::unordered_map<string, vector<bool> > rmOnset;

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
        _FF(NULL),
        _F(NULL),
        _G(NULL),
        _dupF(NULL),
        _dupG(NULL),
        _patch(NULL),
        _allExplored(false),
        _debug(false) {
            _candNameList.clear();
            _isClauseOn.clear();
            _s = new SatSolverV; 
            _rmSolver = new SatSolverV; 
            _candSolver = new SatSolverV;
            _costSolver = new SatSolverV; }//_s -> init(); }
    ~CirMgr() {}//{ delete _F; delete _G; delete _patch; }

    //	info
    void setName(const string& n ) 	{ _name = n; }
    CirNet* getF() 						{ return _F; }
    CirNet* getG() 						{ return _G; }
    CirNet* getPath() 					{ return _patch; }
    unsigned getNumClauses() const 	{ return _s -> getNumClauses(); }
    unsigned getNumVars() const		{ return _s -> nVars(); }
    unsigned getErrorNum() const    { return _F -> getErrorNum(); }

    //	in cirParse.cpp
    bool readVerilog(const string& filename, bool isF);
    bool removeComment(const string& inFileName, const string& outFileName) const;
    bool addWeight(const string& fileName);

    //	in cirDup.cpp
    CirNet* dupNet(CirNet* n) const;
    CirNet* connectXor(CirNet* f, CirNet* g);	// we shouldn't create the whole XOR net, instead, we simply add the XOR cnf constraint to the solver
    void    miterNet(CirNet* src, CirNet* target);

    // in cirMgr.cpp
    void test();
    void testRM();
    void createMux4Candidates();
    void removeInvBuf();
    void removeCandFromFanoutCone(bool removePo = false);
    void removeCandFromFanoutConeRec(CirGate* g, std::unordered_set<std::string>& nameHash, GateList& tmpPoList);
    void reduceCandidates();
    void writeToPatch(const string& fileName);
    void writeToOut(const string& fileName, const string& inputFile);

    // in cirSim.cpp
    void randSim(CirNet* n) const { n -> randSim(); }
    void knownSim(CirNet* g, CirNet* f) const;
    void addCandIdx(idxVec& candIdx, unsigned startIdx, unsigned numToAdd);
    void analyzeVec(assignmentVec& t_1, assignmentVec& t_0, idxVec& candIdx);
    bool checkValidCut(assignmentVec& t_1, assignmentVec& t_0);
    void generalizeCut(assignmentVec& t_1, assignmentVec& t_0, idxVec& candIdx, idxVec& cutIdx);

    //	in cirReport.cpp
    void reportCandList() const;
    void reportSortedCand() const;
    void reportMuxAssignment() const;
    void reportFanoutCone(CirGate* g) const;
    void reportFanoutConeRec(CirGate* g, unsigned level) const;

    //	in cirSat.cpp
    bool solve() { return _s -> solve(); }
    void createVarAll() const;
    void createVar(CirNet* n) const { n -> createVar(_s); }
    void addToSolver(CirNet* n) const { n -> addToSolver(_s); }
    void tieGate(CirGate* g1, CirGate* g2);
    void tiePi(CirNet* f, CirNet* g, int solver = 0);	// _F = _G, _dupF = _dupG
    void tieConst(CirNet* f, CirNet* g);
    void addXorConstraint(CirNet* f, CirNet* g, int solver = 0);	// _F ^ _G, _dupF ^ _dupG
    void addErrorConstraint(CirNet* n, bool val, int solver = 0);	// _F(t = 0), _dupF(t = 1)
    void addConstConstraint(CirNet* n, int solver = 0);
    void markOnsetClause(const ClauseId& cid);
    void markOffsetClause(const ClauseId& cid);
    void buildVarMap(CirNet* ntk);
    void buildVarMap(CirGate* g);
    CirNet* getItp();
    CirNet* getItp(const string& name); // for RM
    CirNet* buildItp(const string& fileName);
    void retrieveProof(Reader& rdr, vector<unsigned>& clausePos, vector<ClauseId>& usedClause);
    bool proveEQ(CirNet* f, CirNet* g);
    void initCandSolver();
    void setUpImpVar();
    void addAllToCandSolver();
    void setUpCandSolver();
    void assumeCut(vector<Lit>& Lit_vec_origin);
    void assumeCut(idxVec& cutIdx, std::vector<Lit>& Lit_vec_origin);
    void generatePatch();
    void generatePatch(idxVec& cutIdx);
    void UNSATGeneralizationWithUNSATCore(idxVec& cutIdx, std::vector<Lit>& Lit_vec_origin, idxVec& generalizedCut);
    void SATGeneralization(idxVec& cutIdx);
    void getCutWithDecisionOrdered(bool zeroFirst, unsigned& bestCost);
    unsigned searchInvCount(const std::string& bestName, const std::string& gateName);
    void searchInvCountRec(CirGate* g, CirGate* best, unsigned& invCount);

    // in cirMulti.cpp
    void formulaRM(CirNet* f, CirNet* g, int solver); // _s = 0; _rmSolver = 1;
    void markMultiOnsetClause(const ClauseId& cid, const string& name);
    void markMultiOffsetClause(const ClauseId& cid, const string& name);

    // in cirCost.cpp
    void sortCandidate();
    void createVar4CostSolver();
    void addCostConstraint(unsigned cost);
    bool getCut(idxVec& cutIdx, bool zeroFirst);
    void updateIndices(std::vector<unsigned>& indices, unsigned& currCost);
    void addBlockingCut(idxVec& cutIdx, bool isSat);
    unsigned getTotalCost();
    unsigned getCost(idxVec& cutIdx);
    void addAllBlockings();
    void tieGateWithAssignment(const std::vector<bool>& assign);
    void restoreTiedGates();
    void cutTest(idxVec& cutIdx);

    // in cirProof.cpp
    void checkValidPatch();
    void checkPo();
    void genProofModel(SatSolverV& solver);

private:
	string 								_name;
   CirNet*                       _FF; // for checking patch validity
	CirNet*								_F;
	CirNet*								_G;
	CirNet*								_dupF;
	CirNet*								_dupG;
	CirNet*								_patch;
	SatSolverV*							_s;            // for gen patch
	SatSolverV*							_costSolver;   // for finding random cut
    SatSolverV*                         _candSolver;   // for gen valid cut
    SatSolverV*                         _rmSolver;     // for cofactor reduction
    CirNet*                             _out;
	vector<string>						_candNameList;
   vector<string>                _candFF;
	vector<CirGate*>					_sortedCandGate;
	vector<bool>						_muxAssignment;
	VarMap	                            _costVar2Gate;

    vector<bool>						_isClauseOn;
    vector<bool>						_isClauseOnDup;
    rmOnset                             _rmOnset;
    rmOnset                             _rmOnsetDup;
    vector<VAR_GROUP>                   _varGroup;
    VarMap                              _var2Gate;
    std::vector<std::vector<bool> >		_blockingClauses;
    bool                                _allExplored;
    idxVec                              _bestCut;
    // for restore
    std::vector<Var>					_varsDup;
    // for cofactor reduction
    static unsigned                     _patchGateCount; // to create gate name in multiple errors
    std::vector< vector<bool> >         _rvset;
    std::vector<CirNet*>                _rcset;
    std::vector<Var>                    _rmOut;

    // for debug
    bool     _debug; // will cause problem in CIC machine

};

#endif
