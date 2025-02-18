#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <conio.h>
#include <windows.h>

using namespace std;
class Bank
{
    public:
        void menu();
        void bank_management();
        void atm_management();
};
void Bank::menu()
{
    p:
    system("cls");
    int choice;
    char ch;
    string email,pin,pass;
    cout<<"\n\n\t\t\tControl Panel";
    cout<<"\n\n 1. Bank Management";
    cout<<"\n 2. ATM Management";
    cout<<"\n 3. Exit";
    cout<<"\n\n Enter Your Choice : ";
    cin>>choice;
    switch(choice)
    {
        case 1:
            cout<<"\n\n\t\t\tLogin Account";
            cout<<"\n\n E-mail : ";
            cin>>email;
            cout<<"\n\n\t\t Pin Code : ";
            for(int i=1;i<=5;i++)
            {
                ch = getch();
                pin +=ch;
                cout<<"*";
            }
            cout<<"\n\n Password : ";
            for(int i=1;i<=5;i++)
            {
                ch = getch();
                pass +=ch;
                cout<<"*";
            }
            if(email =="admin@gmail.com" && pin=="12345" && pass=="54321")
            {
                bank_management();
            }
            else
            {
                cout<<"\n\n Your E-mail, Pin & Password is wrong...";
            }

            break;
        case 2:
            atm_management();
            break;
        case 3:
            exit();
            break;
        default:
            cout<<"\n\n Invalid Value...Please Try Again";
    }
    getch();
    goto p;
}

struct Record {
    size_t id;
    string name;
    size_t age;
    double balance=0;
};

// Function to check if a record exists
bool recordExists(const size_t& id, const vector<Record>& records) {
    for (const auto& record : records) {
        if (record.id == id) {
            return true;
        }
    }
    return false;
}

// Function to read the CSV file
vector<Record> readCSV(const string& filename) {
    vector<Record> records;
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        Record record;
        string idStr, ageStr,balanceStr;
        getline(ss, idStr, ',');
        getline(ss, record.name, ',');
        getline(ss, ageStr, ',');
        getline(ss, balanceStr, ',');
        record.id = stoi(idStr);
        record.age = stoi(ageStr);
        record.balance = stod(balanceStr);
        records.push_back(record);
    }

    file.close();
    return records;
}

// Function to get a record by ID (returns Record)
Record getRecordById(const string& filename, size_t id) {
    vector<Record> records = readCSV(filename);
    for (const auto& record : records) {
        if (record.id == id) {
            return record;
        }
    }
    throw runtime_error("Record with ID " + to_string(id) + " not found.");
}
// Function to write the CSV file
void writeCSV(const string& filename, const vector<Record>& records) {
    ofstream file(filename);
    for (const auto& record : records) {
        file << record.id << "," << record.name << "," << record.age<<","<<record.balance << "\n";
    }
    file.close();
}

// Function to add a new record
void addRecord(const string& filename, const Record& newRecord) {
    vector<Record> records = readCSV(filename);

    if (recordExists(newRecord.id, records)) {
        cout << "Record with ID " << newRecord.id << " already exists." << endl;
        return;
    }

    ofstream file(filename, ios::app);
    file << newRecord.id << "," << newRecord.name << "," << newRecord.age<< "," << newRecord.balance << "\n";
    file.close();
    cout << "Record added successfully." << endl;
}

