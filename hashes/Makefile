all:
	gcc -c -g md5.c
	gcc -c -g sha256.c
	gcc -g sha1.c md5.c hmac.c -o hmac
clean:
	rm *.o