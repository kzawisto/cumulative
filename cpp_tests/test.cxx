

#include<gtest/gtest.h>
#include <src/cdf_integral.h>
#include<iostream>

TEST(test, test_example) {
	std::vector<Point>vec{Point{1,2},Point{2,3}, Point{3,3}};

	ASSERT_EQ(point_cross_product(vec).size(), 6);

}
int main(int argc, char **argv) {
	 testing::InitGoogleTest(&argc, argv);

	 return RUN_ALL_TESTS();
}
