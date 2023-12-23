#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/smart_ptr/scoped_array.hpp>
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
using ScopedOathbreakers = boost::scoped_ptr<DeadMenOfDunharrow>;

TEST_CASE("ScopedPtr evaluates to") {
    SECTION("true when full") {
        ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{} };
        REQUIRE(aragorn);
    }
    SECTION("false when empty") {
        ScopedOathbreakers aragorn;
        REQUIRE_FALSE(aragorn);
    }
}

TEST_CASE("ScopedPtr is an RAII wrapper") {
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 0);
    ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{} };
    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);

    {
        ScopedOathbreakers legolas{ new DeadMenOfDunharrow{} };
        REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 2);
    }

    REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
}

TEST_CASE("ScopedPtr supports pointer semantics, like") {
    auto message = "The way is shut.";
    ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{message} };
    SECTION("operator*") {
        REQUIRE((*aragorn).message == message);
    }
    SECTION("operator->") {
        REQUIRE(aragorn->message == message);
    }
    SECTION("get() which returns the raw pointer") {
        REQUIRE(aragorn.get()->message == message);
    }
}

TEST_CASE("ScopedPtr supports comparison to nullptr") {
    SECTION("operator==") {
        ScopedOathbreakers aragorn;
        REQUIRE(aragorn == nullptr);
    }
    SECTION("operator!=") {
        ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{} };
        REQUIRE(aragorn != nullptr);
    }
}

TEST_CASE("ScopedPtr supports swap") {
    auto message1 = "The way is shut.";
    auto message2 = "Until the time comes.";

    ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{message1} };
    ScopedOathbreakers legolas{ new DeadMenOfDunharrow{message2} };

    REQUIRE(aragorn->message == message1);
    REQUIRE(legolas->message == message2);

    swap(aragorn, legolas);

    REQUIRE(aragorn->message == message2);
    REQUIRE(legolas->message == message1);
}

TEST_CASE("ScopedPtr reset") {
    ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{} };
    SECTION("destruct owned object.") {
        aragorn.reset();
        REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 0);
    }
    SECTION("can replace an owned object.") {
        auto message = "It was made by those who are Dead.";
        auto new_dead_men = new DeadMenOfDunharrow{ message };
        REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 2);
        aragorn.reset(new_dead_men);
        REQUIRE(DeadMenOfDunharrow::oaths_to_fulfill == 1);
        REQUIRE(aragorn->message == new_dead_men->message);
        REQUIRE(aragorn.get() == new_dead_men);
    }
}

void by_ref(const ScopedOathbreakers&) {}
void by_val(ScopedOathbreakers) {}

TEST_CASE("ScopedPrt can") {
    ScopedOathbreakers aragorn{ new DeadMenOfDunharrow{} };

    SECTION("be passed by reference") {
        by_ref(aragorn);
    }

    SECTION("not be copied") {
        // DOES NOT COMPILE
        // by_val(aragorn);
        // auto son_of_arathorn_1{ aragorn };
        // auto son_of_arathorn_2 = aragorn;
    }

    SECTION("not be moved") {
        // DOES NOT COMPILE
        // by_val(std::move(aragorn));
        // auto son_of_arathorn_1{ std::move(aragorn) };
        // auto son_of_arathorn_2 = std::move(aragorn);
    }
}

TEST_CASE("ScopedArray suports opearator[]") {
    boost::scoped_array<int> sqares{
        new int[5] { 0, 4, 9, 16, 25 }
    };

    sqares[0] = 1;
    REQUIRE(sqares[0] == 1);
    REQUIRE(sqares[1] == 4);
    REQUIRE(sqares[2] == 9);
}