#include <stdio.h>
#include <stdlib.h>

typedef enum {
    TYPE_A = 0,
    TYPE_B = 1,
    TYPE_C = 2
} DataType;

typedef struct {
    DataType type; // Discriminant
    int commonData;
    // Other members specific to this type
} StructA;

typedef struct {
    DataType type; // Discriminant
    unsigned commonData;
    // Other members specific to this type
} StructB;

typedef struct {
    DataType type; // Discriminant
    char commonData;
    // Other members specific to this type
} StructC;

typedef union {
    StructA a;
    StructB b;
    StructC c;
} UnionOfStructs;

int main() {
    UnionOfStructs myUnion;

    myUnion.a.type = TYPE_B;
    myUnion.a.commonData = -255;

    // Now 'myUnion' holds an instance of 'StructA'

    printf("Data in StructA: type = %d, commonData = %d\n", myUnion.a.type, myUnion.a.commonData);
    printf("Data in StructB: type = %d, commonData = %u\n", myUnion.b.type, myUnion.b.commonData); // Accessing other members, which may contain garbage values
    printf("Data in StructC: type = %d, commonData = %d\n", myUnion.c.type, myUnion.c.commonData); // Accessing other members, which may contain garbage values

    return 0;
}

