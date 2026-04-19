#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <limits> // Added for input validation

using namespace std;

// ===== USER PROFILE =====
struct UserProfile {
    string fullName;
    string emailAddress;
};

// ===== SESSION STATE =====
struct SessionState {
    int currentOtp     = 0;
    time_t otpCreatedAt  = 0;
    int failedAttempts   = 0;
    bool isLocked        = false;
    string activeToken   = "";
    bool isLoggedIn      = false;
};

// ===== CONSTANTS =====
const int OTP_EXPIRY_SECONDS = 30;
const int MAX_FAILED_ATTEMPTS = 3;
const int OTP_MIN = 100000;
const int OTP_MAX = 999999;

// ===== GLOBAL STATE =====
UserProfile user;
SessionState session;

// ===== INPUT VALIDATION HELPER =====
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ===== HELPERS =====
string getCurrentTimestamp() {
    time_t now = time(0);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return string(buffer);
}

void writeLog(string event) {
    ofstream logFile("activity_log.txt", ios::app);
    logFile << "[" << getCurrentTimestamp() << "] " << event << endl;
    logFile.close();
}

bool isOtpExpired() {
    return (time(0) - session.otpCreatedAt) > OTP_EXPIRY_SECONDS;
}

string generateToken(string email) {
    srand(time(0));
    return "TKN-" + email.substr(0, 3) + "-" + to_string(rand() % 90000 + 10000);
}

void showDivider(string title = "") {
    cout << "\n";
    if (title != "")
        cout << "===== " << title << " =====\n";
    else
        cout << "========================\n";
}

// ===== SAVE / LOAD USER =====
void saveUserToFile() {
    ofstream userFile("user_data.txt");
    userFile << user.fullName << "\n" << user.emailAddress << "\n";
    userFile.close();
}

bool loadUserFromFile() {
    ifstream userFile("user_data.txt");
    if (!userFile) return false;
    getline(userFile, user.fullName);
    getline(userFile, user.emailAddress);
    userFile.close();
    return true;
}

// ===== FEATURE 1: REGISTER =====
void registerUser() {
    showDivider("REGISTER");
    cout << "Enter your full name : ";
    cin.ignore();
    getline(cin, user.fullName);

    cout << "Enter your email     : ";
    getline(cin, user.emailAddress);

    if (user.fullName.empty() || user.emailAddress.empty()) {
        cout << "[!] Name and email cannot be empty.\n";
        return;
    }

    saveUserToFile();
    cout << "[+] Account created for " << user.fullName << "\n";
    writeLog("New user registered: " + user.emailAddress);
}

// ===== FEATURE 2: GENERATE OTP =====
void generateOtp() {
    showDivider("GENERATE OTP");

    if (user.emailAddress.empty()) {
        cout << "[!] Please register first.\n";
        return;
    }

    if (session.isLocked) {
        cout << "[!] Account is locked. Cannot generate OTP.\n";
        return;
    }

    srand(time(0));
    session.currentOtp   = rand() % (OTP_MAX - OTP_MIN + 1) + OTP_MIN;
    session.otpCreatedAt = time(0);

    ofstream emailFile("simulated_email.txt");
    emailFile << "To      : " << user.emailAddress           << "\n";
    emailFile << "Subject : Your Login OTP\n\n";
    emailFile << "Hello " << user.fullName << ",\n\n";
    emailFile << "Your OTP is : " << session.currentOtp     << "\n";
    emailFile << "Expires in  : " << OTP_EXPIRY_SECONDS << " seconds\n\n";
    emailFile << "Do NOT share this OTP with anyone.\n";
    emailFile.close();

    cout << "[+] OTP sent to " << user.emailAddress << "\n";
    cout << "[i] Check 'simulated_email.txt' to view the OTP.\n";
    cout << "[i] OTP expires in " << OTP_EXPIRY_SECONDS << " seconds.\n";
    writeLog("OTP generated for: " + user.emailAddress);
}

// ===== FEATURE 3: LOGIN =====
void loginWithOtp() {
    showDivider("LOGIN");

    if (session.isLocked) {
        cout << "[!] Account is locked after " << MAX_FAILED_ATTEMPTS << " wrong attempts.\n";
        return;
    }

    if (session.currentOtp == 0) {
        cout << "[!] No OTP found. Please generate one first.\n";
        return;
    }

    if (isOtpExpired()) {
        cout << "[!] OTP has expired. Please generate a new one.\n";
        session.currentOtp = 0;
        return;
    }

    int enteredOtp;
    cout << "Enter OTP : ";
    if (!(cin >> enteredOtp)) {
        cout << "[!] Invalid input. Please enter numbers only.\n";
        clearInput();
        return;
    }

    if (enteredOtp == session.currentOtp) {
        session.activeToken = generateToken(user.emailAddress);
        session.isLoggedIn  = true;
        session.failedAttempts = 0;
        session.currentOtp  = 0;

        cout << "[+] Login successful! Welcome, " << user.fullName << ".\n";
        cout << "[+] Session Token : " << session.activeToken << "\n";
        writeLog("Successful login for: " + user.emailAddress);
    } else {
        session.failedAttempts++;
        int remaining = MAX_FAILED_ATTEMPTS - session.failedAttempts;

        cout << "[-] Wrong OTP.";
        if (remaining > 0)
            cout << " You have " << remaining << " attempt(s) left.\n";
        else {
            cout << "\n";
            session.isLocked = true;
            cout << "[!] Too many wrong attempts. Account LOCKED.\n";
            writeLog("Account locked: " + user.emailAddress);
        }
        writeLog("Failed OTP attempt for: " + user.emailAddress);
    }
}

