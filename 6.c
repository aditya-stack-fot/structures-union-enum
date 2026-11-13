/*
Task: Memory-Efficient Typed Records and Safe Union Access.
Sample run: The program prints structure/union sizes, alignments, member
addresses and offsets, then safely displays five tagged values.
Edge cases: a 19-character string fits, a longer string is rejected,
records can change type, and only the active union member is read.
*/
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdalign.h>

#define TEXT_SIZE 20

typedef struct {
    int number;
    float decimal;
    char text[TEXT_SIZE];
} StructData;

typedef union {
    int number;
    float decimal;
    char text[TEXT_SIZE];
} UnionData;

typedef enum { VALUE_INT, VALUE_FLOAT, VALUE_TEXT } ValueType;

typedef struct {
    UnionData data;
    ValueType type;
} TaggedValue;

int set_integer(TaggedValue *value, int number) {
    if (value == NULL) return 0;
    value->data.number = number;
    value->type = VALUE_INT;
    return 1;
}

int set_float(TaggedValue *value, float number) {
    if (value == NULL) return 0;
    value->data.decimal = number;
    value->type = VALUE_FLOAT;
    return 1;
}

int set_text(TaggedValue *value, const char *text) {
    if (value == NULL || text == NULL || strlen(text) >= TEXT_SIZE) return 0;
    strcpy(value->data.text, text);
    value->type = VALUE_TEXT;
    return 1;
}

void display_tagged_value(const TaggedValue *value) {
    if (value == NULL) { printf("Invalid record.\n"); return; }
    if (value->type == VALUE_INT) printf("Integer: %d\n", value->data.number);
    else if (value->type == VALUE_FLOAT) printf("Float: %.2f\n", value->data.decimal);
    else if (value->type == VALUE_TEXT) printf("Text: %s\n", value->data.text);
    else printf("Invalid value tag.\n");
}

int main(void) {
    StructData s;
    UnionData u;
    TaggedValue records[5];
    int i;
    s.number = 10;
    s.decimal = 220.5f;
    strcpy(s.text, "Structure");

    printf("STRUCTURE ANALYSIS\n");
    printf("Size: %zu bytes | Alignment: %zu bytes\n", sizeof(StructData), alignof(StructData));
    printf("Address of structure: %p\n", (void *)&s);
    printf("number address: %p | offset: %zu\n", (void *)&s.number, offsetof(StructData, number));
    printf("decimal address: %p | offset: %zu\n", (void *)&s.decimal, offsetof(StructData, decimal));
    printf("text address: %p | offset: %zu\n", (void *)s.text, offsetof(StructData, text));

    printf("\nUNION ANALYSIS\n");
    printf("Size: %zu bytes | Alignment: %zu bytes\n", sizeof(UnionData), alignof(UnionData));
    printf("number address: %p\n", (void *)&u.number);
    printf("decimal address: %p\n", (void *)&u.decimal);
    printf("text address: %p\n", (void *)u.text);
    printf("100 structures: %zu bytes\n", 100 * sizeof(StructData));
    printf("100 unions: %zu bytes\n", 100 * sizeof(UnionData));

    set_integer(&records[0], 10);
    set_float(&records[1], 220.5f);
    set_text(&records[2], "Union Programming");
    set_integer(&records[3], -25);
    if (!set_text(&records[4], "1234567890123456789")) printf("String did not fit.\n");
    printf("\nTAGGED RECORDS (size: %zu bytes)\n", sizeof(TaggedValue));
    for (i = 0; i < 5; i++) {
        printf("Record %d: ", i + 1);
        display_tagged_value(&records[i]);
    }
    if (!set_text(&records[4], "This string is too long")) printf("Overlength string rejected.\n");
    records[4].type = (ValueType)99;
    printf("Invalid tag test: ");
    display_tagged_value(&records[4]);
    set_text(&records[4], "Restored");
    set_integer(&records[0], 99);
    set_text(&records[0], "Changed to text");
    set_float(&records[0], 4.5f);
    printf("Record 1 after INT -> TEXT -> FLOAT: ");
    display_tagged_value(&records[0]);

    printf("\nOBSERVATION\nA union shares memory between its members, so it can save space when\nonly one type is needed at a time. Its values cannot coexist independently.\nStructure fields have separate storage; padding may be added for alignment.\n");
    return 0;
}
