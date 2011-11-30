gcc -Wall -g -D_FILE_OFFSET_BITS=64 -DUNIX -DLINUX -fPIC -Wno-write-strings -c -I. *.cpp
ar rcs libfyut.a *.o
gcc -g -D_FILE_OFFSET_BITS=64 -DUNIX -DLINUX -fPIC -Wno-write-strings -shared -I. *.cpp -o ../../lib/libfyut.so
cp fyut.h ../../include/fyut.h
