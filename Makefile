CUDA_PATH = /opt/cuda

INCLUDE = -I/usr/include -I$(CUDA_PATH)/include

LIBDIR = -L/usr/lib -L$(CUDA_PATH)/lib64


# In 64-bit systems, install gcc-multilib and use -m32 flag
# In 32-bit systems, omit the -m32 flag from definition of $(CC)

#CC = gcc -m32
#CC = gcc
#CC = i486-mingw32-gcc
CC = nvcc
GCC = gcc
CFLAGS = $(COMPILERFLAGS) $(INCLUDE)
#LIBRARIES = -lglut -lopengl32 -lGLU -lm -lalut -lopenal
LIBRARIES = -lcurand
COMPILERFLAGS = -ccbin $(GCC)

TEST_FILES = main.o
EXECUTABLE = main

all: $(EXECUTABLE)


$(EXECUTABLE): main.o 
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(LIBDIR) $(LIBRARIES)\
	 main.o 

main.o: main.cu
	$(CC) -c main.cu

clean:
	rm -f *.o $(EXECUTABLE)

check:
	 ./$(EXECUTABLE) param.template
