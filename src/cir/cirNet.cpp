// File name [ cirNet.cpp ]

#include <iostream>
#include <unordered_set>
#include <string>

#include "cir/cirNet.h"

using namespace std;

CirNet::~CirNet()
{
	if( _const0 ) delete _const0;
	if( _const1 ) delete _const1;
	for( unsigned i = 0; i < _gateList.size(); ++i )
		delete _gateList[i];
	for( unsigned i = 0; i < _piList.size(); ++i )
		delete _piList[i];
	for( unsigned i = 0; i < _poList.size(); ++i )
		delete _poList[i];
	for( unsigned i = 0; i < _errorList.size(); ++i )
		delete _errorList[i];
}

void
CirNet::init()
{
	if( _const0 ) delete _const0;
	if( _const1 ) delete _const1;
	for( unsigned i = 0; i < _gateList.size(); ++i )
		delete _gateList[i];
	for( unsigned i = 0; i < _piList.size(); ++i )
		delete _piList[i];
	for( unsigned i = 0; i < _poList.size(); ++i )
		delete _poList[i];
	for( unsigned i = 0; i < _errorList.size(); ++i )
		delete _errorList[i];
	_gateList.clear();
	_piList.clear();
	_poList.clear();
	_errorList.clear();
	_name2GateMap.clear();
}

