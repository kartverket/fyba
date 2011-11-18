gcc -Wall -g -D_FILE_OFFSET_BITS=64 -DUNIX -DLINUX -fPIC -Wno-write-strings -c -I. -I../include *.cpp
ar rcs libfygm.a *.o
gcc -g -D_FILE_OFFSET_BITS=64 -DUNIX -DLINUX -fPIC -Wno-write-strings -shared -I. -I../include *.cpp -o ../lib/libfygm.so
cp fygm.h ../include/fygm.h
