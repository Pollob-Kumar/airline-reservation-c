#include <stdio.h> //ei library input, outp kaj er jonno
#include <stdlib.h> //ei library use hoy exit(), system(), random numbers etc kaj er jonno

void mainMenu();
void signup();
void login();

int main() {
    printf("=========================================\n");
    printf("     Welcome to Airline Reservation\n");
    printf("=========================================\n");

   mainMenu();
    return 0;
}

void mainMenu() {
    int choice;

    while (1) {
        printf("\n--------- Main Menu ---------\n");
        printf("1. Signup\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("\n-----------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            signup();
            break;
        case 2:
            login();
            break;
        case 3:
            printf("\nThank you for using the system!\n");
            exit(0);
        default:
            printf("\nInvalid choice! Try again.\n");
        }
    }
}

void signup(){
    printf("signup page");
}
void login(){
    printf("login page");
}