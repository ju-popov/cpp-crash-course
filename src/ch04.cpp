#include <iostream>
#include <chrono>
#include <thread>

namespace ch4 {
    struct TimerClass {
        explicit TimerClass(const char *name) : name{name}, timestamp{getCurrentTimestamp()} {
            std::cout << this->name << ": TimerClass() timestamp: " << timestamp << std::endl;
        }

        ~TimerClass() {
            if (this->timestamp == 0) {
                return;
            }

            long long age = getCurrentTimestamp() - this->timestamp;

            std::cout << this->name << ": ~TimerClass() timestamp: " << this->timestamp << " age: " << age << std::endl;
        }

        TimerClass(const TimerClass &other) : name{other.name}, timestamp{other.timestamp} {}

        TimerClass &operator=(const TimerClass &other) {
            if (this == &other) {
                return *this;
            }

            this->name = other.name;
            this->timestamp = other.timestamp;

            return *this;
        }

        TimerClass(TimerClass &&other) noexcept: name{other.name}, timestamp{other.timestamp} {
            other.timestamp = 0;
            other.name = nullptr;
        }

        TimerClass &operator=(TimerClass &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            this->name = other.name;
            this->timestamp = other.timestamp;

            other.name = nullptr;
            other.timestamp = 0;

            return *this;
        }

    private:
        const char *name;
        long long timestamp;

        static long long getCurrentTimestamp() {
            auto now = std::chrono::system_clock::now();
            auto duration = now.time_since_epoch();
            return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        }
    };
}

int main() {
    ch4::TimerClass timerOne{"one"};
    ch4::TimerClass timerOneCopyConstructor{timerOne};
    ch4::TimerClass timerOneAssignmentOperator = timerOne;

    ch4::TimerClass timerTwo{"two"};
    ch4::TimerClass timerTwoMoveConstructor{std::move(timerTwo)};

    ch4::TimerClass timerThree{"three"};
    ch4::TimerClass timerThreeMoveAssignmentOperator = std::move(timerThree);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}
