override CXXFLAGS += -lpthread -Wpedantic -Wall -Wextra -Wsign-conversion -Wconversion -std=c++2a -IuWebSockets/src -IuWebSockets/uSockets/src -I3rd_party/boost_1_77_0
override LDFLAGS += build/*.o uWebSockets/uSockets/*.o -lz -lssl -lcrypto -Llibs

# -> uWebSockets <-

# WITH_LIBDEFLATE=1 enables fast paths for SHARED_COMPRESSOR and inflation
ifeq ($(WITH_LIBDEFLATE),1)
	override CXXFLAGS += -I uWebSockets/libdeflate -DUWS_USE_LIBDEFLATE
	override LDFLAGS += uWebSockets/libdeflate/libdeflate.a
endif

all: BookWorld

.PHONY: DEP_OBJS
DEP_OBJS: uWebSockets/uSockets/socket.o build/sqlite3.o

build/sqlite3.o:
	$(CC) -o build/sqlite3.o -c sqlite3/sqlite3.c -Wall -lpthread -ldl -lm -g

export WITH_OPENSSL = 1
uWebSockets/uSockets/socket.o:
	$(MAKE) -C uWebSockets/uSockets

BookWorld: DEP_OBJS
	$(CXX) $(CXXFLAGS) -ldl -lm -g $(LDFLAGS) src/*.cpp -o BookWorld 

clean:
	rm -f build/*.o
	rm -f uWebSockets/uSockets/*.o
