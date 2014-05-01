#
#  (C) Copyright 2014 ZeXx86 (tomasj@spirit-system.com)
#  (C) Copyright 2013 ZeXx86 (tomasj@spirit-system.com)
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

CXX=g++
LIBS=-lGL -lGLU -lGLEW `sdl2-config --cflags --libs` -lSDL2_ttf -lSDL2_image
CXXFLAGS=-O3

gen-linux: main.o gl.o event.o mouse.o kbd.o camera.o terrain.o logic.o octree.o tex.o frustum.o polygonise.o font.o water.o vbo.o shader.o
	$(CXX) $(CXXFLAGS) main.o gl.o event.o mouse.o kbd.o camera.o terrain.o logic.o octree.o tex.o frustum.o polygonise.o font.o water.o vbo.o shader.o $(LIBS_DIR) $(LIBS) -o gen-linux

clean:
	rm -f *.o gen-linux
