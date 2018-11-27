CXX = g++
CXXFLAGS := -Wall -std=c++17

SRC = file-management/

file_management.o: $(SRC)file_management.h $(SRC)file_management.cpp
	$(CXX) $(CXXFLAGS) -c $(SRC)file_management.cpp -o file_management.o

clean:
	rm file_management.o	