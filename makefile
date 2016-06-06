CFLAGS=
LDFLAGS=-L/usr/lib -lallegro -lallegro_image
INCLUDE=-I. -I/usr/include/allegro5

OBJS=main.o
OBJ=Mandelbrot.o

all: mandelbrot

clean:
		rm -rf *.o main pokoloruj

mandelbrot: $(OBJS) $(OBJ)
		cc -g -o $@ $^ $(INCLUDE) $(CFLAGS) $(LDFLAGS)
main.o: main.c
		cc -c -g $^
%.o: %.s
	nasm -f elf64 $<

