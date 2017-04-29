// File name [ cirGate.h ]

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <vector>
#include <string>

#include "minisat/sat.h"

using namespace std;

class CirGate;
class CirGateV;

enum GateType
{
	Gate_Const,
	Gate_Pi,
	Gate_Po,
	Gate_Buf,
	Gate_Inv,
	Gate_And,
	Gate_Or,
	Gate_Nand,
	Gate_Nor,
	Gate_Xor,
	Gate_Xnor,
	Gate_Error,

	Gate_UNDEF
};

typedef vector<CirGateV> GateVList;
typedef vector<CirGate*> GateList;
class CirGateV
{
public:
	explicit CirGateV(CirGate* g = 0, bool inv = false): _gateV(reinterpret_cast<size_t>(g)) {
		if( inv ) setInv();
	}
	~CirGateV() {}
	
	CirGate* getGate() { return (CirGate*)(_gateV & PTR_MASK); }
	bool isInv() const { return _gateV & INV_MASK; }
	void setInv() { _gateV |= INV_MASK; }
	void flipInv() { _gateV ^= INV_MASK; }

private:
        static const size_t INV_MASK = 0x1;
        static const size_t EDGE_BIT = 2;
        static const size_t PTR_MASK = (~(size_t(0)) >> EDGE_BIT) << EDGE_BIT;
	size_t _gateV;
};

class CirGate
{
public:
	CirGate(const string& name = "", unsigned id = 0): 
		_name	(name),
		_id		(id),
		_ref	(0) {}
	virtual ~CirGate() {}

	//virtual void getCNF(SatSolver* s) const = 0;

//	gate info
	void setId(unsigned i) 			{ _id = i; }
	unsigned getId() 				{ return _id; }
	const string& getName() 		{ return _name; }
	virtual const GateType getType() const = 0;

//	gate io
	void setFaninSize(unsigned s) 					{ _in.resize(s); }
	void setFanin(CirGateV gateV, unsigned idx) 	{ _in[idx] = gateV; }
	void pushBackFanin(CirGateV gateV) 				{ _in.push_back(gateV); }
	unsigned getFaninSize() 						{ return _in.size(); }
	CirGate* getFanin(unsigned idx) 				{ return _in[idx].getGate(); }
	void setFanoutSize(unsigned s) 					{ _out.resize(s); }
	void setFanout(CirGateV gateV, unsigned idx) 	{ _out[idx] = gateV; }
	void pushBackFanout(CirGateV gateV) 			{ _out.push_back(gateV); }
	unsigned getFanoutSize() 						{ return _out.size(); }
	CirGate* getFanout(unsigned idx) 				{ return _out[idx].getGate(); }

//	dfs traversal
	static void incRef() 	{ ++_globalRef; }
	void setToRef() 		{ _ref = _globalRef; }
	bool isRef() 			{ return _ref == _globalRef; }

//	report
	virtual void report() const = 0;

protected:
	string					_name;
	unsigned				_id;
	GateVList				_in;
	GateVList				_out;	
	static unsigned 		_globalRef;
	mutable unsigned		_ref;
};

#define CirGateType(T) class T : public CirGate \
{ \
public: \
	explicit T(const string& name = "", unsigned id = 0): CirGate(name, id) {} \
	~T() {} \
	const GateType getType() const;\
	void report() const;\
};

CirGateType(CirConstGate);
CirGateType(CirPiGate);
CirGateType(CirPoGate);
CirGateType(CirBufGate);
CirGateType(CirInvGate);
CirGateType(CirAndGate);
CirGateType(CirOrGate);
CirGateType(CirNandGate);
CirGateType(CirNorGate);
CirGateType(CirXorGate);
CirGateType(CirXnorGate);
CirGateType(CirErrorGate);

#endif
