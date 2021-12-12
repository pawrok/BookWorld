override CXXFLAGS += -pthread -Wpedantic -Wall -Wextra -Wsign-conversion -g -Wconversion -std=c++2a -lssl -lcrypto

# 3rd_party library includes
override CXXFLAGS += -I3rd_party/uWebSockets/src 
override CXXFLAGS += -I3rd_party/uWebSockets/uSockets/src 
override CXXFLAGS += -I/usr/include/openssl 
# override CXXFLAGS += -isystem 3rd_party/boost_1_77_0 
override CXXFLAGS += -isystem 3rd_party/sqlite3
override CXXFLAGS += -isystem 3rd_party/spdlog/include

override LDFLAGS += build/*.o 3rd_party/uWebSockets/uSockets/*.o -lssl -lcrypto -lz -Llibs

# -> uWebSockets <-

# WITH_LIBDEFLATE=1 enables fast paths for SHARED_COMPRESSOR and inflation
ifeq ($(WITH_LIBDEFLATE),1)
	override CXXFLAGS += -I 3rd_party/uWebSockets/libdeflate -DUWS_USE_LIBDEFLATE
	override LDFLAGS += 3rd_party/uWebSockets/libdeflate/libdeflate.a
endif

all: BookWorld

.PHONY: DEP_OBJS
DEP_OBJS: 3rd_party/uWebSockets/uSockets/socket.o build/sqlite3.o

build/sqlite3.o:
	gcc -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION -c 3rd_party/sqlite3/sqlite3.c -o build/sqlite3.o

export WITH_OPENSSL = 1
3rd_party/uWebSockets/uSockets/socket.o:
	$(MAKE) -C 3rd_party/uWebSockets/uSockets

SRC_DIR = ./src
.PHONY: CPP_FILES
CPP_FILES:
	@for f in $(shell ls ${SRC_DIR} | grep .cpp); do \
		$(CXX) -c ${SRC_DIR}/$${f} -o build/$${f}.o $(CXXFLAGS); \
	done

BookWorld: DEP_OBJS CPP_FILES
	$(CXX) -o BookWorld $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f build/*.o


##########   Tests with Catch2   ##########

TEST_DIR=test
CPP_FLAGS=-std=c++2a -Wall -g

INCLUDES=\
	-I$(TEST_DIR)

SRCS=\
	$(TEST_DIR)/test_main.cpp\
	$(TEST_DIR)/common.cpp

OBJS=$(SRCS:.cpp=.o)

test: $(OBJS)
	@g++ $(CPP_FLAGS) $(INCLUDES) -o run_test $(OBJS) build/common.cpp.o
	@./run_test

%.o : %.cpp
	g++ $(CPP_FLAGS) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

cleantest:
	rm $(OBJS)