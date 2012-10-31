MEX=mex
CORELIB=../
OUTNAME=mexfactorgraph

all:
	$(MEX) -f ./mexopts.sh -cxx mexfactorgraph.cpp customnode.cpp $(CORELIB)/factornode.cpp $(CORELIB)/equalitynode.cpp $(CORELIB)/addnode.cpp $(CORELIB)/network.cpp $(CORELIB)/message.cpp -output $(OUTNAME)
