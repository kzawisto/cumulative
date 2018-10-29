/*
 * test_utility.cxx
 *
 *  Created on: 27 paź 2018
 *      Author: krystian
 */


#include<gtest/gtest.h>
#include<src/tools.h>


TEST(test, selector){
	std::vector<double> vec{0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8};
	std::vector<long> a{1,2,2,3};
	std::vector<double> result{0.2, 0.3,0.3,0.4};
	auto actual = get_selector(vec, a);
	ASSERT_EQ(actual, result);
}

TEST(test, get_range_until) {

	std::vector<long> result{4,5,6,7};
	auto actual = get_range_until(4, 8);
	ASSERT_EQ(actual, result);
}
