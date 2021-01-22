include ../make.settings
CFLAGS  = -g -Wall -O2 -lpthread

INCLUDE = -I../build/cjson -I../build/nats -I../build/app
LIBS    = ../build/app/libapp.a ../build/nats/libnats_static.a ../build/cjson/libcjson.a

src    = samples.c
target = samples

all : $(target)

$(target) :
	$(CC) -o $(target) $(src) $(LIBS) $(INCLUDE) $(CFLAGS) -lrt

clean:
	-$(RM) $(target) $(target).o
