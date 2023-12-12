#include <unordered_map>
#include <iostream>

class AccountDatabase {
public:
    AccountDatabase() = default;
    virtual ~AccountDatabase() = default;

    virtual long long getAmount(const long accountId) const = 0;
    virtual void setAmount(const long accountId, const long long amount) = 0;
};

class InMemoryAccountDatabase : public AccountDatabase {
public:
    long long getAmount(const long accountId) const override {
        auto it = this->accounts.find(accountId);
        if (it == this->accounts.end()) {
            return 0;
        }

        return it->second;
    }

    void setAmount(const long accountId, const long long amount) override {
        this->accounts[accountId] = amount;
    }

private:
    std::unordered_map<long, long long> accounts;
};

class Logger {
public:
    Logger() = default;
    virtual ~Logger() = default;

    virtual void transfer(const long fromAccountId, const long toAccountId, const long long amount) const = 0;
};

class ConsoleLogger : public Logger {
public:
    ConsoleLogger(const char* name) : name{name} {}

    void transfer(const long fromAccountId, const long toAccountId, const long long amount) const override {
        std::cout << this->name << ": " << 
            "transfer from account: " << fromAccountId <<
            " to account: " << toAccountId <<
            " amount: " << amount <<
            std::endl;
    }

private:
    const char* name;
};

class Bank {
public:
    Bank(AccountDatabase& accountDatabase) : accountDatabase{accountDatabase} {}

    void setLogger(Logger *logger) {
        this->logger = logger;
    }

    void transfer(const long fromAccountId, const long toAccountId, const long long amount) {
        if (this->logger != nullptr) {
            this->logger->transfer(fromAccountId, toAccountId, amount);
        }

        auto fromAccountAmount = this->accountDatabase.getAmount(fromAccountId);
        auto toAccountAmount = this->accountDatabase.getAmount(toAccountId);

        this->accountDatabase.setAmount(fromAccountId, fromAccountAmount - amount);
        this->accountDatabase.setAmount(toAccountId, toAccountAmount + amount);
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