#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool check_operator(const char *operator);
void error_message(const char *name);
bool convert_str_double(char **input, double *result, int len);
bool calculate(double *result, double *input, char *operator, int len);

enum ERROR_CODES
{
    TO_FEW_ARGUMENT_ERROR = 13,
    UNKNOWN_OPERATOR_ERROR = 42,
    NOT_NUMBER_ERROR = 15,
    DIVIDE_BY_ZERO_ERROR = 16
};

int main(int argc, char **argv)
{
    // check amount of arguments
    if (argc <= 3)
    {
        error_message(argv[0]);
        return TO_FEW_ARGUMENT_ERROR;
    }
    // check if operation is valid
    if (!check_operator(argv[1]))
    {
        error_message(argv[0]);
        return UNKNOWN_OPERATOR_ERROR;
    }

    // if only one operand is given
    if (argc == 3)
    {
        printf("Result: %s\n", argv[3]);
        return EXIT_SUCCESS;
    }

    double *values = malloc(sizeof(double) * argc - 2);

    // check if arguments were numbers
    if (!convert_str_double(argv, values, argc - 2))
    {
        fprintf(stderr, "Some Input was not a valid double.\n");
        return NOT_NUMBER_ERROR;
    }

    double result;
    if (!calculate(&result, values, argv[1], argc - 2))
    {
        fprintf(stderr, "Dividing by zero is not possible.\n");
        return DIVIDE_BY_ZERO_ERROR;
    }

    printf("Result: %f\n", result);
    free(values);
    return EXIT_SUCCESS;
}

bool check_operator(const char *operator)
{
    switch (*operator)
    {
    case '+':
        return true;
    case '-':
        return true;
    case '*':
        return true;
    case '/':
        return true;
    }
    return false;
}

void error_message(const char *name)
{
    fprintf(stderr, "Usage: %s '<operator>' <number> ...\nAvailable operators : +, -, *, / ", name);
}

bool convert_str_double(char **input, double *result, int len)
{
    char *end;
    for (int i = 0; i < len; i++)
    {
        *(result + i) = strtod(input[i + 2], &end);

        // check if input was number
        if (strcmp(end, ""))
        {
            return false;
        }
    }
    return true;
}

bool calculate(double *result, double *input, char *operator, int len)
{
    double tmp = input[0];
    if (*operator== '+')
    {
        for (int i = 1; i < len; i++)
        {
            tmp += input[i];
        }
    }
    else if (*operator== '-')
    {
        for (int i = 1; i < len; i++)
        {
            tmp -= input[i];
        }
    }
    else if (*operator== '*')
    {
        for (int i = 1; i < len; i++)
        {
            tmp *= input[i];
        }
    }
    else if (*operator== '/')
    {
        for (int i = 1; i < len; i++)
        {
            // if division by zero is tried then fail
            if (input[i] == 0)
            {
                return false;
            }
            tmp = tmp / input[i];
        }
    }
    *result = tmp;
    return true;
}