#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_MOVIES 5
#define MAX_SEATS 100
#define MAX_USERS 10
#define MAX_UNDO_REDO 10 // Maximum number of undo/redo operations allowed

// Define optional items
struct OptionalItem {
    char name[50];
    float price;
};

struct Movie {
    char title[50];
    int availableSeats;
    float ticketPrice;
    float rating;
};

struct Ticket {
    int movieIndex;
    int numSeats;
    int seatNumbers[10];
    float totalAmount;
};

// Function declarations
float offerOptionalItems();
void saveTicketToFile(struct Movie movies[], struct Ticket *ticket);
void undoLastBooking(struct Movie movies[], int seats[], struct Ticket *ticket, struct Ticket undoStack[], int *undoTop, struct Ticket redoStack[], int *redoTop);

struct User {
    char username[50];
    char password[50];
};

void displayMovies(struct Movie movies[], int numMovies) {
    int i;
    printf("Available Movies:\n");
    for (i = 0; i < numMovies; i++) {
        printf("%d. %s (Rating: %.1f) - Available Seats: %d\n", i + 1, movies[i].title, movies[i].rating, movies[i].availableSeats);
    }
}

void displaySeatingChart(int seats[], int numRows, int numCols) {
    int i, j;
    printf("Seating Chart:\n");
    for (i = 0; i < numRows; i++) {
        for (j = 0; j < numCols; j++) {
            int seatNumber = i * numCols + j;
            if (seats[seatNumber] == 0) {
                printf("O "); // Available seat
            } else {
                printf("X "); // Booked seat
            }
        }
        printf("\n");
    }
}

void bookSeats(struct Movie movies[], int movieIndex, int seats[], int numRows, int numCols, struct Ticket *ticket, struct Ticket undoStack[], int *undoTop, struct Ticket redoStack[], int *redoTop) {
    int i;
    if (movieIndex < 0 || movieIndex >= MAX_MOVIES) {
        printf("Invalid movie selection.\n");
        return;
    }

    if (movies[movieIndex].availableSeats == 0) {
        printf("Sorry, this movie is housefull.\n");
        return;
    }

    int numSeatsToBook;
    printf("Enter the number of seats you want to book (1-10): ");
    scanf("%d", &numSeatsToBook);

    if (numSeatsToBook < 1 || numSeatsToBook > 10) {
        printf("Invalid number of seats. Please enter a number between 1 and 10.\n");
        return;
    }

    printf("Enter the seat numbers you want to book (e.g., 1 2 3): ");
    for (i = 0; i < numSeatsToBook; i++) {
        scanf("%d", &ticket->seatNumbers[i]);

        if (ticket->seatNumbers[i] < 1 || ticket->seatNumbers[i] > MAX_SEATS) {
            printf("Invalid seat number. Please enter a number between 1 and 100.\n");
            return;
        }

        int row = (ticket->seatNumbers[i] - 1) / numCols;
        int col = (ticket->seatNumbers[i] - 1) % numCols;

        if (seats[ticket->seatNumbers[i] - 1] == 0) {
            seats[ticket->seatNumbers[i] - 1] = 1; // Mark the seat as booked
            movies[movieIndex].availableSeats--;
            ticket->totalAmount += movies[movieIndex].ticketPrice;
        } else {
            printf("Seat %d is already booked. Booking failed.\n", ticket->seatNumbers[i]);
            return;
        }
    }

    ticket->movieIndex = movieIndex;
    ticket->numSeats = numSeatsToBook;
    printf("Seats booked successfully for %s. Total amount: Rs.%.2f\n", movies[movieIndex].title, ticket->totalAmount);

    // Offer optional items
    float optionalItemsTotal = offerOptionalItems();

    // Add the cost of optional items to the total amount
    ticket->totalAmount += optionalItemsTotal;

    // Save ticket data to a file with an attractive format
    saveTicketToFile(movies, ticket);

    // Push the booking onto the undo stack
    if (*undoTop < MAX_UNDO_REDO) {
        undoStack[*undoTop] = *ticket;
        (*undoTop)++;
    } else {
        printf("Undo stack is full. Cannot save more undo operations.\n");
    }

    // Clear the redo stack when a new booking is made
    *redoTop = 0;
}

void undoLastBooking(struct Movie movies[], int seats[], struct Ticket *ticket, struct Ticket undoStack[], int *undoTop, struct Ticket redoStack[], int *redoTop) {
    if (*undoTop > 0) {
        // Pop the last booking from the undo stack
        (*undoTop)--;
        struct Ticket lastBooking = undoStack[*undoTop];

        // Push the last booking onto the redo stack
        if (*redoTop < MAX_UNDO_REDO) {
            redoStack[*redoTop] = lastBooking;
            (*redoTop)++;
        } else {
            printf("Redo stack is full. Cannot save more redo operations.\n");
        }

        // Undo the booking (free seats and adjust available seats and total amount)
        int i;
        for (i = 0; i < lastBooking.numSeats; i++) {
            int seatNumber = lastBooking.seatNumbers[i];
            seats[seatNumber - 1] = 0; // Mark the seat as available
            movies[lastBooking.movieIndex].availableSeats++;
            ticket->totalAmount -= movies[lastBooking.movieIndex].ticketPrice;
        }

        printf("Last booking has been undone.\n");
    } else {
        printf("No booking to undo.\n");
    }
}

