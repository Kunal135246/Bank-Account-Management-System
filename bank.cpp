/*
 * ============================================================
 *  BANK ACCOUNT MANAGEMENT SYSTEM  (C++ Mini Project)
 * ============================================================
 *  Classes     : Customer, Transaction, Account, Bank
 *  Modules     : Transactions (deposit / withdraw / transfer)
 *                Reports      (summary / statement / bank report)
 *  OOP Concepts: Classes & Objects, Encapsulation
 *
 *  Compile : g++ -std=c++11 BankManagementSystem.cpp -o bank
 *  Run     : ./bank        (Windows: bank.exe)
 * ============================================================
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <limits>
#include <ctime>
using namespace std;

/* ------------------------------------------------------------
 *  Helper functions (input validation & formatting)
 * ------------------------------------------------------------ */
namespace Util {

    string currentTime() {
        time_t now = time(nullptr);
        char buf[32];
        strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", localtime(&now));
        return string(buf);
    }

    void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    int readInt(const string &prompt) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value) { clearInput(); return value; }
            cout << "  [!] Invalid input. Please enter a whole number.\n";
            clearInput();
        }
    }

    double readAmount(const string &prompt) {
        double value;
        while (true) {
            cout << prompt;
            if (cin >> value && value > 0) { clearInput(); return value; }
            cout << "  [!] Invalid amount. Enter a number greater than 0.\n";
            clearInput();
        }
    }

    string readLine(const string &prompt) {
        string text;
        while (true) {
            cout << prompt;
            getline(cin, text);
            if (!text.empty()) return text;
            cout << "  [!] This field cannot be empty.\n";
        }
    }

    void line(char c = '-', int n = 70) { cout << string(n, c) << "\n"; }

    void pause() {
        cout << "\nPress Enter to continue...";
        cin.get();
    }
}

/* ============================================================
 *  CLASS: Customer
 *  Stores personal details of the account holder.
 * ============================================================ */
class Customer {
private:
    int    customerId;
    string name;
    string phone;
    string address;

public:
    Customer() : customerId(0) {}

    Customer(int id, const string &n, const string &ph, const string &addr)
        : customerId(id), name(n), phone(ph), address(addr) {}

    // Getters
    int    getId()      const { return customerId; }
    string getName()    const { return name; }
    string getPhone()   const { return phone; }
    string getAddress() const { return address; }

    // Setters (with basic validation)
    void setPhone(const string &ph)     { if (!ph.empty())   phone = ph; }
    void setAddress(const string &addr) { if (!addr.empty()) address = addr; }

    void display() const {
        cout << "  Customer ID : " << customerId << "\n"
             << "  Name        : " << name        << "\n"
             << "  Phone       : " << phone       << "\n"
             << "  Address     : " << address     << "\n";
    }
};

/* ============================================================
 *  CLASS: Transaction
 *  A single record of money movement (immutable once created).
 * ============================================================ */
class Transaction {
private:
    int    transactionId;
    string type;          // DEPOSIT, WITHDRAWAL, TRANSFER-IN, TRANSFER-OUT
    double amount;
    double balanceAfter;
    string timestamp;

public:
    Transaction(int id, const string &t, double amt, double balAfter)
        : transactionId(id), type(t), amount(amt),
          balanceAfter(balAfter), timestamp(Util::currentTime()) {}

    int    getId()           const { return transactionId; }
    string getType()         const { return type; }
    double getAmount()       const { return amount; }
    double getBalanceAfter() const { return balanceAfter; }
    string getTimestamp()    const { return timestamp; }

    void display() const {
        cout << "  " << left  << setw(6)  << transactionId
             << setw(21) << timestamp
             << setw(14) << type
             << right << setw(12) << fixed << setprecision(2) << amount
             << setw(14) << balanceAfter << "\n";
    }
};

/* ============================================================
 *  CLASS: Account
 *  Holds balance and transaction history. The balance can only
 *  be changed through deposit() / withdraw() (Encapsulation).
 * ============================================================ */
class Account {
private:
    int                accountNumber;
    string             accountType;      // "Savings" or "Current"
    double             balance;
    Customer           holder;
    vector<Transaction> history;
    int                nextTxnId;

    static const double MIN_BALANCE_SAVINGS;   // must remain in savings account

    double minimumBalance() const {
        return (accountType == "Savings") ? MIN_BALANCE_SAVINGS : 0.0;
    }

    void record(const string &type, double amt) {
        history.push_back(Transaction(nextTxnId++, type, amt, balance));
    }

public:
    Account(int accNo, const string &type, const Customer &c, double openingBalance)
        : accountNumber(accNo), accountType(type), balance(openingBalance),
          holder(c), nextTxnId(1)
    {
        if (openingBalance > 0) record("OPENING", openingBalance);
    }

