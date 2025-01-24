EXEC_NAME := a.out

CXXFLAGS   += -I./../..
CXXFLAGS   += -Wall -Wextra
CXXFLAGS   += -std=c++20
CXXFLAGS   += -O3
CXXFLAGS   += -march=native
CXXFLAGS   += -DNDEBUG

CPP_FILES  := $(wildcard *.cpp)
OBJ_FILES  := $(CPP_FILES:.cpp=.o)

LDFLAGS     += -lstdc++

.PHONY: all clean test

all: $(EXEC_NAME)

clean:
	rm -rf $(OBJ_FILES)
	rm -rf $(EXEC_NAME)

test: $(EXEC_NAME)
	./a.out

$(EXEC_NAME): $(OBJ_FILES)
	$(CXX) $(LD_FLAGS) $^ -o $(EXEC_NAME)

$(OBJ_FILES): $(SRC_FILES)

$(SRC_FILES):
	$(CXX) -c $(CXXFLAGS) $@
