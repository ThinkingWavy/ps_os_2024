#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

typedef char* (*plugin_func_t)(const char*);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <string> <plugin1> <plugin2> ... <pluginN>\n", argv[0]);
        return 1;
    }

    char *input_string = argv[1];
    char *result_string = strdup(input_string); // Duplicate the input string for manipulation

    for (int i = 2; i < argc; ++i) {
        char *plugin_path = argv[i];
        void *handle = dlopen(plugin_path, RTLD_LAZY);
        if (!handle) {
            fprintf(stderr, "Error loading plugin %s: %s\n", plugin_path, dlerror());
            free(result_string);
            return 1;
        }

        dlerror(); // Clear any existing error

        plugin_func_t plugin_func = (plugin_func_t) dlsym(handle, "transform");
        char *error = dlerror();
        if (error != NULL) {
            fprintf(stderr, "Error finding symbol 'transform' in %s: %s\n", plugin_path, error);
            dlclose(handle);
            free(result_string);
            return 1;
        }

        char *new_result = plugin_func(result_string);
        if (new_result == NULL) {
            fprintf(stderr, "Error in plugin %s\n", plugin_path);
            dlclose(handle);
            free(result_string);
            return 1;
        }

        free(result_string);
        result_string = new_result;

        dlclose(handle);
        printf("%s: %s\n", plugin_path, result_string);
    }

    free(result_string);
    return 0;
}
