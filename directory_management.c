#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 10007

typedef struct Record {
    char Order_Date[15];
    char Time[10];
    float Aging;
    int Customer_Id;
    char Gender[10];
    char Device_Type[20];
    char Customer_Login_type[20];
    char Product_Category[50];
    char Product[100];
    float Sales;
    int Quantity;
    float Discount;
    float Profit;
    float Shipping_Cost;
    char Order_Priority[20];
    char Payment_method[20];
    struct Record* next;
} Record;

Record* hashTable[TABLE_SIZE];

unsigned int hash(int key) {
    unsigned long hash = 5381;
    hash = hash * 11 + key;
    return hash % TABLE_SIZE;
}


void insertRecord(Record* rec) {
    unsigned int index = hash(rec->Customer_Id);
    rec->next = hashTable[index];
    hashTable[index] = rec;
}

Record* searchRecord(int Customer_Id) {
    unsigned int index = hash(Customer_Id);
    Record* curr = hashTable[index];
    while (curr) {
        if (curr->Customer_Id == Customer_Id) 
        return curr;
        curr = curr->next;
    }
    return NULL;
}

int deleteRecord(int Customer_Id) {
    unsigned int index = hash(Customer_Id);
    Record* curr = hashTable[index];
    Record* prev = NULL;

    while (curr) {
        if (curr->Customer_Id == Customer_Id) {
            if (prev) prev->next = curr->next;
            else hashTable[index] = curr->next;
            free(curr);
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }
    return 0;
}

void printRecord(Record* r) {
    if (!r) return;
    printf("Customer_Id: %d | Gender: %s | Aging: %.2f\n", r->Customer_Id, r->Gender, r->Aging);
    printf("Date: %s Time: %s\n", r->Order_Date, r->Time);
    printf("Device: %s | Login: %s\n", r->Device_Type, r->Customer_Login_type);
    printf("Product: %s (%s) | Qty: %d | Sales: %.2f | Discount: %.2f%%\n",
           r->Product, r->Product_Category, r->Quantity, r->Sales, r->Discount * 100);
    printf("Profit: %.2f | Shipping: %.2f | Priority: %s | Payment: %s\n\n",
           r->Profit, r->Shipping_Cost, r->Order_Priority, r->Payment_method);
}

void loadCSV(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Cannot open file.\n");
        return;
    }

    char line[8192];
    fgets(line, sizeof(line), fp);

    while (fgets(line, sizeof(line), fp)) {
        Record* r = malloc(sizeof(Record));
        if (!r) { printf("Memory error.\n"); return; }

        sscanf(line, "%[^,],%[^,],%f,%d,%[^,],%[^,],%[^,],%[^,],%[^,],%f,%d,%f,%f,%f,%[^,],%[^\n]",
               r->Order_Date, r->Time, &r->Aging, &r->Customer_Id, r->Gender,
               r->Device_Type, r->Customer_Login_type, r->Product_Category, r->Product,
               &r->Sales, &r->Quantity, &r->Discount, &r->Profit, &r->Shipping_Cost,
               r->Order_Priority, r->Payment_method);

        r->next = NULL;
        insertRecord(r);
    }

    fclose(fp);
    printf("CSV loaded successfully.\n");
}


void insertNewRecordFromUser() {
    Record* newRec = malloc(sizeof(Record));
    if (!newRec) {
        printf("Memory allocation failed.\n");
        return;
    }

    printf("Enter Order Date (YYYY-MM-DD): ");
    scanf("%s", newRec->Order_Date);
    printf("Enter Time (HH:MM): ");
    scanf("%s", newRec->Time);
    printf("Enter Aging (float): ");
    scanf("%f", &newRec->Aging);
    printf("Enter Customer_Id (int): ");
    scanf("%d", &newRec->Customer_Id);

    if (searchRecord(newRec->Customer_Id)) {
        printf("Error: Customer_Id already exists.\n");
        free(newRec);
        return;
    }

    printf("Enter Gender: ");
    scanf("%s", newRec->Gender);
    printf("Enter Device Type: ");
    scanf("%s", newRec->Device_Type);
    printf("Enter Customer Login Type: ");
    scanf("%s", newRec->Customer_Login_type);
    printf("Enter Product Category: ");
    scanf("%s", newRec->Product_Category);
    printf("Enter Product: ");
    scanf(" %[^\n]", newRec->Product);
    printf("Enter Sales (float): ");
    scanf("%f", &newRec->Sales);
    printf("Enter Quantity (int): ");
    scanf("%d", &newRec->Quantity);
    printf("Enter Discount (float, e.g., 0.1 for 10%%): ");
    scanf("%f", &newRec->Discount);
    printf("Enter Profit (float): ");
    scanf("%f", &newRec->Profit);
    printf("Enter Shipping Cost (float): ");
    scanf("%f", &newRec->Shipping_Cost);
    printf("Enter Order Priority: ");
    scanf("%s", newRec->Order_Priority);
    printf("Enter Payment Method: ");
    scanf("%s", newRec->Payment_method);

    newRec->next = NULL;
    insertRecord(newRec);
    printf("New record inserted successfully.\n");
}
void saveToCSV(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("Cannot open file for writing.\n");
        return;
    }
    fprintf(fp, "Order_Date,Time,Aging,Customer_Id,Gender,Device_Type,Customer_Login_type,Product_Category,Product,Sales,Quantity,Discount,Profit,Shipping_Cost,Order_Priority,Payment_method\n");

    for (int i = 0; i < TABLE_SIZE; i++) {
        Record* curr = hashTable[i];
        while (curr) {
            fprintf(fp, "%s,%s,%.2f,%d,%s,%s,%s,%s,%s,%.2f,%d,%.2f,%.2f,%.2f,%s,%s\n",
                    curr->Order_Date, curr->Time, curr->Aging, curr->Customer_Id,
                    curr->Gender, curr->Device_Type, curr->Customer_Login_type,
                    curr->Product_Category, curr->Product, curr->Sales,
                    curr->Quantity, curr->Discount, curr->Profit,
                    curr->Shipping_Cost, curr->Order_Priority, curr->Payment_method);
            curr = curr->next;
        }
    }

    fclose(fp);
    printf("Data saved back to CSV.\n");
}


int main() {
    int choice, id;
    char filename[] = "entries.csv";

    loadCSV(filename);

    while (1) {
        printf("\n--- Directory Menu ---\n");
        printf("1. Search Record\n");
        printf("2. Delete Record\n");
        printf("3. Modify Record (Product & Sales)\n");
        printf("4. Insert New Record\n");
        printf("5. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            printf("Enter Customer_Id: ");
            scanf("%d", &id);
            Record* r = searchRecord(id);
            if (r) printRecord(r);
            else printf("Record not found.\n");
        }

        else if (choice == 2) {
            printf("Enter Customer_Id to delete: ");
            scanf("%d", &id);
            if (deleteRecord(id)) printf("Deleted.\n");
            else printf("Record not found.\n");
        }

        else if (choice == 3) {
            printf("Enter Customer_Id to modify: ");
            scanf("%d", &id);
            Record* r = searchRecord(id);
            if (r) {
                printf("Enter new product name: ");
                scanf(" %[^\n]", r->Product);
                printf("Enter new sales value: ");
                scanf("%f", &r->Sales);
                printf("Modified successfully.\n");
            } else {
                printf("Record not found.\n");
            }
        }

        else if (choice == 4) {
            insertNewRecordFromUser();
        }

        else if (choice == 5) {
            saveToCSV(filename);
            printf("Exiting...\n");
            break;
        }        

        else {
            printf("Invalid option.\n");
        }
    }

    return 0;
}
