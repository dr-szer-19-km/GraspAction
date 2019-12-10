CXX ?= g++

CXXFLAGS += -Wall -Wextra -Wpedantic -std=c++17 -O2

SOURCEZ = main.cpp parser.cpp env.cpp algo.cpp
OBJECTZ = $(SOURCEZ:.cpp=.o)

.PHONY: all, clean, link, rebuild, OB, SC

all: $(OBJECTZ) link

OB:
	@echo $(OBJECTZ)
SC:
	@echo $(SOURCEZ)

link:
	$(CXX) -o szereg.exe $(OBJECTZ) 

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

rebuild: clean all

clean:
	rm -rf $(OBJECTZ)
