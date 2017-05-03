// File name [ cirParse.cpp ]
#include <iostream>
#include <fstream>
#include <cassert>

#include "cir/cirMgr.h"
#include "util/parse.h"

using namespace std;

bool
CirMgr::readVerilog(const string& filename, bool isF)
{
	string tmpFilename = filename + ".tmp";
  	if (!removeComment(filename, tmpFilename)) return false;
	CirNet* n = new CirNet;
  	if (!n -> parse(tmpFilename)) return false;
	if( isF ) _F = n;
	else _G = n;
	
	return true;
}

bool CirMgr::removeComment(const std::string& inFilename,
                           const std::string& outFilename) const {
  std::ifstream ifs(inFilename.c_str());
  // cannot open file
  if (ifs.fail()) {
    std::cerr << "Cannot open file \"" << inFilename << "\"!" << std::endl;
    return false;
  }
  std::ofstream ofs(outFilename.c_str());
  // cannot open file
  if (ofs.fail()) {
    std::cerr << "Cannot open file \"" << outFilename << "\"!" << std::endl;
    return false;
  }
  // start reading
  char c, prev = '\0';
  bool lineCommentFlg = false, blockCommentFlg = false;
  while (!ifs.eof()) {
    ifs.get(c);
    // inside line comment
    if (lineCommentFlg) {
      if (c == '\n') {
        lineCommentFlg = false;
        ofs << std::endl;
      }
    } else if (blockCommentFlg) {
      if (prev == '*' && c == '/')
        blockCommentFlg = false;
    } else if (c == '/' && ifs.peek() == '/') {
      lineCommentFlg = true;
    } else if (c == '/' && ifs.peek() == '*') {
      blockCommentFlg = true;
    } else {
      ofs << c;
    }
    prev = c;
  }
  ifs.close();
  ofs.close();
  return true;
}

