/*
Task: Safe Heterogeneous Data Container Library.
Sample run: Six containers hold 10, 3.14, 'A', "Hello", 0 and "Data".
The final string is copied, then the original is changed to show deep copying.
Edge cases: integer zero, wrong-type getter, empty values, empty string,
string-to-other-type changes, repeated clear, self-copy and allocation failure.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { TYPE_EMPTY, TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING } DataType;
typedef union { int integer; float decimal; char character; char *string; } Data;
typedef struct { Data data; DataType type; } Container;

void container_init(Container *container) {
    if (container != NULL) {
        container->type = TYPE_EMPTY;
        container->data.string = NULL;
    }
}

void container_clear(Container *container) {
    if (container == NULL) return;
    if (container->type == TYPE_STRING) free(container->data.string);
    container->data.string = NULL;
    container->type = TYPE_EMPTY;
}

int container_set_int(Container *container, int value) {
    if (container == NULL) return 0;
    container_clear(container);
    container->data.integer = value;
    container->type = TYPE_INT;
    return 1;
}

int container_set_float(Container *container, float value) {
    if (container == NULL) return 0;
    container_clear(container);
    container->data.decimal = value;
    container->type = TYPE_FLOAT;
    return 1;
}

int container_set_char(Container *container, char value) {
    if (container == NULL) return 0;
    container_clear(container);
    container->data.character = value;
    container->type = TYPE_CHAR;
    return 1;
}

int container_set_string(Container *container, const char *value) {
    char *copy;
    size_t length;
    if (container == NULL || value == NULL) return 0;
    length = strlen(value);
    copy = malloc(length + 1);
    if (copy == NULL) return 0;
    memcpy(copy, value, length + 1);
    container_clear(container);
    container->data.string = copy;
    container->type = TYPE_STRING;
    return 1;
}

int container_get_int(const Container *container, int *out_value) {
    if (container == NULL || out_value == NULL || container->type != TYPE_INT) return 0;
    *out_value = container->data.integer;
    return 1;
}

int container_get_float(const Container *container, float *out_value) {
    if (container == NULL || out_value == NULL || container->type != TYPE_FLOAT) return 0;
    *out_value = container->data.decimal;
    return 1;
}

int container_get_char(const Container *container, char *out_value) {
    if (container == NULL || out_value == NULL || container->type != TYPE_CHAR) return 0;
    *out_value = container->data.character;
    return 1;
}

int container_get_string(const Container *container, const char **out_value) {
    if (container == NULL || out_value == NULL || container->type != TYPE_STRING) return 0;
    *out_value = container->data.string;
    return 1;
}

const char *container_type_name(const Container *container) {
    if (container == NULL) return "INVALID";
    switch (container->type) {
        case TYPE_EMPTY: return "EMPTY";
        case TYPE_INT: return "INTEGER";
        case TYPE_FLOAT: return "FLOAT";
        case TYPE_CHAR: return "CHARACTER";
        case TYPE_STRING: return "STRING";
        default: return "INVALID";
    }
}

void container_print(const Container *container) {
    if (container == NULL) { printf("Invalid container"); return; }
    switch (container->type) {
        case TYPE_EMPTY: printf("(empty)"); break;
        case TYPE_INT: printf("%d", container->data.integer); break;
        case TYPE_FLOAT: printf("%.2f", container->data.decimal); break;
        case TYPE_CHAR: printf("%c", container->data.character); break;
        case TYPE_STRING: printf("%s", container->data.string); break;
        default: printf("(invalid type)");
    }
}

int container_copy(Container *destination, const Container *source) {
    char *string_copy = NULL;
    if (destination == NULL || source == NULL) return 0;
    if (destination == source) return 1;
    if (source->type == TYPE_STRING) {
        size_t length;
        if (source->data.string == NULL) return 0;
        length = strlen(source->data.string);
        string_copy = malloc(length + 1);
        if (string_copy == NULL) return 0;
        memcpy(string_copy, source->data.string, length + 1);
    }
    container_clear(destination);
    destination->type = source->type;
    if (source->type == TYPE_INT) destination->data.integer = source->data.integer;
    else if (source->type == TYPE_FLOAT) destination->data.decimal = source->data.decimal;
    else if (source->type == TYPE_CHAR) destination->data.character = source->data.character;
    else if (source->type == TYPE_STRING) destination->data.string = string_copy;
    else if (source->type == TYPE_EMPTY) destination->data.string = NULL;
    else { container_init(destination); free(string_copy); return 0; }
    return 1;
}

int main(void) {
    Container items[6], copied;
    int i, number;
    float decimal;
    const char *text;
    for (i = 0; i < 6; i++) container_init(&items[i]);
    container_init(&copied);

    container_set_int(&items[0], 10);
    container_set_float(&items[1], 3.14f);
    container_set_char(&items[2], 'A');
    container_set_string(&items[3], "Hello");
    container_set_int(&items[4], 0);
    container_set_string(&items[5], "Data");

    printf("CONTAINER ARRAY (each record has type and value)\n");
    for (i = 0; i < 6; i++) {
        printf("Index %d | %s | ", i, container_type_name(&items[i]));
        container_print(&items[i]);
        printf("\n");
    }

    printf("\nDeep copy demonstration\n");
    if (container_copy(&copied, &items[5])) {
        container_set_string(&items[5], "Changed original");
        printf("Original: "); container_print(&items[5]);
        printf("\nCopy: "); container_print(&copied); printf("\n");
    } else printf("Copy failed.\n");

    printf("\nType transition STRING -> INT -> STRING -> FLOAT\n");
    container_set_string(&items[0], "First text");
    container_set_int(&items[0], 25);
    container_set_string(&items[0], "Second text");
    container_set_float(&items[0], 8.5f);
    container_print(&items[0]); printf("\n");

    printf("\nGetter checks\n");
    if (container_get_int(&items[4], &number)) printf("Integer zero is valid: %d\n", number);
    if (!container_get_float(&items[4], &decimal)) printf("Wrong-type float getter correctly failed.\n");
    if (container_get_string(&items[3], &text)) {
        printf("String getter: %s\n", text);
        /* Passing the current string pointer is safe: the setter copies it first. */
        if (container_set_string(&items[3], text)) printf("Same-pointer string replacement worked.\n");
    }
    if (container_copy(&copied, &copied)) printf("Self-copy is safe.\n");

    container_clear(&copied);
    container_clear(&copied);
    printf("Clearing an empty container again is safe.\n");
    for (i = 0; i < 6; i++) container_clear(&items[i]);
    container_clear(&copied);
    printf("All containers cleared.\n");
    return 0;
}
