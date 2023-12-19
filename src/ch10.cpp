#include <iostream>
#include <ostream>

namespace ch10 {
    struct SpeedUpdate {
        double velocity_mps;
    };

    struct CarDetected {
        double distance_m;
        double velocity_mps;
    };

    struct BrakeCommand {
        double time_to_collision_s;
    };

    template <typename T>
    class AutoBrake {
    public:
        explicit AutoBrake(const T& publish) : m_publish{publish}, m_velocity_mps{0.0}, m_collision_threshold_s{5.0} {}

        void observe(const SpeedUpdate& update) {
            m_velocity_mps = update.velocity_mps;
        }

        void observe(const CarDetected& update) {
            const auto relative_velocity_mps = m_velocity_mps - update.velocity_mps;
            const auto time_to_collision_s = update.distance_m / relative_velocity_mps;

            if (time_to_collision_s > 0.0 && time_to_collision_s <= m_collision_threshold_s) {
                m_publish(BrakeCommand{time_to_collision_s});
            }
        }

        void set_collision_threshold_s(const double collision_threshold_s) {
            if (collision_threshold_s < 1.0) {
                throw std::invalid_argument{"collision_threshold_s less than 1.0"};
            }

            m_collision_threshold_s = collision_threshold_s;
        }

        [[nodiscard]] double get_collision_threshold_s() const {
            return m_collision_threshold_s;
        }

        [[nodiscard]] double get_velocity_mps() const {
            return m_velocity_mps;
        }
    private:
        const T& m_publish;
        double m_velocity_mps;
        double m_collision_threshold_s;
    };

    class ServiceBus {
    public:
        void publish(const BrakeCommand& cmd) {
            std::cout << "publishing: " << cmd.time_to_collision_s << std::endl;
        }
    };

    constexpr void assert_that(bool statement, const char* message) {
        if (!statement) {
            throw std::runtime_error{message};
        }
    }

    void initial_speed_is_zero() {
        AutoBrake auto_break{[](const BrakeCommand& cmd) {}};

        assert_that(auto_break.get_velocity_mps() == 0.0, "Velocity not equal to zero");
    }

    void initial_sensitivity_is_five() {
        AutoBrake auto_break{[](const BrakeCommand& cmd) {}};

        assert_that(auto_break.get_collision_threshold_s() == 5.0, "Initial sensitivity is not five");
    }

    void sensitivity_greater_than_one() {
        AutoBrake auto_break{[](const BrakeCommand& cmd) {}};

        try {
            auto_break.set_collision_threshold_s(0.5);
        } catch (const std::exception& e) {
            return;
        }

        assert_that(false, "no exception thrown");
    }

    void speed_is_saved() {
        AutoBrake auto_break{[](const BrakeCommand& cmd) {}};

        auto_break.observe(SpeedUpdate{100.0});
        assert_that(auto_break.get_velocity_mps() == 100.0, "speed not saved");

        auto_break.observe(SpeedUpdate{50.0});
        assert_that(auto_break.get_velocity_mps() == 50.0, "speed not saved");

        auto_break.observe(SpeedUpdate{0.0});
        assert_that(auto_break.get_velocity_mps() == 0.0, "speed not saved");
    }

    void alert_when_imminent_collision() {
        int brake_command_count{0};

        auto lambda = [&brake_command_count](const BrakeCommand&) {
            brake_command_count++;
        };

        AutoBrake auto_brake{lambda};

        auto_brake.set_collision_threshold_s(10.0);
        auto_brake.observe(SpeedUpdate{100.0});
        auto_brake.observe(CarDetected{100.0, 0.0});

        assert_that(brake_command_count == 1, "Brake not engaged");
    }

    void no_alert_when_no_imminent_collision() {
        int brake_command_count{0};

        auto lambda = [&brake_command_count](const BrakeCommand&) {
            brake_command_count++;
        };

        AutoBrake auto_brake{lambda};

        auto_brake.set_collision_threshold_s(2.0);
        auto_brake.observe(SpeedUpdate{100.0});
        auto_brake.observe(CarDetected{1000.0, 50.0});

        assert_that(brake_command_count == 0, "Brake engaged");
    }


    void run_test(void(*unit_test)(), const char* name) {
        try {
            unit_test();
            printf("[+] Test: %s successful.\n", name);
        } catch (const std::exception& e) {
            printf("[-] Test: %s failed. %s\n", name, e.what());
        }
    }
}

int main() {
    ch10::run_test(ch10::initial_speed_is_zero, "initial speed is 0");
    ch10::run_test(ch10::initial_sensitivity_is_five, "initial sensitivity is 5");
    ch10::run_test(ch10::sensitivity_greater_than_one, "sensitivity greater than one");
    ch10::run_test(ch10::speed_is_saved, "speed is saved");
    ch10::run_test(ch10::alert_when_imminent_collision, "alert when imminent collision");
    ch10::run_test(ch10::no_alert_when_no_imminent_collision, "no alert when no imminent collision");

    return 0;
}
