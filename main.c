#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USERS_FILE "users.txt"
#define ADMINS_FILE "admins.txt"
#define FLIGHTS_FILE "flights.txt"
#define BOOKINGS_FILE "bookings.txt"
#define NOTIFS_FILE "notifications.txt"
#define HISTORY_FILE "history.txt"

#define ADMIN_CODE "234809"
#define LINE_BUF 1024

/* ---------- Data structures ---------- */
typedef struct {
    int id;
    char name[100];
    char email[100];
    char phone[30];
    char password[60];
    double balance;
    int ticketCount;
    int cancelledCount;
    int deleted; // 0 active, 1 deleted
} User;

typedef struct {
    int id;
    char name[100];
    char email[100];
    char phone[30];
    char password[60];
    double income;
} Admin;

typedef struct {
    int id;
    char name[100];
    char source[100];
    char destination[100];
    int totalSeats;
    int bookedSeats;
    double price;
    int cancelled; // 0 or 1
    char cancelReason[256];
} Flight;

typedef struct {
    int id;
    char userEmail[100];
    int flightId;
    int seatsBooked;
    double totalPrice;
    char status[20]; // Booked, Cancelled
    char cancelReason[256];
} Booking;

typedef struct {
    int id;
    char userEmail[100];
    char message[256];
    int readFlag; // 0 unread, 1 read
} Notification;

typedef struct {
    int id;
    char type[50]; // BOOK, CANCEL, ADMIN_CANCEL_FLIGHT, DELETE_USER, etc.
    int entityId;  // booking id or flight id or user id
    char details[512];
} HistoryRecord;

/* ---------- Globals ---------- */
User *users = NULL;
int userCount = 0;

Admin *admins = NULL;
int adminCount = 0;

Flight *flights = NULL;
int flightCount = 0;

Booking *bookings = NULL;
int bookingCount = 0;

Notification *notifs = NULL;
int notifCount = 0;

HistoryRecord *historyRecords = NULL;
int historyCount = 0;

/* ---------- Helpers ---------- */

void trim_newline(char *s) {
    if (!s) return;
    size_t l = strlen(s);
    while (l > 0 && (s[l-1] == '\n' || s[l-1] == '\r')) { s[l-1] = '\0'; l--; }
}

int next_id_from_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return 1;
    char line[LINE_BUF];
    int max = 0;
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        char tmp[LINE_BUF];
        strcpy(tmp, line);
        char *tok = strtok(tmp, "|");
        if (!tok) continue;
        int id = atoi(tok);
        if (id > max) max = id;
    }
    fclose(f);
    return max + 1;
}

void press_enter(void) {
    printf("\nPress Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/* ---------- Load / Save ---------- */

void load_users() {
    FILE *f = fopen(USERS_FILE, "r");
    userCount = 0;
    if (!f) return;
    char line[LINE_BUF];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        users = realloc(users, sizeof(User)*(userCount+1));
        char tmp[LINE_BUF]; strcpy(tmp, line);
        char *tok = strtok(tmp, "|");
        if (!tok) continue;
        users[userCount].id = atoi(tok);
        tok = strtok(NULL, "|"); if(!tok) strcpy(users[userCount].name, ""); else strncpy(users[userCount].name, tok, sizeof(users[userCount].name));
        tok = strtok(NULL, "|"); if(!tok) strcpy(users[userCount].email, ""); else strncpy(users[userCount].email, tok, sizeof(users[userCount].email));
        tok = strtok(NULL, "|"); if(!tok) strcpy(users[userCount].phone, ""); else strncpy(users[userCount].phone, tok, sizeof(users[userCount].phone));
        tok = strtok(NULL, "|"); if(!tok) strcpy(users[userCount].password, ""); else strncpy(users[userCount].password, tok, sizeof(users[userCount].password));
        tok = strtok(NULL, "|"); if(!tok) users[userCount].balance = 0.0; else users[userCount].balance = atof(tok);
        tok = strtok(NULL, "|"); if(!tok) users[userCount].ticketCount = 0; else users[userCount].ticketCount = atoi(tok);
        tok = strtok(NULL, "|"); if(!tok) users[userCount].cancelledCount = 0; else users[userCount].cancelledCount = atoi(tok);
        tok = strtok(NULL, "|"); if(!tok) users[userCount].deleted = 0; else users[userCount].deleted = atoi(tok);
        userCount++;
    }
    fclose(f);
}

void save_users() {
    FILE *f = fopen(USERS_FILE, "w");
    if (!f) { perror("save users"); return; }
    for (int i=0;i<userCount;i++) {
        fprintf(f, "%d|%s|%s|%s|%s|%.2f|%d|%d|%d\n",
            users[i].id, users[i].name, users[i].email, users[i].phone, users[i].password,
            users[i].balance, users[i].ticketCount, users[i].cancelledCount, users[i].deleted);
    }
    fclose(f);
}

void load_admins() {
    FILE *f = fopen(ADMINS_FILE, "r");
    adminCount = 0;
    if (!f) return;
    char line[LINE_BUF];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        admins = realloc(admins, sizeof(Admin)*(adminCount+1));
        char tmp[LINE_BUF]; strcpy(tmp, line);
        char *tok = strtok(tmp, "|");
        admins[adminCount].id = atoi(tok);
        tok = strtok(NULL, "|"); strncpy(admins[adminCount].name, tok?tok:"", sizeof(admins[adminCount].name));
        tok = strtok(NULL, "|"); strncpy(admins[adminCount].email, tok?tok:"", sizeof(admins[adminCount].email));
        tok = strtok(NULL, "|"); strncpy(admins[adminCount].phone, tok?tok:"", sizeof(admins[adminCount].phone));
        tok = strtok(NULL, "|"); strncpy(admins[adminCount].password, tok?tok:"", sizeof(admins[adminCount].password));
        tok = strtok(NULL, "|"); admins[adminCount].income = tok?atof(tok):0.0;
        adminCount++;
    }
    fclose(f);
}

