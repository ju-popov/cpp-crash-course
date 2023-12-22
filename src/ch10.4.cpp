#include <ostream>
#include <functional>

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace ch10_4 {
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

class ServiceBusMock : public ch10_4::IServiceBus {
    MOCK_METHOD1(publish, void(const ch10_4::BrakeCommand& cmd));
    MOCK_METHOD1(subscribe, void(const std::function<void(const ch10_4::SpeedUpdate&)>& callback));
    MOCK_METHOD1(subscribe, void(const std::function<void(const ch10_4::CarDetected&)>& callback));
};

struct Ch10_4Nice : public ::testing::Test {
    ::testing::NiceMock<ServiceBusMock> bus;
    ch10_4::AutoBrake auto_brake{bus};
};

TEST_F(Ch10_4Nice, InitialCarSpeedIsZero) {
    EXPECT_EQ(0.0, auto_brake.get_velocity_mps());
}

TEST_F(Ch10_4Nice, InitialSensitivityIsFive) {
    EXPECT_EQ(5.0, auto_brake.get_collision_threshold_s());
}

TEST_F(Ch10_4Nice, SensitivityGreaterThanOne) {
    EXPECT_THROW(auto_brake.set_collision_threshold_s(0.5), std::invalid_argument);
}

// TODO
