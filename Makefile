SRC = src/
INC = include/

CXX = g++
CXXFLAGS := -Wall -std=c++17

file_management.o: $(INC)stypox/file_management.h $(SRC)file_management.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC)file_management.cpp -o file_management.o

clean:
	rm file_management.o	