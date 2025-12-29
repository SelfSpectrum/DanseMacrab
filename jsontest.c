#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	FILE* file = fopen("./resources/combat/weapons.json", "r");
	if (!file) {
		printf("Error opening file\n");
		return 1;
	}
	char buffer[1024];
	fread(buffer, sizeof(char), sizeof(buffer), file);
	fclose(file);
	cJSON *json = cJSON_Parse(buffer);
	char *string = cJSON_Print(json);
	printf("%s\n", string);
	free(string);
	string = NULL;
	string = cJSON_Print(cJSON_GetObjectItem(json, "rustyKatana"));
	printf("%s\n", string);
	free(string);
	return 0;
}
