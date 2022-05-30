#
# **************************************************************
# *                Simple C++ Makefile Template                *
# *                                                            *
# * Author: Arash Partow (2003)                                *
# * URL: http://www.partow.net/programming/makefile/index.html *
# *                                                            *
# * Copyright notice:                                          *
# * Free use of this C++ Makefile template is permitted under  *
# * the guidelines and in accordance with the the MIT License  *
# * http://www.opensource.org/licenses/MIT                     *
# *                                                            *
# **************************************************************
#

CXX      := -c++
CXXFLAGS := -pedantic-errors -Wall -Wextra -Werror -std=c++17
LDFLAGS  := -L/usr/lib -lstdc++ -lm
BUILD    := .build
TARGET   := cgrep
INCLUDE  := -Iinclude/
SRC      := $(wildcard src/*.cpp)

OBJECTS  := $(SRC:%.cpp=$(BUILD)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all: build $(TARGET)

$(BUILD)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^ $(LDFLAGS)

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info

build:
	@mkdir -p $(BUILD)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O3
release: all

clean:
	-@rm -rvf $(BUILD)/*

info:
	@echo "[*] Binary:          ${TARGET}      "
	@echo "[*] Build Dir:       ${BUILD}       "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"
                       