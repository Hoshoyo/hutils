all: debug
	@bin/big

pkcs1: asm
	@gcc -g pkcs1.c hobig.c asn1.c bin/random.o -o bin/pkcs1
	@bin/pkcs1
asn1: asm
	gcc -g test/asn1_test.c asn1.c hobig.c -o bin/asn1 bin/random.o
	./bin/asn1
debug: asm
	@gcc -g test/hobig_test.c -o bin/big bin/random.o
release: asm
	@gcc -g -O2 test/hobig_test.c -o bin/big bin/random.o

build-benchmark: asm
	# build Go
	@go build test/bench.go
	@mv bench bin/go_benchmark
	# build C
	@gcc -g -O2 test/bench.c -o bin/c_benchmark bin/random.o
benchmark: build-benchmark
	@./benchmark.sh
plot:
	gnuplot -p -e "plot \
		'test/c_addition_result.txt' 	 title 'C add' with lines, \
		'test/c_subtraction_result.txt'  title 'C sub' with lines, \
		'test/go_addition_result.txt' 	 title 'Go add' with lines, \
		'test/go_subtraction_result.txt' title 'Go sub' with lines"
asm:
	@mkdir -p bin
	@nasm -felf64 random.asm -o bin/random.o
clean:
	rm test/*.txt