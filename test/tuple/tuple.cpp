#include <gtest/gtest.h>
#include <iostream>
#include <tuple>

#include "tuple.hpp"

TEST(TestCase, rtuple_test) {
    using namespace meta;

    int a  = 1;
    int& b = a;
    rtuple<int, int&, double> rtuple1(a, b, 2);

    EXPECT_EQ(std::get<0>(rtuple1), a);
    EXPECT_EQ(std::get<1>(rtuple1), b);
    EXPECT_EQ(std::get<2>(rtuple1), 2);

    b                    = 2;
    std::get<2>(rtuple1) = 3;

    EXPECT_EQ(std::get<0>(rtuple1), 1);
    EXPECT_EQ(std::get<1>(rtuple1), 2);
    EXPECT_EQ(std::get<2>(rtuple1), 3);

    {
        auto&& [x, y, z] = rtuple1;

        EXPECT_EQ(x, 1);
        EXPECT_EQ(y, 2);
        EXPECT_EQ(z, 3);

        x = 5;
        y = 6;
        z = 7;

        EXPECT_EQ(std::get<0>(rtuple1), 5);
        EXPECT_EQ(std::get<1>(rtuple1), 6);
        EXPECT_EQ(std::get<2>(rtuple1), 7);
    }

    {
        auto [x, y, z] = rtuple1;

        EXPECT_EQ(x, 5);
        EXPECT_EQ(y, 6);
        EXPECT_EQ(z, 7);

        x = 7;
        y = 8;
        z = 9;

        EXPECT_EQ(std::get<0>(rtuple1), 5);
        EXPECT_EQ(std::get<1>(rtuple1), 8);
        EXPECT_EQ(std::get<2>(rtuple1), 7);
    }

    int d        = 1;
    const int& e = d;

    rtuple<int, const int&, int&&> rtuple2(d, e, 1);

    EXPECT_EQ(std::get<0>(rtuple2), d);
    EXPECT_EQ(std::get<1>(rtuple2), e);
    EXPECT_EQ(std::get<2>(rtuple2), 1);

    d                    = 2;
    std::get<2>(rtuple2) = 3;

    EXPECT_EQ(std::get<0>(rtuple2), 1);
    EXPECT_EQ(std::get<1>(rtuple2), d);
    EXPECT_EQ(std::get<2>(rtuple2), 3);
}

TEST(TestCase, mtuple_test) {
    using namespace meta;

    int a  = 1;
    int& b = a;
    mtuple<int, int&, double> mtuple1(a, b, 2);

    EXPECT_EQ(std::get<0>(mtuple1), a);
    EXPECT_EQ(std::get<1>(mtuple1), b);
    EXPECT_EQ(std::get<2>(mtuple1), 2);

    b                    = 2;
    std::get<2>(mtuple1) = 3;

    EXPECT_EQ(std::get<0>(mtuple1), 1);
    EXPECT_EQ(std::get<1>(mtuple1), 2);
    EXPECT_EQ(std::get<2>(mtuple1), 3);

    {
        auto&& [x, y, z] = mtuple1;

        EXPECT_EQ(x, 1);
        EXPECT_EQ(y, 2);
        EXPECT_EQ(z, 3);

        x = 5;
        y = 6;
        z = 7;

        EXPECT_EQ(std::get<0>(mtuple1), 5);
        EXPECT_EQ(std::get<1>(mtuple1), 6);
        EXPECT_EQ(std::get<2>(mtuple1), 7);
    }

    {
        auto [x, y, z] = mtuple1;

        EXPECT_EQ(x, 5);
        EXPECT_EQ(y, 6);
        EXPECT_EQ(z, 7);

        x = 7;
        y = 8;
        z = 9;

        EXPECT_EQ(std::get<0>(mtuple1), 5);
        EXPECT_EQ(std::get<1>(mtuple1), 8);
        EXPECT_EQ(std::get<2>(mtuple1), 7);
    }

    int d        = 1;
    const int& e = d;

    mtuple<int, const int&, int&&> mtuple2(d, e, 1);

    EXPECT_EQ(std::get<0>(mtuple2), d);
    EXPECT_EQ(std::get<1>(mtuple2), e);
    EXPECT_EQ(std::get<2>(mtuple2), 1);

    d                    = 2;
    std::get<2>(mtuple2) = 3;

    EXPECT_EQ(std::get<0>(mtuple2), 1);
    EXPECT_EQ(std::get<1>(mtuple2), d);
    EXPECT_EQ(std::get<2>(mtuple2), 3);
}
