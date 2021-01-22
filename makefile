include make.settings
# default compile output
all :
	$(MAKE) app_sdk_c
	$(MAKE) -C samples -f samples.mk

# linkedge device access sdk
app_sdk_c :
	$(MAKE) -C deps -f deps.mk
	$(MAKE) -C app -f app.mk

# clean tempory compile resource
clean:
	$(MAKE) -C deps -f deps.mk clean
	$(MAKE) -C app -f app.mk clean
	$(MAKE) -C samples -f samples.mk clean
	-$(RM) -r ./build
	-$(RM) -r ./deps/cJSON-1.7.7/
	-$(RM) -r ./deps/nats.c-master/

.PHONY: deps app samples 
