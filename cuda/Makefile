CC     = nvcc
CFLAGS = -O3 -gencode=arch=compute_20,code=compute_20

all: clean main clean2 new_line

main: main.o getopt_pp.o setting.o smith_waterman.o smith_waterman.cuda.o dna.o string.o bit_array.o
	$(CC) $(CFLAGS) main.o getopt_pp.o setting.o smith_waterman.o smith_waterman.cuda.o dna.o string.o bit_array.o -o a.out

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o

getopt_pp.o: getopt_pp.cpp
	$(CC) $(CFLAGS) -c getopt_pp.cpp -o getopt_pp.o

setting.o: setting.cpp
	$(CC) $(CFLAGS) -c setting.cpp -o setting.o

smith_waterman.o: smith_waterman.cpp
	$(CC) $(CFLAGS) -c smith_waterman.cpp -o smith_waterman.o

smith_waterman.cuda.o: smith_waterman.cu
	$(CC) $(CFLAGS) -c smith_waterman.cu -o smith_waterman.cuda.o

dna.o: dna.cpp
	$(CC) $(CFLAGS) -c dna.cpp -o dna.o

string.o: string.cpp
	$(CC) $(CFLAGS) -c string.cpp -o string.o

bit_array.o: bit_array.cpp
	$(CC) $(CFLAGS) -c bit_array.cpp -o bit_array.o

clean:
	rm -f *.o

clean2:
	rm -f *.o

new_line:
	echo "\n\n\n"

test1: test_run1
test2: test_run2
test3: test_run3
test4: test_run4
test5: test_run5
test6: test_run6
test7: test_run7

test_run1:
	./a.out ../sequence_1.txt ../sequence_2.txt

test_run2:
	./a.out ../sequence_2.txt ../sequence_3.txt

test_run3:
	./a.out ../sequence_3.txt ../sequence_4.txt

test_run4:
	./a.out ../sequence_5.txt ../sequence_6.txt 5 -3 -4 1

test_run5:
	./a.out ../sequence_7.txt ../sequence_8.txt 5 -3 -4 1

test_run6:
	./a.out ../sequence_9.txt ../sequence_10.txt 5 -3 -4 1

test_run7:
	./a.out ../sequence_11.txt ../sequence_12.txt 5 -3 -4 0
