/*
Task: Rectangle Inventory and Geometry Analysis.
Sample run: Add rectangles 201 (5 x 4), 202 (6.5 x 3.5), and 203 (4 x 4),
then view, sort, update, calculate the summary and filter by area.
Edge cases: invalid/duplicate IDs, non-positive and non-finite dimensions,
equal areas, square comparison, missing IDs and empty collection.
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_RECTANGLES 1000
#define TOLERANCE 0.0001f

typedef struct {
    int id;
    float width;
    float height;
    float area;
    float perimeter;
} Rectangle;

int read_line(char *text, int size) {
    int ch;
    if (fgets(text, size, stdin) == NULL) return 0;
    if (strchr(text, '\n') == NULL) {
        ch = getchar();
        if (ch != '\n' && ch != EOF) {
            while (ch != '\n' && ch != EOF) ch = getchar();
            return -1;
        }
    }
    text[strcspn(text, "\n")] = '\0';
    return 1;
}

int read_int(const char *message, int *value) {
    char line[100], extra;
    printf("%s", message);
    return read_line(line, sizeof(line)) == 1 && sscanf(line, " %d %c", value, &extra) == 1;
}

int find_rectangle(const Rectangle *items, int count, int id) {
    int i;
    for (i = 0; i < count; i++) if (items[i].id == id) return i;
    return -1;
}

int read_dimensions(float *width, float *height) {
    char line[100], extra;
    printf("Width: ");
    if (read_line(line, sizeof(line)) != 1 || sscanf(line, " %f %c", width, &extra) != 1 ||
        !isfinite(*width) || *width <= 0.0f) return 0;
    printf("Height: ");
    if (read_line(line, sizeof(line)) != 1 || sscanf(line, " %f %c", height, &extra) != 1 ||
        !isfinite(*height) || *height <= 0.0f) return 0;
    return 1;
}

void calculate_rectangle(Rectangle *r) {
    r->area = r->width * r->height;
    r->perimeter = 2.0f * (r->width + r->height);
}

int add_rectangle(Rectangle *items, int *count) {
    Rectangle r;
    if (*count >= MAX_RECTANGLES) { printf("Maximum capacity reached.\n"); return 0; }
    if (!read_int("Rectangle ID: ", &r.id) || r.id <= 0 || find_rectangle(items, *count, r.id) != -1) {
        printf("Invalid or duplicate ID.\n"); return 0;
    }
    if (!read_dimensions(&r.width, &r.height)) { printf("Dimensions must be finite positive numbers.\n"); return 0; }
    calculate_rectangle(&r);
    items[*count] = r;
    (*count)++;
    printf("Rectangle %d added.\n", r.id);
    return 1;
}

void display_rectangle(const Rectangle *r) {
    printf("ID: %d | %.2f x %.2f | Area: %.2f | Perimeter: %.2f | %s\n",
           r->id, r->width, r->height, r->area, r->perimeter,
           fabsf(r->width - r->height) <= TOLERANCE ? "Square" : "Non-square");
}

void display_all(const Rectangle *items, int count) {
    int i;
    if (count == 0) { printf("No rectangles stored.\n"); return; }
    for (i = 0; i < count; i++) display_rectangle(&items[i]);
}

void update_rectangle(Rectangle *items, int count) {
    int id, index;
    float width, height;
    if (!read_int("ID to update: ", &id)) { printf("Invalid ID.\n"); return; }
    index = find_rectangle(items, count, id);
    if (index == -1) { printf("Rectangle not found.\n"); return; }
    if (!read_dimensions(&width, &height)) { printf("Invalid dimensions.\n"); return; }
    items[index].width = width;
    items[index].height = height;
    calculate_rectangle(&items[index]);
    printf("Rectangle %d updated.\n", id);
}

void delete_rectangle(Rectangle *items, int *count) {
    int id, index, i;
    if (!read_int("ID to delete: ", &id)) { printf("Invalid ID.\n"); return; }
    index = find_rectangle(items, *count, id);
    if (index == -1) { printf("Rectangle not found.\n"); return; }
    for (i = index; i < *count - 1; i++) items[i] = items[i + 1];
    (*count)--;
    printf("Rectangle %d deleted.\n", id);
}

void show_summary(const Rectangle *items, int count) {
    int i, largest = 0, smallest = 0;
    float total = 0.0f;
    if (count == 0) { printf("No geometry summary is available.\n"); return; }
    for (i = 0; i < count; i++) {
        total += items[i].area;
        if (items[i].area > items[largest].area) largest = i;
        if (items[i].area < items[smallest].area) smallest = i;
    }
    printf("Largest area: "); display_rectangle(&items[largest]);
    printf("Smallest area: "); display_rectangle(&items[smallest]);
    printf("Total area: %.2f | Average area: %.2f\n", total, total / count);
}

void sort_by_area(Rectangle *items, int count) {
    int i, j;
    Rectangle temp;
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (items[j].area > items[j + 1].area ||
                (items[j].area == items[j + 1].area && items[j].id > items[j + 1].id)) {
                temp = items[j]; items[j] = items[j + 1]; items[j + 1] = temp;
            }
        }
    }
    printf("Sorted by area in ascending order.\n");
    display_all(items, count);
}

void filter_area(const Rectangle *items, int count) {
    char line[100], extra;
    float limit;
    int i, found = 0;
    printf("Show areas greater than: ");
    if (read_line(line, sizeof(line)) != 1 || sscanf(line, " %f %c", &limit, &extra) != 1 || !isfinite(limit)) {
        printf("Invalid threshold.\n"); return;
    }
    for (i = 0; i < count; i++) if (items[i].area > limit) { display_rectangle(&items[i]); found = 1; }
    if (!found) printf("No rectangle has an area above that threshold.\n");
}

int main(void) {
    Rectangle *items;
    int count, choice;
    if (!read_int("Initial number of rectangles (1-1000): ", &count) || count < 1 || count > MAX_RECTANGLES) {
        printf("Invalid count.\n"); return 1;
    }
    items = malloc((size_t)count * sizeof(Rectangle));
    if (items == NULL) { printf("Memory allocation failed.\n"); return 1; }
    {
        int initial = count, i;
        count = 0;
        for (i = 0; i < initial; i++) add_rectangle(items, &count);
    }
    do {
        printf("\n1 Add  2 Display  3 Search  4 Update  5 Delete\n6 Summary  7 Sort  8 Filter  0 Exit\n");
        if (!read_int("Choice: ", &choice)) { printf("Invalid choice.\n"); continue; }
        if (choice == 1) {
            if (count < MAX_RECTANGLES) {
                Rectangle *temp = realloc(items, (size_t)(count + 1) * sizeof(Rectangle));
                if (temp == NULL) printf("Could not allocate more memory.\n");
                else { items = temp; add_rectangle(items, &count); }
            } else printf("Maximum capacity reached.\n");
        } else if (choice == 2) display_all(items, count);
        else if (choice == 3) {
            int id, index;
            if (read_int("ID to search: ", &id)) {
                index = find_rectangle(items, count, id);
                if (index == -1) printf("Rectangle not found.\n"); else display_rectangle(&items[index]);
            } else printf("Invalid ID.\n");
        } else if (choice == 4) update_rectangle(items, count);
        else if (choice == 5) delete_rectangle(items, &count);
        else if (choice == 6) show_summary(items, count);
        else if (choice == 7) sort_by_area(items, count);
        else if (choice == 8) filter_area(items, count);
        else if (choice != 0) printf("Unknown choice.\n");
    } while (choice != 0);
    free(items);
    printf("Memory released.\n");
    return 0;
}
