// File name [ main.cpp ]
#include <iostream>

#include "cir/cirMgr.h"

using namespace std;

int main(int argc, char** argv) {
	CirMgr cirMgr;
	cirMgr.readVerilog(argv[1], 1);			// 1: for F
	cirMgr.readVerilog(argv[2], 0);			// 0: for G
	CirNet* f = cirMgr.getF();
	CirNet* g = cirMgr.getG();
	f -> reportNetList();
	f -> buildTopoList();
	f -> reportTopoList();
	CirNet* dup = cirMgr.dupNet(f);
	cout << "=====" << endl;
	dup -> reportNetList();
	
}

