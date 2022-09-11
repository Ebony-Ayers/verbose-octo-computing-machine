COMPILER = ccache g++
CLFAGS = -std=c++20 -ggdb
LDFLAGS = 
DEBUG_FLAGS = -DDEBUG -ggdb
WARNINGS = -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -Wpedantic -Wconversion -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wformat=2 -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast
PCH = pch.h.gch
OBJECT_FILES = scalar.o

.PHONY: debug
debug: scalarGrapher.out Makefile
	

.PHONY: release
release: scalar.cpp $(PCH) Makefile
	$(COMPILER) $(CLFAGS) -O3 $(WARNINGS) -o scalarGrapher.out scalar.cpp $(LDFLAGS)


scalarGrapher.out: scalar.o $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) -o scalarGrapher.out $(OBJECT_FILES) $(LDFLAGS)
scalar.o: scalar.cpp $(PCH) Makefile
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) -c scalar.cpp $(LDFLAGS)

pch.h.gch: pch.h
	$(COMPILER) $(CLFAGS) $(WARNINGS) $(DEBUG_FLAGS) pch.h

.PHONY: clear
clear:
	rm *.so
	rm *.o
