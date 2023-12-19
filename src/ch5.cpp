#include <unordered_map>
#include <iostream>

namespace ch5 {
    class Logger {
    public:
        Logger() = default;

        virtual ~Logger() = default;

        virtual void transfer(long fromAccount, long toAccount, long long amount) const = 0;
    };

    class ConsoleLogger : public Logger {
    public:
        explicit ConsoleLogger(const char *name) : name{name} {}

        void transfer(const long fromAccount, const long toAccount, const long long amount) const override {
            std::cout << this->name << ": " <<
                      "transfer from account: " << fromAccount <<
                      " to account: " << toAccount <<
                      " amount: " << amount <<
                      std::endl;
        }

    private:
        const char *name;
    };

    class AccountDatabase {
    public:
        AccountDatabase() = default;

        virtual ~AccountDatabase() = default;

        [[nodiscard]] virtual long long getAmount(long account) const = 0;

        virtual void setAmount(long account, long long amount) = 0;
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
        explicit Bank(AccountDatabase &accountDatabase) : m_accountDatabase{accountDatabase} {}

        void setLogger(Logger *logger) {
            this->m_logger = logger;
        }

        void transfer(const long fromAccount, const long toAccount, const long long amount) {
            if (this->m_logger != nullptr) {
                this->m_logger->transfer(fromAccount, toAccount, amount);
            }

            auto fromAccountAmount = this->m_accountDatabase.getAmount(fromAccount);
            auto toAccountAmount = this->m_accountDatabase.getAmount(toAccount);

            this->m_accountDatabase.setAmount(fromAccount, fromAccountAmount - amount);
            this->m_accountDatabase.setAmount(toAccount, toAccountAmount + amount);
        }

    private:
        AccountDatabase &m_accountDatabase;
        Logger *m_logger{};
    };
}

int main() {
    ch5::InMemoryAccountDatabase accountDatabase;

    accountDatabase.setAmount(1, 100);
    accountDatabase.setAmount(2, 200);
    accountDatabase.setAmount(3, 300);

    ch5::Bank bank{accountDatabase};

    ch5::ConsoleLogger consoleLogger{"consoleLogger"};
    bank.setLogger(&consoleLogger);

    bank.transfer(1, 2, 50);

    std::cout << "account 1 amount: " << accountDatabase.getAmount(1) << std::endl;
    std::cout << "account 2 amount: " << accountDatabase.getAmount(2) << std::endl;
    std::cout << "account 3 amount: " << accountDatabase.getAmount(3) << std::endl;

    return 0;
}
