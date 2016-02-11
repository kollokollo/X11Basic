# Example how to link X11-Basic Object codes (produced by xbc)
#   with your c-Program                        

# Example how to use the x11basic-Library
#   with your c-Program                       (c) Markus Hoffmann

# demo1 simply uses some of the X11-basic commands provided by libx11basic

# demo2 links with library.bas and calls X11-Basic procedures and function and uses the X11-Basic Variables
#       from within the c sourcecode. You can also make a shared library from library.o and dynamically link 
#	against it.


all: demo1 demo2


demo1: c-demo.c
	gcc -fomit-frame-pointer -o $@  -lx11basic -ldl -lm -lreadline -lncurses -lasound -lX11 c-demo.c 
	
demo2: library.o c-demo2.c
	gcc -fomit-frame-pointer -o $@  -lx11basic -ldl -lm -lreadline -lncurses -lasound -lX11  c-demo2.c library.o
	
%.b: %.bas
	xbbc -o $@ $<
%.c: %.b
	xb2c -l -o $@ $<
%.o: %.c
	gcc -DNOMAIN -c $< -o $@
%.so: %.o
	gcc -shared $< -o $@


clean:
	rm -f library.c library.o library.b 
distclean: clean
	rm -f demo1 demo2 
