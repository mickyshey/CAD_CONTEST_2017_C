// File name [ cirNet.h ]
#ifndef CIRNET_H
#define CIRNET_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <map>

#include "cir/cirGate.h"
#include "minisat/sat.h"

using namespace std;

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
	void setName(const string& n ) 					        { _name = n; }
	const string& getName() 								{ return _name; }
    unsigned getPiNum()                                     { return _piList.size(); }
    unsigned getPoNum()                                     { return _poList.size(); }
    unsigned getGateNum()                                   { return _gateList.size(); }
    CirGate* getConstGate(bool inv)                         { if(inv) return _const0; else return _const1; }
	CirGate* getPo(unsigned idx)							{ return _poList[idx]; }
    CirGate* getGate(unsigned id)                           { return _gateList[id]; }
	CirGate* getGateByName(const string& n) 				{ return _name2GateMap[n]; }

//	in cirNet.cpp
	void init();
	CirGate* createGate(const GateType& t, const string& name = "", unsigned id = 0);
	void createConst(bool is0);
	const GateList& buildTopoList() const;			// from PI to PO
	void buildTopoListRec(CirGate* g) const;
	//CirNet* constructNet();
    void sweep();

//	in cirParse.cpp
	bool parse(const string& filename);
	void connect(const vector<string>& tokens, const GateType& t);
	CirGate* createGateRec(const vector<string>& tokens, const GateType& t, unsigned i);
	
//	in cirReport.cpp
	void reportGateAll() const;
	void reportNetList() const;
	void reportNetListRec(CirGate* g, unsigned level) const;
	void reportTopoList() const;

//	in cirSat.cpp
	//void reassignId() const;
	void addToSolver(SatSolver* s) const;

private:
	string								_name;
	unsigned 							_id;
	CirGate*							_const0;
	CirGate*							_const1;
	GateList							_gateList;			
	GateList							_piList, _poList;
	GateList							_errorList;
	unordered_map<string, CirGate*>		_name2GateMap;	
	mutable GateList					_topoList;
};




#endif
