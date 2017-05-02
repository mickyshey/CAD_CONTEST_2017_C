// File name [ cirMgr.h ]
#ifndef CIRMGR_H
#define CIRMGR_H

#include <iostream>
#include <string>
#include <map>

#include "cir/cirNet.h"

using namespace std;

typedef map<CirGate*, Var> VarMap;

class CirMgr
{
public:
	explicit CirMgr(const string& name = ""): _name(name), _F(NULL), _G(NULL), _patch(NULL), _out(NULL) {}
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

//      in cirProof.cpp
        void checkPo();
        void genProofModel(SatSolver& solver);

//      in cirMgr.cpp
        CirNet* miterCkt(CirNet* f, CirNet* g) { return _out;} // FIXME


private:
	string 								_name;
	CirNet*								_F;
	CirNet*								_G;
	CirNet*								_patch;
        CirNet*                                                         _out;
};

#endif
