CC = aarch64-apple-darwin23-gcc-14
AR = aarch64-apple-darwin23-gcc-ar-14

all:
	$(CC) -Iext -c *.c
	$(AR) rcs libeva.a *.o
	rm *.o
	mv libeva.a ../maria/lib
