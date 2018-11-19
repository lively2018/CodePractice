binaries = CheckPermutation CheckPermutationSort CheckPermutationHash
CheckPermutationHash: CheckPermutationHash.o
	gcc -o $@ $^
CheckPermutationHash.o: CheckPermutationHash.c
	gcc -c $^
CheckPermutationSort: CheckPermutationSort.o
	gcc -o $@ $^
CheckPermutationSort.o: CheckPermutationSort.c
	gcc -c $^
CheckPermutation: CheckPermutation.o
	gcc -o $@ $^
CheckPermutation.o: CheckPermutation.c
	gcc -c $^
clean:
	rm ${binaries} *.o
