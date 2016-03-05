USER_OBJS := libkohaerenzstiftung.a
OBJS := ../src/httpd.o
LIBS := -ldl `pkg-config --libs glib-2.0` -lmicrohttpd

CFLAGS = `pkg-config --cflags glib-2.0` -I../../kohaerenzstiftung/

# All Target
all: httpd

# Tool invocations
httpd: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -rdynamic -o "httpd" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'

# Other Targets
clean:
	 rm -rf $(OBJS) httpd
