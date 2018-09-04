
PLATFORM := $(shell uname)

GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW
EXT =
CPPFLAGS = `pkg-config --cflags glfw3` -g -O0

CC = g++
EXE = assign3_part2
OBJS = main.o Shader.o Camera.o Object.o Scene.o Model.o Light.o ShaderObj.o

ifeq ($(PLATFORM), Darwin)
    GL_LIBS += -framework OpenGL
endif

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

main.o: main.cpp InputState.h
	$(CC) $(CPPFLAGS) -c main.cpp

Shader.o : Shader.cpp Shader.hpp
	$(CC) $(CPPFLAGS) -c Shader.cpp

Object.o : Object.cpp Object.h
	$(CC) $(CPPFLAGS) -c Object.cpp

Model.o : Model.cpp Model.h
	$(CC) $(CPPFLAGS) -c Model.cpp

Scene.o : Scene.cpp Scene.h
	$(CC) $(CPPFLAGS) -c Scene.cpp

Camera.o: Camera.cpp Camera.h
	$(CC) $(CPPFLAGS) -c Camera.cpp

Light.o : Light.cpp Light.h
	$(CC) $(CPPFLAGS) -c Light.cpp

ShaderObj.o: ShaderObj.cpp ShaderObj.h
	$(CC) $(CPPFLAGS) -c ShaderObj.cpp


clean:
	rm -f *.o $(EXE)$(EXT)
