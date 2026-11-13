/*
Task: Interactive Command-Driven Record Manager (in-memory task list).
Sample run: add 101 Read notes, ADD 102 Finish lab, view, complete 101,
find 101, delete 102, help, exit.
Edge cases: mixed-case and unknown commands, duplicate/missing IDs, long
input lines, empty descriptions/list, EOF and dynamic array expansion.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DESCRIPTION_SIZE 100
#define LINE_SIZE 150

typedef enum { PENDING, DONE } TaskStatus;
typedef struct { int id; char description[DESCRIPTION_SIZE]; TaskStatus status; } Task;
typedef enum { CMD_ADD, CMD_DELETE, CMD_VIEW, CMD_UPDATE, CMD_COMPLETE, CMD_FIND, CMD_HELP, CMD_EXIT, CMD_UNKNOWN } CommandType;

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

void to_upper(char *text) {
    int i;
    for (i = 0; text[i] != '\0'; i++) text[i] = (char)toupper((unsigned char)text[i]);
}

CommandType get_command(const char *text) {
    char command[LINE_SIZE];
    if (strlen(text) >= sizeof(command)) return CMD_UNKNOWN;
    strcpy(command, text);
    to_upper(command);
    if (strcmp(command, "ADD") == 0) return CMD_ADD;
    if (strcmp(command, "DELETE") == 0) return CMD_DELETE;
    if (strcmp(command, "VIEW") == 0) return CMD_VIEW;
    if (strcmp(command, "UPDATE") == 0) return CMD_UPDATE;
    if (strcmp(command, "COMPLETE") == 0) return CMD_COMPLETE;
    if (strcmp(command, "FIND") == 0) return CMD_FIND;
    if (strcmp(command, "HELP") == 0) return CMD_HELP;
    if (strcmp(command, "EXIT") == 0) return CMD_EXIT;
    return CMD_UNKNOWN;
}

int find_task(const Task *tasks, int count, int id) {
    int i;
    for (i = 0; i < count; i++) if (tasks[i].id == id) return i;
    return -1;
}

int read_id(int *id) {
    char line[LINE_SIZE], extra;
    printf("Task ID: ");
    return read_line(line, sizeof(line)) == 1 && sscanf(line, " %d %c", id, &extra) == 1 && *id > 0;
}

int read_description(char *description) {
    int result;
    printf("Description: ");
    result = read_line(description, DESCRIPTION_SIZE);
    if (result != 1 || description[0] == '\0') {
        printf("Description is empty or too long.\n"); return 0;
    }
    return 1;
}

void add_task(Task **tasks, int *count, int *capacity) {
    Task task, *temp;
    if (!read_id(&task.id)) { printf("Invalid task ID.\n"); return; }
    if (find_task(*tasks, *count, task.id) != -1) { printf("Task ID already exists.\n"); return; }
    if (!read_description(task.description)) return;
    if (*count == *capacity) {
        int new_capacity = (*capacity == 0) ? 2 : *capacity * 2;
        if (new_capacity < *capacity || (size_t)new_capacity > (size_t)-1 / sizeof(Task)) {
            printf("Capacity limit reached.\n"); return;
        }
        temp = realloc(*tasks, (size_t)new_capacity * sizeof(Task));
        if (temp == NULL) { printf("Memory allocation failed.\n"); return; }
        *tasks = temp;
        *capacity = new_capacity;
    }
    task.status = PENDING;
    (*tasks)[*count] = task;
    (*count)++;
    printf("Task %d added.\n", task.id);
}

void view_tasks(const Task *tasks, int count) {
    int i;
    if (count == 0) { printf("Task list is empty.\n"); return; }
    for (i = 0; i < count; i++)
        printf("%d | %s | %s\n", tasks[i].id, tasks[i].description,
               tasks[i].status == DONE ? "DONE" : "PENDING");
}

void delete_task(Task *tasks, int *count) {
    int id, index, i;
    if (!read_id(&id)) { printf("Invalid task ID.\n"); return; }
    index = find_task(tasks, *count, id);
    if (index == -1) { printf("Task not found.\n"); return; }
    for (i = index; i < *count - 1; i++) tasks[i] = tasks[i + 1];
    (*count)--;
    printf("Task %d deleted.\n", id);
}

void update_task(Task *tasks, int count) {
    int id, index;
    char description[DESCRIPTION_SIZE];
    if (!read_id(&id)) { printf("Invalid task ID.\n"); return; }
    index = find_task(tasks, count, id);
    if (index == -1) { printf("Task not found.\n"); return; }
    if (!read_description(description)) return;
    strcpy(tasks[index].description, description);
    printf("Task %d updated.\n", id);
}

void complete_task(Task *tasks, int count) {
    int id, index;
    if (!read_id(&id)) { printf("Invalid task ID.\n"); return; }
    index = find_task(tasks, count, id);
    if (index == -1) { printf("Task not found.\n"); return; }
    tasks[index].status = DONE;
    printf("Task %d marked DONE.\n", id);
}

void find_and_show(const Task *tasks, int count) {
    int id, index;
    if (!read_id(&id)) { printf("Invalid task ID.\n"); return; }
    index = find_task(tasks, count, id);
    if (index == -1) printf("Task not found.\n");
    else printf("%d | %s | %s\n", tasks[index].id, tasks[index].description,
                tasks[index].status == DONE ? "DONE" : "PENDING");
}

void show_help(void) {
    printf("ADD - create a task\nDELETE - remove a task\nVIEW - list all tasks\n");
    printf("UPDATE - change a description\nCOMPLETE - mark a task done\n");
    printf("FIND - search by ID\nHELP - show commands\nEXIT - quit\n");
}

int main(void) {
    Task *tasks = NULL;
    int count = 0, capacity = 0, input_result, running = 1;
    char command[LINE_SIZE];
    while (running) {
        CommandType choice;
        printf("\nCommand (HELP for list): ");
        input_result = read_line(command, sizeof(command));
        if (input_result == 0) break;
        if (input_result == -1) { printf("Command was too long.\n"); continue; }
        choice = get_command(command);
        switch (choice) {
            case CMD_ADD: add_task(&tasks, &count, &capacity); break;
            case CMD_DELETE: delete_task(tasks, &count); break;
            case CMD_VIEW: view_tasks(tasks, count); break;
            case CMD_UPDATE: update_task(tasks, count); break;
            case CMD_COMPLETE: complete_task(tasks, count); break;
            case CMD_FIND: find_and_show(tasks, count); break;
            case CMD_HELP: show_help(); break;
            case CMD_EXIT: running = 0; break;
            default: printf("Unknown command. Type HELP.\n");
        }
    }
    free(tasks);
    printf("Task memory released. Goodbye.\n");
    return 0;
}
