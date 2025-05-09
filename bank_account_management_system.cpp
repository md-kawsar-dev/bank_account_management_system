#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <string>
#include <unistd.h>
// #include <termios.h>

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
char ch;
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
bool recordExists(const int& id, const vector<Record>& records, const string& pin="") {
    for (const auto& record : records) {
        if (record.id == id && (pin=="" || record.pin==stoi(pin))) {
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
        // time_t now = time(0);
        // char* dt = ctime(&now);
        // Get current time
        time_t now = time(0);
        tm *ltm = localtime(&now);

        // Format date as DD-MM-YYYY
        char date[11]; // Buffer for formatted date
        strftime(date, sizeof(date), "%d-%m-%Y", ltm);
        file << id << "," << amount << "," << type << ","<<remarks<<","<< date;
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
    if(found)cout<<"Account No :"<<record.id <<"  Name :"<<record.name<<"  Pin :"<<record.pin<<"  Password :"<<record.pass<<"  Address :"<<record.address<<"  Phone :"<<record.phone<<"   Balance :"<<record.balance<<endl;
    if(!found)cout << "Record with Account No " << id << " not found." << endl;

}
// Function to add a new record
void addRecord(const string& filename, const Record& record) {

    ofstream file(filename, ios::app);
    file << record.id << "," << record.name << "," << record.pin<<","<< record.pass<<","<< record.address<<","<< record.phone<<","<<record.balance << "\n";
    file.close();
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
        string pass,name,fname,address,phone,fileName="bank.csv",pinCode;

    public:
        void menu();
        void bank_management(int cal_fun=0);
        void atm_management();
        void new_account();
        void existing_account();
        void deposit(string from,int id=0,double amount=0);
        void withdraw(string from="",int id=0,double amount=0);
        void transfer(string from="",int id=0,double amount=0,int to_id=0);
        void payment(int user_id=0,string bill_type="",double amount=0,string form="");
        void search();
        void edit();
        void delete_user();
        void show_all_records();
        void payment_all_records(int cal_fun=0);

};
void Bank::menu()
{
    system("clear");
    int choice;
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
                {
                    system("clear");
                    cout<<"\n\n\t\t\tATM Management System";
                    cout<<"\n\n\t\t\tLogin Account";
                    cout<<"\n\n Account No. : ";
                    cin>>id;
                    cout<<"\n\n\t\t Pin Code : ";
                    for(int i=1;i<=5;i++)
                    {
                        ch = getch();
                        pinCode +=ch;
                        cout<<"*";
                    }
                    vector<Record> records = readCSV(fileName);
                    if(!recordExists(id, records, pin)) {
                        cout << "\n\n\t\t Account No or Pin Code is wrong...";
                        sleep(2);
                    }else{
                        atm_management();
                    }
                    break;
                }
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
void callatm_bank(string from)
{
    Bank b;
    if(from=="atm")
    {
        b.atm_management();
    }else{
        b.bank_management();
    }
}
void callReturn(string from)
{
    Bank b;
    // if press enter go to atm management
    cout<<"\n\n Press Enter to go back...";
    cin.ignore();
    cin.get(ch);
    if(ch=='\n')
    {
        if(from=="atm")
        {
            b.atm_management();
        }else{
            b.bank_management(1);
        }
    }
    else
    {
        cout<<"\n\n Invalid Value...Please Try Again";
        if(from=="atm")
        {
            b.atm_management();
        }else{
            b.bank_management(1);
        }
    }
    
}
void Bank::bank_management(int cal_fun)
{

    system("clear");
    int choice;
    bool flag = true;
    cout<<"\n\n\t\t\tBank Management System";
    cout<<"\n\n 1. New Account";
    cout<<"\n 2. Existing Account";
    cout<<"\n 3. Deposit Option";
    cout<<"\n 4. Withdraw Option";
    cout<<"\n 5. Transfer Option";
    cout<<"\n 6. Payment Option";
    cout<<"\n 7. Search Account Record";
    cout<<"\n 8. Edit Account Record";
    cout<<"\n 9. Delete Account Record";
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
                new_account();
                break;
            case 2:
                existing_account();
                break;
            case 3:
                {
                    system("clear");
                    cout<<"\n\n\t\t\tDeposit Amount";
                    if(!isFileOpen(fileName))
                    {
                        cout<<"\n\n File Not Found...";
                        sleep(2);
                        callReturn("bank");
                    }else{
                        int a_id;
                        double amount;
                        cout<<"\n\n Enter Account No : ";
                        cin>>a_id;
                        vector<Record> records = readCSV(fileName);
                        if (!recordExists(a_id, records)) {
                            cout<<"\n\n Record with Account No " << a_id << " not found.";
                            sleep(2);
                            callReturn("bank");
                        }else{
                            cout<<"\n\n Enter Deposit Amount : ";
                            cin>>amount;
                            deposit("bank",a_id,amount);
                            cout<<"\n\n\t\t Thank You... \n\n"; 
                            sleep(2);
                            callReturn("bank");
                        }
                    }
                    break;
                }
            case 4:
                {
                    system("clear");
                    cout<<"\n\n\t\t\tWithdraw Amount";
                    if(!isFileOpen(fileName))
                    {
                        cout<<"\n\n File Not Found...";
                        sleep(2);
                        callReturn("bank");
                    }else{
                        int a_id;
                        double amount;
                        cout<<"\n\n Enter Account No : ";
                        cin>>a_id;
                        vector<Record> records = readCSV(fileName);
                        if (!recordExists(a_id, records)) {
                            cout<<"\n\n Record with Account No " << a_id << " not found.";
                            sleep(2);
                            callReturn("bank");
                        }else{
                            cout<<"\n\n Enter Withdraw Amount : ";
                            cin>>amount;
                            if(withdrawBalance(fileName,a_id,amount))
                            {
                                cout << "\n\nWithdraw successfully to Account No " << a_id << "!";
                                sleep(2);
                                callReturn("bank");
                            }else{
                                cout << "\n\nWithdraw failed to Account No " << a_id << "!";
                                sleep(2);
                                callReturn("bank");
                               
                            }
                        }
                    }
                    break;
                }
            case 5:
                {
                    system("clear");
                    cout<<"\n\n\t\t\tTransfer Amount";
                    if(!isFileOpen(fileName))
                    {
                        cout<<"\n\n File Not Found...";
                        sleep(2);
                        callatm_bank("bank");
                    }else{
                        int from_id,to_id;
                        double amount;
                        cout<<"\n\n Enter From Account No : ";
                        cin>>from_id;
                        cout<<"\n\n Enter To Account No : ";
                        cin>>to_id;
                        cout<<"\n\n Enter Transfer Amount : ";
                        cin>>amount;
                        transfer("bank",from_id,amount,to_id);
                        
                    }
                    break;
                }
            case 6:
                {
                    
                    system("clear");
                    int user_id;
                    string bill_type;
                    double amount;
                    cout<<"\n\n\t\t\tBills Payment Option";
                    cout<<"\n\n Enter Account No : ";
                    cin>>user_id;
                    cout<<"\n\n Enter Bill Type : ";
                    cin>>bill_type;
                    cout<<"\n\n Enter Amount : ";
                    cin>>amount;
                    payment(user_id, bill_type, amount,"bank");
                    break;
                }
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
                show_all_records();
                break;
            case 11:
                payment_all_records();
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
 
    system("clear");
    int choice;
    cout<<"\n\n\t\t\tATM Management System";
    cout<<"\n\n 1. User Login & Check Balance";
    cout<<"\n 2. Withdraw Amount";
    cout<<"\n 3. Account Detailes";
    cout<<"\n 4. Deposit Amount"; // Uncommented the Deposit Amount option
    cout<<"\n 5. Transfer Amount"; // Uncommented the Transfer Amount option
    cout<<"\n 6. Go Back";
    cout<<"\n\n Enter Your Choice : ";
    cin>>choice;
    switch(choice)
    {
        case 1: {
            system("clear");
            vector<Record> records = readCSV(fileName);
            float balance =0;
            for (const auto& record : records) {
                if (record.id == id) {
                    balance = record.balance;
                    break;
                }
            }
            cout << "\n\n\t\t Current Balance : " << balance;
            sleep(3);
            atm_management();
            break;
        }
        case 2:
            system("clear");
            double amount;
            cout<<"\n\n Enter Withdraw Amount : ";
            cin>>amount;
            withdraw("atm",id,amount);
            break;
        case 3:
            {
                system("clear");
                cout<<"\n\n\t\t\tAccount Detailes";
                vector<Record> records = readCSV(fileName);
                
                for (const auto& record : records) {
                    if (record.id == id) {
                        cout << "\n\nAccount No :"<<record.id <<"  Name :"<<record.name<<"  Pin :"<<record.pin<<"  Password :"<<record.pass<<"  Address :"<<record.address<<"  Phone :"<<record.phone<<"   Balance :"<<record.balance<<endl;
                        break;
                    }
                }
                cout<<"\n\n\t\t Thank You... \n\n";
                sleep(2);
                callReturn("atm");
                break;
            }
        case 4:
            {
                system("clear");
                double amount;
                cout<<"\n\n Enter Deposit Amount : ";
                cin>>amount;
                deposit("atm", id, amount);
                cout<<"\n\n\t\t Thank You... \n\n";
                sleep(2);
                // if press enter go to atm management
                callReturn("atm");
                break;
            }
        case 5:
            {
                system("clear");
                double amount;
                cout<<"\n\n Enter Transfer Amount : ";
                cin>>amount;
                cout<<"\n\n Enter To Account No : ";
                int to_id;
                cin >> to_id; // Added input for to_id
                transfer("atm", id, amount, to_id); // Uncommented transfer function with to_id
                cout<<"\n\n\t\t Thank You... \n\n";
                sleep(2);
                // if press enter go to atm management
                callReturn("atm");
                break;
            }
        case 6:
            menu();
            break;
        default:
            cout<<"\n\n Invalid Value...Please Try Again";
    }
    
}
void Bank::new_account()
{

    system("clear");
    cout<<"\n\n\t\t\tAdd New Account";
    cout<<"\n\n Account No : ";
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
        cout<<"\n\n Account No Already Exist...";
        sleep(2);
        new_account();
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
        cout<<"\n\n\t\t Account Added Successfully...";
        sleep(2);
        callReturn("bank");

    }
    

}
void Bank::existing_account()
{
        system("clear");
        cout<<"\n\n\t\t\tExisting Account";
        if(!isFileOpen(fileName))
        {
            cout<<"\n\n File Not Found...";
            sleep(2);
            existing_account();

        }else{
            int a_id;
            cout<<"\n\n Enter Account No : ";
            cin>>a_id;
            vector<Record> records = readCSV(fileName);
            if (!recordExists(a_id, records)) {
                cout<<"\n\n Record with Account No " << a_id << " not found.";
                sleep(2);
                existing_account();
            }else{

                allInfoByID(fileName,a_id);
                callReturn("bank");
            }
        }
        // Sleep(3000);
}
void Bank::deposit(string from,int id,double amount)
{
    system("clear");
    addBalance(fileName,id,amount);
    
}
void Bank::withdraw(string from,int id,double amount)
{

    system("clear");
    if(withdrawBalance(fileName,id,amount))
    {
        cout << "\n\nWithdraw successfully to ID " << id << "!";
        if(from=="atm")
        {
            cout<<"\n\n\t\t Thank You... \n\n";
            sleep(2);
            atm_management();
        }else{
            cout<<"\n\n\t\t Thank You... \n\n";
            sleep(2);
            bank_management(1);
        }
    }else{
        cout << "\n\nWithdraw failed to ID " << id << "!";
        sleep(2);
        cout<<"\n\n Press Enter to go back...";
        cin.ignore();
        cin.get(ch);
        if(from=="atm")
        {
            if(ch=='\n')
            {
                atm_management();
            }
            else
            {
                cout<<"\n\n Invalid Value...Please Try Again";
                atm_management();
            }
        }else{
            if(ch=='\n')
            {
                bank_management(1);
            }
            else
            {
                cout<<"\n\n Invalid Value...Please Try Again";
                bank_management(1);
            }
        }
    }
}
void Bank::transfer(string from,int id,double amount,int to_id)
{
    system("clear");
    cout<<"\n\n\t\t\tTransfer Amount";
    if(!isFileOpen(fileName))
    {
        cout<<"\n\n File Not Found...";
        sleep(2);
        callReturn(from);
    }else{
       
        vector<Record> records = readCSV(fileName);
        if (!recordExists(id, records)) {
            cout<<"\n\n Record with From Account No " << id << " not found.";
            sleep(2);
            callReturn(from);
        }else if (!recordExists(to_id, records)) {
            cout<<"\n\n Record with To Account No " << to_id << " not found.";
            sleep(2);
            callReturn(from);
        }else{
            if(transferBalance(fileName,id,to_id,amount)) // corrected from from_id to id
            {
                cout << "\n\nTransfer successfully from Account No " << id << " to Account No " << to_id << " Amount : " << amount << "!";
                sleep(2);
                callReturn(from);
            }else{
                cout << "\n\nTransfer failed from Account No " << id << " to Account No " << to_id << "!";
                sleep(2);
                callReturn(from);
            }
        }
    }
}
void Bank::payment(int user_id,string bill_type,double amount,string from)
{
    
    vector<Record> records = readCSV(fileName);
    if (!recordExists(user_id, records)) {
        cout<<"\n\n\t\tRecord with Account No " << user_id << " not found.";
        sleep(2);
        callReturn(from);
    }
    else
    {
        if(paymentBalance(fileName,user_id,amount,bill_type))
        {
            cout << "\n\n\t\t"<<bill_type<<" Bill Pay Successfully!!!...";
            sleep(2);
            callReturn(from);
        }else{
            cout << "\n\n\t\t"<<bill_type <<" Bill failed from Account No " << user_id << "!";
            sleep(2);
            callReturn(from);
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
        cout<<"\n\n Enter Account No : ";
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
        cout<<"\n\n Enter Account No : ";
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
        cout<<"\n\n Enter Account No : ";
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
void Bank::show_all_records()
{
    system("clear");
    cout<<"\n\n\t\t\tAll User Records";
    vector<Record> records = readCSV(fileName);
    if(records.size() ==0)
    {
        cout<<"\n\n\t\t No Record Found...";
        bank_management();
    }else{
        for (const auto& record : records) {
            cout << "\n\nID :"<<record.id <<"  Name :"<<record.name<<"  Pin :"<<record.pin<<"  Password :"<<record.pass<<"  Address :"<<record.address<<"  Phone :"<<record.phone<<"   Balance :"<<record.balance<<endl;
        }
        bank_management(1);
    }
}

void Bank::payment_all_records(int cal_fun)
{
    if(cal_fun==0)
    {
        system("clear");
    }
    // payment history by Account No
    cout<<"\n\n\t Please Enter Account No : ";
    int user_id;
    cin>>user_id;
    // check Account No exist or not
    vector<Record> records = readCSV(fileName);
    if (!recordExists(user_id, records)) {
        cout<<"\n\n\t\t User not found with ID " << user_id;
        payment_all_records(1);
    }else{
        system("clear");
        cout<<"\n\n\t\t\tPayment History By Account No : "<<user_id<<endl;
        ifstream file("history.csv");
        if(!file)
        {
            cout<<"\n\n File Not Found...";
            bank_management(1);
        }else{
            string line;
            while (getline(file, line))
            {
                if(line.find(to_string(user_id)) != string::npos)
                {
                    // show only type and balance and date format d-m-y
                    // cout<<line<<endl;
                    // cout<<line<<endl;
                    stringstream ss(line);
                    string id, type, balance, date,remarks;

                    // Assuming data format: user_id,type,balance,date (YYYY-MM-DD)
                    getline(ss, id, ',');
                    getline(ss, balance, ',');
                    getline(ss, type, ',');
                    getline(ss, remarks, ',');
                    getline(ss, date, ',');
                    // Output only required fields
                    cout << "Type: " << type << ", Balance: " << balance << ", Date: " << date << endl;
                }
            }
            file.close();
            bank_management(1);
        }
    }
}
int main()
{
    Bank obj;
    obj.menu();
    return 0;
}