void save_admins() {
    FILE *f = fopen(ADMINS_FILE, "w");
    if (!f) { perror("save admins"); return; }
    for (int i=0;i<adminCount;i++) {
        fprintf(f, "%d|%s|%s|%s|%s|%.2f\n",
            admins[i].id, admins[i].name, admins[i].email, admins[i].phone, admins[i].password, admins[i].income);
    }
    fclose(f);
}

void load_flights() {
    FILE *f = fopen(FLIGHTS_FILE, "r");
    flightCount = 0;
    if (!f) return;
    char line[LINE_BUF];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        flights = realloc(flights, sizeof(Flight)*(flightCount+1));
        char tmp[LINE_BUF]; strcpy(tmp, line);
        char *tok = strtok(tmp, "|");
        flights[flightCount].id = atoi(tok);
        tok = strtok(NULL, "|"); strncpy(flights[flightCount].name, tok?tok:"", sizeof(flights[flightCount].name));
        tok = strtok(NULL, "|"); strncpy(flights[flightCount].source, tok?tok:"", sizeof(flights[flightCount].source));
        tok = strtok(NULL, "|"); strncpy(flights[flightCount].destination, tok?tok:"", sizeof(flights[flightCount].destination));
        tok = strtok(NULL, "|"); flights[flightCount].totalSeats = tok?atoi(tok):0;
        tok = strtok(NULL, "|"); flights[flightCount].bookedSeats = tok?atoi(tok):0;
        tok = strtok(NULL, "|"); flights[flightCount].price = tok?atof(tok):0.0;
        tok = strtok(NULL, "|"); flights[flightCount].cancelled = tok?atoi(tok):0;
        tok = strtok(NULL, "|"); strncpy(flights[flightCount].cancelReason, tok?tok:"", sizeof(flights[flightCount].cancelReason));
        flightCount++;
    }
    fclose(f);
}

void save_flights() {
    FILE *f = fopen(FLIGHTS_FILE, "w");
    if (!f) { perror("save flights"); return; }
    for (int i=0;i<flightCount;i++) {
        fprintf(f, "%d|%s|%s|%s|%d|%d|%.2f|%d|%s\n",
            flights[i].id, flights[i].name, flights[i].source, flights[i].destination,
            flights[i].totalSeats, flights[i].bookedSeats, flights[i].price,
            flights[i].cancelled, flights[i].cancelReason);
    }
    fclose(f);
}

void load_bookings() {
    FILE *f = fopen(BOOKINGS_FILE, "r");
    bookingCount = 0;
    if (!f) return;
    char line[LINE_BUF];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        bookings = realloc(bookings, sizeof(Booking)*(bookingCount+1));
        char tmp[LINE_BUF]; strcpy(tmp, line);
        char *tok = strtok(tmp, "|");
        bookings[bookingCount].id = atoi(tok);
        tok = strtok(NULL, "|"); strncpy(bookings[bookingCount].userEmail, tok?tok:"", sizeof(bookings[bookingCount].userEmail));
        tok = strtok(NULL, "|"); bookings[bookingCount].flightId = tok?atoi(tok):0;
        tok = strtok(NULL, "|"); bookings[bookingCount].seatsBooked = tok?atoi(tok):0;
        tok = strtok(NULL, "|"); bookings[bookingCount].totalPrice = tok?atof(tok):0.0;
        tok = strtok(NULL, "|"); strncpy(bookings[bookingCount].status, tok?tok:"", sizeof(bookings[bookingCount].status));
        tok = strtok(NULL, "|"); strncpy(bookings[bookingCount].cancelReason, tok?tok:"", sizeof(bookings[bookingCount].cancelReason));
        bookingCount++;
    }
    fclose(f);
}