    // ---------- Getters ----------
    int      getAccountNumber() const { return accountNumber; }
    string   getAccountType()   const { return accountType; }
    double   getBalance()       const { return balance; }
    const Customer &getHolder() const { return holder; }
    const vector<Transaction> &getHistory() const { return history; }

    // ---------- Operations ----------
    bool deposit(double amount, const string &label = "DEPOSIT") {
        if (amount <= 0) return false;
        balance += amount;
        record(label, amount);
        return true;
    }

    bool withdraw(double amount, const string &label = "WITHDRAWAL") {
        if (amount <= 0) return false;
        if (balance - amount < minimumBalance()) return false;   // insufficient funds
        balance -= amount;
        record(label, amount);
        return true;
    }

    double availableToWithdraw() const { return balance - minimumBalance(); }

    // ---------- Reports ----------
    void displaySummary() const {
        Util::line('=');
        cout << "                     ACCOUNT SUMMARY\n";
        Util::line('=');
        cout << "  Account No  : " << accountNumber << "\n"
             << "  Account Type: " << accountType   << "\n";
        holder.display();
        cout << "  Balance     : Rs. " << fixed << setprecision(2) << balance << "\n"
             << "  Transactions: " << history.size() << "\n";
        Util::line('=');
    }

    void displayStatement() const {
        Util::line('=');
        cout << "         MINI STATEMENT  -  Account No: " << accountNumber << "\n";
        Util::line('=');
        if (history.empty()) {
            cout << "  No transactions found.\n";
        } else {
            cout << "  " << left << setw(6) << "ID" << setw(21) << "Date & Time"
                 << setw(14) << "Type" << right << setw(12) << "Amount"
                 << setw(14) << "Balance" << "\n";
            Util::line('-');
            for (size_t i = 0; i < history.size(); ++i) history[i].display();
        }
        Util::line('=');
    }
};
const double Account::MIN_BALANCE_SAVINGS = 500.0;

/* ============================================================
 *  CLASS: Bank
 *  Manages all accounts and exposes the Transactions and
 *  Reports modules.
 * ============================================================ */
class Bank {
private:
    string           bankName;
    vector<Account>  accounts;
    int              nextAccountNo;
    int              nextCustomerId;

    Account *findAccount(int accNo) {
        for (size_t i = 0; i < accounts.size(); ++i)
            if (accounts[i].getAccountNumber() == accNo) return &accounts[i];
        return nullptr;
    }

public:
    Bank(const string &name)
        : bankName(name), nextAccountNo(1001), nextCustomerId(1) {}

    string getName() const { return bankName; }

    /* ------------------- Account management ------------------- */
    void createAccount() {
        cout << "\n--- CREATE NEW ACCOUNT ---\n";
        string name    = Util::readLine("Customer name      : ");
        string phone   = Util::readLine("Phone number       : ");
        string address = Util::readLine("Address            : ");

        int choice;
        do {
            choice = Util::readInt("Account type (1.Savings  2.Current): ");
        } while (choice != 1 && choice != 2);
        string type = (choice == 1) ? "Savings" : "Current";

        double minOpen = (choice == 1) ? 500.0 : 0.0;
        double opening;
        while (true) {
            cout << "Opening deposit (min Rs. " << minOpen << "): ";
            if (cin >> opening && opening >= minOpen) { Util::clearInput(); break; }
            cout << "  [!] Invalid opening amount.\n";
            Util::clearInput();
        }

        Customer c(nextCustomerId++, name, phone, address);
        accounts.push_back(Account(nextAccountNo, type, c, opening));
        cout << "\n  [OK] Account created successfully! Account Number: "
             << nextAccountNo << "\n";
        nextAccountNo++;
    }

    void closeAccount() {
        cout << "\n--- CLOSE ACCOUNT ---\n";
        int accNo = Util::readInt("Enter account number: ");
        for (size_t i = 0; i < accounts.size(); ++i) {
            if (accounts[i].getAccountNumber() == accNo) {
                if (accounts[i].getBalance() > 0)
                    cout << "  Note: Rs. " << fixed << setprecision(2)
                         << accounts[i].getBalance() << " will be paid out to the customer.\n";
                accounts.erase(accounts.begin() + i);
                cout << "  [OK] Account " << accNo << " closed.\n";
                return;
            }
        }
        cout << "  [!] Account not found.\n";
    }

    /* ------------------- MODULE 1: Transactions ------------------- */
    void deposit() {
        cout << "\n--- DEPOSIT ---\n";
        Account *acc = findAccount(Util::readInt("Enter account number: "));
        if (!acc) { cout << "  [!] Account not found.\n"; return; }

        double amt = Util::readAmount("Amount to deposit: Rs. ");
        if (acc->deposit(amt))
            cout << "  [OK] Deposited Rs. " << fixed << setprecision(2) << amt
                 << ". New balance: Rs. " << acc->getBalance() << "\n";
        else
            cout << "  [!] Deposit failed.\n";
    }

