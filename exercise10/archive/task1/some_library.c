int factorial_of_integer(int x) {
	if (x < 2) return 1;
	return x * factorial_of_integer(x - 1);
}
