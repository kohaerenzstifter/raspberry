C_SRCS += ../wwwidget.c 
OBJS += ./wwwidget.o 
C_DEPS += ./wwwidget.d 

%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/glib-2.0/ \
		-I/usr/lib/arm-linux-gnueabihf/glib-2.0/include \
		-O3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP \
		-MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'

all: libhttpdWwwidget.so

libhttpdWwwidget.so: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -shared -o "libhttpdWwwidget.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'

clean:
	rm -rf $(OBJS)$(C_DEPS) libhttpdWwwidget.so
