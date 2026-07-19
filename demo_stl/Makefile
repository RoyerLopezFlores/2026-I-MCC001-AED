CXX := g++
CXXFLAGS := -std=c++2b -O2 -pthread
TBB_LIBS := -ltbb

.PHONY: all clean

all: generator pipeDemo stl_demo main control

generator: generator.cpp
	$(CXX) $(CXXFLAGS) generator.cpp -o generator.exe

pipeDemo: pipeDemo.cpp
	$(CXX) $(CXXFLAGS) pipeDemo.cpp -o pipeDemo.exe

stl_demo: stlDemo.cpp
	$(CXX) $(CXXFLAGS) stlDemo.cpp $(TBB_LIBS) -o stl_demo.exe

main: main.cpp BenchmarkUtils.cpp BenchmarkUtils.hpp Persona.hpp
	$(CXX) $(CXXFLAGS) main.cpp BenchmarkUtils.cpp $(TBB_LIBS) -o main.exe

control: control.cpp BenchmarkUtils.cpp BenchmarkUtils.hpp DataQueue.hpp Consumer.hpp Persona.hpp
	$(CXX) $(CXXFLAGS) control.cpp BenchmarkUtils.cpp $(TBB_LIBS) -o control.exe

clean:
	-del /Q *.exe 2>nul || rm -f *.exe
