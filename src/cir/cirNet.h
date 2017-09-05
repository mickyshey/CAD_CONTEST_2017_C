// File name [ cirNet.h ]
#ifndef CIRNET_H
#define CIRNET_H

#include <iostream>
#include <string>
#include <map>
#include <unordered_set>

#include "cir/cirGate.h"
//#include "minisat/sat.h"
#include "sat/sat.h"

using namespace std;

typedef map<string, CirGate*> name2GateMap;

class CirNet
{
public:
	explicit CirNet(const string& name = "", unsigned id = 0): 
		_name(name),
		_id(id),
		_const0(NULL),
		_const1(NULL) { init(); }
	~CirNet();

//	net info
	void setName(const string& n )					{ _name = n; }
	const string& getName() const					{ return _name; }
    unsigned getPiNum() const						{ return _piList.size(); }
    unsigned getPoNum() const						{ return _poList.size(); }
    unsigned getGateNum() const						{ return _gateList.size(); }
	unsigned getErrorNum() const					{ return _errorList.size(); }
    CirGate* getConst(bool val) 					{ if(val) return _const1; else return _const0; }
	CirGate* getPi(unsigned idx) const			{ return _piList[idx]; }
	CirGate* getPo(unsigned idx) const			{ return _poList[idx]; }
    CirGate* getGate(unsigned id) 					{ return _gateList[id]; }
	CirGate* getError(unsigned idx)					{ return _errorList[idx]; }
	CirGate* getGateByName(const string& n) 		{ return _name2GateMap[n]; }

//	in cirNet.cpp
	void init();
	CirGate* createGate(const GateType& t, const string& name = "", unsigned id = 0);
	void createConst(bool is0);
	const GateList& buildTopoList() const;			// from PI to PO --> from PO to PI?? 
	void buildTopoListRec(CirGate* g) const;
    const GateList& dfsFromGate(CirGate* g) const;
    void dfsFromGateRec(CirGate* g, GateList& list, std::unordered_set<std::string>& nameHash) const;
    const GateList& totGateList() const;
    void pushBackPOList(CirGate* g)                 { _poList.push_back(g); }
    void pushBackPIList(CirGate* g)                 { _piList.push_back(g); }
    void deletePI(CirGate* g);
	//CirNet* constructNet();
    void removeInvBuf(std::vector<std::string>& nameList);
    void sweep();
	CirGate* createMux(CirGate* g_F, CirGate* g_dupF);
    void randSim();
	void knownSim();
   void maintainPoList(GateList& tmpPoList);
   void swapPoList(GateList& tmpPoList) { _poList.swap(tmpPoList); tmpPoList.clear(); }

//	in cirParse.cpp
	bool parse(const string& filename);
	void connect(const vector<string>& tokens, const GateType& t);
	CirGate* createGateRec(const vector<string>& tokens, const GateType& t, unsigned i);
	
//	in cirReport.cpp
	void reportGateAll() const;
	void reportNetList() const;
	void reportNetListRec(CirGate* g, unsigned level) const;
	void reportTopoList() const;
   void reportSimV() const;
   void reportPi() const;
   void reportPo() const;

//	in cirSat.cpp
	//void reassignId() const;
	void createVar(SatSolverV* s, int solver = 0) const;   // 0: setVar, 1: setCandVar
	void addToSolver(SatSolverV* s, int solver = 0) const;   // 0: getVar, 1: getCandVar

private:
	string								_name;
	unsigned 							_id;
	CirGate*							_const0;
	CirGate*							_const1;
    // do not use _gateList to do anything !!!
	GateList							_gateList;			
	GateList							_piList, _poList;
	GateList							_errorList;
	name2GateMap					    _name2GateMap;	
	mutable GateList					_topoList;
    mutable GateList                    _totGateList;
};




#endif
