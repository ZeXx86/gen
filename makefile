CXX?=g++
LIBS=-lSDL -lSDL_image -lGL -lGLU `sdl-config --cflags --libs`
#LIBS_DIR?=-L/usr/lib32/nvidia-experimental-310/ -L/home/tomas/Programming/SDL-1.2.15/build/.libs -L/home/tomas/Programming/SDL_image-1.2.12/.libs -L/usr/lib/i386-linux-gnu/
#CFLAGS?=-m32

gen: main.o gl.o event.o mouse.o kbd.o camera.o terrain.o logic.o octree.o
	$(CXX) $(CFLAGS) main.o gl.o event.o mouse.o kbd.o camera.o terrain.o logic.o octree.o $(LIBS_DIR) $(LIBS) -o gen

clean:
	rm -f *.o gen
