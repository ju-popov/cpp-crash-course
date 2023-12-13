#include <unordered_map>
#include <iostream>

class Logger {
public:
    Logger() = default;
    virtual ~Logger() = default;

    virtual void transfer(const long fromAccount, const long toAccount, const long long amount) const = 0;
};

class ConsoleLogger : public Logger {
public:
    explicit ConsoleLogger(const char* name) : name{name} {}

    void transfer(const long fromAccount, const long toAccount, const long long amount) const override {
        std::cout << this->name << ": " << 
            "transfer from account: " << fromAccount <<
            " to account: " << toAccount <<
            " amount: " << amount <<
            std::endl;
    }

private:
    const char* name;
};

class AccountDatabase {
public:
    AccountDatabase() = default;
    virtual ~AccountDatabase() = default;

    virtual long long getAmount(const long account) const = 0;
    virtual void setAmount(const long account, const long long amount) = 0;
};

class InMemoryAccountDatabase : public AccountDatabase {
public:
    long long getAmount(const long account) const override {
        auto it = this->accounts.find(account);
        if (it == this->accounts.end()) {
            return 0;
        }

        return it->second;
    }

    void setAmount(const long account, const long long amount) override {
        this->accounts[account] = amount;
    }

private:
    std::unordered_map<long, long long> accounts;
};

class Bank {
public:
    explicit Bank(AccountDatabase& accountDatabase) : accountDatabase{accountDatabase} {}

    void setLogger(Logger *logger) {
        this->logger = logger;
    }

    void transfer(const long fromAccount, const long toAccount, const long long amount) {
        if (this->logger != nullptr) {
            this->logger->transfer(fromAccount, toAccount, amount);
        }

        auto fromAccountAmount = this->accountDatabase.getAmount(fromAccount);
        auto toAccountAmount = this->accountDatabase.getAmount(toAccount);

        this->accountDatabase.setAmount(fromAccount, fromAccountAmount - amount);
        this->accountDatabase.setAmount(toAccount, toAccountAmount + amount);
    }

private:
    AccountDatabase& accountDatabase;
    Logger *logger{};
};
   
int main() {
    InMemoryAccountDatabase accountDatabase;

    accountDatabase.setAmount(1, 100);
    accountDatabase.setAmount(2, 200);
    accountDatabase.setAmount(3, 300);

    Bank bank{accountDatabase};

    ConsoleLogger consoleLogger{"consoleLogger"};
    bank.setLogger(&consoleLogger);

    bank.transfer(1, 2, 50);

    std::cout << "account 1 amount: " << accountDatabase.getAmount(1) << std::endl;
    std::cout << "account 2 amount: " << accountDatabase.getAmount(2) << std::endl;
    std::cout << "account 3 amount: " << accountDatabase.getAmount(3) << std::endl;

    return 0;
}
