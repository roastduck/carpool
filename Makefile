buildtype ?= Release

CXX = g++
AR = ar
GYP = node-gyp
MKDIR = mkdir
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
BUILD_DIR = build/$(buildtype)
CXXSRC = $(filter-out backend.cpp, $(notdir $(wildcard $(CXXSRC_DIR)/*.cpp)))
CXXOBJ = $(patsubst %.cpp, %.o, $(CXXSRC))
LIBBACKEND = $(BUILD_DIR)/libbackend.a
BACKEND = $(BUILD_DIR)/backend.node

all: $(BACKEND)

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

clean:
	$(RM) -r build

.PHONY: all clean