void save_bookings() {
    FILE *f = fopen(BOOKINGS_FILE, "w");
    if (!f) { perror("save bookings"); return; }
    for (int i=0;i<bookingCount;i++) {
        fprintf(f, "%d|%s|%d|%d|%.2f|%s|%s\n",
            bookings[i].id, bookings[i].userEmail, bookings[i].flightId, bookings[i].seatsBooked,
            bookings[i].totalPrice, bookings[i].status, bookings[i].cancelReason);
    }
    fclose(f);
}

void load_notifications() {
    FILE *f = fopen(NOTIFS_FILE, "r");
    notifCount = 0;
    if (!f) return;
    char line[LINE_BUF];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        notifs = realloc(notifs, sizeof(Notification)*(notifCount+1));
        char tmp[LINE_BUF]; strcpy(tmp, line);
        char *tok = strtok(tmp, "|");
        notifs[notifCount].id = atoi(tok);
        tok = strtok(NULL, "|"); strncpy(notifs[notifCount].userEmail, tok?tok:"", sizeof(notifs[notifCount].userEmail));
        tok = strtok(NULL, "|"); strncpy(notifs[notifCount].message, tok?tok:"", sizeof(notifs[notifCount].message));
        tok = strtok(NULL, "|"); notifs[notifCount].readFlag = tok?atoi(tok):0;
        notifCount++;
    }
    fclose(f);
}

void save_notifications() {
    FILE *f = fopen(NOTIFS_FILE, "w");
    if (!f) { perror("save notifs"); return; }
    for (int i=0;i<notifCount;i++) {
        fprintf(f, "%d|%s|%s|%d\n", notifs[i].id, notifs[i].userEmail, notifs[i].message, notifs[i].readFlag);
    }
    fclose(f);
}

void load_history() {
    FILE *f = fopen(HISTORY_FILE, "r");
    historyCount = 0;
    if (!f) return;
    char line[LINE_BUF];
    while (fgets(line, sizeof(line), f)) {
        trim_newline(line);
        if (strlen(line) == 0) continue;
        historyRecords = realloc(historyRecords, sizeof(HistoryRecord)*(historyCount+1));
        char tmp[LINE_BUF]; strcpy(tmp, line);
        char *tok = strtok(tmp, "|");
        historyRecords[historyCount].id = atoi(tok);
        tok = strtok(NULL, "|"); strncpy(historyRecords[historyCount].type, tok?tok:"", sizeof(historyRecords[historyCount].type));
        tok = strtok(NULL, "|"); historyRecords[historyCount].entityId = tok?atoi(tok):0;
        tok = strtok(NULL, "|"); strncpy(historyRecords[historyCount].details, tok?tok:"", sizeof(historyRecords[historyCount].details));
        historyCount++;
    }
    fclose(f);
}

void save_history() {
    FILE *f = fopen(HISTORY_FILE, "w");
    if (!f) { perror("save history"); return; }
    for (int i=0;i<historyCount;i++) {
        fprintf(f, "%d|%s|%d|%s\n", historyRecords[i].id, historyRecords[i].type, historyRecords[i].entityId, historyRecords[i].details);
    }
    fclose(f);
}

/* ---------- Utility finders ---------- */

int find_user_index_by_email(const char *email) {
    for (int i=0;i<userCount;i++) if (strcmp(users[i].email, email)==0 && users[i].deleted==0) return i;
    return -1;
}
int find_user_index_by_id(int uid) {
    for (int i=0;i<userCount;i++) if (users[i].id == uid) return i;
    return -1;
}
int find_admin_index_by_email(const char *email) {
    for (int i=0;i<adminCount;i++) if (strcmp(admins[i].email, email)==0) return i;
    return -1;
}
int find_flight_index_by_id(int fid) {
    for (int i=0;i<flightCount;i++) if (flights[i].id == fid) return i;
    return -1;
}
int find_booking_index_by_id(int bid) {
    for (int i=0;i<bookingCount;i++) if (bookings[i].id == bid) return i;
    return -1;
}

/* ---------- App functions ---------- */

void load_all() {
    load_users();
    load_admins();
    load_flights();
    load_bookings();
    load_notifications();
    load_history();
}

void save_all() {
    save_users();
    save_admins();
    save_flights();
    save_bookings();
    save_notifications();
    save_history();
}

/* Notifications helper */
void add_notification(const char *userEmail, const char *message) {
    Notification n;
    n.id = next_id_from_file(NOTIFS_FILE);
    strncpy(n.userEmail, userEmail, sizeof(n.userEmail));
    strncpy(n.message, message, sizeof(n.message));
    n.readFlag = 0;
    notifs = realloc(notifs, sizeof(Notification)*(notifCount+1));
    notifs[notifCount++] = n;
    save_notifications();
}

