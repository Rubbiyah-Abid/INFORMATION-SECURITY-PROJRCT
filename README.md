# OTP / 2FA Attack & Defense Simulation (C++)

## 📌 Project Overview
This project is a console-based **OTP (One-Time Password) authentication system** built in C++. It demonstrates both **security mechanisms** and **common cyber attacks** such as brute force, phishing, and session hijacking in a controlled simulation environment.

The goal is to help understand how OTP systems work and how attackers attempt to break them, along with basic defense strategies.

---

## 👨‍💻 Developed By
**RUBBIYAH ABID **  
BS(IT) – FCIT  
Course: INFORMATION Security 

---

## ⚙️ Features

### 🔐 Authentication System
- User registration (name & email)
- OTP generation (6-digit random code)
- OTP expiration (30 seconds)
- Login validation using OTP
- Session token generation
- Account lock after 3 failed attempts

### 🛡️ Security Logging
- Activity logging in `activity_log.txt`
- Timestamp-based event tracking
- Login success & failure records

### 📁 File Handling
- User data stored in `user_data.txt`
- OTP simulation email stored in `simulated_email.txt`

---

## ⚔️ Attack Simulations

### 1. Brute Force Attack
- Simulates repeated guessing of OTP
- Shows how lockout protects the system

### 2. Phishing Attack
- Fake login page simulation
- Demonstrates user OTP leakage risk

### 3. Session Hijacking
- Shows token interception scenario
- Demonstrates session invalidation risk

---

## 🧠 Security Features Implemented
- OTP expiry mechanism
- Account lockout system
- Input validation
- Session token-based authentication
- Logging for audit trail

---

## ▶️ How to Run

### Compile:
```bash
g++ main.cpp -o otp_system
