#include <stdio.h>
#include <stdlib.h>

int fibonacci_of_integer(int n) {
	{
		// Taken from https://www.geeksforgeeks.org/program-for-nth-fibonacci-number/
		int a = 0, b = 1, c;
		if (n == 0) return a;
		for (int i = 2; i <= n; i++) {
			c = a + b;
			a = b;
			b = c;
		}
		return b;
	}
}

int main(int argc, char const* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <number>\n", argv[0]);
		exit(EXIT_SUCCESS);
	}

	long x = strtol(argv[1], NULL, 10);
	printf("%d\n", fibonacci_of_integer(x));

	return EXIT_SUCCESS;
}
