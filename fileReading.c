#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(void) {
	clock_t start, end;
	double cpu_time_used;
	FILE *file = fopen("./resources/text/english.tsv", "r");
	char string[512];
	char *token;
	char *saveptr;
	int id = 3411;
	int i;
	start = clock();
	fgets(string, sizeof(string), file);
	while (fgets(string, sizeof(string), file) != NULL) {
		token = strtok_r(string, "	", &saveptr);
		i = atoi(token);
		if (i == id) printf("%s\n\n", strtok_r(NULL, "	", &saveptr));
		//printf("%s\n", strtok_r(NULL, "	", &saveptr));
	}
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("CPU time used: %f seconds\n", cpu_time_used);
	fclose(file);
	return 0;
}
