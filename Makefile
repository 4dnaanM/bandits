CXX = g++
CXXFLAGS = -std=c++14
TARGET = selectArm
SOURCES = selectArm.cpp Arm.cpp
BUILD_DIR = /Users/Adnaan/Repos/bandits/

all: $(TARGET)

$(TARGET):
	$(CXX) $(CXXFLAGS) $(addprefix $(BUILD_DIR),$(SOURCES)) -o $(BUILD_DIR)$(TARGET)
	cd $(BUILD_DIR) && ./$(TARGET)

clean:
	rm -f $(BUILD_DIR)$(TARGET)