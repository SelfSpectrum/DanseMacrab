#include <stdio.h>
#include <string.h>

// Function to check if a string is empty
int isEmpty(const char *str) {
    return str[0] == '\0'; // Check if the first character is '\0'
}

int main() {
    FILE *file = fopen("./resources/anims/mainMenu/crab.tsv", "r");
    char line[256] = "data,data2,moredata,,evenmoredata";
    fgets(line, sizeof(line), file);
    fgets(line, sizeof(line), file);
    char *token, *saveptr;
    int i = 0;

    // Tokenize the line using strtok_r()
    token = strtok_r(line, "	", &saveptr);
    while (token != NULL) {
        // Check if the token is empty using isEmpty()
        if (isEmpty(token)) {
            printf("Empty field %d\n", i);
        } else {
            printf("Field %d: %s\n", i, token);
        }
        // Get the next token
        token = strtok_r(NULL, "	", &saveptr);
        i++;
    }
    fclose(file);
    return 0;
}

