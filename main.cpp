#include <iostream>
#include <random>
#include <algorithm>
#include <unordered_set>
#include "libraries/sql/sqlite3.h"

const std::string dbName = "data.db";
const int FLAG_PASSWORD_FOUND = 0;
const int FLAG_SERVICE_EXISTS = 1;
const int MIN_PASSWORD_LENGTH = 8;
const int MAX_PASSWORD_LENGTH = 18;

// Helper function for searching the database 
// PARAMATERS:
/*
TYPE: STRING || NAME: W || USAGE: The name of the service we will be searching
TYPE: INT || NAME: FLAG || USAGE: A Flag to check what our final output will be (if everything goes correctly)
*/
std::string search(const std::string &W, const int &FLAG){
    sqlite3* db;
    int rc = sqlite3_open(dbName.c_str(), &db);
    if (rc || FLAG < FLAG_PASSWORD_FOUND || FLAG >= 3 || W.empty()){ return "FAILED - Flag is not correct / service is empty or there was an error opening the database"; }
    // We initiate the database and check if we can not open the database / flag is not correct / service is empty //
    
    std::string sqlCode = "SELECT password FROM data WHERE service = ?;";
    sqlite3_stmt* stmp;
    rc = sqlite3_prepare_v2(db, sqlCode.c_str(), -1, &stmp, 0);if (rc != SQLITE_OK) { sqlite3_finalize(stmp);sqlite3_close(db);return "FAILED - Preparation"; }
    rc = sqlite3_bind_text(stmp, 1, W.c_str(), -1, SQLITE_STATIC);if (rc != SQLITE_OK) { sqlite3_finalize(stmp);sqlite3_close(db);return "FAILED - Binding"; }
    // We create the sql code, prepare the database, and bind our W //

    rc = sqlite3_step(stmp);
    if (rc == SQLITE_ROW){
        // If we have any data, we grab it in a variable P, and make sure it isnt NULL //
        const char* P = reinterpret_cast<const char*>(sqlite3_column_text(stmp, 0));if (!P) { return "NOSERVICE"; }

        int L = sqlite3_column_bytes(stmp, 0);
        std::string password(P, L);
        // We get it's size for memory issues and make it into a string //

        sqlite3_finalize(stmp);
        sqlite3_close(db);
        if (FLAG == FLAG_PASSWORD_FOUND) { return password; } else if (FLAG == FLAG_SERVICE_EXISTS) { return "EXISTS"; } else { return "N"; }
        // Finally we return accordingly to our FLAG //
    } else {
        // If we cant find any data that means that the service did not exist //
        sqlite3_finalize(stmp);
        sqlite3_close(db);
        return "NOSERVICE";
    }
    return "ENDED";
}

