buildtype ?= Release

CXX = g++
AR = ar
GYP = node-gyp
NPM = npm
MKDIR = mkdir
LN = ln
RM = rm

ifneq ($(buildtype), Release)
ifneq ($(buildtype), Debug)
$(error buildtype must be Release or Debug)
endif
endif

CXXFLAGS:=-std=c++11 -Wall -Wextra -MMD -MP
GYPFLAGS:=
ifeq ($(buildtype), Debug)
	CXXFLAGS:=$(CXXFLAGS) -g
	GYPFLAGS:=$(GYPFLAGS) --debug
else
	CXXFLAGS:=$(CXXFLAGS) -Ofast -DNDEBUG
	GYPFLAGS:=$(GYPFLAGS) --release
endif

CXXSRC_DIR = src/backend
NODESRC_DIR = src/frontend
BUILD_DIR = build/$(buildtype)
CXXSRC = $(filter-out backend.cpp, $(notdir $(wildcard $(CXXSRC_DIR)/*.cpp)))
CXXOBJ = $(patsubst %.cpp, %.o, $(CXXSRC))
LIBBACKEND = $(BUILD_DIR)/libbackend.a
BACKEND = $(BUILD_DIR)/backend.node
FRONTEND = $(NODESRC_DIR)
ELECTRON = node_modules/.bin/electron

all: $(BACKEND) $(FRONTEND) | node_modules

$(BUILD_DIR):
	$(MKDIR) -p $@

$(BUILD_DIR)/%.o: $(CXXSRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $^

-include $(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.d))

$(LIBBACKEND): $(addprefix $(BUILD_DIR)/, $(CXXOBJ))
	$(AR) cr $@ $^

build/Makefile: binding.gyp
	$(GYP) configure

$(BACKEND): build/Makefile src/backend/backend.cpp $(LIBBACKEND)
	$(GYP) $(GYPFLAGS) build

node_modules: $(NODESRC_DIR)/package.json
	$(NPM) install $(NODESRC_DIR)

run: all
	$(LN) -f $(BACKEND) build/backend.node
	$(ELECTRON) $(FRONTEND)

clean:
	$(RM) -r build
	$(RM) -r node_modules

.PHONY: all run clean

.DELETE_ON_ERROR: $(ELECTRON)

