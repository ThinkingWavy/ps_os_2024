#include <stdio.h>
#include <stdlib.h>

int factorial_of_integer(int x) {
	if (x < 2) return 1;
	return x * factorial_of_integer(x - 1);
}

int main(int argc, char const *argv[])
{
    if(argc < 2){
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    long x = strtol(argv[1], NULL, 10);
    printf("%d\n", factorial_of_integer(x));

    return EXIT_SUCCESS;
}
