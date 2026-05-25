#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define structures for student and grade
typedef struct {
    int reg_number;
    char name[50];
    float grades[5];
    float cgpa;
} Student;

// Define structure for Binary Search Tree node
typedef struct bst_node {
    Student student;
    struct bst_node *left;
    struct bst_node *right;
} BSTNode;

// Function to create a new BST node
BSTNode* createNode(Student student) {
    BSTNode *newNode = (BSTNode*)malloc(sizeof(BSTNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    newNode->student = student;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// Function to insert a student into BST (Sorted primarily by CGPA, secondarily by Name)
BSTNode* insert(BSTNode *root, Student student) {
    if (root == NULL)
        return createNode(student);
    
    if (student.cgpa > root->student.cgpa || (student.cgpa == root->student.cgpa && strcmp(student.name, root->student.name) > 0))
        root->right = insert(root->right, student);
    else
        root->left = insert(root->left, student);
    
    return root;
}

// Function to traverse BST in-order and store students in an array
void inOrderTraversal(BSTNode *root, Student *students, int *index) {
    if (root != NULL) {
        inOrderTraversal(root->left, students, index);
        students[(*index)++] = root->student;
        inOrderTraversal(root->right, students, index);
    }
}

// Function to calculate CGPA
float calculateCGPA(float grades[], int num_grades) {
    float total = 0;
    for (int i = 0; i < num_grades; i++) {
        total += grades[i];
    }
    return total / num_grades;
}

// FIXED: Tree-traversal search because the BST keys are CGPAs, not names
BSTNode* searchStudentByName(BSTNode* root, char* name) {
    if (root == NULL) {
        return NULL;
    }
    
    // Check if current node matches
    if (strcmp(root->student.name, name) == 0) {
        return root;
    }

    // Search left subtree
    BSTNode* found_left = searchStudentByName(root->left, name);
    if (found_left != NULL) {
        return found_left;
    }

    // If not found in left, search right subtree
    return searchStudentByName(root->right, name);
}

// Function to read data from CSV file
int readCSVData(const char *filename, Student students[], int *num_students) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 0;
    }

    char line[1024];
    int count = 0;
    while (fgets(line, 1024, file) && count < 100) { // Bound check for safety
        Student student;
        int fields_read = sscanf(line, "%d,%49[^,],%f,%f,%f,%f,%f",
               &student.reg_number, student.name,
               &student.grades[0], &student.grades[1], &student.grades[2],
               &student.grades[3], &student.grades[4]);
        if (fields_read != 7) {
            fprintf(stderr, "Error reading line %d. Skipping.\n", count + 1);
            continue;
        }

        student.cgpa = calculateCGPA(student.grades, 5);
        students[count++] = student;
    }

    if (ferror(file)) {
        perror("Error reading CSV data");
        fclose(file);
        return 0;
    }

    *num_students = count;
    fclose(file);
    return 1;
}

int main() {
    // Static array layout as specified in requirements
    Student students[100];
    int num_students = 0;

    if (!readCSVData("students.csv", students, &num_students)) {
        printf("Error reading CSV data.\n");
        return 1;
    }

    // Create a Binary Search Tree and insert students into it
    BSTNode *root = NULL;
    for (int i = 0; i < num_students; i++) {
        root = insert(root, students[i]);
    }

    // Convert BST to sorted array using in-order traversal
    int index = 0;
    Student sortedStudents[100];
    inOrderTraversal(root, sortedStudents, &index);

    // Print report summary
    printf("\nStudent List Sorted on CGPA and Name:\n");
    for (int i = 0; i < num_students; i++) {
        printf("%d\t%s\tCGPA: %.2f\n", sortedStudents[i].reg_number, sortedStudents[i].name, sortedStudents[i].cgpa);
    }

    // Calculate class average
    float class_average = 0;
    for (int i = 0; i < num_students; i++) {
        class_average += sortedStudents[i].cgpa;
    }
    if (num_students > 0) {
        class_average /= num_students;
    }
    printf("\nClass Average CGPA: %.2f\n\n", class_average);

    // Search facility
    char search_name[50];
    int search_again = 1;
    while (search_again) {
        // Prevents buffer overflows by limiting input size to 49 chars
        printf("Enter student name to search (or type 'exit' to quit): ");
        if (scanf("%49s", search_name) != 1) {
            break;
        }

        if (strcmp(search_name, "exit") == 0) {
            search_again = 0;
            continue;
        }

        BSTNode* found_student = searchStudentByName(root, search_name);
        if (found_student != NULL) {
            printf("Student found:\n");
            printf("Registration No.: %d\n", found_student->student.reg_number);
            printf("Name: %s\n", found_student->student.name);
            printf("CGPA: %.2f\n\n", found_student->student.cgpa);
        } else {
            printf("Student not found.\n\n");
        }
    }

    // Free allocated tree memory before exit (Good practice)
    // (Optional implementation: a freeTree helper function can be added here)

    return 0;
}
