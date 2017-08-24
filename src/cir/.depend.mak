cirGate.o: cirGate.cpp ../cir/cirGate.h ../sat/sat.h ../sat/SolverV.h \
 ../sat/SolverTypesV.h ../sat/GlobalV.h ../sat/VarOrderV.h ../sat/HeapV.h \
 ../sat/ProofV.h ../sat/FileV.h
cirMgr.o: cirMgr.cpp ../cir/cirMgr.h ../cir/cirNet.h ../cir/cirGate.h \
 ../sat/sat.h ../sat/SolverV.h ../sat/SolverTypesV.h ../sat/GlobalV.h \
 ../sat/VarOrderV.h ../sat/HeapV.h ../sat/ProofV.h ../sat/FileV.h \
 ../cir/reader.h ../util/parse.h
cirSim.o: cirSim.cpp ../cir/cirMgr.h ../cir/cirNet.h ../cir/cirGate.h \
 ../sat/sat.h ../sat/SolverV.h ../sat/SolverTypesV.h ../sat/GlobalV.h \
 ../sat/VarOrderV.h ../sat/HeapV.h ../sat/ProofV.h ../sat/FileV.h \
 ../cir/reader.h
reader.o: reader.cpp reader.h
cirParse.o: cirParse.cpp ../cir/cirMgr.h ../cir/cirNet.h ../cir/cirGate.h \
 ../sat/sat.h ../sat/SolverV.h ../sat/SolverTypesV.h ../sat/GlobalV.h \
 ../sat/VarOrderV.h ../sat/HeapV.h ../sat/ProofV.h ../sat/FileV.h \
 ../cir/reader.h ../util/parse.h
cirDup.o: cirDup.cpp ../cir/cirMgr.h ../cir/cirNet.h ../cir/cirGate.h \
 ../sat/sat.h ../sat/SolverV.h ../sat/SolverTypesV.h ../sat/GlobalV.h \
 ../sat/VarOrderV.h ../sat/HeapV.h ../sat/ProofV.h ../sat/FileV.h \
 ../cir/reader.h
cirProof.o: cirProof.cpp cirMgr.h ../cir/cirNet.h ../cir/cirGate.h \
 ../sat/sat.h ../sat/SolverV.h ../sat/SolverTypesV.h ../sat/GlobalV.h \
 ../sat/VarOrderV.h ../sat/HeapV.h ../sat/ProofV.h ../sat/FileV.h \
 reader.h
cirMulti.o: cirMulti.cpp ../cir/cirMgr.h ../cir/cirNet.h ../cir/cirGate.h \
 ../sat/sat.h ../sat/SolverV.h ../sat/SolverTypesV.h ../sat/GlobalV.h \
 ../sat/VarOrderV.h ../sat/HeapV.h ../sat/ProofV.h ../sat/FileV.h \
 ../cir/reader.h ../util/parse.h
cirReport.o: cirReport.cpp ../cir/cirMgr.h ../cir/cirNet.h \
 ../cir/cirGate.h ../sat/sat.h ../sat/SolverV.h ../sat/SolverTypesV.h \
 ../sat/GlobalV.h ../sat/VarOrderV.h ../sat/HeapV.h ../sat/ProofV.h \
 ../sat/FileV.h ../cir/reader.h
cirNet.o: cirNet.cpp ../cir/cirNet.h ../cir/cirGate.h ../sat/sat.h \
 ../sat/SolverV.h ../sat/SolverTypesV.h ../sat/GlobalV.h \
 ../sat/VarOrderV.h ../sat/HeapV.h ../sat/ProofV.h ../sat/FileV.h
cirCost.o: cirCost.cpp ../cir/cirMgr.h ../cir/cirNet.h ../cir/cirGate.h \
 ../sat/sat.h ../sat/SolverV.h ../sat/SolverTypesV.h ../sat/GlobalV.h \
 ../sat/VarOrderV.h ../sat/HeapV.h ../sat/ProofV.h ../sat/FileV.h \
 ../cir/reader.h
cirSat.o: cirSat.cpp ../cir/cirMgr.h ../cir/cirNet.h ../cir/cirGate.h \
 ../sat/sat.h ../sat/SolverV.h ../sat/SolverTypesV.h ../sat/GlobalV.h \
 ../sat/VarOrderV.h ../sat/HeapV.h ../sat/ProofV.h ../sat/FileV.h \
 ../cir/reader.h ../util/parse.h