bool CirNet::parse(const std::string& filename) {
  std::ifstream ifs(filename.c_str());
  std::string str;
  std::vector<std::string> tokens;
	vector<string> outputs;
  init();
	// set const 0 and 1
  _const0 = createGate(Gate_Const, "1'b0", 0);		// const 0 id: 0
  _const1 = createGate(Gate_Const, "1'b1", 1);		// const 1 id: 1
  unsigned wireCount = 0;
  unsigned id = 1;
  // perform 2-phase reading
  while (!ifs.eof()) {								// construct gate
    str = readUntil(ifs, ';');
    tokens = split(str, " ,;()\t\r\n");
    unsigned n = tokens.size();
    if (n == 0) continue;
    if (tokens[0] == "module") {
      // module <module name> <name0> <name1> ...
      assert(n >= 2);
      setName(tokens[1]);
    } else if(tokens[0] == "input") {
      // input <name0> <name1> ...
      assert(n >= 2);
      for (unsigned i = 1; i < n; ++i)
		createGate(Gate_Pi, tokens[i], ++id);
    } else if(tokens[0] == "output") {
      // output <name0> <name1> ...
      assert(n >= 2);
      for (unsigned i = 1; i < n; ++i)
		outputs.push_back(tokens[i]);
    } else if (tokens[0] == "wire") {				// do nothing except t_xx
      assert(n >= 2 && wireCount <= 2);
	  ++wireCount;
	  if( wireCount == 2 ) {
      	for (unsigned i = 1; i < n; ++i)
          createGate(Gate_Error, tokens[i], ++id);
	  }
    } else if (tokens[0] == "endmodule") {
      // endmodule
      break;
    } else if(tokens[0] == "buf") {
      assert(n == 3);
	  createGate(Gate_Buf, tokens[1], ++id);
    } else if(tokens[0] == "not") {
	  assert(n == 3);
	  createGate(Gate_Inv, tokens[1], ++id);
	} else if(tokens[0] == "and") {
      assert(n >= 4);				
	  createGate(Gate_And, tokens[1], ++id);
	} else if(tokens[0] == "or") {
      assert(n >= 4);			
	  createGate(Gate_Or, tokens[1], ++id);
	} else if(tokens[0] == "nand") {
      assert(n >= 4);		
	  createGate(Gate_Nand, tokens[1], ++id);
	} else if(tokens[0] == "nor") {
      assert(n >= 4);	
	  createGate(Gate_Nor, tokens[1], ++id);
	} else if(tokens[0] == "xor") {
      assert(n >= 4);
	  createGate(Gate_Xor, tokens[1], ++id);
	} else if(tokens[0] == "xnor") {
      assert(n >= 4);		
	  createGate(Gate_Xnor, tokens[1], ++id);
	}
  }
  for( unsigned i = 0; i < outputs.size(); ++i ) {			// connect po to gate
    CirGate* po = createGate(Gate_Po, outputs[i] + "_po", ++id);
	po -> setFaninSize(1);
	CirGate* in0 = _name2GateMap[outputs[i]];
	po -> setFanin(CirGateV(in0, false), 0);
  }
  ifs.close();
  ifs.open(filename.c_str());
  while (!ifs.eof()) {										// start connecting
    str = readUntil(ifs, ';');
    tokens = split(str, " ,;()\t\r\n");
    unsigned n = tokens.size();
    if (n == 0) continue;
    if (tokens[0] == "endmodule") {
      break;
    } else if(tokens[0] == "buf") {
      // buf (<name0>, <name1>) 
      assert(n == 3);
	  CirGate* buf = _name2GateMap[tokens[1]];
	  buf -> setFaninSize(1);
      CirGate* in0 = _name2GateMap[tokens[2]];
	  buf -> setFanin(CirGateV(in0, false), 0);
	  in0 -> pushBackFanout(CirGateV(buf, false));
    } else if(tokens[0] == "not") {
	  // not (<name0>, <name1>)
	  assert(n == 3);
	  CirGate* inv = _name2GateMap[tokens[1]];
	  inv -> setFaninSize(1);
	  CirGate* in0 = _name2GateMap[tokens[2]];
      inv -> setFanin(CirGateV(in0, false), 0);
	  in0 -> pushBackFanout(CirGateV(inv, false));
	} else if(tokens[0] == "and") {
      // and (<name0>, <name1>, <name2>, <name3>, ...) 
      assert(n >= 4);				
	  CirGate* andGate = _name2GateMap[tokens[1]];
	  andGate -> setFaninSize(n - 2);
	  for( unsigned i = 2; i < n; ++i ) {
      	CirGate* in = _name2GateMap[tokens[i]];
	  	andGate -> setFanin(CirGateV(in, false), i - 2);
		in -> pushBackFanout(CirGateV(andGate, false));
	  }
	} else if(tokens[0] == "or") {
      // or (<name0>, <name1>, <name2>, <name3>, ...) 
      assert(n >= 4);			
	  CirGate* orGate = _name2GateMap[tokens[1]];
	  orGate -> setFaninSize(n - 2);
	  for( unsigned i = 2; i < n; ++i ) {
      	CirGate* in = _name2GateMap[tokens[i]];
	  	orGate -> setFanin(CirGateV(in, false), i - 2);
		in -> pushBackFanout(CirGateV(orGate, false));
	  }
	} else if(tokens[0] == "nand") {
      // nand (<name0>, <name1>, <name2>, <name3>, ...) 
      assert(n >= 4);		
	  CirGate* nandGate = _name2GateMap[tokens[1]];
	  nandGate -> setFaninSize(n - 2);
	  for( unsigned i = 2; i < n; ++i ) {
      	CirGate* in = _name2GateMap[tokens[i]];
	  	nandGate -> setFanin(CirGateV(in, false), i - 2);
		in -> pushBackFanout(CirGateV(nandGate, false));
	  }
	} else if(tokens[0] == "nor") {
      // nor (<name0>, <name1>, <name2>, <name3>, ...) 
      assert(n >= 4);			
	  CirGate* norGate = _name2GateMap[tokens[1]];
	  norGate -> setFaninSize(n - 2);
	  for( unsigned i = 2; i < n; ++i ) {
      	CirGate* in = _name2GateMap[tokens[i]];
	  	norGate -> setFanin(CirGateV(in, false), i - 2);
		in -> pushBackFanout(CirGateV(norGate, false));
	  }
	} else if(tokens[0] == "xor") {
      // xor (<name0>, <name1>, <name2>, <name3>, ...) 
      assert(n >= 4);		
	  CirGate* xorGate = _name2GateMap[tokens[1]];
	  xorGate -> setFaninSize(n - 2);
	  for( unsigned i = 2; i < n; ++i ) {
      	CirGate* in = _name2GateMap[tokens[i]];
	  	xorGate -> setFanin(CirGateV(in, false), i - 2);
		in -> pushBackFanout(CirGateV(xorGate, false));
	  }
	} else if(tokens[0] == "xnor") {
      // xnor (<name0>, <name1>, <name2>, <name3>, ...) 
      assert(n >= 4);	
	  CirGate* xnorGate = _name2GateMap[tokens[1]];
	  xnorGate -> setFaninSize(n - 2);
	  for( unsigned i = 2; i < n; ++i ) {
      	CirGate* in = _name2GateMap[tokens[i]];
	  	xnorGate -> setFanin(CirGateV(in, false), i - 2);
		in -> pushBackFanout(CirGateV(xnorGate, false));
	  }
	}
  }
  ifs.close();
  return true;
}

bool
CirMgr::addWeight(const string& fileName)
{
    std::ifstream ifs(fileName.c_str());
    std::string str;
    std::vector<std::string> tokens;
    bool validWeight = false;

    if(ifs.fail()) {
        cerr << "Cannot open file \"" << fileName <<  "\"!!" << endl;
        return false;
    }
    while(!ifs.eof()) {
        str = readUntil(ifs, '\n');
        tokens = split(str, " \r\n");
        if(tokens.size() < 2) break;
        CirGate* gate = _F->getGateByName(tokens[0]);
        // gate->report();
        int num;
        validWeight = str2Int(tokens[1], num);
        if(validWeight) gate->setWeight(num);
    }
    return validWeight;
}
