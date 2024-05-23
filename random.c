#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define AMOUNT 2

int main() {
    int seed = 49; // Seed value
    int randomNumber;
    int i;
    srand(seed);
    for (i = 0; i < AMOUNT; i++) {
    	// Generate a random number using the customRand function with the specified seed
	randomNumber = rand();
    	// Print the generated random number
    	printf("Random number generated with seed %d: %d\n", seed, randomNumber);
    }
    seed = 51;
    srand(seed);
    for (i = 0; i < AMOUNT; i++) {
    	// Generate a random number using the customRand function with the specified seed
	randomNumber = rand();
    	// Print the generated random number
    	printf("Random number generated with seed %d: %d\n", seed, randomNumber);
    }
    seed = 49;
    srand(seed);
    for (i = 0; i < AMOUNT; i++) {
    	// Generate a random number using the customRand function with the specified seed
	randomNumber = rand();
    	// Print the generated random number
    	printf("Random number generated with seed %d: %d\n", seed, randomNumber);
    }
    srand(time(NULL));
    for (i = 0; i < AMOUNT; i++) {
    	// Generate a random number using the customRand function with the specified seed
	randomNumber = rand();
    	// Print the generated random number
    	printf("Random number generated with random seed: %d\n", randomNumber);
    }
    seed = rand();
    srand(seed);
    for (i = 0; i < 12; i++) {
    	printf("Random number generated with seed generated with seed: %d, %d\n", randomNumber, seed);
    	seed = randomNumber = rand();
	srand(seed);
    }
    return 0;
}
