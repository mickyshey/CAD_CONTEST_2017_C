// File name [ cirGate.h ]

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <vector>
#include <string>

//#include "minisat/sat.h"
#include "sat/sat.h"

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
    Gate_Mux,

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
    
    CirGate* getGate() const    { return (CirGate*)(_gateV & PTR_MASK); }
    //CirGate* getGate() const  { return (CirGate*)(_gateV & ~(size_t)INV_MASK); }
    bool isInv() const          { return (bool)(_gateV & (size_t)INV_MASK); }
    void setInv()               { _gateV |= (size_t)INV_MASK; }
    void flipInv()              { _gateV ^= (size_t)INV_MASK; }
    bool operator != (CirGateV gateV) { return ((*this).getGate() != gateV.getGate()) || ((*this).isInv() != gateV.isInv()); }
    bool operator == (CirGateV gateV) { return ((*this).getGate() == gateV.getGate()) && ((*this).isInv() == gateV.isInv()); }

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
        _name       (name),
        _id         (id),
        _var        (0),
        _costVar    (0),
        _candVar    (0),
        _impVar     (0),
        _weight     (0),
        _simV       (0),
        _ref        (0) {}
    virtual ~CirGate() {}

    virtual void addToSolver(SatSolverV* s, int solver = 0) const = 0;

//  gate info
    void setId(unsigned i)                      { _id = i; }
    unsigned getId()                            { return _id; }
    const string& getName()                     { return _name; }
    void setWeight(unsigned w)                  { _weight = w; }
    unsigned getWeight()                        { return _weight; }
    virtual const GateType getType() const = 0;

    // for patch modification
    std::string getBestGateName()               { return _bestGateName; }
    std::vector<std::pair<GateType, std::string> >& getRemovedGate() { return _removedGate; }
    void setBestGateName(const std::string s)   { _bestGateName = s; }
    void pushBackTypeNamePair(const GateType& t, const string& s)  { _removedGate.push_back({t, s}); }
    const std::pair<GateType, std::string>& getTypeNamePair(unsigned idx)  { return _removedGate[idx]; }
    unsigned getRemovedGateSize() const         { return _removedGate.size(); }
    void swapRemovedGateAndCloneBestGateName(CirGate* g)  { _removedGate.swap(g -> getRemovedGate()); _bestGateName = g -> getBestGateName();}
    // bool isConst0() { return (getType() == Gate_Const) && (!(CirGateV)*this.isInv()); }
    // bool isConst1() { return (getType() == Gate_Const) && ((CirGateV)*this.isInv()); }

//  gate io
    void setFaninSize(unsigned s)                   { _in.resize(s); }
    void reserveFaninSize(unsigned s)               { _in.reserve(s); }

    //we should always setFaninSize first to ensure idx is within size
    void setFanin(CirGateV gateV, unsigned idx)     { assert(idx < _in.size()); _in[idx] = gateV; }
    void pushBackFanin(CirGateV gateV)              { _in.push_back(gateV); }
    unsigned getFaninSize() const                       { return _in.size(); }
    CirGateV getFaninV(unsigned idx) const          { assert(idx < _in.size()); return _in[idx]; }
    CirGate* getFanin(unsigned idx) const           { assert(idx < _in.size()); return _in[idx].getGate(); }
    unsigned getFaninId(unsigned idx) const         { assert(idx < _in.size()); return _in[idx].getGate() -> _id; }
    bool isFaninInv(unsigned idx) const             { assert(idx < _in.size()); return _in[idx].isInv(); }
    void setFanoutSize(unsigned s)                  { _out.resize(s); }
    void reserveFanoutSize(unsigned s)              { _out.reserve(s); }
    void clearFanout()                                  { _out.clear(); }
    void eraseFanout(unsigned idx)                  { _out.erase(_out.begin() + idx); }
    void setFanout(CirGateV gateV, unsigned idx)    { assert(idx < _out.size()); _out[idx] = gateV; }
    void pushBackFanout(CirGateV gateV)             { _out.push_back(gateV); }
    unsigned getFanoutSize() const                  { return _out.size(); }
    CirGateV getFanoutV(unsigned idx) const         { assert(idx < _out.size()); return _out[idx]; }
    CirGate* getFanout(unsigned idx) const          { assert(idx < _out.size()); return _out[idx].getGate(); }

// SAT
    Var getFaninVar(unsigned idx) const             { assert(idx < _in.size()); return _in[idx].getGate() -> _var; }
    Var getFaninCandVar(unsigned idx) const         { assert(idx < _in.size()); return _in[idx].getGate() -> _candVar; }
    Var getFaninRMVar(unsigned idx) const           { assert(idx < _in.size()); return _in[idx].getGate() -> _rmVar; }
    void setVar(Var v)                              { _var = v; }
    Var  getVar()                                   { return _var; }
    void setCostVar(Var v)                          { _costVar = v; }
    Var  getCostVar()                               { return _costVar; }
    void setCandVar(Var v)                          { _candVar = v; }
    Var  getCandVar()                               { return _candVar; }
    void setImpVar(Var v)                           { _impVar = v; }
    Var  getImpVar()                                { return _impVar; }
    void setRMVar(Var v)                            { _rmVar = v; }
    Var  getRMVar()                                 { return _rmVar; }

//  dfs traversal
    static void incRef()            { ++_globalRef; }
    void setToRef()                 { _ref = _globalRef; }
    bool isRef()                    { return _ref == _globalRef; }

// simulation
    virtual void simulate() = 0;
    size_t getSimV() { return _simV; }
    void setSimV(size_t s) { _simV = s; }
    size_t getFaninSimV(unsigned idx) const { assert(idx < _in.size()); return _in[idx].getGate() -> _simV; }

//  report
    virtual void report() const = 0;

protected:
    string                  _name;
    unsigned                _id;
    Var                     _var;
    Var                     _rmVar; // for solving new rv assignment
    Var                     _costVar;
    Var                     _candVar;
    Var                     _impVar;
    GateVList               _in;
    GateVList               _out;
    unsigned                _weight;
    static unsigned         _globalRef;
    mutable unsigned        _ref;
    size_t                  _simV;

    // for patch modification
    std::vector<std::pair<GateType, std::string> >    _removedGate;
    std::string            _bestGateName;
};

#define CirGateType(T) class T : public CirGate \
{ \
public: \
    explicit T(const string& name = "", unsigned id = 0): CirGate(name, id) {} \
    ~T() {} \
    const GateType getType() const;\
    void report() const;\
    void addToSolver(SatSolverV* s, int solver = 0) const;\
    void simulate();\
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
CirGateType(CirMuxGate);

#endif
