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
debug: scalarGrapher_v1.out scalarGrapher_v2.out scalarGrapher_v3.out simdGrapher.out Makefile
	

.PHONY: release
release: scalar_v1.cpp simd.cpp $(PCH) Makefile
	$(COMPILER) $(CLFAGS) -Ofast -o scalarGrapher_v1.out scalar_v1.cpp $(LDFLAGS)
	$(COMPILER) $(CLFAGS) -Ofast -o scalarGrapher_v2.out scalar_v2.cpp $(LDFLAGS)
	$(COMPILER) $(CLFAGS) -Ofast -o scalarGrapher_v3.out scalar_v3.cpp $(LDFLAGS)
	$(COMPILER) $(CLFAGS) -Ofast $(SIMD_FLAGS) -o simdGrapher.out simd.cpp $(LDFLAGS)
.PHONY: releaseNoSIMD
releaseNoSIMD: scalar_v1.cpp simd.cpp $(PCH) Makefile
	$(COMPILER) $(CLFAGS) -Ofast -o scalarGrapher_v1.out scalar_v1.cpp $(LDFLAGS)
	$(COMPILER) $(CLFAGS) -Ofast -o scalarGrapher_v2.out scalar_v2.cpp $(LDFLAGS)
	$(COMPILER) $(CLFAGS) -Ofast -o scalarGrapher_v3.out scalar_v3.cpp $(LDFLAGS)

simdGrapher.out: simd.o $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) $(SIMD_FLAGS) -o simdGrapher.out simd.o $(LDFLAGS)
simd.o: simd.cpp function.cpp $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) $(SIMD_FLAGS) -c simd.cpp $(LDFLAGS)


scalarGrapher_v1.out: scalar_v1.o $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) -o scalarGrapher_v1.out scalar_v1.o $(LDFLAGS)
scalar_v1.o: scalar_v1.cpp $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) -c scalar_v1.cpp $(LDFLAGS)
scalarGrapher_v2.out: scalar_v2.o $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) -o scalarGrapher_v2.out scalar_v2.o $(LDFLAGS)
scalar_v2.o: scalar_v2.cpp $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) -c scalar_v2.cpp $(LDFLAGS)
scalarGrapher_v3.out: scalar_v3.o $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) -o scalarGrapher_v3.out scalar_v3.o $(LDFLAGS)
scalar_v3.o: scalar_v3.cpp $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) -c scalar_v3.cpp $(LDFLAGS)

pch.h.gch: pch.h
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) pch.h

.PHONY: clear
clear:
	rm *.so
	rm *.o