/* History helper */
void add_history(const char *type, int entityId, const char *details) {
    HistoryRecord h;
    h.id = next_id_from_file(HISTORY_FILE);
    strncpy(h.type, type, sizeof(h.type));
    h.entityId = entityId;
    strncpy(h.details, details, sizeof(h.details));
    historyRecords = realloc(historyRecords, sizeof(HistoryRecord)*(historyCount+1));
    historyRecords[historyCount++] = h;
    save_history();
}

/* ---------- Menus: Signup/Login/Main ---------- */

void main_menu();

void signup_menu() {
    printf("\n--- Signup ---\n");
    printf("1. User Signup\n2. Admin Signup\n3. Return\n4. Exit\nChoice: ");
    int opt;
    if (scanf("%d", &opt) != 1) { while (getchar()!='\n'); printf("Invalid\n"); return; }
    while (getchar()!='\n');
    if (opt == 3) return;
    if (opt==4) {
        save_all();
        printf("Exiting. Goodbye.\n");
        exit(0);
        }
    if (opt == 1) {
        User u;
        u.id = next_id_from_file(USERS_FILE);
        printf("Name: "); fgets(u.name, sizeof(u.name), stdin); trim_newline(u.name);
        printf("Email: "); fgets(u.email, sizeof(u.email), stdin); trim_newline(u.email);
        if (find_user_index_by_email(u.email) != -1) { printf("Email already registered.\n"); return; }
        printf("Phone: "); fgets(u.phone, sizeof(u.phone), stdin); trim_newline(u.phone);
        printf("Password: "); fgets(u.password, sizeof(u.password), stdin); trim_newline(u.password);
        u.balance = 0.0; u.ticketCount = 0; u.cancelledCount = 0; u.deleted = 0;
        users = realloc(users, sizeof(User)*(userCount+1));
        users[userCount++] = u;
        save_users();
        printf("User signed up successfully. You can login now.\n");
        add_history("SIGNUP_USER", u.id, u.email);
    } else if (opt == 2) {
        char code[64];
        printf("Admin hidden code: "); fgets(code, sizeof(code), stdin); trim_newline(code);
        if (strcmp(code, ADMIN_CODE) != 0) { printf("Wrong admin code.\n"); return; }
        Admin a; a.id = next_id_from_file(ADMINS_FILE);
        printf("Name: "); fgets(a.name, sizeof(a.name), stdin); trim_newline(a.name);
        printf("Email: "); fgets(a.email, sizeof(a.email), stdin); trim_newline(a.email);
        if (find_admin_index_by_email(a.email) != -1) { printf("Admin email exists.\n"); return; }
        printf("Phone: "); fgets(a.phone, sizeof(a.phone), stdin); trim_newline(a.phone);
        printf("Password: "); fgets(a.password, sizeof(a.password), stdin); trim_newline(a.password);
        a.income = 0.0;
        admins = realloc(admins, sizeof(Admin)*(adminCount+1));
        admins[adminCount++] = a;
        save_admins();
        printf("Admin created. You can login now.\n");
        add_history("SIGNUP_ADMIN", a.id, a.email);
    } else {
        printf("Invalid option\n");
    }
}

/* Login menu */
void user_dashboard(int userIdx);
void admin_dashboard(int adminIdx);

void show_user_notifications(int userIdx) {
    int found = 0;
    for (int i=0;i<notifCount;i++) {
        if (strcmp(notifs[i].userEmail, users[userIdx].email)==0 && notifs[i].readFlag==0) {
            printf("\n Notification: %s\n", notifs[i].message);
            notifs[i].readFlag = 1;
            found++;
        }
    }
    if (found) save_notifications();
}

void login_menu() {
    printf("\n--- Login ---\n");
    printf("1. User Login\n2. Admin Login\n3. Return\nChoice: ");
    int opt;
    if (scanf("%d", &opt) != 1) { while (getchar()!='\n'); printf("Invalid\n"); return; }
    while (getchar()!='\n');
    if (opt == 3) return;
    char email[100], password[60];
    printf("Email: "); fgets(email, sizeof(email), stdin); trim_newline(email);
    printf("Password: "); fgets(password, sizeof(password), stdin); trim_newline(password);
    if (opt == 1) {
        int idx = find_user_index_by_email(email);
        if (idx == -1) { printf("User not found or deleted.\n"); return; }
        if (strcmp(users[idx].password, password) != 0) { printf("Wrong password.\n"); return; }
        show_user_notifications(idx);
        add_history("LOGIN_USER", users[idx].id, users[idx].email);
        user_dashboard(idx);
    } else if (opt == 2) {
        int idx = find_admin_index_by_email(email);
        if (idx == -1) { printf("Admin not found.\n"); return; }
        if (strcmp(admins[idx].password, password) != 0) { printf("Wrong password.\n"); return; }
        add_history("LOGIN_ADMIN", admins[idx].id, admins[idx].email);
        admin_dashboard(idx);
    } else {
        printf("Invalid option\n");
    }
}

