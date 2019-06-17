all:
	@mkdir -p bin
	@nasm -felf64 random.asm -o bin/random.o
	@gcc -g test/hobig_test.c -o bin/big bin/random.o
	@./bin/big
clean:
	rm bin/big