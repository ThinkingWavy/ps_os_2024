#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENV_VAR "OFFSET"

bool check_operator(const char* operator);
void error_message(const char* name);
bool convert_str_double(char** input, double* result, int len);
bool calculate(double* result, double* input, char* operator, int len);
double check_offset();

enum ERROR_CODES {
	TO_FEW_ARGUMENT_ERROR = 13,
	UNKNOWN_OPERATOR_ERROR = 42,
	NOT_NUMBER_ERROR = 15,
	DIVIDE_BY_ZERO_ERROR = 16
};

int main(int argc, char** argv) {
	// check amount of arguments
	if (argc <= 2) {
		error_message(argv[0]);
		return TO_FEW_ARGUMENT_ERROR;
	}
	// check if operation is valid
	if (!check_operator(argv[1])) {
		error_message(argv[0]);
		return UNKNOWN_OPERATOR_ERROR;
	}

	double offset = check_offset();
	double* values = malloc(sizeof(double) * argc - 2);

	// check if arguments were numbers
	if (!convert_str_double(argv, values, argc - 2)) {
		fprintf(stderr, "Some Input was not a valid double.\n");
		free(values);
		return NOT_NUMBER_ERROR;
	}
	// if only one operand is given
	if (argc == 3) {
		printf("Result: %lf\n", values[0] + offset);
		free(values);
		return EXIT_SUCCESS;
	}

	double result;
	if (!calculate(&result, values, argv[1], argc - 2)) {
		fprintf(stderr, "Dividing by zero is not possible.\n");
		free(values);
		return DIVIDE_BY_ZERO_ERROR;
	}

	printf("Result: %lf\n", result + offset);
	free(values);
	return EXIT_SUCCESS;
}

bool check_operator(const char* operator) {
	switch (*operator) {
		case '+': return true;
		case '-': return true;
		case '*': return true;
		case '/': return true;
	}
	return false;
}

void error_message(const char* name) {
	fprintf(stderr, "Usage: %s '<operator>' <number> ...\nAvailable operators : +, -, *, / \n", name);
}

bool convert_str_double(char** input, double* result, int len) {
	char* end;
	for (int i = 0; i < len; i++) {
		*(result + i) = strtod(input[i + 2], &end);

		// check if input was number
		if (strcmp(end, "")) {
			return false;
		}
	}
	return true;
}

double check_offset() {
	double offset = 0.0;
	char* end;
	char* env = getenv(ENV_VAR);
	if (env == NULL) {
		return 0.0;
	}
	offset = strtod(env, &end);
	if (strcmp(end, "")) {
		perror("no valid offset found\n");
		return 0.0;
	}
	return offset;
}

bool calculate(double* result, double* input, char* operator, int len) {
	double tmp = input[0];
	if (*operator== '+') {
		for (int i = 1; i < len; i++) {
			tmp += input[i];
		}
	} else if (*operator== '-') {
		for (int i = 1; i < len; i++) {
			tmp -= input[i];
		}
	} else if (*operator== '*') {
		for (int i = 1; i < len; i++) {
			tmp *= input[i];
		}
	} else if (*operator== '/') {
		for (int i = 1; i < len; i++) {
			// if division by zero is tried then fail
			if (input[i] == 0) {
				return false;
			}
			tmp = tmp / input[i];
		}
	}
	*result = tmp;
	return true;
}