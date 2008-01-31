#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <splitter.h>

void usage(char *bin);

int main(int argc, char *argv[]){
	int i, n, *array = NULL;

	if(argc < 2 || strncmp(argv[1], "-h", 2) == 0){
		usage(argv[0]);
		return(1);
	}

	n = splitnums(argv[1], &array, 0);
	for(i=0; i<n; i++){
		printf("%d ", array[i]);
	}
	printf("\n");
	free(array);

	return(0);
}

void usage(char *bin){
	fprintf(stderr,"\nexpands a list of numbers & number ranges into individual values.\n\n");
	fprintf(stderr,"usage: %s <numbers>\n\n", bin);
	fprintf(stderr,"<numbers> may be a mixture of comma-separated numbers and ranges (no spaces).\n");
	fprintf(stderr, "example: %s 1,2,6-10\n\n", bin);
}
