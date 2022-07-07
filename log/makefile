CXX = g++ -std=c++11 -g
CXXFLAGS = -w -Wall -Wextra -O3 -fopenmp -pthread

SRC_FILES := $(wildcard ./*.cpp)
SRC_FILES_NO_DIR := $(notdir $(SRC_FILES))
OBJS := $(patsubst %.cpp,%.o,$(SRC_FILES_NO_DIR))

all: log_test_run

log_test_run: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJS):$(SRC_FILES_NO_DIR)
	$(CXX) -c $^

.PHONY:clean
clean:
	rm -rf log_test_run $(OBJS)