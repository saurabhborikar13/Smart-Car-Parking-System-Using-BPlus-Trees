
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//---------------PARKING SPACE USING B+ TREE---------------//


#define ORDER 4
#define SPACE_ORDER 4

// Time structure for tracking parking duration
typedef struct
{
    int year;
    int month;
    int day;
    int hr;
    int min;
    int sec;

} ParkingTime;


// Parking space structure
typedef struct
{
    int id;
    bool occupied;
    int totalOccupancy;
    int revenue;
} ParkingSpace;

typedef struct BPNodeForSpaceTree {
    bool is_leaf;
    int num_keys;
    int keys[SPACE_ORDER];          //extra 1 inde is used which helps in splitting
    ParkingSpace *data[SPACE_ORDER];  // For leaf nodes
    struct BPNodeForSpaceTree *children[SPACE_ORDER + 1];
    struct BPNodeForSpaceTree *next;  // For leaf nodes (linked list)
} BPNodeForSpaceTree;


// B+ trees for different membership types
BPNodeForSpaceTree *goldSpaces = NULL;     // IDs 1-10
BPNodeForSpaceTree *premiumSpaces = NULL;  // IDs 11-20
BPNodeForSpaceTree *normalSpaces = NULL;   // IDs 21-50


ParkingSpace* search(BPNodeForSpaceTree *root, int id);


BPNodeForSpaceTree* createNewNodeForSpaceTree(bool is_leaf) {
    BPNodeForSpaceTree* newNode = (BPNodeForSpaceTree*)malloc(sizeof(BPNodeForSpaceTree));
    newNode->is_leaf = is_leaf;
    newNode->num_keys = 0;
    
    // Initialize all children to NULL
    for (int i = 0; i < SPACE_ORDER + 1; i++) {
        newNode->children[i] = NULL;
    }
    
    newNode->next = NULL;  // Only used for leaf nodes
    return newNode;
}


void splitChild(BPNodeForSpaceTree *parent, int index) {
    BPNodeForSpaceTree *child = parent->children[index];
    BPNodeForSpaceTree *newChild = createNewNodeForSpaceTree(child->is_leaf);
    int mid = (SPACE_ORDER - 1) / 2;

    if (child->is_leaf) {
        // Leaf node split
        int j = 0;
        for (int i = mid; i < child->num_keys; i++, j++) {
            newChild->keys[j] = child->keys[i];
            newChild->data[j] = child->data[i];
        }

        newChild->num_keys = child->num_keys - mid;
        child->num_keys = mid;

        // Update leaf link
        newChild->next = child->next;
        child->next = newChild;

        // Shift parent's keys and children
        for (int i = parent->num_keys; i > index; i--) {
            parent->keys[i] = parent->keys[i - 1];
            parent->children[i + 1] = parent->children[i];
        }

        parent->keys[index] = newChild->keys[0];  // Duplicate key in parent
        parent->children[index + 1] = newChild;
        parent->num_keys++;
    } else {
        // Internal node split
        int j = 0;
        for (int i = mid + 1; i < child->num_keys; i++, j++) {
            newChild->keys[j] = child->keys[i];
            newChild->children[j] = child->children[i];
        }
        newChild->children[j] = child->children[child->num_keys];
        newChild->num_keys = child->num_keys - (mid + 1);
        child->num_keys = mid;

        // Shift parent's keys and children
        for (int i = parent->num_keys; i > index; i--) {
            parent->keys[i] = parent->keys[i - 1];
            parent->children[i + 1] = parent->children[i];
        }

        parent->keys[index] = child->keys[mid];  // Promote middle key
        parent->children[index + 1] = newChild;
        parent->num_keys++;
    }
}


BPNodeForSpaceTree* insertNonFull(BPNodeForSpaceTree *node, ParkingSpace *space) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        while (i >= 0 && space->id < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            node->data[i + 1] = node->data[i];
            i--;
        }
        node->keys[i + 1] = space->id;
        node->data[i + 1] = space;
        node->num_keys++;
    } else {
        while (i >= 0 && space->id < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->num_keys == SPACE_ORDER - 1) {
            splitChild(node, i);
            if (space->id > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], space);
    }

    return node;
}

BPNodeForSpaceTree* insert(BPNodeForSpaceTree *root, ParkingSpace *space) {

    BPNodeForSpaceTree* ans;

    if (root == NULL) {
        BPNodeForSpaceTree *newNode = createNewNodeForSpaceTree(true);
        newNode->keys[0] = space->id;
        newNode->data[0] = space;
        newNode->num_keys = 1;
        ans =  newNode;
    }

    else if (root->num_keys == SPACE_ORDER - 1) {
        BPNodeForSpaceTree *newRoot = createNewNodeForSpaceTree(false);
        newRoot->children[0] = root;
        splitChild(newRoot, 0);
        ans = insertNonFull(newRoot, space);
    } 
    else{
        ans = insertNonFull(root, space);
    }
    return ans;
}



ParkingSpace *search(BPNodeForSpaceTree *root, int id)
{   
    ParkingSpace* ans = NULL;
    bool flag = true;

    if (root == NULL)
        ans = NULL;
    else
    {
        BPNodeForSpaceTree *cur = root;

        // Navigate to the appropriate leaf node
        while (!cur->is_leaf)
        {
            int i = 0;
            // Find which child to follow
            while (i < cur->num_keys && id >= cur->keys[i])
            {
                i++;
            }
            cur = cur->children[i];
        }

        // Search for the key in the leaf node
        for (int i = 0; i < cur->num_keys && flag == true; i++)
        {
            if (cur->keys[i] == id)
            {
                ans = cur->data[i];
                flag = false;
            }
        }
    }
    return ans;
}


