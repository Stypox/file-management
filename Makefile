#Makefile settings

CXX = g++
CXXFLAGS = -Wall -g -std=c++17
CXXFLAGSAFTER = -lstdc++fs
SRC = ./file-management/

#executable dependencies
executable: $(SRC)main.o $(SRC)fileManagement.o
	$(CXX) $(CXXFLAGS) -o executable $(SRC)main.o $(SRC)fileManagement.o $(CXXFLAGSAFTER)

#main.o dependencies
main.o: $(SRC)main.cpp $(SRC)fileManagement.h
	$(CXX) $(CXXFLAGS) -c $(SRC)main.cpp $(CXXFLAGSAFTER)

#fileManagement dependencies
fileManagement.o: $(SRC)fileManagement.h