void redoLastBooking(struct Movie movies[], int seats[], struct Ticket *ticket, struct Ticket undoStack[], int *undoTop, struct Ticket redoStack[], int *redoTop) {
    if (*redoTop > 0) {
        // Pop the last redo booking from the redo stack
        (*redoTop)--;
        struct Ticket lastRedoBooking = redoStack[*redoTop];

        // Push the last redo booking onto the undo stack
        if (*undoTop < MAX_UNDO_REDO) {
            undoStack[*undoTop] = lastRedoBooking;
            (*undoTop)++;
        } else {
            printf("Undo stack is full. Cannot save more undo operations.\n");
        }

        // Redo the booking (book seats and adjust available seats and total amount)
        int i;
        for (i = 0; i < lastRedoBooking.numSeats; i++) {
            int seatNumber = lastRedoBooking.seatNumbers[i];
            seats[seatNumber - 1] = 1; // Mark the seat as booked
            movies[lastRedoBooking.movieIndex].availableSeats--;
            ticket->totalAmount += movies[lastRedoBooking.movieIndex].ticketPrice;
        }

        printf("Last undo booking has been redone.\n");
    } else {
        printf("No booking to redo.\n");
    }
}

float offerOptionalItems() {
    int i;
    struct OptionalItem optionalItems[] = {
        {"Popcorn", 5.0},
        {"Soda", 3.0},
        {"Candy", 2.0}
    };
    int numOptionalItems = sizeof(optionalItems) / sizeof(optionalItems[0]);

    printf("Optional Items:\n");
    for (i = 0; i < numOptionalItems; i++) {
        printf("%d. %s - Rs.%.2f\n", i + 1, optionalItems[i].name, optionalItems[i].price);
    }

    float optionalItemsTotal = 0.0;
    int numSelectedItems = 0;
    printf("Select optional items (enter item numbers, separated by spaces, or 0 to skip): ");
    do {
        int itemNumber;
        scanf("%d", &itemNumber);

        if (itemNumber == 0) {
            break; // User chose to skip optional items
        }

        if (itemNumber >= 1 && itemNumber <= numOptionalItems) {
            optionalItemsTotal += optionalItems[itemNumber - 1].price;
            numSelectedItems++;
        } else {
            printf("Invalid item number. Please enter a valid item number or 0 to skip.\n");
        }
    } while (numSelectedItems < numOptionalItems);

    return optionalItemsTotal;
}

void saveTicketToFile(struct Movie movies[], struct Ticket *ticket) {
    int i;
    FILE *file = fopen("ticket_data.txt", "w");
    if (file != NULL) {
        fprintf(file, "********************************************\n");
        fprintf(file, "                Movie Ticket                \n");
        fprintf(file, "********************************************\n");
        fprintf(file, "Movie: %s\n", movies[ticket->movieIndex].title);
        fprintf(file, "--------------------------------------------\n");
        fprintf(file, "Number of Seats: %d\n", ticket->numSeats);
        fprintf(file, "Seat Numbers: ");
        for (i = 0; i < ticket->numSeats; i++) {
            fprintf(file, "%d ", ticket->seatNumbers[i]);
        }
        fprintf(file, "\n");
        fprintf(file, "Total Amount: Rs.%.2f\n", ticket->totalAmount);
        fprintf(file, "********************************************\n\n");
        fclose(file);
        printf("Ticket saved as 'ticket_data.txt'.\n");
    } else {
        printf("Failed to save ticket data to a file.\n");
    }
}

int main() {
    int i;
    struct Movie movies[MAX_MOVIES];
    strcpy(movies[0].title, "Avengers Endgame");
    movies[0].availableSeats = MAX_SEATS;
    movies[0].ticketPrice = 10.0;
    movies[0].rating = 4.5;

    strcpy(movies[1].title, "Oppenheimer");
    movies[1].availableSeats = MAX_SEATS;
    movies[1].ticketPrice = 12.0;
    movies[1].rating = 3.8;

    int numMovies = 2;

    int seats[MAX_SEATS] = {0};

    int numRows = 10;
    int numCols = 10;

    struct Ticket ticket;
    struct Ticket undoStack[MAX_UNDO_REDO];
    int undoTop = 0; // Initialize the undo stack top
    struct Ticket redoStack[MAX_UNDO_REDO];
    int redoTop = 0; // Initialize the redo stack top

    int choice;
    printf("Welcome to the Movie Ticket Booking System!\n");

    while (1) {
        printf("\nMenu:\n");
        printf("1. Display available movies\n");
        printf("2. Display seating chart\n");
        printf("3. Book seats\n");
        printf("4. Undo last booking\n");
        printf("5. Redo last booking\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayMovies(movies, numMovies);
                break;
            case 2:
                displaySeatingChart(seats, numRows, numCols);
                break;
            case 3:
                displayMovies(movies, numMovies);
                printf("Enter the movie number you want to book seats for (or 0 to cancel): ");
                int movieIndex;
                scanf("%d", &movieIndex);

                if (movieIndex == 0) {
                    break;
                }

                if (movieIndex >= 1 && movieIndex <= numMovies) {
                    bookSeats(movies, movieIndex - 1, seats, numRows, numCols, &ticket, undoStack, &undoTop, redoStack, &redoTop);
                } else {
                    printf("Invalid movie selection.\n");
                }
                break;
            case 4:
                undoLastBooking(movies, seats, &ticket, undoStack, &undoTop, redoStack, &redoTop);
                break;
            case 5:
                redoLastBooking(movies, seats, &ticket, undoStack, &undoTop, redoStack, &redoTop);
                break;
            case 6:
                printf("Exiting the program.\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
        }

        // Check if all seats are sold out
        int allSeatsSoldOut = 1;
        for (i = 0; i < numMovies; i++) {
            if (movies[i].availableSeats > 0) {
                allSeatsSoldOut = 0;
                break;
            }
        }

        if (allSeatsSoldOut) {
            printf("All seats are sold out. Housefull!\n");
            return 0;
        }
    }

    return 0;
}

