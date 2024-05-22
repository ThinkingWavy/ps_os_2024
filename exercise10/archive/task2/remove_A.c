void string_manipulation(char *str) {
    int count = 0;
    for (int i = 0; str[i]; i++) {
        if (str[i] != 'A' && str[i] != 'a') {
            str[count++] = str[i];
        }
    }
    str[count] = '\0';
}