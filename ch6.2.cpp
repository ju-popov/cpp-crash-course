#include <unordered_map>
#include <iostream>

template<typename T>
class Bank {
public:
    void transfer(T& fromAccount, T& toAccount, const long long amount) {
        std::cout << 
            "transfer from account: " << fromAccount.getType() << "/" << fromAccount.getName() << 
            " to account: " << toAccount.getType() << "/" << toAccount.getName() <<
            " amount: " << amount <<
            std::endl;

        fromAccount.setAmount(fromAccount.getAmount() - amount);
        toAccount.setAmount(toAccount.getAmount() + amount);
    }
};

class Account {
public:
    virtual ~Account() = default;

    virtual long long getAmount() const = 0;
    virtual void setAmount(const long long amount) = 0;
    virtual const char* getType() const = 0;
    virtual const char* getName() const = 0;
};

class CheckingAccount : public Account {
public:
    explicit CheckingAccount(const char* name) : name{name}, amount{0} {}

    long long getAmount() const override {
        return this->amount;
    };

    void setAmount(const long long amount) override {
        this->amount = amount;
    };

    const char* getType() const override {
        return "checking";
    };

    const char* getName() const override {
        return this->name;
    };

private:
    const char* name;
    long long amount;
};

class SavingAccount : public Account {
public:
    explicit SavingAccount(const char* name) : name{name}, amount{0} {}

    long long getAmount() const override {
        return this->amount;
    };

    void setAmount(const long long amount) override {
        this->amount = amount;
    };

    const char* getType() const override {
        return "saving";
    };

    const char* getName() const override {
        return this->name;
    };

private:
    const char* name;
    long long amount;
};

int main() {
    CheckingAccount account1{"one"};
    SavingAccount account2{"two"};
    CheckingAccount account3{"three"};

    account1.setAmount(100);
    account2.setAmount(200);
    account3.setAmount(300);

    Bank<Account> bank{};

    bank.transfer(account1, account2, 50);

    std::cout << "account: " << account1.getType() << "/" << account1.getName() << " amount: " << account1.getAmount() << std::endl;
    std::cout << "account: " << account2.getType() << "/" << account2.getName() << " amount: " << account2.getAmount() << std::endl;
    std::cout << "account: " << account3.getType() << "/" << account3.getName() << " amount: " << account3.getAmount() << std::endl;

    return 0;
}
