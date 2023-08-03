ASIO_HOME = ./asio.1.10.6

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
		-I $(ASIO_HOME)/include \
		-I.


RELEASE_FLAGS := \
	-O3 \
	-DDTREE_ICASE \
	-DNDEBUG \
	-DASIO_STANDALONE \
	-DENABLE_LOG \
	-DLINUX \
	-DHAVE_PCAP \
	-DHTTP_MAX_HEADER_SIZE=16384 \
	-DHTTP_PARSER_STRICT=0

DEBUG_FLAGS := \
	-O0 \
	-D_DEBUG \
	-DASIO_STANDALONE \
	-DENABLE_LOG \
	-DLINUX \
	-DHAVE_PCAP \
	-DHTTP_MAX_HEADER_SIZE=16384 \
	-DHTTP_PARSER_STRICT=0

LDFLAGS += \
	-static-libstdc++ -static-libgcc \
    -fmerge-all-constants

LIBS += \
	-lpthread

DIR := . http

SRC := $(foreach d, $(DIR), $(wildcard $(d)/*.cpp))

RELEASE_OBJ := $(patsubst %.cpp, %.o, $(SRC))
DEBUG_OBJ := $(patsubst %.cpp, %.d.o, $(SRC))

#CXX := g++
#CC := gcc

.PHONY: all clean install

all: amba_http_server

%.o : %.cpp
	$(CXX) -c $^ $(CXXFLAGS) $(RELEASE_FLAGS) -o $@

%.d.o : %.cpp
	$(CXX) -c $^ $(CXXFLAGS) $(DEBUG_FLAGS) -o $@

amba_http_server : $(RELEASE_OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIBS)

install: amba_http_server
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $^; do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done
	install -D -m 755 config.ini $(DESTDIR)/etc/config.ini

clean:
	find . -regex "\(.*\.o\|.*\.exe\)" | xargs rm 

.PHONY : 