/* ---------- User features ---------- */

void list_flights_short() {
    if (flightCount == 0) { printf("No flights available.\n"); return; }
    printf("\nID | Name | From->To | Total | Booked | Available | Price\n");
    for (int i=0;i<flightCount;i++) {
        printf("%d | %s | %s->%s | %d | %d | %d | %.2f$\n",
            flights[i].id, flights[i].name, flights[i].source, flights[i].destination,
            flights[i].totalSeats, flights[i].bookedSeats, flights[i].totalSeats - flights[i].bookedSeats, flights[i].price);
        if (flights[i].cancelled) printf("   [CANCELLED] Reason: %s\n", flights[i].cancelReason);
    }
}

void book_ticket_user(int userIdx) {
    if (flightCount == 0) { printf("No flights.\n"); press_enter(); return; }
    list_flights_short();
    printf("Enter Flight ID to book (0 return): ");
    int fid; if (scanf("%d", &fid) != 1) { while(getchar()!='\n'); printf("Invalid\n"); return; }
    while (getchar()!='\n');
    if (fid==0) return;
    int fidx = find_flight_index_by_id(fid);
    if (fidx == -1) { printf("Flight not found\n"); press_enter(); return; }
    if (flights[fidx].cancelled) { printf("Flight cancelled: %s\n", flights[fidx].cancelReason); press_enter(); return; }
    printf("Available seats: %d\n", flights[fidx].totalSeats - flights[fidx].bookedSeats);
    printf("Enter seats to book: ");
    int seats; if (scanf("%d", &seats) != 1) { while(getchar()!='\n'); printf("Invalid\n"); return; }
    while (getchar()!='\n');
    if (seats <= 0) { printf("Invalid seats\n"); press_enter(); return; }
    if (flights[fidx].totalSeats - flights[fidx].bookedSeats < seats) { printf("Not enough seats\n"); press_enter(); return; }
    double total = seats * flights[fidx].price;
    if (users[userIdx].balance < total) { printf("Insufficient balance. Load dollars first.\n"); press_enter(); return; }

    users[userIdx].balance -= total;
    flights[fidx].bookedSeats += seats;
    if (adminCount>0) admins[0].income += total; // simple income accounting

    Booking b;
    b.id = next_id_from_file(BOOKINGS_FILE);
    strncpy(b.userEmail, users[userIdx].email, sizeof(b.userEmail));
    b.flightId = fid;
    b.seatsBooked = seats;
    b.totalPrice = total;
    strncpy(b.status, "Booked", sizeof(b.status));
    b.cancelReason[0] = '\0';
    bookings = realloc(bookings, sizeof(Booking)*(bookingCount+1));
    bookings[bookingCount++] = b;

    users[userIdx].ticketCount += seats;
    save_all();
    char details[256];
    snprintf(details, sizeof(details), "User %s booked %d seats on flight %s (BookingID:%d)", users[userIdx].email, seats, flights[fidx].name, b.id);
    add_history("BOOK", b.id, details);

    printf("Booking successful. Booking ID: %d, Total: %.2f$\n", b.id, total);
    press_enter();
}

void view_user_bookings(int userIdx) {
    printf("\nYour Bookings:\n");
    int found = 0;
    for (int i=0;i<bookingCount;i++) {
        if (strcmp(bookings[i].userEmail, users[userIdx].email)==0) {
            int fidx = find_flight_index_by_id(bookings[i].flightId);
            const char *fname = fidx!=-1?flights[fidx].name:"Unknown";
            printf("BookingID:%d | Flight:%s (ID:%d) | Seats:%d | Total:%.2f | Status:%s\n",
                bookings[i].id, fname, bookings[i].flightId, bookings[i].seatsBooked, bookings[i].totalPrice, bookings[i].status);
            if (strcmp(bookings[i].status,"Cancelled")==0 && strlen(bookings[i].cancelReason)>0) printf("  Cancel Reason: %s\n", bookings[i].cancelReason);
            found++;
        }
    }
    if (!found) printf("No bookings found.\n");
    press_enter();
}

