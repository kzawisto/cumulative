/*
 * test_misc.cxx
 *
 *  Created on: 8 lis 2018
 *      Author: krystian
 */

#include<gtest/gtest.h>
#include<vector>

template<typename K, typename Op1, typename Op2>
void merge_in_order_iteration(std::vector<K> v, std::vector<K> v2,
Op1 op1, Op2 op2
) {
	auto i = 0;
	auto i2 = 0;
	while(i2 != v2.size() || i != v.size()) {
		while(i != v.size() && (i2 == v2.size() || v[i] <= v2[i2] ) ) {
			op1(v[i]);
			i++;
		}
		while(i2 != v2.size() && (i == v.size() || v[i] >= v2[i2] ) ) {
			op2(v2[i2]);
			i2++;
		}
	}

}
TEST(test, test_merge_in_order) {
	std::vector<double> vec {1,2,3,4,4,5,6,7,7,8,8,9,9,9};
	std::vector<double> vec2 {0,1,3,3,4,5,7,7,10};
	merge_in_order_iteration(vec, vec2,
			[](double a){std::cout<< "v1 " << a << " ";},
			[](double a){std::cout<< "v2 " << a << " ";}
	);

}
