# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall

# Executable Name
EXEC = market_data_merger

# Unit Test Executable
UNIT_TEST_EXEC = unit_test

# Source and Header Files
SRCS = src/main.cpp src/DataMerger.cpp
HEADERS = src/DataMerger.h src/DataLine.h
UNIT_TEST = unit_test.cpp

all: $(EXEC)

$(EXEC): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(EXEC)

test: $(UNIT_TEST) $(EXEC)
	$(CXX) $(CXXFLAGS) $(UNIT_TEST) -o $(UNIT_TEST_EXEC)

clean:
	rm -f $(EXEC) $(UNIT_TEST_EXEC)
