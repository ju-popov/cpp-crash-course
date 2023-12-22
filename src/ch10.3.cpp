#include <ostream>
#include <functional>

#define BOOST_TEST_MODULE Ch10_3
#include <boost/test/unit_test.hpp>

namespace ch10_3 {
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
        std::function<void(const ch10_3::SpeedUpdate &)> speed_update_callback{};
        std::function<void(const ch10_3::CarDetected &)> car_detected_callback{};
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
}

BOOST_AUTO_TEST_CASE(InitialCarSpeedIsZero) {
    ch10_3::ServiceBusMock bus{};
    ch10_3::AutoBrake auto_brake{bus};

    BOOST_TEST(0 == auto_brake.get_velocity_mps());
}

BOOST_AUTO_TEST_CASE(InitialSensitivityIsFive) {
    ch10_3::ServiceBusMock bus{};
    ch10_3::AutoBrake auto_brake{bus};

    BOOST_TEST(5 == auto_brake.get_collision_threshold_s());
}

BOOST_AUTO_TEST_CASE(SensitivityGreaterThanOne) {
    ch10_3::ServiceBusMock bus{};
    ch10_3::AutoBrake auto_brake{bus};

    BOOST_CHECK_THROW(auto_brake.set_collision_threshold_s(0.5), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(SpeedIsSaved) {
    ch10_3::ServiceBusMock bus{};
    ch10_3::AutoBrake auto_brake{bus};

    bus.speed_update_callback(ch10_3::SpeedUpdate{100.0});
    BOOST_TEST(100.0 == auto_brake.get_velocity_mps());

    bus.speed_update_callback(ch10_3::SpeedUpdate{50.0});
    BOOST_TEST(50.0 == auto_brake.get_velocity_mps());

    bus.speed_update_callback(ch10_3::SpeedUpdate{0.0});
    BOOST_TEST(0.0 == auto_brake.get_velocity_mps());
}

BOOST_AUTO_TEST_CASE(AlertWhenImminentCollisionDetected) {
    ch10_3::ServiceBusMock bus{};
    ch10_3::AutoBrake auto_brake{bus};

    bus.speed_update_callback(ch10_3::SpeedUpdate{100.0});
    bus.car_detected_callback(ch10_3::CarDetected{100.0, 0.0});

    BOOST_TEST(1 == bus.commands_published);
    BOOST_TEST(1.0 == bus.last_command.time_to_collision_s);
}

BOOST_AUTO_TEST_CASE(NoAlertWhenNotImminentCollisionDetected) {
    ch10_3::ServiceBusMock bus{};
    ch10_3::AutoBrake auto_brake{bus};

    bus.speed_update_callback(ch10_3::SpeedUpdate{100.0});
    bus.car_detected_callback(ch10_3::CarDetected{1000.0, 50.0});

    BOOST_TEST(0 == bus.commands_published);
}
