# 🚗 Smart Car Parking System

A **C-based project** that simulates an efficient and automated car parking facility. It utilizes **B+ Trees** to manage parking spaces and vehicle records efficiently, allowing for fast insertions, deletions, and lookups. The system supports vehicle registration, exit handling, membership upgrades, sorting features, and persistent data storage.

---

## 📌 Overview

The Smart Car Parking System is designed to automate key operations in a parking facility, such as:

- Vehicle registration and space allocation
- Handling exits and calculating fees
- Managing memberships
- Sorting and reporting based on various metrics
- Efficient data management using **B+ Trees**
- Persistent data storage in files

---

## ⚙️ Features

### 🚘 Vehicle Registration & Update
- Register new vehicles upon entry.
- Update existing vehicle details.
- Allocate parking space based on availability and membership.

### 🚪 Vehicle Exit Handling
- Calculate parking duration and applicable fees.
- Apply membership-based discounts.
- Update membership status based on usage.

### 📊 Sorting & Reporting
- Sort vehicles by:
  - Total parking hours
  - Total amount paid
- Sort parking spaces by:
  - Occupancy count
  - Revenue generated

### 💾 Data Persistence
- Vehicle and space data stored in:
  - `inputForVehicle.txt`
  - `inputForSpaces.txt`
- Files are updated when the `close` command is issued.

### 🏅 Membership Management
- Vehicles are auto-upgraded to premium status based on usage.
- Premium members get discounted parking rates.

---

## 🧱 Data Structures (Using B+ Trees)

### 🔹 `Space`
Represents a parking space node in a B+ Tree.
- ID
- Occupancy status
- Occupancy count
- Revenue generated

### 🔸 `VehicleDB`
Represents vehicle record node in a B+ Tree.
- Vehicle number
- Owner name
- Arrival & departure times
- Duration
- Space ID
- Membership status
- Total amount paid
- Parking count

---

## 🛠️ How It Works

### 1. Initialization
- Loads data from `inputForVehicle.txt` and `inputForSpaces.txt` (if available).
- Initializes B+ Trees for vehicle and space management.

### 2. Vehicle Entry
- Checks if vehicle already exists.
- If new, creates a vehicle record and assigns a space.
- Records arrival time.

### 3. Vehicle Exit
- Calculates parking duration and fee.
- Applies discounts for premium members.
- Updates revenue and space availability.
- Updates the vehicle’s total hours and payment stats.

### 4. Sorting & Display
- Sorts data based on time, payments, and space metrics using in-order B+ Tree traversal.
- Displays summary reports.

### 5. Exit and Save
- On `close` command:
  - All data is saved to `inputForVehicle.txt` and `inputForSpaces.txt`.
  - Program exits gracefully.

---

## 📁 File Structure

```text
Smart-Car-Parking-System-Using-BPLus-Trees/
├── program.c
├── inputForVehicle.txt
├── inputForSpaces.txt
└── README.md
```

## **Compilation and Execution**

#### **On Windows, macOS, and Linux:**
1. **Open a terminal or command prompt.**
2. **Navigate to the directory containing `smart_parking.c`.**
3. **Compile and run the code:**
   - **On Windows:**
     ```bash
     gcc program.c -o program.exe
     program.exe
     ```
   - **On macOS/Linux:**
     ```bash
     gcc program.c -o program
     ./program
     ```

---

