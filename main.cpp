// Includes //
#include <iostream>
#include <algorithm>
#include "libraries/sql/sqlite3.h"
// ---------- //


const std::string dbName = "data.db"; // The name of the database

// Helper function for searching the database 
// PARAMATERS:
/*
TYPE: STRING || NAME: W || USAGE: The name of the service we will be searching
TYPE: INT || NAME: FLAG || USAGE: A Flag to check what our final output will be (if everything goes correctly)
*/
std::string search(const std::string &W, const int &FLAG){
    sqlite3* db;
    int rc = sqlite3_open(dbName.c_str(), &db);
    if (rc || FLAG < 0 || FLAG >= 3 || W.empty()){ return "FAILED - Flag is not correct / service is empty or there was an error opening the database"; }
    // We initiate the database and check if we can not open the database / flag is not correct / service is empty //
    
    std::string sqlCode = "SELECT password FROM data WHERE service = ?;";
    sqlite3_stmt* stmp;
    rc = sqlite3_prepare_v2(db, sqlCode.c_str(), -1, &stmp, 0);if (rc != SQLITE_OK) { sqlite3_finalize(stmp);sqlite3_close(db);return "FAILED - Preparation"; }
    rc = sqlite3_bind_text(stmp, 1, W.c_str(), -1, SQLITE_STATIC);if (rc != SQLITE_OK) { sqlite3_finalize(stmp);sqlite3_close(db);return "FAILED - Binding"; }
    // We prepare the sql code and the database while also making sure no errors are generated //

    rc = sqlite3_step(stmp);
    // We execute the sql code and get into an if statement //
    if (rc == SQLITE_ROW){
        // If we have any data, we grab it in a variable P, and make sure it isnt NULL //
        const char* P = reinterpret_cast<const char*>(sqlite3_column_text(stmp, 0));if (!P) { return "NOSERVICE"; }

        int L = sqlite3_column_bytes(stmp, 0);
        std::string password(P, L);
        // We get it's size for memory issues and make it into a string //

        sqlite3_finalize(stmp);
        sqlite3_close(db);
        if (FLAG == 0) { return password; } else if (FLAG == 1) { return "EXISTS"; } else { return "N"; }
        // We close off, by finalizing the stmp and close the db. Finally we return accordingly to our FLAG //
    } else {
        // If we cant find any data that means that the service did not exist thus return NOSERVICE //
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
    std::string rtn = search(W, 1);
    // We quickly make sure both of our paramaters are not empty and perform a search to see if the service
    // Already exists //

    // If our returning message is not exists (basic readability) we can continue, otherwise we return exactly what our rtn was//
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
        // Like before we prepare and bind while also making sure no errors are generated // 

        rc = sqlite3_step(stmt);
        // Our prepared sql statement is finally in use (simplier terms we start executing the sql code) //
        if (rc != SQLITE_DONE){
            // If we are not done, that means an issue must have occured with executing the sql code //
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
    std::string rtn = search(W, 1);
    // We make sure the service exists, (is not empty) and actually search for it

    if (rtn == "EXISTS"){
        std::string P = search(W, 0);
        // If the service exists grab the password and store it in a variable //
        
        sqlite3* db;
        int rc = sqlite3_open(dbName.c_str(), &db); if (rc) { return "FAILED - Couldnt open the database"; }
        
        std::string sqlCode = "DELETE FROM data WHERE service = ? AND password = ?;";
        sqlite3_stmt* stmt;
        // Initiate the database, the sql code and the stmt //

        rc = sqlite3_prepare(db, sqlCode.c_str(), -1, &stmt, 0); if (rc != SQLITE_OK) { return "FAILED - Preparation error"; };
        rc = sqlite3_bind_text(stmt, 1, W.c_str(), -1, SQLITE_STATIC); if (rc != SQLITE_OK) { sqlite3_finalize(stmt);sqlite3_close(db);return "FAILED - Binding"; }
        rc = sqlite3_bind_text(stmt, 2, P.c_str(), -1, SQLITE_STATIC); if (rc != SQLITE_OK) { sqlite3_finalize(stmt);sqlite3_close(db);return "FAILED - Binding"; }
        // Prepare, bind, bind //

        rc = sqlite3_step(stmt); // Executing the sql code //
        if (rc != SQLITE_DONE){
            // If its not done, that means there was an error with the sql code //
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return "FAILED - Issue with sql code";
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return "SUCCESS";
        // Otherwise it succeded //
    }
    return "FAILED";
    // Failed //
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
    // Make a final check, close db and finalize //

    for (std::string item : services){
        std::cout << "PASSWORD: " << item << "\n";
    }
    // Print all the services //
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
        std::cout << "ENTER YOUR COMMAND:  ";std::cin >> cmd;
        // Print everything, grab input of command //

        std::tolower(cmd); // Make sure the command is lowercased //
        if (cmd == 'w'){
            // W = Create new password //
            system("cls");
            std::string W, P = "";
            std::cout << "Enter the service for the password: ";std::cin >> W;std::cout << "\nEnter the password now: ";std::cin>>P;std::transform(W.begin(), W.end(), W.begin(), ::tolower);
            std::cout << createPass(W, P) << "\n";
        } else if (cmd == 'r'){
            // R = Read X Password //
            system("cls");
            std::string W = "";std::cout << "Enter the service for the password: ";std::cin>>W;std::transform(W.begin(), W.end(), W.begin(), ::tolower);
            std::cout << "PASSWORD:  " << search(W, 0) << "\n";
        } else if (cmd == 'd') {
            // D = Delete X Password //
            system("cls");
            std::string W = "";
            std::cout << "Enter the service for the password: ";std::cin>>W;std::transform(W.begin(), W.end(), W.begin(), ::tolower);
            std::cout << deletePass(W) << "\n";
        } else if (cmd == 'l') {
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
    // Closing and returning //
}