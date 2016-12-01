num = ${DEPTH}
all:
	gcc ./benchmark.c -D PARALLEL -D MAX_DEPTH=${DEPTH} -lpthread -pthread -o main.bin
	./main.bin > log/log${num}.log
