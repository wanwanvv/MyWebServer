TARGET_DIR := bin
TARGET := $(TARGET_DIR)/server_run
INC_DIRS := include
SRC_DIRS := dir1 \
            dir2 \
            dir3
OUTPUT_OBJ_DIR := output
CUR_DIR = $(shell pwd)
OUTPUT_OBJ_DIR := $(patsubst %,$(CUR_DIR)/%,$(OUTPUT_OBJ_DIR))
INC_DIRS := $(patsubst %,$(CUR_DIR)/%,$(INC_DIRS))
SRC_DIRS := $(patsubst %,$(CUR_DIR)/%,$(SRC_DIRS))
SRC_DIRS += $(CUR_DIR)
SRC_FILES := $(foreach dirs,$(SRC_DIR),$(wildcard $(dirs)/*.cpp))
SRC_FILES_NO_DIR := $(notdir $(SRC_FILES))

VPATH := $(SRC_DIRS)

OBJS := $(patsubst %.cpp,%.o,$(SRC_FILES_NO_DIR))
OBJS := $(patsubst %,$(OUTPUT_OBJ_DIR)/%,$(OBJS))

# -lpthread
CXX = g++
CXXFLAGS = -g -w -Wall -Wextra -O3 -std=c++11 -fopenmp -pthread
INCFLAGS := $(patsubst %,-I%,$(INC_DIRS)

TARGET := bin/server_run

all: clean $(OUTPUT_OBJ_DIR) $(OUTPUT_OBJ_DIR) $(TARGET)

# libs add here
$(TARGET) : $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS)

$(OBJS) : $(OUTPUT_OBJ_DIR)/%.o:%.cpp
	$(CXX) -o $@ -c $^ $(CXXFLAGS) $(INCFLAGS)

$(OUTPUT_OBJ_DIR)
	mkdir -p $@

$(TARGET_DIR):
	mkdir -p $@

clean:
	rm -rf bin
