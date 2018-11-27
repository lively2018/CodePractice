HashTable: HashTable.o
	gcc -o $@ $^
HashTable.o: HashTable.c
	gcc -c $^
HashTableLink: HashTableLink.o
	gcc -o $@ $^
HashTableLink.o: HashTableLink.c
	gcc -c $^
clean:
	rm HashTableLink HashTable *.o