CirGate*
CirNet::createGate(const GateType& t, const string& name, unsigned id)
{
	if( t == Gate_Const ) {							
		CirConstGate* g = new CirConstGate(name, id);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Pi ) {
		CirPiGate* g = new CirPiGate(name, id);
		_piList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Po ) {							
		CirPoGate* g = new CirPoGate(name, id);
		_poList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Buf ) {
		CirBufGate* g = new CirBufGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Inv ) {
		CirInvGate* g = new CirInvGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_And ) {
		CirAndGate* g = new CirAndGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Or ) {
		CirOrGate* g = new CirOrGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Nand ) {
		CirNandGate* g = new CirNandGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Nor ) {
		CirNorGate* g = new CirNorGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Xor ) {
		CirXorGate* g = new CirXorGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Xnor ) {
		CirXnorGate* g = new CirXnorGate(name, id);
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Error ) {
		CirErrorGate* g = new CirErrorGate(name, id);
		_errorList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	if( t == Gate_Mux ) {
		CirMuxGate* g = new CirMuxGate(name, id);
		// where should the MUX gate be placed ?
		_gateList.push_back(g);
		_name2GateMap[name] = g;
		return g;
	}
	assert(0);
	return NULL;
}

void
CirNet::createConst(bool val)
{
	if( val ) {
		_const1 = createGate(Gate_Const, "1'b1", 1);
	}
	else _const0 = createGate(Gate_Const, "1'b0", 0);
}

const GateList&
CirNet::buildTopoList() const
{
	CirGate::incRef();
	_topoList.clear();
	_const0 -> setToRef();
	_const1 -> setToRef();
	_topoList.push_back(_const0);		// always 0 at 0, 1 at 1
	_topoList.push_back(_const1);
	for( unsigned i = 0; i < _poList.size(); ++i )
		buildTopoListRec(_poList[i]);
	return _topoList;
}

void
CirNet::buildTopoListRec(CirGate* g) const
{
	if( g -> isRef() ) return;
	g -> setToRef();
	for( unsigned i = 0; i < g -> getFaninSize(); ++i )
		buildTopoListRec(g -> getFanin(i));
	_topoList.push_back(g);
}

const GateList&
CirNet::dfsFromGate(CirGate* g) const
{
	std::cout << "using dfsFromGate ..." << std::endl;
/*
    CirGate::incRef();
    GateList list;
    dfsFromGateRec(g, list);
    return list;
*/
}
void
CirNet::dfsFromGateRec(CirGate* g, GateList& list, std::unordered_set<std::string>& nameHash) const
{
    if( g -> isRef() ) return;
    g -> setToRef();
    for( unsigned i = 0; i < g -> getFaninSize(); ++i )
        dfsFromGateRec(g -> getFanin(i), list, nameHash);
    list.push_back(g);
	nameHash.insert(g -> getName());	
}

const GateList&
CirNet::totGateList() const
{
	// remember to clear _totGateList first
	_totGateList.clear();

	std::unordered_set<std::string> nameHash;
    CirGate::incRef();
	_const0 -> setToRef();
	_const1 -> setToRef();
    _totGateList.push_back(_const0);
    _totGateList.push_back(_const1);
	
	nameHash.insert(_const0 -> getName());
	nameHash.insert(_const1 -> getName());
    for( unsigned i = 0; i < _poList.size(); ++i ) dfsFromGateRec(_poList[i], _totGateList, nameHash);
    for( std::map<string, CirGate*>::const_iterator it = _name2GateMap.begin(); it != _name2GateMap.end(); ++it ) {
/*
        bool find = false;
        CirGate* g = it -> second;
        for( unsigned i = 0; i < _totGateList.size(); ++i ) {
           if(g == _totGateList[i]) find = true;
        }
        if(!find) dfsFromGateRec(g, _totGateList);
*/
		std::string name = it -> first;
		if( nameHash.find(name) == nameHash.end() ) {
			dfsFromGateRec(it -> second, _totGateList, nameHash);
		}
    }
    return _totGateList;
}

void
CirNet::deletePI(CirGate* g)
{
   assert(_name2GateMap.find(g -> getName()) != _name2GateMap.end());
   _name2GateMap.erase(_name2GateMap.find(g -> getName()));
   for( unsigned i = 0; i < _piList.size(); ++i ) {
      if( _piList[i] == g ) {
         _piList.erase(_piList.begin() + i);
         return;
      }
   }
   assert(0);
}

void
CirNet::removeInvBuf(std::vector<std::string>& nameList)
{
	std::unordered_set<std::string> nameHash;
	if( nameList.size() ) {
		for( unsigned i = 0; i < nameList.size(); ++i ) {
			nameHash.insert(nameList[i]);
		}
	}

	totGateList();
	unsigned count = 0;
	for( unsigned i = 0; i < _totGateList.size(); ++i ) {
		const GateType type = _totGateList[i] -> getType();
		if( type == Gate_Inv || type == Gate_Buf ) {
/*
			std::cout << "simplifing: " << _totGateList[i] -> getName() << std::endl;
			std::cout << "fanout info: " << std::endl;
			for( unsigned j = 0; j < _totGateList[i] -> getFanoutSize(); ++j ) {
				std::cout << _totGateList[i] -> getFanout(j) -> getName() << " ";
			}
			std::cout << std::endl;
			std::cout << "fanin info: " << std::endl;
			CirGate* tmpIn = _totGateList[i] -> getFanin(0);
			std::cout << tmpIn -> getName() << "(" << _totGateList[i] -> isFaninInv(0) << ")" << std::endl;
			std::cout << "current fanin's fanout: " << std::endl;
			for( unsigned j = 0; j < tmpIn -> getFanoutSize(); ++j )
				std::cout << tmpIn -> getFanout(j) -> getName() << " ";
			std::cout << std::endl;
*/
			assert(_totGateList[i] -> getFaninSize() == 1 );
			if( nameList.size() && _totGateList[i] -> getWeight() ) {
				assert(nameHash.find(_totGateList[i] -> getName()) != nameHash.end());
				// update the weight of fanin gate if necessary
				CirGate* tmpG = _totGateList[i] -> getFanin(0);
				if( _totGateList[i] -> getWeight() < tmpG -> getWeight() ) {
					tmpG -> setWeight(_totGateList[i] -> getWeight());
               tmpG -> setBestGateName(_totGateList[i] -> getName());
            }
            // record GateType and name for patch modification
            tmpG -> pushBackTypeNamePair(_totGateList[i] -> getType(), _totGateList[i] -> getName());
				// delete the name in nameHash
				nameHash.erase(nameHash.find(_totGateList[i] -> getName()));
			}

			// update the connection 
			CirGateV inV = _totGateList[i] -> getFaninV(0);
			// flipInv if gateType is Gate_Inv
			if( type == Gate_Inv ) inV.flipInv();
			CirGate* in = inV.getGate();
			// delete the fanout pointing to _totGateList[i]
			for( unsigned j = 0; j < in -> getFanoutSize(); ++j ) {
				if( in -> getFanout(j) == _totGateList[i] ) { in -> eraseFanout(j); break; }
			}
			in -> reserveFanoutSize(in -> getFanoutSize() + _totGateList[i] -> getFanoutSize());
			for( unsigned j = 0; j < _totGateList[i] -> getFanoutSize(); ++j ) {
				CirGateV outV = _totGateList[i] -> getFanoutV(j);
				CirGate* out = outV.getGate();
				// get the corresponding fanin idx to update
				unsigned inIdx = 0;
				if( out -> getFaninSize() > 1 ) {
					assert(out -> getFaninSize() == 2);
					if( out -> getFanin(1) == _totGateList[i] ) inIdx = 1;
					assert(out -> getFanin(inIdx) == _totGateList[i]);
				}
				assert(!out -> isFaninInv(inIdx));
				out -> setFanin(inV, inIdx);
				in -> pushBackFanout(CirGateV(out, false));
			} 
			// delete the gate
			assert(_name2GateMap.find(_totGateList[i] -> getName()) != _name2GateMap.end());
			_name2GateMap.erase(_name2GateMap.find(_totGateList[i] -> getName()));
			delete _totGateList[i];
			++count;
		}
	}
	
	// update the _candNameList
	if( nameList.size() ) {
		std::vector<std::string> tmpList;
		tmpList.reserve(nameHash.size());
		std::unordered_set<std::string>::iterator it;
		for( it = nameHash.begin(); it != nameHash.end(); ++it ) tmpList.push_back(*it);
		nameHash.clear();
		nameList.swap(tmpList);
		tmpList.clear();

      // report the removed gate
      /* for( unsigned i = 0; i < nameList.size(); ++i ) { */
      /*    CirGate* g = getGateByName(nameList[i]); */
      /*    std::cout << g -> getName() << "  " << g -> getRemovedGateSize() << std::endl; */
      /* } */
	}

	std::cout << "# gates deleted: " << count << endl;
}

void
CirNet::sweep()
{
    /*
        cout << "// Sweeping..." << endl;
        for(size_t i = 0; i < _topoList.size(); i++) {
            CirGate* g = _topoList[i];
            GateType type = g.getType();
            if(type != Gate_Buf && type != Gate_Inv) {
                for(unsigned j = 0; j < g -> getFaninSize(); j++) {
                    bool isInv = false;
                    CirGate* in = g -> getFanin(j);

                }
            }
        }
    */
}

CirGate*
CirNet::createMux(CirGate* g_F, CirGate* g_dupF)
{
   
	assert(g_F && g_dupF);
	CirGate* mux = createGate(Gate_Mux, g_F -> getName() + "_mux");
	CirGate* sel = createGate(Gate_Pi, g_F -> getName() + "_sel");
	mux -> setFaninSize(3);
	mux -> setFanin(CirGateV(g_F, false), 0);
	// create a XNOR gate connecting g_F & g_dupF
	CirGate* xnor = createGate(Gate_Xnor, g_F -> getName() + "_xnor");
	xnor -> setFaninSize(2);
	xnor -> setFanin(CirGateV(g_F, false), 0); 
	xnor -> setFanin(CirGateV(g_dupF, false), 1); 
	assert(xnor -> getFaninSize() == 2);
	mux -> setFanin(CirGateV(xnor, false), 1);
	mux -> setFanin(CirGateV(sel, false), 2);
	for( unsigned i = 0; i < g_F -> getFanoutSize(); ++i ) {
		CirGate* out = g_F -> getFanout(i);
		//cout << "fanout type: " << out -> getType() << endl;
		mux -> pushBackFanout(CirGateV(out, false));
		// Po, Buf, Inv
		if( out -> getFaninSize() == 1 ) {
			assert(out -> getFanin(0) == g_F);
			out -> setFanin(CirGateV(mux, false), 0);
		}
		else {
			assert(out -> getFaninSize() == 2 );
			if(out -> getFanin(0) == g_F) out -> setFanin(CirGateV(mux, false), 0);
			else out -> setFanin(CirGateV(mux, false), 1);
		}
	}
	g_F -> clearFanout();
	g_F -> pushBackFanout(CirGateV(mux, false));
	g_F -> pushBackFanout(CirGateV(xnor, false));
	unsigned numFanout = g_dupF -> getFanoutSize();
	g_dupF -> pushBackFanout(CirGateV(xnor, false));
	assert(g_dupF -> getFanoutSize() == numFanout + 1);
	xnor -> pushBackFanout(CirGateV(mux, false));
	sel -> pushBackFanout(CirGateV(mux, false));
	assert(g_F -> getFanoutSize() == 2);
	
	return mux;
}

void
CirNet::randSim()
{
   // we perform simulation after createVar, meaning that we have _topoList constructed already
   assert(_topoList.size());
   for( unsigned i = 0; i < _topoList.size(); ++i ) {
      _topoList[i] -> simulate();
   }
}

void
CirNet::knownSim()
{
   // the one performing knownSim should always be golden, i.e. no Gate_Error
   assert(_topoList.size());
   for( unsigned i = 0; i < _topoList.size(); ++i ) {
      if( _topoList[i] -> getType() == Gate_Pi ) continue;
      _topoList[i] -> simulate();
   }
}

void
CirNet::maintainPoList(GateList& tmpPoList)
{
   GateList tmpList;
   for( unsigned i = 0; i < tmpPoList.size(); ++i ) {
      assert( _name2GateMap.find(tmpPoList[i] -> getName()) != _name2GateMap.end() );
      CirGate* g = _name2GateMap.at(tmpPoList[i] -> getName());
      tmpList.push_back(g);
   }
   assert(tmpPoList.size() == tmpList.size());
   _poList.swap(tmpList);
   tmpList.clear();
}
