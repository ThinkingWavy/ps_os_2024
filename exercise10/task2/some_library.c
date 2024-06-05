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