// ===== ATTACK 1: BRUTE FORCE =====
void simulateBruteForce() {
    showDivider("BRUTE FORCE ATTACK SIMULATION");

    if (session.currentOtp == 0) {
        cout << "[!] No active OTP. Generate one first to run this demo.\n";
        return;
    }

    int searchStart = session.currentOtp - 3;
    int searchEnd   = session.currentOtp + 3;
    bool found = false;

    for (int guess = searchStart; guess <= searchEnd; guess++) {
        if (session.isLocked) {
            cout << "\n[Defense] Account locked! Brute force BLOCKED.\n";
            writeLog("Brute force blocked by lockout");
            return;
        }

        if (isOtpExpired()) {
            cout << "\n[Defense] OTP expired! Brute force FAILED.\n";
            writeLog("Brute force blocked by OTP expiry");
            return;
        }

        cout << "[Attacker] Trying: " << guess << " ... ";

        if (guess == session.currentOtp) {
            cout << "MATCH!\n";
            writeLog("Brute force demo: OTP matched");
            found = true;
            break;
        } else {
            session.failedAttempts++;
            cout << "Wrong\n";
            if (session.failedAttempts >= MAX_FAILED_ATTEMPTS) {
                session.isLocked = true;
            }
        }
    }

    if (!found) cout << "\n[Defense] Brute force failed.\n";
}

// ===== ATTACK 2: PHISHING =====
void simulatePhishing() {
    showDivider("PHISHING ATTACK SIMULATION");

    if (user.emailAddress.empty()) {
        cout << "[!] Register a user first to run this demo.\n";
        return;
    }

    generateOtp();

    cout << "\n[Scenario] User received OTP. Attacker sending fake page...\n";
    cout << "\n--- FAKE BANK LOGIN PAGE ---\n";
    cout << "  Enter the OTP sent to your email: ";

    int capturedOtp;
    if (!(cin >> capturedOtp)) {
        clearInput();
        return;
    }

    cout << "\n[Attacker] Got it! Captured OTP: " << capturedOtp << "\n";
    if (!isOtpExpired() && capturedOtp == session.currentOtp) {
        cout << "\n[!] PHISHING SUCCEEDED!\n";
        writeLog("Phishing attack succeeded");
    } else {
        cout << "\n[*] Phishing failed.\n";
        writeLog("Phishing attack failed");
    }
}

// ===== ATTACK 3: SESSION HIJACK =====
void simulateSessionHijack() {
    showDivider("SESSION HIJACK SIMULATION");

    if (!session.isLoggedIn || session.activeToken.empty()) {
        cout << "[!] No active session.\n";
        return;
    }

    string stolenToken = session.activeToken;
    cout << "[Attacker] Intercepted token: " << stolenToken << "\n";
    cout << "[!] SESSION HIJACK SUCCEEDED!\n";
    
    session.activeToken = "";
    session.isLoggedIn  = false;
    cout << "\n[Defense] Session token invalidated.\n";
    writeLog("Session token destroyed after hijack demo");
}

// ===== VIEW LOG / STATUS =====
void viewActivityLog() {
    showDivider("ACTIVITY LOG");
    ifstream logFile("activity_log.txt");
    if (!logFile) {
        cout << "[i] No activity recorded yet.\n";
        return;
    }
    string line;
    while (getline(logFile, line)) cout << line << "\n";
    logFile.close();
}

void viewStatus() {
    showDivider("CURRENT STATUS");
    cout << "User         : " << (user.fullName.empty() ? "Not registered" : user.fullName) << "\n";
    cout << "Account      : " << (session.isLocked ? "LOCKED" : "Active") << "\n";
    cout << "Logged In    : " << (session.isLoggedIn ? "Yes" : "No") << "\n";
    cout << "OTP Active   : " << (session.currentOtp != 0 && !isOtpExpired() ? "Yes" : "No") << "\n";
}

void resetSession() {
    session = {0, 0, 0, false, "", false};
    cout << "[+] Session reset.\n";
    writeLog("Session manually reset");
}

int main() {
    cout << "====================================\n";
    cout << "  OTP/2FA Attack & Defense Demo\n";
    cout << "====================================\n";

    loadUserFromFile();
    int choice;

    while (true) {
        showDivider("MAIN MENU");
        cout << " 1. Register\n 2. Generate OTP\n 3. Login with OTP\n";
        cout << " 4. Brute Force Demo\n 5. Phishing Demo\n 6. Hijack Demo\n";
        cout << " 7. Status\n 8. Logs\n 9. Reset\n 0. Exit\nChoice : ";
        
        if (!(cin >> choice)) {
            cout << "[!] Invalid input. Please enter a number.\n";
            clearInput();
            continue;
        }

        switch (choice) {
            case 1: registerUser(); break;
            case 2: generateOtp(); break;
            case 3: loginWithOtp(); break;
            case 4: simulateBruteForce(); break;
            case 5: simulatePhishing(); break;
            case 6: simulateSessionHijack(); break;
            case 7: viewStatus(); break;
            case 8: viewActivityLog(); break;
            case 9: resetSession(); break;
            case 0: return 0;
            default: cout << "[!] Invalid choice.\n";
        }
    }
}