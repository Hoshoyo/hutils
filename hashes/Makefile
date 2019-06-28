all:
	gcc -Wall -c -g md5.c
	gcc -Wall -c -g sha256.c
	gcc -Wall -g sha1.c md5.c hmac.c -o hmac
clean:
	rm *.o