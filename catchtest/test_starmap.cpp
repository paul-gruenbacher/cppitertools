#include <imap.hpp>

#include "helpers.hpp"

#include <vector>
#include <list>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::starmap;

namespace {
    long f(long d, int i) {
        return d * i;
    }

    std::string g(const std::string& s, int i, char c) {
        std::stringstream ss;
        ss << s << ' ' << i << ' ' << c;
        return ss.str();
    }

    struct Callable {
        int operator()(int a, int b, int c) {
            return a + b + c;
        }

        int operator()(int a) {
            return a;
        }
    };
}

TEST_CASE("starmap: works with function pointer and lambda", "[starmap]") {
    using Vec = const std::vector<int>;
    const std::vector<std::pair<double, int>> v1 =
        {{1l, 2}, {3l, 11}, {6l, 7}};
    Vec vc = {2l, 33l, 42l};

    SECTION("with function") {
        auto sm = starmap(f, v1);
        Vec v(std::begin(sm), std::end(sm));
        REQUIRE( v == vc );
    }

    SECTION("with lambda") {
        auto sm = starmap([](long a, int b) { return a * b; }, v1);
        Vec v(std::begin(sm), std::end(sm));
        REQUIRE( v == vc );
    }
}

TEST_CASE("starmap: list of tuples", "[starmap]") {
    using Vec = const std::vector<std::string>;
    using T = std::tuple<std::string, int, double>;
    std::list<T> li =
        {T{"hey", 42, 'a'}, T{"there", 3, 'b'}, T{"yall", 5, 'c'}};

    auto sm = starmap(g, li);
    Vec v(std::begin(sm), std::end(sm));
    Vec vc = {"hey 42 a", "there 3 b", "yall 5 c"};

    REQUIRE( v == vc );
}

TEST_CASE("starmap: tuple of tuples", "[starmap]") {
    using Vec = const std::vector<int>;
    auto tup = std::make_tuple(std::make_tuple(10, 19, 60),std::make_tuple(7));
    auto sm = starmap(Callable{}, tup);
    Vec v(std::begin(sm), std::end(sm));
    Vec vc = {89, 7};

    REQUIRE( v == vc );
}

TEST_CASE("starmap: tuple of pairs", "[starmap]") {
    using Vec = const std::vector<int>;
    auto p = std::make_pair(std::array<int, 3>{{15, 100, 2000}},
                            std::make_tuple(16));
    Callable c;
    auto sm = starmap(c, p);

    Vec v(std::begin(sm), std::end(sm));
    Vec vc = {2115, 16};

    REQUIRE( v == vc );
}

TEST_CASE("starmap: moves rvalues, binds to lvalues", "[starmap]") {
    itertest::BasicIterable<std::tuple<int>> bi{};
    starmap(Callable{}, bi);
    REQUIRE_FALSE( bi.was_moved_from() );
    starmap(Callable{}, std::move(bi));
    REQUIRE( bi.was_moved_from() );
}
