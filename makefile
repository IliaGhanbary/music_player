HEAD_DIR := MY_HEADER_FILES
SRC_DIR := MY_SOURCE_FILES
OBJ_DIR := MY_OBJECT_FILES

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

TARGET ?= Project1.exe

CXX := g++
CXXFLAGS := -I$(HEAD_DIR) -Wall -Wextra -MMD -MP

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) 
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"

clean:
	@rm -f $(OBJ_FILES) $(OBJ_DIR)/*.d $(TARGET)
.PHONY: all clean

-include $(OBJ_FILES:.o=.d)