ParkingSpace* findFirstAvailableSpace(BPNodeForSpaceTree *root) {
    ParkingSpace* ans = NULL;
    bool flag = true;

    if (root == NULL)
        ans = NULL;
    else
    {
        BPNodeForSpaceTree *cur = root;

        // Navigate to leftmost leaf node
        while (!cur->is_leaf)
        {
            cur = cur->children[0];
        }

        // Traverse through leaf nodes
        while (cur != NULL && flag == true)
        {
            for (int i = 0; i < cur->num_keys && flag == true; i++)
            {
                if (cur->data[i]->occupied == 0)
                {
                    ans =  cur->data[i];
                    flag= false;
                }
            }
            cur = cur->next;
        }

        return ans; // No available space found
    }
}


ParkingSpace* findAvailableSpace(char *membershipType) {
    ParkingSpace *space = NULL;
    BPNodeForSpaceTree *targetTree = NULL;
    
    // Determine which tree to search based on membership
    if (strcmp(membershipType, "Golden") == 0) {
        targetTree = goldSpaces;
    } else if (strcmp(membershipType, "Premium") == 0) {
        targetTree = premiumSpaces;
    } else { // Normal
        targetTree = normalSpaces;
    }
    
    space = findFirstAvailableSpace(targetTree);
    
   //not foundcheck other premium or normal
    if (space == NULL && strcmp(membershipType, "Premium") == 0) {
        space = findFirstAvailableSpace(normalSpaces);
    }
    if(space == NULL && strcmp(membershipType,"Golden") == 0){
        space = findFirstAvailableSpace(premiumSpaces);
    }
    if(space == NULL && strcmp(membershipType,"Golden") == 0){
        space = findFirstAvailableSpace(normalSpaces);
    }

    return space;
}

// based on space id

ParkingSpace* getSpaceNode(int id) {
    BPNodeForSpaceTree *targetTree = NULL;
    ParkingSpace* ans = NULL;
    // Determine which tree to search based on membership
    if (id<=10 && id>=1) {
        targetTree = goldSpaces;
    } else if (id<=20 && id>=11) {
        targetTree = premiumSpaces;
    } else if(id>20){ // Normal
        targetTree = normalSpaces;
    }
    
    BPNodeForSpaceTree *cur = targetTree;
    
    // Navigate to leftmost leaf node
    while (cur!=NULL && !cur->is_leaf) {
        cur = cur->children[0];
    }
    
    bool flag = true;
    // Traverse through leaf nodes
    while (cur != NULL && flag == true) {
        for (int i = 0; i < cur->num_keys && flag == true; i++) {
            if (cur->data[i]->id == id) {
                ans=  cur->data[i];
                flag = false;
            }
        }
        cur = cur->next;
    }
    
    return ans;
}

void initializeParkingSpaces()
{
    // printf("initializeParkingSpace()\n");
    FILE *file = fopen("inputForSpaces.txt", "r");
    if (file == NULL)
    {
        printf("Error opening inputForSpaces.txt. Creating default spaces.\n");
        return;
    }

    int id, occupied, totalOccupancy, revenue;
    while (fscanf(file, "%d %d %d %d", &id, &occupied, &totalOccupancy, &revenue) == 4)
    {
        // printf("inside the while of initalize parking space.\n");
        // First check if this space already exists
        ParkingSpace *existingSpace = getSpaceNode(id);
        if (existingSpace != NULL)
        {
            // Update existing space instead of creating a new one
            existingSpace->occupied = occupied;
            existingSpace->totalOccupancy = totalOccupancy;
            existingSpace->revenue = revenue;
        }
        else
        {
            // Only create new space if it doesn't exist
            ParkingSpace *space = (ParkingSpace *)malloc(sizeof(ParkingSpace));
            if (space == NULL)
                printf("fault.\n");
            else{
                space->id = id;
                space->occupied = occupied;
                space->totalOccupancy = totalOccupancy;
                space->revenue = revenue;
                // Insert into the appropriate tree based on ID range
                if (id <= 10)
                {
                    goldSpaces = insert(goldSpaces, space);
                }
                else if (id <= 20)
                {
                    premiumSpaces = insert(premiumSpaces, space);
                }
                else
                {
                    normalSpaces = insert(normalSpaces, space);
                }
            }
        }
    }
    fclose(file);
}

void writeParkingSpaceToFile(FILE* file, BPNodeForSpaceTree* root) {
    if (!root) return;
    
    BPNodeForSpaceTree* cur = root;
    // Navigate to leftmost leaf
    while (cur && !cur->is_leaf) {
        cur = cur->children[0];
    }
    
    // Write all leaf nodes
    while (cur) {
        for (int i = 0; i < cur->num_keys; i++) {
            ParkingSpace* space = cur->data[i];
            if (space) {
                fprintf(file, "%d %d %d %d\n", 
                    space->id,
                    space->occupied,
                    space->totalOccupancy,
                    space->revenue);
            }
        }
        cur = cur->next;
    }
}

void saveParkingSpaceToFile()
{
    FILE* file = fopen("inputForSpaces.txt", "w");
    if (!file) {
        printf("Error in opening file.\n");
    }
    
    writeParkingSpaceToFile(file,goldSpaces);
    writeParkingSpaceToFile(file,premiumSpaces);
    writeParkingSpaceToFile(file,normalSpaces);
    fclose(file);

}

void help2(BPNodeForSpaceTree* root){
    if (root == NULL) {
        printf("No parking spaces available in this category.\n");
        return;
    }
    BPNodeForSpaceTree* cur = root;
    while(cur->is_leaf!=true){
        cur=cur->children[0];
    }
    while(cur){
        for(int i=0;i<cur->num_keys;i++){
            ParkingSpace* space = cur->data[i];
            if(space!=NULL){
                printf("%-5d %-10s %-16d %-10d\n",
                    space->id,
                    space->occupied ? "Yes" : "No",
                    space->totalOccupancy,
                    space->revenue);
            }
            
        }
        // printf("cer->next\n");
        cur = cur->next;
    }
}

