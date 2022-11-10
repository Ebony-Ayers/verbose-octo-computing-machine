PLATFORM ?= linux
ifeq ($(PLATFORM), mac)
	COMPILER = clang++
else
	COMPILER = ccache g++
endif
CLFAGS = -std=c++20 -ggdb -march=native
LDFLAGS = 
DEBUG_FLAGS = -DDEBUG -ggdb
WARNINGS = -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wformat=2
ifeq ($(PLATFORM), linux)
	WARNINGS += -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast
endif
SIMD_FLAGS = -mavx -mavx2
PCH = pch.h.gch

.PHONY: debug
debug: scalarGrapher.out simdGrapher.out Makefile
	

.PHONY: release
release: scalar.cpp simd.cpp $(PCH) Makefile
	$(COMPILER) $(CLFAGS) -Ofast -o scalarGrapher.out scalar.cpp $(LDFLAGS)
	$(COMPILER) $(CLFAGS) -Ofast $(SIMD_FLAGS) -o simdGrapher.out simd.cpp $(LDFLAGS)

simdGrapher.out: simd.o $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) $(SIMD_FLAGS) -o simdGrapher.out simd.o $(LDFLAGS)
simd.o: simd.cpp function.cpp $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) $(SIMD_FLAGS) -c simd.cpp $(LDFLAGS)


scalarGrapher.out: scalar.o $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) -o scalarGrapher.out scalar.o $(LDFLAGS)
scalar.o: scalar.cpp $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) -c scalar.cpp $(LDFLAGS)

pch.h.gch: pch.h
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) pch.h

.PHONY: clear
clear:
	rm *.so
	rm *.o
