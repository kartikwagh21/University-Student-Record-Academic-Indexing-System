# University Student Record & Academic Indexing System

A C++ console-based academic record management system that indexes and retrieves student data using both an **AVL Tree** and a **B+ Tree** (Order 4), featuring real-time execution benchmarking and CSV persistence[cite: 2, 4, 7, 11].

---

## 📋 Overview

This project implements a dual-tree indexing engine designed to handle thousands of university student records efficiently using **Student ID** as the primary search key[cite: 1, 2]. Users can interact with either data structure independently or run automated performance comparisons across core operations[cite: 12, 13, 14].

### Student Record Schema

| Field | Data Type | Description |
| :--- | :--- | :--- |
| **Student ID** | `int` | Unique primary key identifier |
| **Student Name** | `string` | Full name of the student |
| **Department** | `string` | Academic branch (e.g., CSE, IT, AIML) |
| **Semester** | `int` | Current academic semester |
| **CGPA** | `float` | Cumulative Grade Point Average |
| **City** | `string` | Student's residential city[cite: 1, 2, 4] |

---

## ✨ Features & Implemented Challenges

### Core Operations (AVL Tree & B+ Tree)
* **Insert Record:** Adds a new student while maintaining tree balance (rotations in AVL; node splits in B+ Tree).
* **Search by ID:** Fast $O(\log n)$ lookup by Student ID.
* **Delete Record:** Removes withdrawn students and updates tree structure.
* **Sorted Display:** Outputs all records in ascending order of Student ID (in-order traversal for AVL; linked leaf traversal for B+ Tree).
* **Range Query:** Retrieves all students whose IDs fall within a specified `[minID, maxID]` interval.
* **Performance Benchmarking:** Measures and compares insertion and search execution times (in microseconds) across both trees using `std::chrono`[cite: 1, 5, 7, 8, 9, 10, 11, 12, 13].

### Completed Assignment Challenges
1. **Challenge 1 – Top CGPA Students:** Filters and displays all students with a `CGPA > 9.0`.
2. **Challenge 2 – Duplicate Record Detection:** Validates Student IDs before insertion and rejects duplicates with an error message (and ignores duplicate entries on CSV load).
3. **Challenge 3 – Data Persistence:** Automatically loads existing records from `students.csv` at startup (falling back to a default 10-student dataset if no file exists) and saves changes back to CSV.
4. **Challenge 5 – Department Filtering:** Queries and displays records belonging to a specific department (e.g., `CSE`, `IT`, `AIML`) while preserving Student ID sorting[cite: 1, 3, 11, 13, 14, 15].

---

## 🛠️ Tech Stack & Requirements

* **Language:** C++ (C++11 or later)
* **Standard Libraries:** `<iostream>`, `<vector>`, `<string>`, `<algorithm>`, `<fstream>`, `<sstream>`, `<chrono>`, `<iomanip>`[cite: 4, 12]
* **Compiler:** `g++`, `clang++`, or MSVC

---

## 🚀 Getting Started

### Compilation
```bash
g++ -std=c++11 -O2 UniversityStudentRecordSystem_Final.cpp -o student_system