void displayParkingSpacesHelper()
{
    help2(goldSpaces);
    // printf("gold Space done.\n");
    help2(premiumSpaces);
    // printf("Premum is done.\n");
    help2(normalSpaces);
    // printf("normal is done.\n");
}

void displayParkingSpaces()
{
    printf("\n");
    printf("------------------------------------------------\n");
    printf("%-5s %-10s %-16s %-10s\n", "ID", "Occupied", "Total Occupancy", "Revenue");
    printf("------------------------------------------------\n");
    displayParkingSpacesHelper();
    printf("------------------------------------------------\n");
}

//--------- VEHICLE - RECORD DATABASE USING B+ TREE--------------//


// Vehicle record structure
typedef struct
{
    char vehicleNumber[20];
    char ownerName[100];
    ParkingTime arrivalTime;
    ParkingTime departureTime;
    int totalParkingHours;
    int spaceID;
    char membership[10]; // "Golden", "Premium", or "None"
    int totalAmountPaid;
    int parkingCount;
} VehicleRecord;


// B+ Tree Node structure for Vehicle Record
typedef struct BPlusTreeNodeForVehicleRecord {
    bool is_leaf;
    int num_keys;
    char *keys[ORDER];          // Using string keys for vehicle numbers
    VehicleRecord *data[ORDER]; // For leaf nodes
    struct BPlusTreeNodeForVehicleRecord *children[ORDER+ 1];
    struct BPlusTreeNodeForVehicleRecord *next;  // For leaf nodes (linked list)
} BPlusTreeNodeForVehicleRecord;

// B+ Tree structure for vehicles
BPlusTreeNodeForVehicleRecord *vehicleDatabase = NULL;

VehicleRecord* searchVehicle(BPlusTreeNodeForVehicleRecord* root, char* vehicleNumber);

BPlusTreeNodeForVehicleRecord* createNewNodeForVehicleTree(bool is_leaf) {
    BPlusTreeNodeForVehicleRecord* newNode = (BPlusTreeNodeForVehicleRecord*)malloc(sizeof(BPlusTreeNodeForVehicleRecord));
    newNode->is_leaf = is_leaf;
    newNode->num_keys = 0;
    
    // Initialize all children to NULL
    for (int i = 0; i < ORDER + 1; i++) {
        newNode->children[i] = NULL;
    }
    
    newNode->next = NULL;  // Only used for leaf nodes
    return newNode;
}
void splitVehicleChild(BPlusTreeNodeForVehicleRecord *parent, int index) {
    BPlusTreeNodeForVehicleRecord *child = parent->children[index];
    BPlusTreeNodeForVehicleRecord *newChild = createNewNodeForVehicleTree(child->is_leaf);
    int mid = (ORDER - 1) / 2;

    if (child->is_leaf) {
        // Leaf node split
        int j = 0;
        for (int i = mid; i < child->num_keys; i++, j++) {
            newChild->keys[j] = child->keys[i];
            newChild->data[j] = child->data[i];
        }

        newChild->num_keys = child->num_keys - mid;
        child->num_keys = mid;

        // Update leaf link
        newChild->next = child->next;
        child->next = newChild;

        // Shift parent's keys and children
        for (int i = parent->num_keys; i > index; i--) {
            parent->keys[i] = parent->keys[i - 1];
            parent->children[i + 1] = parent->children[i];
        }

        parent->keys[index] = strdup(newChild->keys[0]);  // Duplicate key in parent
        parent->children[index + 1] = newChild;
        parent->num_keys++;
    } else {
        // Internal node split
        int j = 0;
        for (int i = mid + 1; i < child->num_keys; i++, j++) {
            newChild->keys[j] = child->keys[i];
            newChild->children[j] = child->children[i];
        }
        newChild->children[j] = child->children[child->num_keys];
        newChild->num_keys = child->num_keys - (mid + 1);
        child->num_keys = mid;

        // Shift parent's keys and children
        for (int i = parent->num_keys; i > index; i--) {
            parent->keys[i] = parent->keys[i - 1];
            parent->children[i + 1] = parent->children[i];
        }

        parent->keys[index] = strdup(child->keys[mid]);  // Promote middle key
        parent->children[index + 1] = newChild;
        parent->num_keys++;
    }
}


BPlusTreeNodeForVehicleRecord* insertVehicleNonFull(BPlusTreeNodeForVehicleRecord *node, VehicleRecord *record) {
    int i = node->num_keys - 1;
   
    if (node->is_leaf) {
        while (i >= 0 && strcmp(record->vehicleNumber, node->keys[i]) < 0) {
            node->keys[i + 1] = node->keys[i];
            node->data[i + 1] = node->data[i];
            i--;
        }
        node->keys[i + 1] = strdup(record->vehicleNumber);
        node->data[i + 1] = record;
        node->num_keys++;
    } else {
        while (i >= 0 && strcmp(record->vehicleNumber, node->keys[i]) < 0) {
            i--;
        }
        i++;
        if (node->children[i]->num_keys == ORDER - 1) {
            splitVehicleChild(node, i);
            if (strcmp(record->vehicleNumber, node->keys[i]) > 0) {
                i++;
            }
        }
        insertVehicleNonFull(node->children[i], record);
    }

    return node;
}



BPlusTreeNodeForVehicleRecord* insertVehicle(BPlusTreeNodeForVehicleRecord *root, VehicleRecord *record) {
    BPlusTreeNodeForVehicleRecord* ans ;
   
    if (root == NULL) {
        BPlusTreeNodeForVehicleRecord *newNode = createNewNodeForVehicleTree(true);
        // newNode->keys[0] = (char *)malloc(strlen(record->vehicleNumber) + 1);
        // strcpy(newNode->keys[0], record->vehicleNumber);

        newNode->keys[0] = strdup(record->vehicleNumber);
        newNode->data[0] = record;
        newNode->num_keys = 1;
        ans = newNode;
    }

    else if (root->num_keys == ORDER - 1) {
        BPlusTreeNodeForVehicleRecord *newRoot = createNewNodeForVehicleTree(false);
        newRoot->children[0] = root;
        splitVehicleChild(newRoot, 0);
        ans = insertVehicleNonFull(newRoot, record);
    }
    
    else{
        ans =insertVehicleNonFull(root, record);
    } 
    return ans;
}