// Helper function for creating new password and adding them to the database //
// PARAMATERS:
/*
TYPE: STRING || NAME: W || USAGE: The actual service we will add
TYPE: STRING || NAME: P || USAGE: The actual password we will add
*/
std::string createPass(const std::string W, const std::string P) {
    if (W.empty() || P.empty()) { return "FAILED - Password or service is empty"; }
    std::string rtn = search(W, FLAG_SERVICE_EXISTS);
    // We quickly make sure both of our paramaters are not empty and perform a search to see if the service
    // Already exists //

    if (rtn != "EXISTS"){
        sqlite3* db;
        int rc = sqlite3_open(dbName.c_str(), &db);if (rc) { return "FAILED - Could not open database"; }
        // Like before we initiate the database and check if we can actually open it //

        std::string sqlCode = "INSERT INTO data (service, password) VALUES (?, ?);";
        sqlite3_stmt* stmt;
        // We define our sql code and a prepared sql statement for later usage //

        rc = sqlite3_prepare_v2(db, sqlCode.c_str(), -1, &stmt, 0);if(rc != SQLITE_OK) { sqlite3_close(db); return "FAILED - Preparation"; }
        rc = sqlite3_bind_text(stmt, 1, W.c_str(), -1, SQLITE_STATIC); if (rc != SQLITE_OK) { sqlite3_finalize(stmt);sqlite3_close(db);return "FAILED - Binding"; }
        rc = sqlite3_bind_text(stmt, 2, P.c_str(), -1, SQLITE_STATIC); if (rc != SQLITE_OK) { sqlite3_finalize(stmt);sqlite3_close(db);return "FAILED - Binding"; }

        rc = sqlite3_step(stmt);
        // We start executing the sql code //
        if (rc != SQLITE_DONE){
            // If we are not done, that means an issue must have occured //
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return "FAILED - Issue with sql code";
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return "SUCCESS";
    }
    return rtn;
}

// Helper function for deleting a certain password //
// PARAMATERS:
/*
TYPE: STRING || NAME: W || USAGE: The actual service we will delete
*/
std::string deletePass(const std::string& W){
    if (W.empty()) { return "FAILED - SERVICE EMPTY"; }
    std::string P = search(W, FLAG_PASSWORD_FOUND);
    // We grab the password

    if (P != "NOSERVICE"){
        sqlite3* db;
        int rc = sqlite3_open(dbName.c_str(), &db); if (rc) { return "FAILED - Couldnt open the database"; }
        
        std::string sqlCode = "DELETE FROM data WHERE password = ?;";
        sqlite3_stmt* stmt;
        // If the the password exists, we initiate the database, the sql code and the stmt //

        rc = sqlite3_prepare(db, sqlCode.c_str(), -1, &stmt, 0); if (rc != SQLITE_OK) { return "FAILED - Preparation error"; };
        rc = sqlite3_bind_text(stmt, 1, P.c_str(), -1, SQLITE_STATIC); if (rc != SQLITE_OK) { sqlite3_finalize(stmt);sqlite3_close(db);return "FAILED - Binding"; }
        
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE){
            // If its not done, that means there was an error with the sql code //
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return "FAILED - Issue with sql code";
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return "SUCCESS";
    }
    return "FAILED";
}

// Helper function to show all the services //
void showAllServices(){
    sqlite3* db;
    int rc = sqlite3_open(dbName.c_str(), &db);
    std::vector<std::string> services;
    // Open the database and create an empty vector to store the services //

    std::string sqlCode = "SELECT service FROM data;";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare(db, sqlCode.c_str(), -1, &stmt, 0);
    // Prepare the database, create the stmt and the sql code //

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        // While we are looping through the data... //
        const char* W = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int L = sqlite3_column_bytes(stmt, 0);
        std::string service(W, L);
        services.push_back(service);
        // Add each service onto that vector //
    }
    if (rc != SQLITE_DONE) { std::cout << sqlite3_errmsg(db);return; }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    for (std::string item : services){
        std::cout << "SERVICE: " << item << "\n";
    }
    // Print all the services //
}

// Helper function to check whether X password is valid
/*
PASSWORD POLICY:
    - Must contain less than 18 characters and at least 8
    - Must contain at least 1 of each uppercase and lowercase letters
    - Must contain at least 1 symbols, making sure that each symbol cant exist more than four times
    - Must contain at least 1 numbers
    - Cant contain spaces
PARAMATERS:
TYPE: STRING || NAME: P || USAGE: The actual password we will check
*/
bool passCheck(std::string P){
    if ((int)P.size() > MAX_PASSWORD_LENGTH || (int)P.size() < MIN_PASSWORD_LENGTH) { std::cout << "Your password is either too small or too long\n";return false; } 
    
    int lowercaseCount = 0;
    int uppercaseCount = 0;
    int numbersCount = 0;
    std::unordered_set<char> symbols;
    // Loop through each character //
    for (char C : P){
        if (C == ' ') { std::cout << "\nYour password must not contain spaces\n";return false; }
        if (std::isupper(C)) { uppercaseCount++; } else if (std::islower(C)) { lowercaseCount++; } // Lower/Upper case checking //
        else if (std::isdigit(C)) { numbersCount++; } // Checking if the character is a digit //
        else {
            if (symbols.count(C) > 4) { std::cout << "Your password must not contain the same symbol four times\n";return false; }
            symbols.insert(C);
        }
    }
    if (lowercaseCount < 1){ std::cout << "Your password must have at least 1 lowercase letters\n";return false; }
    if (uppercaseCount < 1){ std::cout << "Your password must have at least 1 uppercase letters\n";return false; }
    if (numbersCount < 1){ std::cout << "Your password must have at least 1 numbers\n";return false; }
    if ((int)symbols.size() < 1) { std::cout << "Your password must contain at least 1 symbols\n";return false; } 

    return true;
}

