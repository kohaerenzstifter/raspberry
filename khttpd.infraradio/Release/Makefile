C_SRCS += ../infraradio.c 
OBJS += ./infraradio.o 
C_DEPS += ./infraradio.d 
LIBS += $(shell pkg-config --libs json) -lwiringPi

%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/glib-2.0/ $(shell pkg-config --cflags json) \
		-I/usr/lib/arm-linux-gnueabihf/glib-2.0/include \
		-O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP \
		-MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'

all: libhttpdInfraradio.so

libhttpdInfraradio.so: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -shared -o "libhttpdInfraradio.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'

clean:
	rm -rf $(OBJS)$(C_DEPS) libhttpdInfraradio.so
