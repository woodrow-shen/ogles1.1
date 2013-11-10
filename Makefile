#############################################
# Simple Makefile for OpenGL|ES Application #
# Created by Shye-Tzeng Shen                #
#############################################
all: oglesdemo
# which compiler gcc/g++
CC = g++
# where are include files kept
INCLUDE = .
# options for development
# CFLAGS = -g -Wall -ansi
# options for release
CFLAGS = -O -fpermissive -Wall -DEGL_ON_LINUX

oglesdemo: main.o bitmap.o config.o context.o demo.o display.o drv_impl.o egl.o fixed_api.o float_api.o gl.o glm.o matrix_stack.o outfile.o surface.o geometry.o rasterizer.o
	$(CC) $(CFLAGS) -o oglesdemo main.o bitmap.o config.o context.o demo.o display.o drv_impl.o egl.o fixed_api.o float_api.o gl.o glm.o matrix_stack.o outfile.o surface.o geometry.o rasterizer.o
main.o: main.cpp ogles.h uglu.h offviewer.h glm.h app.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c main.cpp
bitmap.o: bitmap.cpp bitmap.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c bitmap.cpp
config.o: config.cpp config.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c config.cpp
context.o: context.cpp context.h 
	$(CC) -I$(INCLUDE) $(CFLAGS) -c context.cpp
demo.o: demo.cpp ogles.h app.h shapes.h cams.h 
	$(CC) -I$(INCLUDE) $(CFLAGS) -c demo.cpp
display.o: display.cpp display.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c display.cpp
drv_impl.o: drv_impl.cpp drv_impl.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c drv_impl.cpp
egl.o: egl.cpp ogles.h context.h surface.h config.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c egl.cpp
fixed_api.o: fixed_api.cpp context.h drv_impl.h fixed.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c fixed_api.cpp
float_api.o: float_api.cpp context.h rasterizer.h drv_impl.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c float_api.cpp
gl.o: gl.cpp context.h ogles.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c gl.cpp
glm.o: glm.cpp ogles.h glm.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c glm.cpp
matrix_stack.o: matrix_stack.cpp stdafx.h matrix_stack.h 
	$(CC) -I$(INCLUDE) $(CFLAGS) -c matrix_stack.cpp
outfile.o: outfile.cpp ogles.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c outfile.cpp
surface.o: surface.cpp surface.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c surface.cpp
geometry.o: geometry.cpp geometry.h 
	$(CC) -I$(INCLUDE) $(CFLAGS) -c geometry.cpp
rasterizer.o: rasterizer.cpp rasterizer.h 
	$(CC) -I$(INCLUDE) $(CFLAGS) -c rasterizer.cpp

clean:
	-rm -rf oglesdemo *.o *.txt *~
	
