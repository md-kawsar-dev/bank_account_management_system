#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
//#include <conio.h>
//#include <windows.h>
#include <ctime>
#include <string>
#include <unistd.h>
#include <termios.h>

using namespace std;
// char getch(void) {
//     struct termios oldt, newt;
//     char ch;
//     tcgetattr(STDIN_FILENO, &oldt);
//     newt = oldt;
//     newt.c_lflag &= ~(ICANON | ECHO);  // Disable buffering and echo
//     tcsetattr(STDIN_FILENO, TCSANOW, &newt);
//     ch = getchar();
//     tcsetattr(STDIN_FILENO, TCSANOW, &oldt);  // Restore terminal settings
//     return ch;
// }

struct Record {
    int id;
    string name;
    int pin;
    string pass,address,phone;
    double balance=0;
};
// Function to check if a file Open
bool isFileOpen(const string& filename) {
    ifstream file(filename);
    return file.is_open();
}
// Function to check if a record exists
bool recordExists(const int& id, const vector<Record>& records) {
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
    ifstream file(filename,ios::out);

    string line;
    while (getline(file, line))
    {
        if (line.find("ID") != string::npos || line.find("Name") != string::npos) {
            continue;
        }
        stringstream ss(line);
        Record record;
        string idStr,balanceStr,pinStr,nameStr,addressStr,passStr,phoneStr;
        getline(ss, idStr, ',');
        getline(ss, nameStr, ',');
        getline(ss, pinStr, ',');
        getline(ss, passStr, ',');
        getline(ss, addressStr, ',');
        getline(ss,phoneStr,',');
        getline(ss, balanceStr, ',');
        record.id = stoi(idStr);
        record.name = nameStr;
        record.pin = stoi(pinStr);
        record.pass = passStr;
        record.address = addressStr;
        record.phone = phoneStr;
        record.balance = stod(balanceStr);
        records.push_back(record);
    }

    file.close();
    return records;
}
// Function add withdraw history in a csv file
void bank_history(int id,double amount,string type,string remarks="") {
    ofstream file("history.csv", ios::app);
    if(!file)
    {
        cout<<"\n\n File Not Found...";
    }else{
        // date and time
        time_t now = time(0);
        char* dt = ctime(&now);
        file << id << "," << amount << "," << type << ","<<remarks<<","<< dt;
        file.close();
    }
    file.close();
}
// Function to write the CSV file
void writeCSV(const string& filename, const vector<Record>& records) {
    ofstream file(filename);
    for (const auto& record : records) {
        file << record.id << "," << record.name << "," << record.pin<<","<< record.pass<<","<< record.address<<","<< record.phone<<","<<record.balance << "\n";
    }
    file.close();
}
// Function allInfoByID(filename,id)
void allInfoByID(const string& filename,const int id)
{
    vector<Record> records = readCSV(filename);
    bool found = false;
    Record record;
    for (const auto& data : records) {
        if (data.id == id) {
            record.id = data.id;
            record.name = data.name;
            record.pin = data.pin;
            record.pass = data.pass;
            record.address = data.address;
            record.phone = data.phone;
            record.balance = data.balance;
            found= true;
            break;
        }
    }
    if(found)cout<<"ID :"<<record.id <<"  Name :"<<record.name<<"  Pin :"<<record.pin<<"  Password :"<<record.pass<<"  Address :"<<record.address<<"  Phone :"<<record.phone<<"   Balance :"<<record.balance<<endl;
    if(!found)cout << "Record with ID " << id << " not found." << endl;

}
// Function to add a new record
void addRecord(const string& filename, const Record& record) {

    ofstream file(filename, ios::app);
    file << record.id << "," << record.name << "," << record.pin<<","<< record.pass<<","<< record.address<<","<< record.phone<<","<<record.balance << "\n";
    file.close();
    cout<<"\n\n New User Account Created Successfully...";
}
// Function to add balance to a record
void addBalance(const string& filename, int id, double amount) {
    vector<Record> records = readCSV(filename);
    bool found = false;

    for (auto& record : records) {
        if (record.id == id) {
            record.balance += amount;
            bank_history(id,amount,"Deposit");
            found = true;
            break;
        }
    }

    if (found) {
        writeCSV(filename, records);
        cout << "\n\nBalance added successfully to ID " << id << "!";
    }
}
// Function to add balance to a record
bool withdrawBalance(const string& filename, int id, double amount) {
    vector<Record> records = readCSV(filename);
    bool found = false;

    for (auto& record : records) {
        if (record.id == id && record.balance >= amount && amount>0) {
            record.balance -= amount;
            bank_history(id,amount,"Withdraw");
            found = true;
            break;
        }
    }

    if (found) {
        writeCSV(filename, records);
        return true;
    }else{
        return false;
    }
}
// Function to transfer balance from one record to another
bool transferBalance(const string& filename, int fromId, int toId, double amount) {
    vector<Record> records = readCSV(filename);
    bool fromFound = false, toFound = false,inSufficient=false;

    for (auto& record : records) {
        if (record.id == fromId) {
            if (record.balance >= amount) {
                record.balance -= amount;
                fromFound = true;
            } else {
                inSufficient = true;
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
        bank_history(fromId,amount,"Transfer");
        bank_history(toId,amount,"deposit");
        return true;
    }
    else{
        return false;
    }

}
bool paymentBalance(const string& filename, int id, double amount,string bill_type) {
    vector<Record> records = readCSV(filename);
    bool found = false;

    for (auto& record : records) {
        if (record.id == id && record.balance >= amount && amount>0) {
            record.balance -= amount;
            bank_history(id,amount,"payment",bill_type);
            found = true;
            break;
        }
    }

    if (found) {
        writeCSV(filename, records);
        return true;
    }else{
        return false;
    }
}
bool updateRecord(const string& filename, const Record& updatedRecord) {
    vector<Record> records = readCSV(filename);
    bool found = false;
    for (auto& record : records) {
        if (record.id == updatedRecord.id) {
            if(updatedRecord.name!="") record.name = updatedRecord.name;
            if(updatedRecord.pin!=0) record.pin = updatedRecord.pin;
            if(updatedRecord.pass!="") record.pass = updatedRecord.pass;
            if(updatedRecord.address!="") record.address = updatedRecord.address;
            if(updatedRecord.phone!="") record.phone = updatedRecord.phone;
            found = true;
            break;
        }
    }

    if (found) {
        writeCSV(filename, records);
        return true;
    } else {
        return false;
    }
}
bool deleteRecord(const string& filename, const int id) {
    vector<Record> records = readCSV(filename);
    vector<Record> filteredRecords;
    for (const auto& record : records) {
        if (record.id != id) {
            filteredRecords.push_back(record);
        }
    }

    if (records.size() != filteredRecords.size()) {
        writeCSV(filename, filteredRecords);
        return true;
    } else {
        return false;
    }
}
class Bank
{
    private:
        int pin,id;
        float balance;
        string pass,name,fname,address,phone,fileName="bank.csv";

    public:
        void menu();
        void bank_management(int cal_fun=0);
        void atm_management();
        void new_user();
        void already_user();
        void deposit();
        void withdraw();
        void transfer();
        void payment();
        void search();
        void edit();
        void delete_user();

};
void Bank::menu()
{
    system("clear");
    int choice;
    char ch;
    bool flag = true;
    string email,pin,pass;
    cout<<"\n\n\t\t\tControl Panel";
    cout<<"\n\n 1. Bank Management";
    cout<<"\n 2. ATM Management";
    cout<<"\n 3. Exit";
    cout<<"\n\n Enter Your Choice : ";
    cin>>choice;
    while (flag)
    {
        
        switch(choice)
        {
            case 1:
                system("clear");
                /*
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
                */
                bank_management();
                break;
            case 2:
                atm_management();
                break;
            case 3:
                flag = false;
                cout<<"\n\n\t\t Thank You... \n\n";
                exit(0);
                break;
            default:
                cout<<"\n\n Invalid Value...Please Try Again";
        }
    }
   
    
}

void Bank::bank_management(int cal_fun)
{
   
    if(cal_fun==0)
    {
        system("clear");
    }
    int choice;
    bool flag = true;
    cout<<"\n\n\t\t\tBank Management System";
    cout<<"\n\n 1. New User";
    cout<<"\n 2. Already User";
    cout<<"\n 3. Deposit Option";
    cout<<"\n 4. Withdraw Option";
    cout<<"\n 5. Transfer Option";
    cout<<"\n 6. Payment Option";
    cout<<"\n 7. Search User Record";
    cout<<"\n 8. Edit User Record";
    cout<<"\n 9. Delete User Record";
    cout<<"\n 10. Show All Records";
    cout<<"\n 11. Payment All Records";
    cout<<"\n 12. Go Back";
    cout<<"\n\n Enter Your Choice : ";
    cin>>choice;
    while (flag)
    {
        switch(choice)
        {
            case 1:
                new_user();
                break;
            case 2:
                already_user();
                break;
            case 3:
                deposit();
                break;
            case 4:
                withdraw();
                break;
            case 5:
                transfer();
                break;
            case 6:
                payment();
                break;
            case 7:
                search();
                break;
            case 8:
                edit();
                break;
            case 9:
                delete_user();
                break;
            case 10:
                break;
            case 11:
                break;
            case 12:
                flag = false;
                menu();
                break;
            default:
                cout<<"\n\n Invalid Value...Please Try Again";
        }
    }
    
    
}
void Bank::atm_management()
{
    p:
    system("clear");
    int choice;
    cout<<"\n\n\t\t\tATM Management System";
    cout<<"\n\n 1. User Login & Check Balance";
    cout<<"\n 2. Withdraw Amount";
    cout<<"\n 3. Account Detailes";
    cout<<"\n 4. Go Back";
    cout<<"\n\n Enter Your Choice : ";
    cin>>choice;
    switch(choice)
    {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            menu();
            break;
        default:
            cout<<"\n\n Invalid Value...Please Try Again";
    }
    // getch();
    // goto p;
    char c;
    cin.get(c);
    if(c == '\n')goto p;
}
void Bank::new_user()
{
    
    system("clear");
    cout<<"\n\n\t\t\tAdd New User";
    cout<<"\n\n User ID : ";
    cin>>id;
    cout<<"\n\n\t\t Name : ";
    cin.ignore();
    getline (cin, name);
    cout<<"\n\n\t\t Address : ";
    getline (cin, address);
    cout<<"\n\n Pin Code (5 digit) : ";
    cin>>pin;
    cout<<"\n\n\t\t Password : ";
    cin>>pass;
    cout<<"\n\n Phone No. : ";
    cin>>phone;
    cout<<"\n\n\t\t Current Balance : ";
    cin>>balance;
    vector<Record> records = readCSV(fileName);

    Record newRecord = {id,name,pin,pass,address, phone, balance};
    if (recordExists(newRecord.id, records)) {
        cout<<"\n\n User ID Already Exist...";
        new_user();
    }
    else
    {
        if(records.size() ==0)
        {
            ofstream file(fileName, ios::app);
            file << "ID" << "," << "Name"<< "," << "Pin"<<","<<"Password"<<","<<"Address"<<","<< "Phone"<<","<<"Balance"<< "\n";
            file.close();
        }
        addRecord(fileName,newRecord);
        bank_management();

    }
    /*
    file.open("bank.csv",ios::in);
    if(!file)
    {
        file.open("bank.csv",ios::out);
        file<<id<<","<<name<<","<<fname<<","<<address<<","<<pin<<","<<pass<<","<<phone<<","<<balance<<","<<"\n";
        file.close();
    }
    else
    {
        file>>ide>>n>>f>>a>>pi>>pa>>ph>>b;
        while(!file.eof())
        {
            if(ide==id)
            {
                cout<<"\n\n User ID Already Exist...";
                getch();
                goto p;
            }
            file>>ide>>n>>f>>a>>pi>>pa>>ph>>b;
        }
        file.close();
        file.open("bank.csv",ios::out);
        file<<id<<","<<name<<","<<fname<<","<<address<<","<<pin<<","<<pass<<","<<phone<<","<<balance<<","<<"\n";
        file.close();
    }
    cout<<"\n\n New User Account Created Successfully...";
    */

}
void Bank::already_user()
{ 
        system("clear");
        cout<<"\n\n\t\t\tAlready User";
        if(!isFileOpen(fileName))
        {
            cout<<"\n\n File Not Found...";
            already_user();

        }else{
            int a_id;
            cout<<"\n\n Enter User ID : ";
            cin>>a_id;
            vector<Record> records = readCSV(fileName);
            if (!recordExists(a_id, records)) {
                cout<<"\n\n Record with ID " << a_id << " not found.";
                already_user();
            }else{
                
                allInfoByID(fileName,a_id);
                bank_management(1);
            }
        }
        // Sleep(3000);
}
void Bank::deposit()
{
    system("clear");
    cout<<"\n\n\t\t\tDeposit Amount";
    if(!isFileOpen(fileName))
    {
        cout<<"\n\n File Not Found...";

    }else{
        int a_id;
        double amount;
        cout<<"\n\n Enter User ID : ";
        cin>>a_id;
        vector<Record> records = readCSV(fileName);
        if (!recordExists(a_id, records)) {
            cout<<"\n\n Record with ID " << a_id << " not found.";
            deposit();
        }else{
            cout<<"\n\n Enter Deposit Amount : ";
            cin>>amount;
            addBalance(fileName,a_id,amount);
            // go to bank management
            bank_management(1);
        }
    }
}
void Bank::withdraw()
{
   
    system("clear");
    cout<<"\n\n\t\t\tWithdraw Amount";
    if(!isFileOpen(fileName))
    {
        cout<<"\n\n File Not Found...";
        bank_management();
    }else{
        int a_id;
        double amount;
        cout<<"\n\n Enter User ID : ";
        cin>>a_id;
        vector<Record> records = readCSV(fileName);
        if (!recordExists(a_id, records)) {
            cout<<"\n\n Record with ID " << a_id << " not found.";
            withdraw();
        }else{
            cout<<"\n\n Enter Withdraw Amount : ";
            cin>>amount;
            if(withdrawBalance(fileName,a_id,amount))
            {
                cout << "\n\nWithdraw successfully to ID " << a_id << "!";
                bank_management(1);
            }else{
                cout << "\n\nWithdraw failed to ID " << a_id << "!";
                withdraw();
            }
        }
    }
}
void Bank::transfer()
{
    system("clear");
    cout<<"\n\n\t\t\tTransfer Amount";
    if(!isFileOpen(fileName))
    {
        cout<<"\n\n File Not Found...";
        transfer();
    }else{
        int from_id,to_id;
        double amount;
        cout<<"\n\n Enter From User ID : ";
        cin>>from_id;
        cout<<"\n\n Enter To User ID : ";
        cin>>to_id;
        vector<Record> records = readCSV(fileName);
        if (!recordExists(from_id, records)) {
            cout<<"\n\n Record with ID " << from_id << " not found.";
            transfer();
        }else if (!recordExists(to_id, records)) {
            cout<<"\n\n Record with ID " << to_id << " not found.";
            transfer();
        }else{
            cout<<"\n\n Enter Transfer Amount : ";
            cin>>amount;
            if(transferBalance(fileName,from_id,to_id,amount))
            {
                cout << "\n\nTransfer successfully from ID " << from_id << " to ID " << to_id << "!";
                bank_management(1);
            }else{
                cout << "\n\nTransfer failed from ID " << from_id << " to ID " << to_id << "!";
                transfer();
            }
        }
    }
}
void Bank::payment()
{
    // payment by user id and amount bill type
    // payment history
    // payment history by user id
    int user_id;
    string bill_type;
    double amount;
    system("clear");
    cout<<"\n\n\t\t\tBills Payment Option";
    cout<<"\n\n Enter User ID : ";
    cin>>user_id;
    cout<<"\n\n Enter Bill Type : ";
    cin>>bill_type;
    cout<<"\n\n Enter Amount : ";
    cin>>amount;
    vector<Record> records = readCSV(fileName);
    if (!recordExists(user_id, records)) {
        cout<<"\n\n\t\tRecord with ID " << user_id << " not found.";
        payment();
    }
    else
    {
        if(paymentBalance(fileName,user_id,amount,bill_type))
        {
            cout << "\n\n\t\t"<<bill_type<<" Bill Pay Successfully!!!...";
            bank_management(1);
        }else{
            cout << "\n\n\t\t"<<bill_type <<" Bill failed from ID " << user_id << "!";
            payment();
        }
    }

}
void Bank::search()
{
    system("clear");
    cout<<"\n\n\t\t\tSearch User Record";
    if(!isFileOpen(fileName))
    {
        cout<<"\n\n File Not Found...";
        search();
    }else{
        int a_id;
        cout<<"\n\n Enter User ID : ";
        cin>>a_id;
        vector<Record> records = readCSV(fileName);
        if (!recordExists(a_id, records)) {
            cout<<"\n\n\t\t User not found with ID " << a_id;
            search();
        }else{
            allInfoByID(fileName,a_id);
            bank_management(1);
        }
    }
}
void Bank::edit()
{
    system("clear");
    cout<<"\n\n\t\t\tEdit User Record";
    if(!isFileOpen(fileName))
    {
        cout<<"\n\n\t\t File Not Found...";
        edit();
    }else{
        int a_id;
        cout<<"\n\n Enter User ID : ";
        cin>>a_id;
        vector<Record> records = readCSV(fileName);
        if (!recordExists(a_id, records)) {
            cout<<"\n\n\t\t User not found with ID " << a_id;
            edit();
        }else{
            system("clear");
            allInfoByID(fileName,a_id);
            cout<<"\n\n\t\t Enter New Record";
            int choice;
            cout<<"\n\n 1. Name";
            cout<<"\n 2. Address";
            cout<<"\n 3. Pin";
            cout<<"\n 4. Password";
            cout<<"\n 5. Phone";
            cout<<"\n 6. Go Back";
            cout<<"\n\n Enter Your Choice : ";
            cin>>choice;
            int new_pin=0;
            string new_name="",new_pass="",new_address="",new_phone="";
            switch(choice)
            {
                case 1:
                    cout<<"\n\n\t\t Enter New Name : ";
                    cin.ignore();
                    getline (cin, new_name);
                    if(updateRecord(fileName,{a_id,new_name,new_pin,new_pass,new_address,new_phone}))
                    {
                        cout<<"\n\n\t\t Name Updated Successfully...";
                        bank_management(1);
                    }else{
                        cout<<"\n\n\t\t Name Updated Failed...";
                        edit();
                    }
                    break;
                case 2:
                    cout<<"\n\n\t\t Enter New Address : ";
                    cin.ignore();
                    getline (cin, new_address);
                    if(updateRecord(fileName,{a_id,new_name,new_pin,new_pass,new_address,new_phone}))
                    {
                        cout<<"\n\n\t\t Address Updated Successfully...";
                        bank_management(1);
                    }else{
                        cout<<"\n\n\t\t Address Updated Failed...";
                        edit();
                    }
                    break;
                case 3:
                    cout<<"\n\n\t\t Enter New Pin : ";
                    cin>>new_pin;
                    if(updateRecord(fileName,{a_id,new_name,new_pin,new_pass,new_address,new_phone}))
                    {
                        cout<<"\n\n\t\t PIN Updated Successfully...";
                        bank_management(1);
                    }else{
                        cout<<"\n\n\t\t PIN Updated Failed...";
                        edit();
                    }
                    break;
                case 4:
                    cout<<"\n\n\t\t Enter New Password : ";
                    cin>>new_pass;
                    if(updateRecord(fileName,{a_id,new_name,new_pin,new_pass,new_address,new_phone}))
                    {
                        cout<<"\n\n\t\t Password Updated Successfully...";
                        bank_management(1);
                    }else{
                        cout<<"\n\n\t\t Password Updated Failed...";
                        edit();
                    }
                    break;
                case 5:
                    cout<<"\n\n\t\t Enter New Phone : ";
                    cin>>new_phone;
                    if(updateRecord(fileName,{a_id,new_name,new_pin,new_pass,new_address,new_phone}))
                    {
                        cout<<"\n\n\t\t Phone Updated Successfully...";
                        bank_management(1);
                    }else{
                        cout<<"\n\n\t\t Phone Updated Failed...";
                        edit();
                    }
                    break;
                case 6:
                    bank_management();
                    break;
                default:
                    cout<<"\n\n\t\t Invalid Value...Please Try Again";
                    edit();
            }
        }
    }
}
void Bank::delete_user()
{
    system("clear");
    cout<<"\n\n\t\t\tDelete User Record";
    if(!isFileOpen(fileName))
    {
        cout<<"\n\n\t\t File Not Found...";
        delete_user();
    }else{
        int a_id;
        cout<<"\n\n Enter User ID : ";
        cin>>a_id;
        vector<Record> records = readCSV(fileName);
        if (!recordExists(a_id, records)) {
            cout<<"\n\n\t\t User not found with ID " << a_id;
            delete_user();
        }else{
            if(deleteRecord(fileName,a_id))
            {
                cout<<"\n\n\t\t User Deleted Successfully...";
                bank_management(1);
            }else{
                cout<<"\n\n\t\t User Deleted Failed...";
                delete_user();
            }
        }
    }
}
int main()
{
    Bank obj;
    obj.menu();
    return 0;
}
