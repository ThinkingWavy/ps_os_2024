#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <string.h>

//only working when all functions have same name
#define FUNCTION_NAME "string_manipulation"


int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s <string> <library> <library> ...\n", argv[0]);
        return EXIT_FAILURE;
    }
    //handler for library
    void *handle;

    //making space for doubling-function
    char *string = malloc(strlen(argv[1]) * 2 + 1);
    strcpy(string, argv[1]);

    //function pointers
    void (*func)(char *);

    for (int i = 2; i < argc; i++) {

        //loading library
        handle = dlopen(argv[i], RTLD_LAZY);
        if (handle == NULL) {
            fprintf(stderr, "Error while loading library.\n");
            return EXIT_FAILURE;
        }

        //searching for function
        func = dlsym(handle, FUNCTION_NAME);

        //if no function with this name is found
        if (dlerror() != NULL) {
            fprintf(stderr, "Error while searching function!\n");
            dlclose(handle);
            return EXIT_FAILURE;
        }

        func(string);
        printf("%s: %s\n", argv[i], string);
        dlclose(handle);
    }
    dlclose(handle);
    free(string);
    return EXIT_SUCCESS;
}
