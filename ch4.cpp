#include <iostream>
#include <vector>
#include <string>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

struct TimerClass {
    TimerClass(const char* name) : name{name} {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        long long timestamp = tv.tv_sec * 1000LL + tv.tv_usec / 1000;

        this->timestamp = timestamp;

        cout << this->name << ": TimerClass() timestamp: " << timestamp << endl;
    }

    ~TimerClass() {
        if (this->timestamp == 0) {
            return;
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);

        long long timestamp = tv.tv_sec * 1000LL + tv.tv_usec / 1000;
        long long age = timestamp - this->timestamp;

        cout << this->name << ": ~TimerClass() timestamp: " << timestamp << " age: " << age << endl;
    }

    TimerClass(const TimerClass& other) {
        this->name = other.name;
        this->timestamp = other.timestamp;
    }

    TimerClass& operator=(const TimerClass& other) {
        if (this == &other) {
            return *this;

        }

        this->name = other.name;
        this->timestamp = other.timestamp;

        return *this;
    }

    TimerClass(TimerClass&& other) noexcept{
        this->name = other.name;
        this->timestamp = other.timestamp;

        other.timestamp = 0;
        other.name = nullptr;
    }

    TimerClass& operator=(TimerClass&& other) noexcept{
        if (this == &other) {
            return *this;
        }

        this->name = other.name;
        this->timestamp = other.timestamp;

        other.timestamp = 0;
        other.name = nullptr;

        return *this;
    }

    private:
        const char* name;
        long long timestamp;
};

int main()
{
    TimerClass timerOne{"one"};
    TimerClass timerOneCopyConstructor{timerOne};
    TimerClass timerOneAssignmentOperator = timerOne;

    TimerClass timerTwo{"two"};
    TimerClass timerTwoMoveConstructor{move(timerTwo)};

    TimerClass timerThree{"three"};
    TimerClass timerThreeMoveAssignmentOperator = move(timerThree);

    usleep(10000);
}

