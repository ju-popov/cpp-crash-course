#include <ostream>
#include <functional>

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

    class IServiceBus {
    public:
        IServiceBus() = default;
        virtual ~IServiceBus() = default;

        virtual void publish(const BrakeCommand& cmd) = 0;

        virtual void subscribe(const std::function<void(const SpeedUpdate&)>& callback) = 0;

        virtual void subscribe(const std::function<void(const CarDetected&)>& callback) = 0;
    };

    class ServiceBusMock : public IServiceBus {
    public:
        void publish(const BrakeCommand& cmd) override {
            commands_published++;
            last_command = cmd;
        }

        void subscribe(const std::function<void(const SpeedUpdate&)>& callback) override {
            speed_update_callback = callback;
        }

        void subscribe(const std::function<void(const CarDetected&)>& callback) override {
            car_detected_callback = callback;
        }

    public:
        int commands_published{};
        BrakeCommand last_command{};
        std::function<void(const ch10::SpeedUpdate &)> speed_update_callback{};
        std::function<void(const ch10::CarDetected &)> car_detected_callback{};
    };

    class AutoBrake {
    public:
        explicit AutoBrake(IServiceBus& service_bus) : m_service_bus{service_bus}, m_velocity_mps{0.0}, m_collision_threshold_s{5.0} {
            m_service_bus.subscribe([this](const SpeedUpdate& update) {
                this->observe(update);
            });

            m_service_bus.subscribe([this](const CarDetected& update) {
                this->observe(update);
            });
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
        void observe(const SpeedUpdate& update) {
            m_velocity_mps = update.velocity_mps;
        }

        void observe(const CarDetected& update) {
            const auto relative_velocity_mps = m_velocity_mps - update.velocity_mps;
            if (relative_velocity_mps <= 0.0) {
                return;
            }

            const auto time_to_collision_s = update.distance_m / relative_velocity_mps;
            if (time_to_collision_s > 0.0 && time_to_collision_s <= m_collision_threshold_s) {
                m_service_bus.publish(BrakeCommand{time_to_collision_s});
            }
        }

    private:
        IServiceBus& m_service_bus;
        double m_velocity_mps;
        double m_collision_threshold_s;
    };

    constexpr void assert_that(bool statement, const char* message) {
        if (!statement) {
            throw std::runtime_error{message};
        }
    }

    void initial_speed_is_zero() {
        ServiceBusMock service_bus{};
        AutoBrake auto_break{service_bus};

        assert_that(auto_break.get_velocity_mps() == 0.0, "Velocity not equal to zero");
    }

    void initial_sensitivity_is_five() {
        ServiceBusMock service_bus{};
        AutoBrake auto_break{service_bus};

        assert_that(auto_break.get_collision_threshold_s() == 5.0, "Initial sensitivity is not five");
    }

    void sensitivity_greater_than_one() {
        ServiceBusMock service_bus{};
        AutoBrake auto_break{service_bus};

        try {
            auto_break.set_collision_threshold_s(0.5);
        } catch (const std::exception& e) {
            return;
        }

        assert_that(false, "no exception thrown");
    }

    void speed_is_saved() {
        ServiceBusMock service_bus{};
        AutoBrake auto_break{service_bus};

        service_bus.speed_update_callback(SpeedUpdate{100.0});
        assert_that(auto_break.get_velocity_mps() == 100.0, "speed not saved");

        service_bus.speed_update_callback(SpeedUpdate{50.0});
        assert_that(auto_break.get_velocity_mps() == 50.0, "speed not saved");

        service_bus.speed_update_callback(SpeedUpdate{0.0});
        assert_that(auto_break.get_velocity_mps() == 0.0, "speed not saved");
    }

    void alert_when_imminent_collision() {
        ServiceBusMock service_bus{};
        AutoBrake auto_break{service_bus};

        auto_break.set_collision_threshold_s(10.0);

        service_bus.speed_update_callback(SpeedUpdate{100.0});
        service_bus.car_detected_callback(CarDetected{100.0, 0.0});

        assert_that(service_bus.commands_published == 1, "Brake not engaged");
    }

    void no_alert_when_no_imminent_collision() {
        ServiceBusMock service_bus{};
        AutoBrake auto_break{service_bus};

        auto_break.set_collision_threshold_s(2.0);

        service_bus.speed_update_callback(SpeedUpdate{100.0});
        service_bus.car_detected_callback(CarDetected{1000.0, 50.0});

        assert_that(service_bus.commands_published == 0, "Brake engaged");
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