// Helper function to generate an password //
std::string generatePassword(){
    std::string P = "";
    std::random_device rd;std::mt19937 gen(rd());

    std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    std::string numbers = "0123456789";
    std::string symbols = "!@#$%&*";

    int length = std::uniform_int_distribution<int>(MIN_PASSWORD_LENGTH, MAX_PASSWORD_LENGTH)(gen);
    // Initiating everything //

    for (int i=0;i<length;++i){ // We loop for random times between 8 and 18 (max/min password length)
        
        int category = gen() % 3; // 0 for letter | 1 for number | 2 for symbol
        
        if (category == 0) { // Add a letter
            char letter = alphabet[gen() % alphabet.size()];if (gen() % 2 == 1) { letter = std::toupper(letter); }
            P += letter;
        } else if (category == 1) { // Add a number
            char number = numbers[gen() % numbers.size()];
            P += number;
        } else { // Add a symbol
            char symbol = symbols[gen() % symbols.size()];
            P += symbol;
        }
    }
    if (!passCheck(P)){
        // If we fail to pass the password check, recall this function //
        P = generatePassword();
        system("cls");
    }
    return P;
}

// Main Function //
int main(){
    sqlite3* db;
    int rc = sqlite3_open(dbName.c_str(), &db);if (rc) { return -1; }
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS data (service TEXT, password TEXT);", 0, 0, 0);
    // Open the database and execute the standard sql code if its the first time //

    while (true) {
        char cmd = 'N';
        std::cout << "\n\n\n--------------SIMPLE PASSWORD MANAGER--------------\n\n\n\n";
        std::cout << "| COMMANDS: W - Creates a new password | R - Reads X password | D - Deletes X password | L - Lists all the services\n| MORE?\n";
        std::cout << "ENTER YOUR COMMAND:  ";std::cin >> cmd;std::cin.ignore();
        // Print everything, grab input of command //

        if (std::tolower(cmd) == 'w'){
            // W = Create new password //
            system("cls");
            std::string W, P = "";
            char choice = 'y';
            std::cout << "Would you like to generate a password automaticly? (y or n): ";std::cin >> choice;std::cin.ignore();
            if (choice == 'y'){
                P = generatePassword();
                std::cout << "Generated password is: " << P << ".Would you like to keep it? (y or n): ";std::cin >> choice;std::cin.ignore();
                if (choice != 'y') { P = "";std::cout << "FINE, MAKE ONE ON YOUR OWN.\n"; }
            }
            std::cout << "Enter the service for the password: ";std::getline(std::cin, W);std::cout << "\nEnter the password now: ";if (P.empty()) { std::cin>>P;std::transform(W.begin(), W.end(), W.begin(), ::tolower); }
            if (!passCheck(P)) { continue; }
            std::cout << createPass(W, P) << "\n";
        } else if (std::tolower(cmd) == 'r'){
            // R = Read X Password //
            system("cls");
            std::string W = "";std::cout << "Enter the service for the password: ";std::getline(std::cin, W);std::transform(W.begin(), W.end(), W.begin(), ::tolower);
            std::cout << "PASSWORD:  " << search(W, FLAG_PASSWORD_FOUND) << "\n";
        } else if (std::tolower(cmd) == 'd') {
            // D = Delete X Password //
            system("cls");
            std::string W = "";
            std::cout << "Enter the service for the password: ";std::cin>>W;std::transform(W.begin(), W.end(), W.begin(), ::tolower);
            std::cout << deletePass(W) << "\n";
        } else if (std::tolower(cmd) == 'l') {
            // L = List all services //
            system("cls");
            showAllServices();
        } else {
            break;
            // If our command is not any of the specified, break //
        }
    }

    sqlite3_close(db);
    return 0;
}
