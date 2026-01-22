CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude -I/usr/local/include -g
LDFLAGS = -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/game

SRCS = $(wildcard $(SRC_DIR)/*.cpp)

all: $(TARGET)

$(TARGET): $(SRCS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(SRCS) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)