    void withdraw() {
        cout << "\n--- WITHDRAW ---\n";
        Account *acc = findAccount(Util::readInt("Enter account number: "));
        if (!acc) { cout << "  [!] Account not found.\n"; return; }

        double amt = Util::readAmount("Amount to withdraw: Rs. ");
        if (acc->withdraw(amt))
            cout << "  [OK] Withdrawn Rs. " << fixed << setprecision(2) << amt
                 << ". New balance: Rs. " << acc->getBalance() << "\n";
        else
            cout << "  [!] Insufficient funds. You can withdraw up to Rs. "
                 << fixed << setprecision(2) << acc->availableToWithdraw() << "\n";
    }

    void transfer() {
        cout << "\n--- FUND TRANSFER ---\n";
        Account *from = findAccount(Util::readInt("From account number: "));
        if (!from) { cout << "  [!] Source account not found.\n"; return; }
        Account *to = findAccount(Util::readInt("To account number  : "));
        if (!to)   { cout << "  [!] Destination account not found.\n"; return; }
        if (from == to) { cout << "  [!] Cannot transfer to the same account.\n"; return; }

        double amt = Util::readAmount("Amount to transfer: Rs. ");
        if (from->withdraw(amt, "TRANSFER-OUT")) {
            to->deposit(amt, "TRANSFER-IN");
            cout << "  [OK] Rs. " << fixed << setprecision(2) << amt
                 << " transferred from " << from->getAccountNumber()
                 << " to " << to->getAccountNumber() << ".\n";
        } else {
            cout << "  [!] Transfer failed: insufficient funds. Available: Rs. "
                 << fixed << setprecision(2) << from->availableToWithdraw() << "\n";
        }
    }

    /* ------------------- MODULE 2: Reports ------------------- */
    void accountSummary() {
        cout << "\n--- ACCOUNT SUMMARY ---\n";
        Account *acc = findAccount(Util::readInt("Enter account number: "));
        if (acc) acc->displaySummary();
        else     cout << "  [!] Account not found.\n";
    }

    void miniStatement() {
        cout << "\n--- TRANSACTION STATEMENT ---\n";
        Account *acc = findAccount(Util::readInt("Enter account number: "));
        if (acc) acc->displayStatement();
        else     cout << "  [!] Account not found.\n";
    }

    void allAccountsReport() const {
        cout << "\n";
        Util::line('=');
        cout << "              " << bankName << " - ALL ACCOUNTS REPORT\n";
        Util::line('=');
        if (accounts.empty()) {
            cout << "  No accounts available.\n";
            Util::line('=');
            return;
        }
        cout << "  " << left << setw(10) << "Acc No" << setw(22) << "Holder"
             << setw(10) << "Type" << right << setw(16) << "Balance (Rs.)" << "\n";
        Util::line('-');
        double total = 0;
        for (size_t i = 0; i < accounts.size(); ++i) {
            const Account &a = accounts[i];
            cout << "  " << left << setw(10) << a.getAccountNumber()
                 << setw(22) << a.getHolder().getName()
                 << setw(10) << a.getAccountType()
                 << right << setw(16) << fixed << setprecision(2) << a.getBalance() << "\n";
            total += a.getBalance();
        }
        Util::line('-');
        cout << "  Total accounts: " << accounts.size()
             << "    Total deposits held: Rs. " << fixed << setprecision(2) << total << "\n";
        Util::line('=');
    }
};

/* ============================================================
 *  MAIN - Menu driven interface
 * ============================================================ */
void showMenu(const string &bankName) {
    cout << "\n";
    Util::line('=');
    cout << "        " << bankName << " - BANK MANAGEMENT SYSTEM\n";
    Util::line('=');
    cout << "  ACCOUNTS\n"
         << "    1. Create New Account\n"
         << "    2. Close Account\n"
         << "  TRANSACTIONS\n"
         << "    3. Deposit\n"
         << "    4. Withdraw\n"
         << "    5. Fund Transfer\n"
         << "  REPORTS\n"
         << "    6. Account Summary\n"
         << "    7. Transaction Statement\n"
         << "    8. All Accounts Report\n"
         << "    0. Exit\n";
    Util::line('-');
}

int main() {
    Bank bank("SBM BANK");
    int choice;

    do {
        showMenu(bank.getName());
        choice = Util::readInt("Enter your choice: ");

        switch (choice) {
            case 1: bank.createAccount();     break;
            case 2: bank.closeAccount();      break;
            case 3: bank.deposit();           break;
            case 4: bank.withdraw();          break;
            case 5: bank.transfer();          break;
            case 6: bank.accountSummary();    break;
            case 7: bank.miniStatement();     break;
            case 8: bank.allAccountsReport(); break;
            case 0: cout << "\nThank you for using " << bank.getName() << ". Goodbye!\n"; break;
            default: cout << "  [!] Invalid choice. Try again.\n";
        }
        if (choice != 0) Util::pause();
    } while (choice != 0);

    return 0;
}