gcc -Wall -g -D_FILE_OFFSET_BITS=64 -DUNIX -DLINUX -fPIC -DI18N_EN -Wno-write-strings -c -I. -I../include *.cpp &&
ar rcs libfyba.a *.o &&
gcc -g -D_FILE_OFFSET_BITS=64 -DUNIX -DLINUX -fPIC -DI18N_EN -Wno-write-strings -shared -I. -I../include *.cpp -o ../../lib/libfyba.so
cp fyba.h ../../include/fyba.h

