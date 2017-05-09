// File name [ cirMgr.h ]
#ifndef CIRMGR_H
#define CIRMGR_H

#include <iostream>
#include <string>
#include <map>

#include "cir/cirNet.h"
#include "minisat/Solver.h"

using namespace std;

typedef map<CirGate*, Var> VarMap;

class CirMgr
{
public:
	explicit CirMgr(const string& name = ""): _name(name), _F(NULL), _G(NULL), _patch(NULL) { _s = new SatSolver; }
	~CirMgr() {}//{ delete _F; delete _G; delete _patch; }

//	info
	void setName(const string& n ) 	{ _name = n; }
	CirNet* getF() 					{ return _F; }
	CirNet* getG() 					{ return _G; }
	CirNet* getPath() 				{ return _patch; }

//	in cirParse.cpp
	bool readVerilog(const string& filename, bool isF);
	bool removeComment(const string& inFileName, const string& outFileName) const;
    bool addWeight(const string& fileName);

//	in cirDup.cpp
	CirNet* dupNet(CirNet* n) const;
	CirNet* connectXor(CirNet* f, CirNet* g);	// we shouldn't create the whole XOR net, instead, we simply add the XOR cnf constraint to the solver

//  test
	void test();

//	SAT related function
	void addToSolver(CirNet* n) { n -> addToSolver(_s); }
	void tiePi(CirNet* f, CirNet* g);					// _F = _G, _dupF = _dupG
	void addXorConstraint(CirNet* f, CirNet* g);		// _F ^ _G, _dupF ^ _dupG
	void addErrorConstraint(CirNet* n, bool val);		// _F(t = 0), _dupF(t = 1)

//      in cirProof.cpp
        void checkPo();
        void genProofModel(SatSolver& solver);

private:
	string 								_name;
	CirNet*								_F;
	CirNet*								_G;
	CirNet*								_dupF;
	CirNet*								_dupG;
	CirNet*								_patch;
	SatSolver*							_s;
        CirNet*                                                         _out;
};

#endif
