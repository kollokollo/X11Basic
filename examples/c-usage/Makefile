# Example how to link X11-Basic object codes (produced by xbc) with your C program                        
# and 
# Example how to use the x11basic-Library from within your C program  
#                                       (c) Markus Hoffmann 2004

# demo1 simply uses some of the X11-basic commands provided by libx11basic

# demo2 links with library.bas and calls X11-Basic procedures and function 
#       and uses the X11-Basic variables from within the c sourcecode. 
#       You can also make a shared library from library.o and dynamically link 
#	against it.


all: demo1 demo2


demo1: c-demo.c
	gcc -fomit-frame-pointer -o $@ $< -lx11basic -ldl -lm -lreadline -lasound -lX11 
	
demo2: library.o c-demo2.c
	gcc -fomit-frame-pointer -o $@ $^ -lx11basic -ldl -lm -lreadline -lasound -lX11 
	
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
