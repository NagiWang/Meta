#include <gtest/gtest.h>

#include "pointer.hpp"

struct pair {
	int a, b;
};

TEST(TestCase, test1) {
	std::cout << "Hello World!\n";
}

TEST(TestCase, test2) {
	meta::shared_ptr<pair> isp1{new pair{1, 2}};

	EXPECT_EQ(isp1.count(), 1);

	auto isp2 = isp1;

	EXPECT_EQ(isp1.count(), 2);
	EXPECT_EQ(isp2.count(), 2);

	auto isp3 = isp2;

	EXPECT_EQ(isp1.count(), 3);
	EXPECT_EQ(isp2.count(), 3);
	EXPECT_EQ(isp3.count(), 3);

	isp3.~shared_ptr();

	EXPECT_EQ(isp1.count(), 2);
	EXPECT_EQ(isp2.count(), 2);

	isp2.~shared_ptr();

	EXPECT_EQ(isp1.count(), 1);

	isp1.~shared_ptr();

	EXPECT_EQ(isp1.count(), 0);

}