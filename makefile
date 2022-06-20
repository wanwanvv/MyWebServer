TARGET_DIR := bin
TARGET := $(TARGET_DIR)/server_run
OUTPUT_OBJ_DIR := output

#指定代码目录下递归文件夹下的文件夹，最多指定当前目录三级文件夹
#INC_DIRS := $(shell find $(CUR_DIR) -maxdepth 3 -type d)
#@echo "INC_DIRS="$(INC_DIRS)

INC_DIRS := buffer \
            epoll \
            http \
			pool \
			server \
			timer
SRC_DIRS := $(INC_DIRS)

CUR_DIR = $(shell pwd)
TARGET_DIR := $(patsubst %,$(CUR_DIR)/%,$(TARGET_DIR))
OUTPUT_OBJ_DIR := $(patsubst %,$(CUR_DIR)/%,$(OUTPUT_OBJ_DIR))
INC_DIRS := $(patsubst %,$(CUR_DIR)/%,$(INC_DIRS))
SRC_DIRS := $(patsubst %,$(CUR_DIR)/%,$(SRC_DIRS))
SRC_DIRS += $(CUR_DIR)
SRC_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
SRC_FILES_NO_DIR := $(notdir $(SRC_FILES))

VPATH := $(SRC_DIRS)

OBJS := $(patsubst %.cpp,%.o,$(SRC_FILES_NO_DIR))
OBJS := $(patsubst %,$(OUTPUT_OBJ_DIR)/%,$(OBJS))

# -lpthread
CXX = g++
CXXFLAGS = -g -w -Wall -Wextra -O3 -std=c++11 -fopenmp -pthread
INCFLAGS := $(patsubst %,-I%,$(INC_DIRS))

TARGET := bin/server_run

all: clean $(OUTPUT_OBJ_DIR) $(TARGET_DIR) $(TARGET)

# libs add here
$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ 

$(OBJS) : $(OUTPUT_OBJ_DIR)/%.o:%.cpp
	$(CXX) -o $@ -c $^ $(CXXFLAGS) $(INCFLAGS)

$(OUTPUT_OBJ_DIR):
	mkdir -p $@

$(TARGET_DIR):
	mkdir -p $@

#分成两行写:
#	rm -rf $(TARGET_DIR)
#	rm -rf$(OUTPUT_OBJ_DIR)
# .PHONY:clean
clean:
	rm -rf $(TARGET_DIR) $(OUTPUT_OBJ_DIR)
