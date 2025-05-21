# Smart Car Parking System 🚗🅿️

A C-based project that simulates an efficient, automated car parking system using **B+ Trees** for space and vehicle management. This system ensures fast search, insertion, and deletion of parking and vehicle records, making it suitable for real-time environments.

## 🔧 Features

- 🧠 **Smart Parking Space Allocation** using B+ Trees
- 🚗 **Vehicle Entry and Exit Management**
- 📂 **File Saving**:
  - Stores all parking space data in `inputForSpaces.txt`
  - Stores all vehicle data in `inputForVehicle.txt`
- ✅ **Duplicate Entry Prevention** for both vehicles and parking spots
- 📉 **Efficient Storage and Lookup** with B+ Tree implementation
- 📤 **Graceful Close Operation** that writes current data to files
- 🧪 Fully written in **C Language** for performance and control

## 🧱 Data Structures Used

- **B+ Tree**:
  - For managing both vehicle and parking space records
  - Offers logarithmic time complexity for insertion, deletion, and search
- **File I/O**:
  - Persist data upon system closure

## 📁 File Structure

```text
SmartCarParkingSystem/
├── program.c
├── inputForVehicle.txt
├── inputForSpaces.txt
└── README.md


