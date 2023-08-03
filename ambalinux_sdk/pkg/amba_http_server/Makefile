#CXX11_HOME = /home/alpha/opt/gcc4.9.2
CXX11_HOME = /usr
ASIO_HOME = ./asio.1.10.6

# if want to disable defined but not used in asio_error.hpp,
# add -Wno-unused-variable

CXXFLAGS := \
    -g3 \
    -fPIC \
    -std=c++1y \
    -Wall \
    -Wextra \
    -Wno-parentheses \
    -Wno-unused-variable \
    -Wdeprecated-declarations \
    -fmerge-all-constants  \
		-I $(CXX11_HOME)/include \
		-I $(ASIO_HOME)/include \
		-I.


RELEASE_FLAGS := \
	-O3 \
	-DDTREE_ICASE \
	-DNDEBUG \
	-DASIO_STANDALONE \
	-DLINUX \
	-DENABLE_LOG\
	-DHAVE_PCAP \
	-DHTTP_MAX_HEADER_SIZE=16384 \
	-DHTTP_PARSER_STRICT=0

DEBUG_FLAGS := \
	-O0 \
	-D_DEBUG \
	-DASIO_STANDALONE \
	-DLINUX \
	-DENABLE_LOG \
	-DHAVE_PCAP \
	-DHTTP_MAX_HEADER_SIZE=16384 \
	-DHTTP_PARSER_STRICT=0

LDFLAGS += \
	-static-libstdc++ -static-libgcc \
    -fmerge-all-constants \
	-L${CXX11_HOME}/lib64

LDFLAGS += -static
	
LIBS := \
	-lpthread \
	-lrt


DIR := . http

SRC := $(foreach d, $(DIR), $(wildcard $(d)/*.cpp))

RELEASE_OBJ := $(patsubst %.cpp, %.o, $(SRC))
DEBUG_OBJ := $(patsubst %.cpp, %.d.o, $(SRC))

CXX := export LD_LIBRARY_PATH=${CXX11_HOME}/lib; ${CXX11_HOME}/bin/g++
CC := export LD_LIBRARY_PATH=${CXX11_HOME}/lib; ${CXX11_HOME}/bin/gcc

all: main

%.o : %.cpp
	$(CXX) -c $^ $(CXXFLAGS) $(RELEASE_FLAGS) -o $@

%.d.o : %.cpp
	$(CXX) -c $^ $(CXXFLAGS) $(DEBUG_FLAGS) -o $@

main : $(RELEASE_OBJ) 
	$(CXX) $^ -o $@.exe $(LDFLAGS) $(LIBS)



clean:
	find . -regex "\(.*\.o\|.*\.exe\)" | xargs rm 

.PHONY : 