void cancel_user_booking(int userIdx) {
    view_user_bookings(userIdx);
    printf("Enter Booking ID to cancel (0 return): ");
    int bid; if (scanf("%d", &bid) != 1) { while(getchar()!='\n'); printf("Invalid\n"); return; }
    while (getchar()!='\n');
    if (bid==0) return;
    int bidx = find_booking_index_by_id(bid);
    if (bidx == -1) { printf("Booking not found\n"); press_enter(); return; }
    if (strcmp(bookings[bidx].userEmail, users[userIdx].email) != 0) { printf("This booking is not yours\n"); press_enter(); return; }
    if (strcmp(bookings[bidx].status,"Cancelled")==0) { printf("Already cancelled\n"); press_enter(); return; }

    bookings[bidx].status[0]='\0'; strncpy(bookings[bidx].status,"Cancelled",sizeof(bookings[bidx].status));
    double refund = bookings[bidx].totalPrice * 0.8;
    users[userIdx].balance += refund;
    users[userIdx].cancelledCount += bookings[bidx].seatsBooked;
    int fidx = find_flight_index_by_id(bookings[bidx].flightId);
    if (fidx!=-1) {
        flights[fidx].bookedSeats -= bookings[bidx].seatsBooked;
        if (flights[fidx].bookedSeats < 0) flights[fidx].bookedSeats = 0;
    }
    save_all();
    char details[256];
    snprintf(details, sizeof(details), "User %s cancelled booking %d, refund %.2f", users[userIdx].email, bookings[bidx].id, refund);
    add_history("CANCEL_USER", bookings[bidx].id, details);
    printf("Cancelled. Refund %.2f$ credited.\n", refund);
    press_enter();
}

void load_dollar_user(int userIdx) {
    printf("Enter amount (USD) to load: ");
    double amt; if (scanf("%lf", &amt) != 1) { while(getchar()!='\n'); printf("Invalid\n"); return; }
    while (getchar()!='\n');
    if (amt <= 0) { printf("Invalid amount\n"); press_enter(); return; }
    users[userIdx].balance += amt;
    save_all();
    char details[256];
    snprintf(details, sizeof(details), "User %s loaded %.2f$", users[userIdx].email, amt);
    add_history("LOAD", users[userIdx].id, details);
    printf("Loaded %.2f$. New balance: %.2f$\n", amt, users[userIdx].balance);
    press_enter();
}

void view_profile_user(int userIdx) {
    printf("\n--- Profile ---\n");
    printf("Name: %s\nEmail: %s\nPhone: %s\nBalance: %.2f$\nTickets(bought seats): %d\nTickets(cancelled seats): %d\n",
        users[userIdx].name, users[userIdx].email, users[userIdx].phone, users[userIdx].balance, users[userIdx].ticketCount, users[userIdx].cancelledCount);
    press_enter();
}

void user_dashboard(int userIdx) {
    while (1) {
        printf("\n--- User Dashboard (%s) ---\n", users[userIdx].name);
        printf("1. Book Ticket\n2. View Booked Tickets\n3. Cancel Ticket\n4. Load Dollar\n5. View Profile\n6. Return\nChoice: ");
        int ch; if (scanf("%d", &ch) != 1) { while(getchar()!='\n'); printf("Invalid\n"); continue; }
        while (getchar()!='\n');
        if (ch==1) book_ticket_user(userIdx);
        else if (ch==2) view_user_bookings(userIdx);
        else if (ch==3) cancel_user_booking(userIdx);
        else if (ch==4) load_dollar_user(userIdx);
        else if (ch==5) view_profile_user(userIdx);
        else if (ch==6) break;
        else printf("Invalid\n");
    }
}

/* ---------- Admin features (power) ---------- */

void add_flight_admin(int adminIdx) {
    Flight f; f.id = next_id_from_file(FLIGHTS_FILE);
    printf("Flight Name: "); fgets(f.name, sizeof(f.name), stdin); trim_newline(f.name);
    printf("Source: "); fgets(f.source, sizeof(f.source), stdin); trim_newline(f.source);
    printf("Destination: "); fgets(f.destination, sizeof(f.destination), stdin); trim_newline(f.destination);
    printf("Total Seats: "); if (scanf("%d", &f.totalSeats) != 1) { while(getchar()!='\n'); printf("Invalid\n"); return; }
    printf("Price per seat: "); if (scanf("%lf", &f.price) != 1) { while(getchar()!='\n'); printf("Invalid\n"); return; }
    while (getchar()!='\n');
    f.bookedSeats = 0; f.cancelled = 0; f.cancelReason[0] = '\0';
    flights = realloc(flights, sizeof(Flight)*(flightCount+1));
    flights[flightCount++] = f;
    save_all();
    char details[256];
    snprintf(details, sizeof(details), "Admin added flight %s (ID:%d)", f.name, f.id);
    add_history("ADD_FLIGHT", f.id, details);
    printf("Flight added with ID %d\n", f.id);
    press_enter();
}

void view_flights_admin() {
    view_flights_admin:; // label not used but for future
    list_flights_short();
    press_enter();
}

