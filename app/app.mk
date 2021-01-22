include ../make.settings
CFLAGS  = -g -Wall -O2 -D_GNU_SOURCE -std=c99

APP_LIB     = libapp.a

INCLUDE_PATH =  -I$(PWD)
INCLUDE      = $(INCLUDE_PATH)/build/cjson $(INCLUDE_PATH)/build/nats

OBJS = ./app.o \
       ./utils.o \

all : $(APP_LIB) install

$(APP_LIB): $(OBJS)
	$(AR) cr $@ $(OBJS)

$(OBJS):%o:%c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

install:
	mkdir -p $(PWD)/build/app
	cp $(APP_LIB) $(PWD)/build/app
	cp *.h $(PWD)/build/app
clean:
	-$(RM) -r $(APP_LIB) $(OBJS)
