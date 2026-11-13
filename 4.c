/*
Task: Student Records and Academic Analytics System.
Sample run: Add students 101, 102 and 103, search 102, update it, delete 101,
then display students sorted by GPA and view the GPA summary.
Edge cases: empty list, duplicate roll number, invalid age/GPA, long name,
missing roll number and capacity expansion. Sorting rearranges the stored array.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_SIZE 50
#define MAX_CAPACITY 10000

typedef struct {
    int roll;
    char name[NAME_SIZE];
    int age;
    float gpa;
} Student;

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
    if (read_line(line, sizeof(line)) != 1) return 0;
    return sscanf(line, " %d %c", value, &extra) == 1;
}

int find_student(Student *students, int count, int roll) {
    int i;
    for (i = 0; i < count; i++) {
        if (students[i].roll == roll) return i;
    }
    return -1;
}

int grow_students(Student **students, int *capacity) {
    Student *temp;
    int new_capacity = (*capacity == 0) ? 2 : *capacity * 2;
    if (new_capacity > MAX_CAPACITY || new_capacity < *capacity) return 0;
    temp = realloc(*students, (size_t)new_capacity * sizeof(Student));
    if (temp == NULL) return 0;
    *students = temp;
    *capacity = new_capacity;
    return 1;
}

int read_student_details(Student *student) {
    int result;
    char line[100], extra;
    printf("Name: ");
    result = read_line(student->name, NAME_SIZE);
    if (result != 1 || student->name[0] == '\0') {
        printf("Invalid or overlong name.\n");
        return 0;
    }
    if (!read_int("Age (16-100): ", &student->age) || student->age < 16 || student->age > 100) {
        printf("Invalid age.\n");
        return 0;
    }
    printf("GPA (0.00-4.00): ");
    if (read_line(line, sizeof(line)) != 1 || sscanf(line, " %f %c", &student->gpa, &extra) != 1 ||
        student->gpa < 0.0f || student->gpa > 4.0f) {
        printf("Invalid GPA.\n");
        return 0;
    }
    return 1;
}

void display_student(const Student *student) {
    printf("Roll: %d | Name: %s | Age: %d | GPA: %.2f\n",
           student->roll, student->name, student->age, student->gpa);
}

void add_student(Student **students, int *count, int *capacity) {
    Student student;
    if (!read_int("Roll number: ", &student.roll) || student.roll <= 0) {
        printf("Invalid roll number.\n"); return;
    }
    if (find_student(*students, *count, student.roll) != -1) {
        printf("That roll number already exists.\n"); return;
    }
    if (!read_student_details(&student)) return;
    if (*count == *capacity && !grow_students(students, capacity)) {
        printf("Could not increase memory. Student was not added.\n"); return;
    }
    (*students)[*count] = student;
    (*count)++;
    printf("Student %d added successfully.\n", student.roll);
}

void display_all(const Student *students, int count) {
    int i;
    if (count == 0) { printf("No student records.\n"); return; }
    for (i = 0; i < count; i++) display_student(&students[i]);
}

void update_student(Student *students, int count) {
    int roll, index;
    Student updated;
    if (!read_int("Roll number to update: ", &roll)) { printf("Invalid input.\n"); return; }
    index = find_student(students, count, roll);
    if (index == -1) { printf("Student not found.\n"); return; }
    updated = students[index];
    if (!read_student_details(&updated)) return;
    updated.roll = roll;
    students[index] = updated;
    printf("Student %d updated.\n", roll);
}

void delete_student(Student *students, int *count) {
    int roll, index, i;
    if (!read_int("Roll number to delete: ", &roll)) { printf("Invalid input.\n"); return; }
    index = find_student(students, *count, roll);
    if (index == -1) { printf("Student not found.\n"); return; }
    for (i = index; i < *count - 1; i++) students[i] = students[i + 1];
    (*count)--;
    printf("Student %d deleted.\n", roll);
}

void show_summary(const Student *students, int count) {
    int i, high = 0, low = 0;
    float total = 0.0f;
    if (count == 0) { printf("No GPA summary is available.\n"); return; }
    for (i = 0; i < count; i++) {
        total += students[i].gpa;
        if (students[i].gpa > students[high].gpa) high = i;
        if (students[i].gpa < students[low].gpa) low = i;
    }
    printf("Highest GPA: "); display_student(&students[high]);
    printf("Lowest GPA: "); display_student(&students[low]);
    printf("Average GPA: %.2f\n", total / count);
}

void sort_by_gpa(Student *students, int count) {
    int i, j;
    Student temp;
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (students[j].gpa < students[j + 1].gpa ||
                (students[j].gpa == students[j + 1].gpa && students[j].roll > students[j + 1].roll)) {
                temp = students[j]; students[j] = students[j + 1]; students[j + 1] = temp;
            }
        }
    }
    printf("Records sorted by GPA (descending).\n");
    display_all(students, count);
}

int main(void) {
    Student *students = NULL;
    int count = 0, capacity = 0, choice;
    if (!read_int("Initial capacity (0-10000): ", &capacity) || capacity < 0 || capacity > MAX_CAPACITY) {
        printf("Invalid initial capacity.\n");
        return 1;
    }
    if (capacity > 0) {
        students = malloc((size_t)capacity * sizeof(Student));
        if (students == NULL) {
            printf("Initial memory allocation failed.\n");
            return 1;
        }
    }
    do {
        printf("\n1 Add  2 Display  3 Search  4 Update  5 Delete\n6 GPA summary  7 Sort by GPA  0 Exit\n");
        if (!read_int("Choice: ", &choice)) { printf("Invalid choice.\n"); continue; }
        if (choice == 1) add_student(&students, &count, &capacity);
        else if (choice == 2) display_all(students, count);
        else if (choice == 3) {
            int roll, index;
            if (read_int("Roll number to search: ", &roll)) {
                index = find_student(students, count, roll);
                if (index == -1) printf("Student not found.\n"); else display_student(&students[index]);
            } else printf("Invalid input.\n");
        } else if (choice == 4) update_student(students, count);
        else if (choice == 5) delete_student(students, &count);
        else if (choice == 6) show_summary(students, count);
        else if (choice == 7) sort_by_gpa(students, count);
        else if (choice != 0) printf("Unknown choice.\n");
    } while (choice != 0);
    free(students);
    printf("Memory released. Goodbye.\n");
    return 0;
}
