#include <stdio.h>
#include <string.h>

int main(void) {
	char data[256] = "This is pretty important data that might get lost after I fuck around %d times, lmao.";
	int value = 97;
	sprintf(data, data, value);
	printf("%s\n\n", data);
}