VehicleRecord *searchVehicle(BPlusTreeNodeForVehicleRecord *root, char *vehicleNumber) {

    VehicleRecord* ans=NULL;
    bool flag = true;

    if (root == NULL)
    {
        ans = NULL;
    }
    else
    {
        BPlusTreeNodeForVehicleRecord *cur = root;

        // Navigate to the appropriate leaf node
        while (!cur->is_leaf)
        {
            int i = 0;
            // Find which child to follow
            while (i < cur->num_keys && strcmp(vehicleNumber, cur->keys[i]) >= 0)
            {
                i++;
            }
            cur = cur->children[i];
        }

        // Search for the key in the leaf node
        for (int i = 0; i < cur->num_keys && flag == true; i++)
        {
            if (strcmp(cur->keys[i], vehicleNumber) == 0)
            {
                ans = cur->data[i];
                flag = false;
            }
        }
    }

    return ans;
}

// Time calculation functions
int daysInMonth(int month, int year)
{
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)))
        return 29;
    return days[month - 1];
}

long long timeToSeconds(ParkingTime t)
{
    long long seconds = 0;
    // Add years
    for (int y = 2020; y < t.year; y++)
    {
        seconds += ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) ? 366 * 86400 : 365 * 86400;
    }
    // Add months
    for (int m = 1; m < t.month; m++)
    {
        seconds += daysInMonth(m, t.year) * 86400;
    }
    // Add days, hours, minutes, seconds
    seconds += (t.day - 1) * 86400 + t.hr * 3600 + t.min * 60 + t.sec;
    return seconds;
}

/**
 * Initializes all parking spaces (1-50) and inserts them into appropriate trees
 */

VehicleRecord *findVehicle(char *vehicleNumber)
{
    return searchVehicle(vehicleDatabase, vehicleNumber);
}

void registerVehicle()
{
    char vehicleNumber[20], ownerName[100];
    printf("Enter vehicle number: ");
    scanf("%19s", vehicleNumber); // Buffer overflow protection
    printf("Enter owner name: ");
    scanf("%99s", ownerName);

    VehicleRecord *existing = findVehicle(vehicleNumber);

    char membership[20];
    strcpy(membership, "None");

    ParkingSpace *space = NULL;

    if (existing!=NULL)
    {

        if (existing->spaceID != 0)
        {
            printf("Vehicle is already parked!\n");
            return;
        }
        strcpy(membership, existing->membership);
    
        space = findAvailableSpace(membership);
    
    }
    else
    {
        space = findAvailableSpace(membership);
    }

    if (!space)
    {
        printf("No available parking space!\n");
        return;
    }

    ParkingTime arrival;
    printf("Enter arrival time (YYYY MM DD HH MM SS): ");
    scanf("%d %d %d %d %d %d", &arrival.year, &arrival.month, &arrival.day,
          &arrival.hr, &arrival.min, &arrival.sec);

    if (existing)
    {
        existing->spaceID = space->id;
        existing->arrivalTime = arrival;
        memset(&existing->departureTime, 0, sizeof(ParkingTime));
    }
    else
    {
        VehicleRecord *newRecord = (VehicleRecord *)malloc(sizeof(VehicleRecord));
        if (!newRecord)
        {
            printf("Memory allocation failed!\n");
            return;
        }
        strcpy(newRecord->vehicleNumber, vehicleNumber);
        strcpy(newRecord->ownerName, ownerName);
        strcpy(newRecord->membership, membership);
        newRecord->arrivalTime = arrival;
        memset(&newRecord->departureTime, 0, sizeof(ParkingTime));
        newRecord->spaceID = space->id;
        newRecord->totalParkingHours = 0;
        newRecord->totalAmountPaid = 0;
        newRecord->parkingCount = 0;
        // printf("vhicle.\n");
        vehicleDatabase = insertVehicle(vehicleDatabase,newRecord);
    }

    ParkingSpace *reqSpace = getSpaceNode(space->id);
    if (reqSpace)
    {
        reqSpace->occupied = 1;
        reqSpace->totalOccupancy++;
        printf("Vehicle registered successfully in space %d\n", reqSpace->id);
    }
    else
    {
        printf("Parking space update failed.\n");
    }
}

void exitVehicle()
{
    char vehicleNumber[20];
    printf("Enter vehicle number: ");
    scanf("%s", vehicleNumber);

    VehicleRecord *vehicle = findVehicle(vehicleNumber);
    if (!vehicle || vehicle->spaceID == 0)
    {
        printf("Vehicle not found or not currently parked!\n");
        
    }
    else{
        printf("Enter departure time (YYYY MM DD HH MM SS): ");
    scanf("%d %d %d %d %d %d", &vehicle->departureTime.year, &vehicle->departureTime.month,
          &vehicle->departureTime.day, &vehicle->departureTime.hr,
          &vehicle->departureTime.min, &vehicle->departureTime.sec);

    // Calculate parking duration in hours
    long long arrivalSec = timeToSeconds(vehicle->arrivalTime);
    long long departureSec = timeToSeconds(vehicle->departureTime);
    int hours = (departureSec - arrivalSec + 3599) / 3600; // Round up

    // Calculate fee
    int fee = 0;
    if (hours <= 3)
    {
        fee = hours * 100;
    }
    else
    {
        fee = 300 + (hours - 3) * 50;
    }

    // Apply membership discount
    if (strcmp(vehicle->membership, "None") != 0)
    {
        fee = fee * 0.9;
    }

    // Update parking space
    ParkingSpace *reqSpace = getSpaceNode(vehicle->spaceID);

    if (reqSpace != NULL)
    {
        reqSpace->revenue += fee;
        reqSpace->occupied = 0;
        printf("Vehicle exited successfully. Fee: %d\n", fee);
    }
    else
    {
        printf("Vehicle is not there.\n");
    }

    // Update records
    vehicle->totalParkingHours += hours;
    vehicle->totalAmountPaid += fee;
    vehicle->parkingCount++;
    vehicle->spaceID = 0;   //as exit the vehicle set the sapce id to 0

    // Update membership status
    if (vehicle->totalParkingHours >= 200)
    {
        strcpy(vehicle->membership, "Golden");
    }
    else if (vehicle->totalParkingHours >= 100)
    {
        strcpy(vehicle->membership, "Premium");
    }

    }   
}



