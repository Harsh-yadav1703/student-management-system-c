/*
 * Student Management System
 * Language: C
 * Features: Add, display, search, and delete student records.
 * Records are stored in students.dat in the same folder as the program.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "students.dat"
#define TEMP_FILE "students.tmp"

typedef struct {
    int roll_no;
    char name[60];
    char course[40];
    float marks;
} Student;

static void clear_input_buffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* discard remaining input */
    }
}

static void read_line(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    } else {
        buffer[0] = '\0';
    }
}

static int read_integer(const char *prompt) {
    int value;
    int result;
    for (;;) {
        printf("%s", prompt);
        result = scanf("%d", &value);
        clear_input_buffer();
        if (result == 1) {
            return value;
        }
        printf("Please enter a valid whole number.\n");
    }
}

static float read_marks(const char *prompt) {
    float value;
    int result;
    for (;;) {
        printf("%s", prompt);
        result = scanf("%f", &value);
        clear_input_buffer();
        if (result == 1 && value >= 0.0f && value <= 100.0f) {
            return value;
        }
        printf("Enter marks between 0 and 100.\n");
    }
}

static int roll_number_exists(int roll_no) {
    FILE *file = fopen(DATA_FILE, "rb");
    Student student;

    if (file == NULL) {
        return 0;
    }

    while (fread(&student, sizeof(Student), 1, file) == 1) {
        if (student.roll_no == roll_no) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

static void add_student(void) {
    FILE *file;
    Student student;

    printf("\n--- Add Student ---\n");
    student.roll_no = read_integer("Enter roll number: ");

    if (roll_number_exists(student.roll_no)) {
        printf("A student with this roll number already exists.\n");
        return;
    }

    read_line("Enter student name: ", student.name, sizeof(student.name));
    read_line("Enter course/class: ", student.course, sizeof(student.course));
    student.marks = read_marks("Enter marks (0-100): ");

    file = fopen(DATA_FILE, "ab");
    if (file == NULL) {
        printf("Unable to open the data file.\n");
        return;
    }

    if (fwrite(&student, sizeof(Student), 1, file) == 1) {
        printf("Student record added successfully.\n");
    } else {
        printf("Could not save the student record.\n");
    }

    fclose(file);
}

static void display_students(void) {
    FILE *file = fopen(DATA_FILE, "rb");
    Student student;
    int count = 0;

    printf("\n--- Student Records ---\n");
    if (file == NULL) {
        printf("No records found yet.\n");
        return;
    }

    printf("%-10s %-25s %-18s %-8s\n", "Roll No.", "Name", "Course", "Marks");
    printf("---------------------------------------------------------------\n");

    while (fread(&student, sizeof(Student), 1, file) == 1) {
        printf("%-10d %-25.25s %-18.18s %-8.2f\n",
               student.roll_no, student.name, student.course, student.marks);
        count++;
    }

    if (count == 0) {
        printf("No records found.\n");
    }

    fclose(file);
}

static void search_student(void) {
    FILE *file = fopen(DATA_FILE, "rb");
    Student student;
    int roll_no;
    int found = 0;

    printf("\n--- Search Student ---\n");
    roll_no = read_integer("Enter roll number to search: ");

    if (file == NULL) {
        printf("No records found yet.\n");
        return;
    }

    while (fread(&student, sizeof(Student), 1, file) == 1) {
        if (student.roll_no == roll_no) {
            printf("\nStudent found:\n");
            printf("Roll number : %d\n", student.roll_no);
            printf("Name        : %s\n", student.name);
            printf("Course      : %s\n", student.course);
            printf("Marks       : %.2f\n", student.marks);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("No student found with roll number %d.\n", roll_no);
    }

    fclose(file);
}

static void delete_student(void) {
    FILE *source = fopen(DATA_FILE, "rb");
    FILE *temporary;
    Student student;
    int roll_no;
    int found = 0;

    printf("\n--- Delete Student ---\n");
    roll_no = read_integer("Enter roll number to delete: ");

    if (source == NULL) {
        printf("No records found yet.\n");
        return;
    }

    temporary = fopen(TEMP_FILE, "wb");
    if (temporary == NULL) {
        printf("Unable to create a temporary file.\n");
        fclose(source);
        return;
    }

    while (fread(&student, sizeof(Student), 1, source) == 1) {
        if (student.roll_no == roll_no) {
            found = 1;
        } else {
            fwrite(&student, sizeof(Student), 1, temporary);
        }
    }

    fclose(source);
    fclose(temporary);

    if (remove(DATA_FILE) != 0 || rename(TEMP_FILE, DATA_FILE) != 0) {
        printf("Could not update the data file.\n");
        return;
    }

    if (found) {
        printf("Student record deleted successfully.\n");
    } else {
        printf("No student found with roll number %d.\n", roll_no);
    }
}

int main(void) {
    int choice;

    printf("====================================\n");
    printf("     STUDENT MANAGEMENT SYSTEM      \n");
    printf("====================================\n");

    do {
        printf("\n1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student\n");
        printf("4. Delete Student\n");
        printf("5. Exit\n");

        choice = read_integer("Enter your choice: ");

        switch (choice) {
            case 1:
                add_student();
                break;
            case 2:
                display_students();
                break;
            case 3:
                search_student();
                break;
            case 4:
                delete_student();
                break;
            case 5:
                printf("Thank you for using the Student Management System.\n");
                break;
            default:
                printf("Invalid choice. Please select 1 to 5.\n");
        }
    } while (choice != 5);

    return 0;
}