//Function to check Balance
void checkBalance(const string& filename,const size_t id)
{
    vector<Record> records = readCSV(filename);
    double currentBalance;
    bool found = false;
    for (const auto& record : records) {
        if (record.id == id) {
            currentBalance = record.balance;
            found= true;
            break;
        }
    }
    if(found)cout<<"ID :"<<id<<endl<<"Balance :"<<currentBalance<<endl;
    if(!found)cout << "Record with ID " << id << " not found." << endl;


}
//Function to all info
void allInfo(const string& filename,const size_t id)
{
    vector<Record> records = readCSV(filename);
    double currentBalance;
    bool found = false;
    for (const auto& record : records) {
        if (record.id == id) {
            cout<<"ID :"<<record.id<<endl;
            cout<<"Name :"<<record.name<<endl;
            cout<<"Age :"<<record.age<<endl;
            cout<<"Balance :"<<record.balance<<endl;
            found= true;
            break;
        }
    }
    if(!found)cout << "Record with ID " << id << " not found." << endl;


}
//Function to all user
void allUser(const string& filename)
{
    vector<Record> records = readCSV(filename);
    double currentBalance;
    bool found = false;
    cout<<"ID\t"<<"Name\t"<<"age\t"<<"Balance"<<endl;
    for (const auto& record : records) {
        cout<<record.id<<"\t"<<record.name<<"\t"<<record.age<<"\t"<<record.balance<<endl;
    }


}
void updateRecord(const string& filename, const Record& updatedRecord) {
    vector<Record> records = readCSV(filename);
    bool found = false;
    for (auto& record : records) {
        if (record.id == updatedRecord.id) {
            record.name = updatedRecord.name;
            record.age = updatedRecord.age;
            record.balance = updatedRecord.balance;
            found = true;
            break;
        }
    }

    if (found) {
        writeCSV(filename, records);
        cout << "Record updated successfully." << endl;
    } else {
        cout << "Record with ID " << updatedRecord.id << " not found." << endl;
    }
}
// Function to delete a record
void deleteRecord(const string& filename, const size_t id) {
    vector<Record> records = readCSV(filename);
    vector<Record> filteredRecords;
    for (const auto& record : records) {
        if (record.id != id) {
            filteredRecords.push_back(record);
        }
    }

    if (records.size() != filteredRecords.size()) {
        writeCSV(filename, filteredRecords);
        cout << "Record deleted successfully." << endl;
    } else {
        cout << "Record with ID " << id << " not found." << endl;
    }
}

// Function to add balance to a record
void addBalance(const string& filename, size_t id, double amount) {
    vector<Record> records = readCSV(filename);
    bool found = false;

    for (auto& record : records) {
        if (record.id == id) {
            record.balance += amount;
            found = true;
            break;
        }
    }

    if (found) {
        writeCSV(filename, records);
        cout << "Balance added successfully to ID " << id << "!" << endl;
    } else {
        cout << "Record with ID " << id << " not found." << endl;
    }
}
// Function to transfer balance from one record to another
void transferBalance(const string& filename, size_t fromId, size_t toId, double amount) {
    vector<Record> records = readCSV(filename);
    bool fromFound = false, toFound = false;

    for (auto& record : records) {
        if (record.id == fromId) {
            if (record.balance >= amount) {
                record.balance -= amount;
                fromFound = true;
            } else {
                cout << "Insufficient balance in account ID " << fromId << "!" << endl;
                return;
            }
        }
    }

    for (auto& record : records) {
        if (record.id == toId) {
            record.balance += amount;
            toFound = true;
        }
    }

    if (fromFound && toFound) {
        writeCSV(filename, records);
        cout << "Balance transferred successfully from ID " << fromId << " to ID " << toId << "!" << endl;
    } else {
        if (!fromFound) cout << "Record with ID " << fromId << " not found." << endl;
        if (!toFound) cout << "Record with ID " << toId << " not found." << endl;
    }
}
int main() {
    string filename = "account.csv";

    bool exit=false;
    while(!exit)
    {
        cout<<"Welcome to Bank Account Management System"<<endl;
        cout<<"========================================="<<endl;
    }
    // Example: Add a new record
    Record newRecord = {2024, "Nasir", 24};
    //addRecord(filename,newRecord);
    //updateRecord(filename, newRecord);
    //deleteRecord(filename,2025);
    //transferBalance(filename,2023,2024,100);
    //addBalance(filename,2023,100);
    //checkBalance(filename,2023);
    //allInfo(filename,2023);
    try {
        // Example: Get record by ID and return the data
        Record record = getRecordById(filename, 2026);
        cout << "ID: " << record.id << "\n";
        cout << "Name: " << record.name << "\n";
        cout << "Age: " << record.age << "\n";
        cout << "Balance: " << record.balance << "\n";
    } catch (const exception& e) {
        cout << e.what() << endl;
    }

    return 0;
}