void saveVehicleRecordsToFile(FILE *file, BPlusTreeNodeForVehicleRecord *node) {
    if (node == NULL) return;

    // Find leftmost leaf node
    BPlusTreeNodeForVehicleRecord *current = node;
    while (current != NULL && !current->is_leaf) {
        current = current->children[0];
    }

    // Traverse through all leaf nodes using next pointers
    while (current != NULL) {
        for (int i = 0; i < current->num_keys; i++) {
            VehicleRecord *record = current->data[i];
            if (record != NULL) {
                fprintf(file, "%s %s %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                    record->vehicleNumber, record->ownerName, record->membership,
                    record->totalParkingHours, record->spaceID, record->totalAmountPaid,
                    record->parkingCount,

                    // Arrival time
                    record->arrivalTime.year, record->arrivalTime.month, record->arrivalTime.day,
                    record->arrivalTime.hr, record->arrivalTime.min, record->arrivalTime.sec,

                    // Departure time
                    record->departureTime.year, record->departureTime.month, record->departureTime.day,
                    record->departureTime.hr, record->departureTime.min, record->departureTime.sec);
            }
        }
        current = current->next;
    }
}

void writeTofile(){
    FILE* file = fopen("inputForVehicle.txt","w");
    if(file == NULL){
        printf("Error.\n");
    }
    else{
        saveVehicleRecordsToFile(file,vehicleDatabase);
    }
    saveParkingSpaceToFile();
}

static int count = 0;

void loadDatabaseFromFiles()
{
    // printf("loading databse from the file.\n");
    // Load vehicle records into B+ tree
    FILE *vehicleFile = fopen("inputForVehicle.txt", "r");
    if (vehicleFile)
    {
        char vehicleNumber[20], ownerName[100], membership[10];
        int totalParkingHours, spaceID, totalAmountPaid, parkingCount;
        int a_year, a_month, a_day, a_hr, a_min, a_sec;
        int d_year, d_month, d_day, d_hr, d_min, d_sec;

        while (fscanf(vehicleFile, "%s %s %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                      vehicleNumber, ownerName, membership, &totalParkingHours, &spaceID,
                      &totalAmountPaid, &parkingCount,
                      &a_year, &a_month, &a_day, &a_hr, &a_min, &a_sec,
                      &d_year, &d_month, &d_day, &d_hr, &d_min, &d_sec) == 19)
        {

            VehicleRecord *record = (VehicleRecord *)malloc(sizeof(VehicleRecord));

            // Copy basic info
            strcpy(record->vehicleNumber, vehicleNumber);
            strcpy(record->ownerName, ownerName);
            strcpy(record->membership, membership);
            record->totalParkingHours = totalParkingHours;
            record->spaceID = spaceID;
            record->totalAmountPaid = totalAmountPaid;
            record->parkingCount = parkingCount;

            // Set arrival time
            record->arrivalTime.year = a_year;
            record->arrivalTime.month = a_month;
            record->arrivalTime.day = a_day;
            record->arrivalTime.hr = a_hr;
            record->arrivalTime.min = a_min;
            record->arrivalTime.sec = a_sec;

            // Set departure time
            record->departureTime.year = d_year;
            record->departureTime.month = d_month;
            record->departureTime.day = d_day;
            record->departureTime.hr = d_hr;
            record->departureTime.min = d_min;
            record->departureTime.sec = d_sec;

            
            VehicleRecord* existing = searchVehicle(vehicleDatabase,record->vehicleNumber);
            if(existing == NULL){
                vehicleDatabase = insertVehicle(vehicleDatabase,record);
                count++;
            }
            else{
                free(record);
            }
            // Insert into B+ tree
            
        }
        // printf("%d",count);
        fclose(vehicleFile);
    }
    else
    {
        printf("Warning: Could not open inputForVehicle.txt. Starting with empty vehicle database.\n");
    }


    initializeParkingSpaces();
}

void displayMenu()
{
    printf("\n\n--- SMART CAR PARKING SYSTEM ---\n\n");
    printf("1. Register Vehicle\n");
    printf("2. Vehicle Want to EXIT\n");
    printf("3. Display Vehicle Information\n");
    printf("4. Display Parking Space Information\n");
    printf("5. Sort Vehicles Based On the Parking Count\n");
    printf("6. Sort Vehicles Based On the Amount paid\n");
    printf("7. Sort Spaces Based on the Total Occupancy.\n");
    printf("8. Sort Spaces on the Revenue\n");
    printf("9. CLOSE\n");
    printf("Plz Enter your choice (1-5): ");
}

