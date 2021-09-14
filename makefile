CXX = g++
SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS :=
CPPFLAGS :=
CXXFLAGS := -I include

all: dir copyart

dir:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)

copyart: $(OBJ_FILES)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<


.PHONY: all clean

clean:
	rm -f $(OBJ_DIR)/*
	rm -f $(BIN_DIR)/*
