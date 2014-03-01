CC     = g++
# CFLAGS = -O3

all: clean main clean2 new_line

main: main.o matrix.o matrix_file.o string.o
	$(CC) $(CFLAGS) main.o matrix.o matrix_file.o string.o -o a.out

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o

matrix.o: matrix.cpp
	$(CC) $(CFLAGS) -c matrix.cpp -o matrix.o

matrix_file.o: matrix_file.cpp
	$(CC) $(CFLAGS) -c matrix_file.cpp -o matrix_file.o

string.o: string.cpp
	$(CC) $(CFLAGS) -c string.cpp -o string.o

clean:
	rm -f *.o

clean2:
	rm -f *.o

new_line:
	echo "\n\n\n"

test1: test_run1
test2: test_run2
test3: test_run3

test_run1:
	./a.out text_1.txt

test_run2:
	./a.out text_2.txt 2 -1 -1