void printUpperPart(BPlusTreeNodeForVehicleRecord* node){
    
        printf("Vehicle Records:\n");
        printf("------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        printf("%-15s %-20s %-20s %-10s %-10s %-15s %-12s %-20s %-20s\n", "Vehicle Number", "Owner Name", "Membership", "Park Hr", "Space ID", "Total Paid", "Park Count", "Arrival Time", "Depart Time");
        printf("------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

// Helper function that only traverses and prints data using next pointers
void displayAllVehicles(BPlusTreeNodeForVehicleRecord *node) {
    if (node == NULL) return;
    
    BPlusTreeNodeForVehicleRecord* current = node;
    while (current != NULL && !current->is_leaf) {
        current = current->children[0];
    }

    // Traverse through all leaf nodes using next pointers
    while (current != NULL) {
        for (int i = 0; i < current->num_keys; i++) {
            VehicleRecord *record = current->data[i];
            if (record != NULL) {
                char arrivalTime[20], departureTime[20];

                // Format arrival time
                sprintf(arrivalTime, "%02d-%02d-%04d %02d:%02d:%02d",
                        record->arrivalTime.day, record->arrivalTime.month, record->arrivalTime.year,
                        record->arrivalTime.hr, record->arrivalTime.min, record->arrivalTime.sec);
                
                // Format departure time
                sprintf(departureTime, "%02d-%02d-%04d %02d:%02d:%02d",
                        record->departureTime.day, record->departureTime.month, record->departureTime.year,
                        record->departureTime.hr, record->departureTime.min, record->departureTime.sec);

                printf("%-15s %-20s %-20s %-10d %-10d %-15d %-10d %-20s %-20s\n",
                       record->vehicleNumber, record->ownerName, record->membership,
                       record->totalParkingHours, record->spaceID, 
                       record->totalAmountPaid, record->parkingCount, 
                       arrivalTime, departureTime);
            }
        }
        current = current->next;
    }
}

//------------------------------FOR VEHICLE SORTING -----------------------------------------------//
//--------------------------------------------------------------------------------------------------//

static int flag = 0;
static int flag2 = 0;

typedef struct vehicleDB
{
    char vehicleNumber[20];
    char OwnerName[100];
    ParkingTime arrivalTime;
    ParkingTime departureTime;
    int totalParkingHours;
    int spaceID;
    char membership[10];
    int totalAmountPaid;
    int parkingCount;
    struct vehicleDB *next; // Linked list for vehicleDBs
} vehicleDB;

vehicleDB *vehicleDBDB = NULL;

void appendTovehicleDB(vehicleDB **vehicleDBDB, char *number, char *owner, int hours, int id,
                       char *membership, int amount, int count, ParkingTime *t, ParkingTime *depa_time)
{
    vehicleDB *newvehicleDB = (vehicleDB *)malloc(sizeof(vehicleDB));
    strcpy(newvehicleDB->vehicleNumber, number);
    strcpy(newvehicleDB->OwnerName, owner);
    newvehicleDB->totalParkingHours = hours;
    newvehicleDB->spaceID = id;
    strcpy(newvehicleDB->membership, membership);
    newvehicleDB->totalAmountPaid = amount;
    newvehicleDB->parkingCount = count;
    newvehicleDB->arrivalTime.year = t->year;
    newvehicleDB->arrivalTime.month = t->month;
    newvehicleDB->arrivalTime.day = t->day;
    newvehicleDB->arrivalTime.hr = t->hr;
    newvehicleDB->arrivalTime.min = t->min;
    newvehicleDB->arrivalTime.sec = t->sec;

    newvehicleDB->departureTime.year = depa_time->year;
    newvehicleDB->departureTime.month = depa_time->month;
    newvehicleDB->departureTime.day = depa_time->day;
    newvehicleDB->departureTime.hr = depa_time->hr;
    newvehicleDB->departureTime.min = depa_time->min;
    newvehicleDB->departureTime.sec = depa_time->sec;
    newvehicleDB->next = NULL;

    if (*vehicleDBDB == NULL)
    {   

        *vehicleDBDB = newvehicleDB;
    }
    else
    {
        vehicleDB *temp = *vehicleDBDB;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = newvehicleDB;
    }
}

void copyVehicleInfoFrom_Tree(BPlusTreeNodeForVehicleRecord *node) {
    if (node == NULL) {
        printf("No vehicles to append.\n");
        return;
    }

    // Find leftmost leaf node
    BPlusTreeNodeForVehicleRecord *current = node;
    while (current != NULL && !current->is_leaf) {
        current = current->children[0];
    }

    // Traverse through all leaf nodes using next pointers
    while (current != NULL) {
        for (int i = 0; i < current->num_keys; i++) {
            VehicleRecord *record = current->data[i];
            if (record != NULL) {
                appendTovehicleDB(&vehicleDBDB,
                                record->vehicleNumber,
                                record->ownerName,
                                record->totalParkingHours,
                                record->spaceID,
                                record->membership,
                                record->totalAmountPaid,
                                record->parkingCount,
                                &record->arrivalTime,
                                &record->departureTime);
            }
        }
        current = current->next;
    }
}

void displayvehicleDB(vehicleDB *head)
{
    if (head == NULL)
    {
        printf("No vehicles to display.\n");
        return;
    }

    printf("Vehicle Info:\n");
    printf("------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("%-15s %-20s %-20s %-10s %-10s %-15s %-12s %-20s %-20s\n", "Vehicle Number", "Owner Name", "Membership", "Park Hr", "Space ID", "Total Paid", "Park Count", "Arrival Time", "Depart Time");
    printf("------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    char arrivalTime[20];
    char departureTime[20];

    vehicleDB *temp = head;
    while (temp != NULL)
    {
        sprintf(arrivalTime, "%02d-%02d-%04d %02d:%02d:%02d",
                temp->arrivalTime.day, temp->arrivalTime.month, temp->arrivalTime.year,
                temp->arrivalTime.hr, temp->arrivalTime.min, temp->arrivalTime.sec);
        sprintf(departureTime, "%02d-%02d-%04d %02d:%02d:%02d",
                temp->departureTime.day, temp->departureTime.month, temp->departureTime.year,
                temp->departureTime.hr, temp->departureTime.min, temp->departureTime.sec);
        printf("%-15s %-20s %-20s %-10d %-10d %-15d %-10d %-20s %-20s\n",
               temp->vehicleNumber, temp->OwnerName, temp->membership,
               temp->totalParkingHours, temp->spaceID, temp->totalAmountPaid, temp->parkingCount, arrivalTime, departureTime);
        temp = temp->next;
    }

    printf("------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

// Merge two sorted lists
vehicleDB *merge(vehicleDB *a, vehicleDB *b)
{
    vehicleDB *result = NULL;

    // Base cases
    if (a == NULL)
        result = b;
    else if (b == NULL)
        result = a;

    // Sort in descending order of parkingCount
    else if (a->parkingCount >= b->parkingCount)
    {
        result = a;
        result->next = merge(a->next, b);
    }
    else
    {
        result = b;
        result->next = merge(a, b->next);
    }
    return result;
}

// split_for_vehicleDB list into two halves
void split_for_vehicleDB(vehicleDB *source, vehicleDB **front, vehicleDB **back)
{
    vehicleDB *fast;
    vehicleDB *slow = source;

    // Handle empty or single-element list
    if (source == NULL || source->next == NULL)
    {
        *front = source;
        *back = NULL;
        return;
    }

    // Slow moves 1 step, fast moves 2 steps
    fast = source->next;
    while (fast != NULL)
    {
        fast = fast->next;
        if (fast != NULL)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // split_for_vehicleDB at midpoint
    *front = source;
    *back = slow->next;
    slow->next = NULL;
}

// Merge sort implementation
vehicleDB *mergeSort(vehicleDB *head)
{
    vehicleDB *a;
    vehicleDB *b;
    vehicleDB *ans;
    // Base caseempty list or single element
    if (head == NULL || head->next == NULL)
    {
        ans = head;
    }
    else
    {
        split_for_vehicleDB(head, &a, &b);
        // Recursively sort sublists
        a = mergeSort(a);
        b = mergeSort(b);
        ans = merge(a, b);
    }
    return ans;
}

// Main sorting function
vehicleDB *sortvehiclesBy_TotalParking(vehicleDB *head)
{
    copyVehicleInfoFrom_Tree(vehicleDatabase);
    
    return mergeSort(vehicleDBDB);
}

// Merge two sorted lists by totalAmountPaid
vehicleDB *mergeAmount(vehicleDB *a, vehicleDB *b)
{
    vehicleDB *result = NULL;

    // Base cases
    if (a == NULL)
        result = b;
    else if (b == NULL)
        result = a;
    else if (a->totalAmountPaid >= b->totalAmountPaid)
    {
        result = a;
        result->next = mergeAmount(a->next, b);
    }
    else
    {
        result = b;
        result->next = mergeAmount(a, b->next);
    }
    return result;
}

// Merge sort implementation for amounts
vehicleDB *mergeSortAmount(vehicleDB *head)
{
    vehicleDB *a;
    vehicleDB *b;
    vehicleDB *res;

    // Base case - empty list or single element
    if (head == NULL || head->next == NULL)
    {
        res = head;
    }
    else
    {
        // split_for_vehicleDB into sublists
        split_for_vehicleDB(head, &a, &b);

        // Recursively sort sublists
        a = mergeSortAmount(a);
        b = mergeSortAmount(b);

        // Merge sorted sublists
        res = mergeAmount(a, b);
    }
    return res;
}

// Main sorting function for amounts
vehicleDB *sortvehiclesBy_TotalAmount(vehicleDB *head)
{
    copyVehicleInfoFrom_Tree(vehicleDatabase);
   
    return mergeSortAmount(vehicleDBDB);
}

//-------------FOR SPACES SORTING----------------------------------//




typedef struct Space
{
    int id;
    int occupied;
    int totalOccupancy;
    int revenue;
    struct Space *next; // Linked list for parking spaces
} space;

space *parkingLot = NULL;      // Linked list for parking spaces
space *goldPointer = NULL;
space *premiumPointer = NULL;
space *normalPointer = NULL;

void initializeparkingSpaces_LL(space **head)
{
    *head = NULL;
    space *prev = NULL;

    for (int i = 1; i <= 50; i++)
    {
        space *newSpace = (space *)malloc(sizeof(space));
        newSpace->id = i;
        newSpace->occupied = 0;
        newSpace->revenue = 0;
        newSpace->totalOccupancy = 0;
        newSpace->next = NULL;

        if (*head == NULL)
        {
            *head = newSpace;
        }
        else
        {
            prev->next = newSpace;
        }
        prev = newSpace;

        if (i == 1)
            goldPointer = newSpace;
        if (i == 11)
            premiumPointer = newSpace;
        if (i == 21)
            normalPointer = newSpace;
    }
}

void gotoThatplaceAndUpdate(space *Spacehead, int id, int occupied, int totalOccupancy, int revenue)
{
    space *temp = Spacehead;
    while (temp && temp->id != id)
    {
        temp = temp->next;
    }

    if (temp)
    {
        temp->occupied = occupied;
        temp->totalOccupancy = totalOccupancy;
        temp->revenue = revenue;
    }
    else
    {
        printf("Error: Space ID %d not found.\n", id);
    }
}

void getHelp(BPNodeForSpaceTree* root){
    if (root == NULL) {
        printf("No parking spaces available in this category.\n");
        return;
    }
    BPNodeForSpaceTree* cur = root;
    while(cur->is_leaf!=true){
        cur=cur->children[0];
    }
    while(cur){
        for(int i=0;i<cur->num_keys;i++){
            ParkingSpace* space = cur->data[i];
            if(space!=NULL){
                gotoThatplaceAndUpdate(parkingLot,space->id,space->occupied,space->totalOccupancy,space->revenue);
            }  
        }
        // printf("cer->next\n");
        cur = cur->next;
    }
}

void copyDataFromSpaceDatafrom_Tree()
{
    // parkingLot=NULL;

    initializeparkingSpaces_LL(&parkingLot);

    // printf("%d\n",parkingLot->next->next->id);

    getHelp(goldSpaces);
    // printf("gold Space done.\n");
    getHelp(premiumSpaces);
    // printf("Premum is done.\n");
    getHelp(normalSpaces);
    // printf("normal is done.\n");
}

void split_for_SpaceDB(space *source, space **front, space **back)
{
    space *fast;
    space *slow = source;

    // Handle empty or single-element list
    if (source == NULL || source->next == NULL)
    {
        *front = source;
        *back = NULL;
        return;
    }

    // Slow moves 1 step, fast moves 2 steps
    fast = source->next;
    while (fast != NULL)
    {
        fast = fast->next;
        if (fast != NULL)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }

    // split_for_vehicleDB at midpoint
    *front = source;
    *back = slow->next;
    slow->next = NULL;
}

space *mergeSpaces_Revenue(space *a, space *b)
{
    space *result = NULL;
    if (a == NULL)
        result = b;
    else if (b == NULL)
        result = a;

    else if (a->revenue >= b->revenue)
    {
        result = a;
        result->next = mergeSpaces_Revenue(a->next, b);
    }
    else
    {
        result = b;
        result->next = mergeSpaces_Revenue(a, b->next);
    }
    return result;
}

space *mergeSortForSpaceRevenue(space *tempHead)
{
    space *a;
    space *b;
    space *ans;
    if (tempHead == NULL || tempHead->next == NULL)
    {
        ans = tempHead;
    }
    else
    {
        split_for_SpaceDB(tempHead, &a, &b);
        a = mergeSortForSpaceRevenue(a);
        b = mergeSortForSpaceRevenue(b);
        ans = mergeSpaces_Revenue(a, b);
    }

    return ans;
}

space *sortSpacesByRevenue()
{
    copyDataFromSpaceDatafrom_Tree();
        parkingLot = mergeSortForSpaceRevenue(parkingLot);  // Update global
        return parkingLot;
}

space *mergeSpaces_ParkingCount(space *a, space *b)
{
    space *result = NULL;
    if (a == NULL)
        result = b;
    else if (b == NULL)
        result = a;

    else if (a->totalOccupancy >= b->totalOccupancy)
    {
        result = a;
        result->next = mergeSpaces_ParkingCount(a->next, b);
    }
    else
    {
        result = b;
        result->next = mergeSpaces_ParkingCount(a, b->next);
    }
    return result;
}

space *mergeSortFor_ParkingCount(space *tempHead)
{
    space *a;
    space *b;
    space *ans;
    if (tempHead == NULL || tempHead->next == NULL)
    {
        ans = tempHead;
    }
    else
    {
        split_for_SpaceDB(tempHead, &a, &b);
        a = mergeSortFor_ParkingCount(a);
        b = mergeSortFor_ParkingCount(b);
        ans = mergeSpaces_ParkingCount(a, b);
    }
    return ans;
}

space *sortSpacesBy_ParkingCount()
{
   
    copyDataFromSpaceDatafrom_Tree();
    parkingLot = mergeSortFor_ParkingCount(parkingLot);  // Update global
    return parkingLot;
}


void displaySpace_DB(space *SpaceHead)
{
    if (SpaceHead == NULL)
    {
        printf("No parking spaces to display.\n");
        return;
    }

    printf("\n\nParking Spaces:\n\n");
    printf("------------------------------------------------\n");
    printf("%-5s %-10s %-16s %-10s\n",
           "ID", "Occupied", "Total Occupancy", "Revenue");
    printf("------------------------------------------------\n");

    space *temp = SpaceHead;
    while (temp != NULL)
    {
        char occupiedStatus[4];
        strcpy(occupiedStatus, temp->occupied ? "Yes" : "No");

        printf("%-5d %-10s %-16d %-10d\n",
               temp->id,
               occupiedStatus,
               temp->totalOccupancy,
               temp->revenue);

        temp = temp->next;
    }
    printf("------------------------------------------------\n\n");
}

int main()
{
    // printf("IN main()\n");
    // Initialize data structures
    // vehicleDatabase = createBPlusTree();
    
    loadDatabaseFromFiles();
    // printf("load done of parking space.\n");
    initializeParkingSpaces();
    space* Head;
    
    int choice = 1;
    while(choice>=1 && choice<9)
    {
        
        displayMenu();
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            registerVehicle();
            break;
        case 2:
            exitVehicle();
            break;
        case 3:
            printUpperPart(vehicleDatabase);  // for formatting perspeccrive
            displayAllVehicles(vehicleDatabase);
            break;
        case 4:
            printf("\n\nParking Spaces:\n\n");
            displayParkingSpaces();
            break;

        case 5:
            printf("Sorted By Parking Count.\n");
            vehicleDBDB = sortvehiclesBy_TotalParking(vehicleDBDB);
            displayvehicleDB(vehicleDBDB);
            vehicleDBDB=NULL;
            break;
        case 6:
            printf("Sorted By Amount Paid.\n");
            vehicleDBDB = sortvehiclesBy_TotalAmount(vehicleDBDB);
            displayvehicleDB(vehicleDBDB);
            vehicleDBDB=NULL;
            break;
        case 7:
            Head = sortSpacesBy_ParkingCount();
            printf("spaces sorted by the total Occupancy.\n ");
            // if(Head==  NULL )printf("hello\n");

            displaySpace_DB(Head);
            break;
        case 8:
            Head = sortSpacesByRevenue();
            printf("space Sorted by the Revenue.\n");
            // if(Head ==  NULL )printf("hello\n");
            displaySpace_DB(Head);
            
            break;
        case 9:
            printf("\n\nExisting System ......\n\nThank You\n\n");
            writeTofile();
            break;
        default:
            printf("Invalid choice\n");
        }
    }

    return 0;
}




