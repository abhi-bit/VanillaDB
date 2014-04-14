all:
	gcc -Wall -o vdb-test vdb.c

clean:
	rm -rf vdb-test sample.db a.out *.o