void admin_view_all_users(int adminIdx) {
    printf("\n--- All Users ---\n");
    for (int i=0;i<userCount;i++) {
        printf("ID:%d | %s | %s | Balance: %.2f | Tickets:%d | Cancelled:%d | Deleted:%d\n",
            users[i].id, users[i].name, users[i].email, users[i].balance, users[i].ticketCount, users[i].cancelledCount, users[i].deleted);
    }
    press_enter();
}

void admin_delete_user(int adminIdx) {
    admin_view_all_users(adminIdx);
    printf("Enter User ID to delete (0 return): ");
    int uid; if (scanf("%d", &uid) != 1) { while(getchar()!='\n'); printf("Invalid\n"); return; }
    while (getchar()!='\n');
    if (uid==0) return;
    int uidx = find_user_index_by_id(uid);
    if (uidx == -1) { printf("User not found\n"); press_enter(); return; }
    printf("Enter reason for deletion: "); char reason[256]; fgets(reason, sizeof(reason), stdin); trim_newline(reason);

    users[uidx].deleted = 1;
    save_users();
    // notify user
    char msg[300];
    snprintf(msg, sizeof(msg), "Your account was deleted by admin. Reason: %s", reason);
    add_notification(users[uidx].email, msg);

    // cancel user's bookings and refund full
    for (int i=0;i<bookingCount;i++) {
        if (strcmp(bookings[i].userEmail, users[uidx].email)==0 && strcmp(bookings[i].status,"Booked")==0) {
            bookings[i].status[0]=0; strncpy(bookings[i].status,"Cancelled",sizeof(bookings[i].status));
            strncpy(bookings[i].cancelReason,"User deleted by admin",sizeof(bookings[i].cancelReason));
            int fidx = find_flight_index_by_id(bookings[i].flightId);
            if (fidx!=-1) {
                flights[fidx].bookedSeats -= bookings[i].seatsBooked;
                if (flights[fidx].bookedSeats < 0) flights[fidx].bookedSeats = 0;
            }
            // refund full
            // find user index (still exists) and add balance
            users[uidx].balance += bookings[i].totalPrice;
        }
    }
    save_all();
    char details[300];
    snprintf(details, sizeof(details), "Admin %s deleted user %s (ID:%d). Reason: %s", admins[adminIdx].email, users[uidx].email, users[uidx].id, reason);
    add_history("DELETE_USER", users[uidx].id, details);
    printf("User deleted, bookings cancelled and user notified.\n");
    press_enter();
}

void admin_view_history(int adminIdx) {
    printf("\n--- History Records ---\n");
    for (int i=0;i<historyCount;i++) {
        printf("ID:%d | Type:%s | Entity:%d | Details:%s\n", historyRecords[i].id, historyRecords[i].type, historyRecords[i].entityId, historyRecords[i].details);
    }
    press_enter();
}

void admin_cancel_any_booking(int adminIdx) {
    // show bookings
    printf("\n--- All Bookings ---\n");
    for (int i=0;i<bookingCount;i++) {
        printf("BookingID:%d | User:%s | FlightID:%d | Seats:%d | Total:%.2f | Status:%s\n",
            bookings[i].id, bookings[i].userEmail, bookings[i].flightId, bookings[i].seatsBooked, bookings[i].totalPrice, bookings[i].status);
    }
    printf("Enter Booking ID to cancel (0 return): ");
    int bid; if (scanf("%d", &bid) != 1) { while(getchar()!='\n'); printf("Invalid\n"); return; }
    while (getchar()!='\n');
    if (bid==0) return;
    int bidx = find_booking_index_by_id(bid);
    if (bidx==-1) { printf("Booking not found\n"); press_enter(); return; }
    if (strcmp(bookings[bidx].status,"Cancelled")==0) { printf("Already cancelled\n"); press_enter(); return; }
    printf("Enter reason for cancellation (user will be notified): ");
    char reason[256]; fgets(reason, sizeof(reason), stdin); trim_newline(reason);

    // cancel and refund full
    bookings[bidx].status[0]=0; strncpy(bookings[bidx].status,"Cancelled",sizeof(bookings[bidx].status));
    strncpy(bookings[bidx].cancelReason, reason, sizeof(bookings[bidx].cancelReason));
    int uidx = find_user_index_by_email(bookings[bidx].userEmail);
    if (uidx!=-1) {
        users[uidx].balance += bookings[bidx].totalPrice;
        users[uidx].cancelledCount += bookings[bidx].seatsBooked;
        char msg[300];
        snprintf(msg, sizeof(msg), "Your booking ID:%d was cancelled by admin. Reason: %s", bookings[bidx].id, reason);
        add_notification(users[uidx].email, msg);
    }
    int fidx = find_flight_index_by_id(bookings[bidx].flightId);
    if (fidx!=-1) {
        flights[fidx].bookedSeats -= bookings[bidx].seatsBooked;
        if (flights[fidx].bookedSeats < 0) flights[fidx].bookedSeats = 0;
    }
    save_all();
    char details[300];
    snprintf(details, sizeof(details), "Admin %s cancelled booking %d. Reason: %s", admins[adminIdx].email, bookings[bidx].id, reason);
    add_history("ADMIN_CANCEL_BOOKING", bookings[bidx].id, details);
    printf("Booking cancelled & user notified. Full refund issued.\n");
    press_enter();
}

