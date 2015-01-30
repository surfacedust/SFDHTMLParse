ifeq ($(shell ./thrift/gen.sh),$(PWD))
endif

CC=/usr/local/gcc-4.8.2/bin/g++
DEBUG=CLOSE

GEN_OBJ:=$(patsubst %.cpp,%.o, $(wildcard ./gen/*.cpp))

PARSER_OBJ:=$(patsubst %.cpp,%.o, $(wildcard ./src/*.cpp))

INCLUDES=-I. -I./include -I./gen/ -I/usr/local/include/thrift

LIB_PATH=-L./lib -L/usr/local/lib/ -L/usr/local/boost155/lib

CPPFLAGS=-O3 -std=c++11 -D$(DEBUG)_DEBUG -g -static -fPIC -finline-functions  \
		 -Wtrigraphs -Wformat -Wparentheses -Wpointer-arith \
		 -D_XOPEN_SOURE=500 -D_GNU_SOURCE -Wp,-D_FORTIFY_SOURCE=2 \
		 --param=ssp-buffer-size=4 -m64 -D_FILE_OFFSET_BITS=64\
		 -D_LARGEFILE_SOURCE -fno-strict-aliasing -fwrapv -DHAVE_NETINET_IN_H\
		 -Wno-missing-braces -Wextra -Wno-missing-field-initializers \
		 -Wcast-align -Wno-literal-suffix -Wno-deprecated\
		 -Wshadow \
		 -Wredundant-decls \
		 -ggdb3 \
		 -ffloat-store -fno-common\
		 -ftree-vectorize

LDFLAGS= -static-libgcc -static-libstdc++ \
             -lthriftnb -lrt -levent -lpthread -llog -lini_config -lthrift -lboost_regex -lgumbo

all: main regex_parser dom_parser gumbo_meta

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@ $(INCLUDES)

main: ./test/main.o ${PARSER_OBJ} ${GEN_OBJ}
	$(CC) -o $@ $^ $(INCLUDES) $(LIB_PATH) $(LDFLAGS)
regex_parser: ./test/regex_parser_test.o ${PARSER_OBJ} ${GEN_OBJ}
	$(CC) -o $@ $^ $(INCLUDES) $(LIB_PATH) $(LDFLAGS)
dom_parser: ./test/dom_parser_test.o ${PARSER_OBJ} ${GEN_OBJ}
	$(CC) -o $@ $^ $(INCLUDES) $(LIB_PATH) $(LDFLAGS)
gumbo_meta: ./test/gumbo_meta_test.o
	$(CC) -o $@ $^ $(INCLUDES) $(LIB_PATH) $(LDFLAGS)
clean:
	rm -f ./src/*.o 
	rm -f gen/*
	rm -f main
	rm -f regex_parser
	rm -f gumbo_meta

rmdata:
	rm -f ./log/* ./*.err ./*.pid nohup.out
