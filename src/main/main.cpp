// File name [ main.cpp ]
#include <iostream>

#include "cir/cirMgr.h"

using namespace std;

int main(int argc, char** argv) {
	CirMgr cirMgr;
	cirMgr.readVerilog(argv[1], 1);			// 1: for F
	cirMgr.readVerilog(argv[2], 0);			// 0: for G
    cirMgr.addWeight(argv[3]);
	
	cirMgr.test();	
    cirMgr.writeToPatch(argv[4]);
    cirMgr.writeToOut(argv[5], argv[1]);
}

