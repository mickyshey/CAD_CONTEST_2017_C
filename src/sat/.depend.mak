reader.o: reader.cpp reader.h
ProofV.o: ProofV.cpp ProofV.h SolverTypesV.h GlobalV.h FileV.h SortV.h
sat.o: sat.cpp sat.h SolverV.h SolverTypesV.h GlobalV.h VarOrderV.h \
 HeapV.h ProofV.h FileV.h
SolverV.o: SolverV.cpp SolverV.h SolverTypesV.h GlobalV.h VarOrderV.h \
 HeapV.h ProofV.h FileV.h SortV.h
FileV.o: FileV.cpp FileV.h GlobalV.h
