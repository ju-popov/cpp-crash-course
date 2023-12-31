#include <ostream>
#include <functional>

#include "gtest/gtest.h"

namespace ch10_2 {
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
        std::function<void(const ch10_2::SpeedUpdate &)> speed_update_callback{};
        std::function<void(const ch10_2::CarDetected &)> car_detected_callback{};
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

struct Ch10_2 : public ::testing::Test {
    ch10_2::ServiceBusMock bus{};
    ch10_2::AutoBrake auto_brake{bus};
};

TEST_F(Ch10_2, InitialCarSpeedIsZero) {
    EXPECT_EQ(0.0, auto_brake.get_velocity_mps());
}

TEST_F(Ch10_2, InitialSensitivityIsFive) {
    EXPECT_EQ(5.0, auto_brake.get_collision_threshold_s());
}

TEST_F(Ch10_2, SensitivityGreaterThanOne) {
    EXPECT_THROW(auto_brake.set_collision_threshold_s(0.5), std::invalid_argument);
}

TEST_F(Ch10_2, SpeedIsSaved) {
    bus.speed_update_callback(ch10_2::SpeedUpdate{100.0});
    EXPECT_EQ(100.0, auto_brake.get_velocity_mps());

    bus.speed_update_callback(ch10_2::SpeedUpdate{50.0});
    EXPECT_EQ(50.0, auto_brake.get_velocity_mps());

    bus.speed_update_callback(ch10_2::SpeedUpdate{0.0});
    EXPECT_EQ(0.0, auto_brake.get_velocity_mps());
}

TEST_F(Ch10_2, AlertWhenImminentCollisionDetected) {
    auto_brake.set_collision_threshold_s(10.0);

    bus.speed_update_callback(ch10_2::SpeedUpdate{100.0});
    bus.car_detected_callback(ch10_2::CarDetected{100.0, 0.0});

    EXPECT_EQ(1, bus.commands_published);
}

TEST_F(Ch10_2, NoAlertWhenNoImminentCollisionDetected) {
    auto_brake.set_collision_threshold_s(2.0);

    bus.speed_update_callback(ch10_2::SpeedUpdate{100.0});
    bus.car_detected_callback(ch10_2::CarDetected{1000.0, 50.0});

    EXPECT_EQ(0, bus.commands_published);
}
