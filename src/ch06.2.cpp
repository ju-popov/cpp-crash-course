#include <iostream>

namespace ch06_2 {
    template<typename T>
    class Bank {
    public:
        void transfer(T &fromAccount, T &toAccount, const long long amount) {
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

        [[nodiscard]] virtual long long getAmount() const = 0;

        virtual void setAmount(long long amount) = 0;

        [[nodiscard]] virtual const char *getType() const = 0;

        [[nodiscard]] virtual const char *getName() const = 0;
    };

    class CheckingAccount : public Account {
    public:
        explicit CheckingAccount(const char *name) : m_name{name}, m_amount{0} {}

        [[nodiscard]] long long getAmount() const override {
            return this->m_amount;
        };

        void setAmount(const long long amount) override {
            this->m_amount = amount;
        };

        [[nodiscard]] const char *getType() const override {
            return "checking";
        };

        [[nodiscard]] const char *getName() const override {
            return this->m_name;
        };

    private:
        const char *m_name;
        long long m_amount;
    };

    class SavingAccount : public Account {
    public:
        explicit SavingAccount(const char *name) : m_name{name}, m_amount{0} {}

        [[nodiscard]] long long getAmount() const override {
            return this->m_amount;
        };

        void setAmount(const long long amount) override {
            this->m_amount = amount;
        };

        [[nodiscard]] const char *getType() const override {
            return "saving";
        };

        [[nodiscard]] const char *getName() const override {
            return this->m_name;
        };

    private:
        const char *m_name;
        long long m_amount;
    };
}

int main() {
    ch06_2::CheckingAccount account1{"one"};
    ch06_2::SavingAccount account2{"two"};
    ch06_2::CheckingAccount account3{"three"};

    account1.setAmount(100);
    account2.setAmount(200);
    account3.setAmount(300);

    ch06_2::Bank<ch06_2::Account> bank{};

    bank.transfer(account1, account2, 50);

    std::cout << "account: " << account1.getType() << "/" << account1.getName() << " amount: " << account1.getAmount()
              << std::endl;
    std::cout << "account: " << account2.getType() << "/" << account2.getName() << " amount: " << account2.getAmount()
              << std::endl;
    std::cout << "account: " << account3.getType() << "/" << account3.getName() << " amount: " << account3.getAmount()
              << std::endl;

    return 0;
}
