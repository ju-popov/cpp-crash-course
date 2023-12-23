#include "catch.hpp"

struct DeadMenOfDunharrow {
    DeadMenOfDunharrow(const char* m="")
    : message{ m } {
        oaths_to_fulfill++;
    }

    ~DeadMenOfDunharrow() {
        oaths_to_fulfill--;
    }

    const char* message;
    static int oaths_to_fulfill;
};

int DeadMenOfDunharrow::oaths_to_fulfill{};

using UniqueOathbreakers = std::unique_ptr<DeadMenOfDunharrow>;

TEST_CASE("UniquePtr can be used in move") {
    auto aragorn = std::make_unique<DeadMenOfDunharrow>();

    SECTION("construction") {
        auto son_of_arathorn = std::move(aragorn);
        REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
    }

    SECTION("assignment") {
        auto son_of_arathorn = std::make_unique<DeadMenOfDunharrow>();
        REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 2);
        son_of_arathorn = std::move(aragorn);
        REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
    }
}

TEST_CASE("UniquePrt to array supports operator[]") {
    std::unique_ptr<int[]> squares{
        new int[5]{ 0, 4, 9, 16, 25 }
    };

    squares[0] = 1;

    REQUIRE(squares[0] == 1);
    REQUIRE(squares[1] == 4);
    REQUIRE(squares[2] == 9);
}