/* Admin cancels flight (already in earlier phases) */
void admin_cancel_flight(int adminIdx) {
    list_flights_short();
    printf("Enter Flight ID to cancel (0 return): ");
    int fid; if (scanf("%d", &fid) != 1) { while(getchar()!='\n'); printf("Invalid\n"); return; }
    while (getchar()!='\n');
    if (fid==0) return;
    int fidx = find_flight_index_by_id(fid);
    if (fidx==-1) { printf("Flight not found\n"); press_enter(); return; }
    if (flights[fidx].cancelled) { printf("Already cancelled\n"); press_enter(); return; }
    printf("Enter reason for flight cancellation: ");
    char reason[256]; fgets(reason, sizeof(reason), stdin); trim_newline(reason);
    flights[fidx].cancelled = 1;
    strncpy(flights[fidx].cancelReason, reason, sizeof(flights[fidx].cancelReason));
    // process bookings
    for (int i=0;i<bookingCount;i++) {
        if (bookings[i].flightId == fid && strcmp(bookings[i].status,"Booked")==0) {
            bookings[i].status[0]=0; strncpy(bookings[i].status,"Cancelled",sizeof(bookings[i].status));
            strncpy(bookings[i].cancelReason, reason, sizeof(bookings[i].cancelReason));
            int uidx = find_user_index_by_email(bookings[i].userEmail);
            if (uidx!=-1) {
                users[uidx].balance += bookings[i].totalPrice;
                users[uidx].cancelledCount += bookings[i].seatsBooked;
                char msg[300];
                snprintf(msg, sizeof(msg), "Your flight '%s' (Booking ID:%d) was cancelled by admin. Reason: %s", flights[fidx].name, bookings[i].id, reason);
                add_notification(users[uidx].email, msg);
            }
        }
    }
    save_all();
    char details[300];
    snprintf(details, sizeof(details), "Admin %s cancelled flight %s (ID:%d). Reason: %s", admins[adminIdx].email, flights[fidx].name, flights[fidx].id, reason);
    add_history("ADMIN_CANCEL_FLIGHT", flights[fidx].id, details);
    printf("Flight cancelled; affected users refunded and notified.\n");
    press_enter();
}

void admin_dashboard(int adminIdx) {
    while (1) {
        printf("\n--- Admin Dashboard (%s) ---\n", admins[adminIdx].name);
        printf("1. Add Flight\n2. View Flights\n3. View Bookings & Cancel Booking\n4. View Users & Delete User\n5. Cancel Flight\n6. View History\n7. Return\nChoice: ");
        int ch; if (scanf("%d",&ch) != 1) { while(getchar()!='\n'); printf("Invalid\n"); continue; }
        while (getchar()!='\n');
        if (ch==1) add_flight_admin(adminIdx);
        else if (ch==2) view_flights_admin();
        else if (ch==3) admin_cancel_any_booking(adminIdx);
        else if (ch==4) {
            admin_view_all_users(adminIdx);
            printf("Options: 1.Delete User 2.Return\nChoice: ");
            int o; if (scanf("%d",&o)!=1) { while(getchar()!='\n'); printf("Invalid\n"); continue;}
            while (getchar()!='\n');
            if (o==1) admin_delete_user(adminIdx);
        }
        else if (ch==5) admin_cancel_flight(adminIdx);
        else if (ch==6) admin_view_history(adminIdx);
        else if (ch==7) break;
        else printf("Invalid\n");
    }
}

/* ---------- Main menu ---------- */

void main_menu() {
    load_all();
    while (1) {
        printf("\n=== AIRLINE RESERVATION SYSTEM ===\n");
        printf("1. Signup\n2. Login\n3. Exit\nChoice: ");
        int ch; if (scanf("%d",&ch)!=1) { while(getchar()!='\n'); printf("Invalid\n"); continue; }
        while (getchar()!='\n');
        if (ch==1) signup_menu();
        else if (ch==2) login_menu();
        else if (ch==3) {
            save_all();
            printf("Exiting. Goodbye.\n");
            break;
        } else printf("Invalid\n");
    }
}

/* ---------- Entry point ---------- */

int main() {
    printf("=== Airline Reservation System (Integrated Phase 1-4) ===\n");
    main_menu();
    return 0;
}

