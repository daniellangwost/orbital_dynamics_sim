CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude -I/usr/local/include -g
LDFLAGS = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
TARGET = $(BUILD_DIR)/game

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
HDRS = $(wildcard $(INC_DIR)/*.hpp)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(SRCS) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)