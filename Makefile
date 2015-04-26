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
LIBRARIES = -lcurand
COMPILERFLAGS = -ccbin $(GCC)

EXECUTABLE = main
OBJFILES = main.o fileio.o

all: $(EXECUTABLE)


$(EXECUTABLE): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(LIBDIR)\
	 $(OBJFILES) $(LIBRARIES)

main.o: main.cu
	$(CC) -c main.cu

fileio.o: fileio.c
	$(CC) -c fileio.c

clean:
	rm -f *.o $(EXECUTABLE)

check:
	 ./$(EXECUTABLE) param.template
