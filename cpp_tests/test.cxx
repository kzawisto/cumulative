

#include<gtest/gtest.h>
#include <src/cdf_integral.h>
#include<iostream>
#include<src/tools.h>
#include <map>
#include <random>
#include <cmath>
#include<algorithm>
#include "src/static_cumulative_tree.h"

TEST(test, test_example) {
	std::vector<Point>vec{Point{1,2},Point{2,3}, Point{3,3}};

	ASSERT_EQ(point_cross_product(vec).size(), 6);

}


TEST(test, test_tree_order_should_output_same_elements_as_input) {
	std::vector<double> vec {1.1, 1.2, 1.3,1.4,1.5,1.5,1.6,1.7, 1.9,2.0};

	auto result = TreeOrder::tree_order(vec);
	std::stable_sort(result.begin(), result.end());
	ASSERT_EQ(result, vec);
}

std::vector<double> get_cumsum(std::vector<double> values) {

	std::vector<double> cumsum ;
	double j =0;
	for(int i = 0;i < values.size(); ++i) {
		j+=values[i];
		cumsum.push_back(j);
	}
	return cumsum;
}
TEST(test, test_insert_into_tree) {

	std::vector<double> vec {1.1, 1.2, 1.3,1.4,1.5,1.5,1.6,1.7, 1.9,2.0};
	std::vector<double> values {1.01, -1.03, -1.04, -1.06, 1.001,  1.04,-1.09,-1.19,-1.0001,-1.54};
	std::vector<double> cumsum = get_cumsum(values);
	CumulativeTree tree;

	auto order_vec = TreeOrder::tree_order_indices({0, vec.size()-1});
	for(auto a: order_vec) {
		tree.insert(vec[a], values[a]);
	}
	//RichTree::inorder_transversal(tree.root);
	CumulativeTree::inorder_transversal_set_cumsum(tree.root,0);
	CumulativeTree::transversal_set_extrema(tree.root);
	ASSERT_EQ(tree.root->cum_max, 1.01);
	ASSERT_NEAR(tree.root->cum_min, -4.8991, 1e-5);
}



TEST(test, test_removal_random) {

	std::vector<double> vec;
	std::vector<double> values;


    std::random_device rd{};
    std::mt19937 gen{10};
    std::normal_distribution<> d{0,1};
	for(int i =0; i < 1000;++i) {
		vec.push_back(i);
		values.push_back(d(gen));
	}



	CumulativeTree tree;
	auto order_vec = TreeOrder::tree_order_indices({0, vec.size()-1});
	for(auto a: order_vec) {
		tree.insert(vec[a], values[a]);
	}
	//RichTree::inorder_transversal(tree.root);
	CumulativeTree::inorder_transversal_set_cumsum(tree.root,0);
	CumulativeTree::transversal_set_extrema(tree.root);
	std::vector<double> vals_cp = values;
	std::vector<double> cumsum = get_cumsum(vals_cp);

	std::random_shuffle(vec.begin(), vec.end());
	for(int i = 0;i < vec.size(); i+=1) {
		auto idx = vec[i];
		for(int j =idx;j<cumsum.size();++j)
			cumsum[j] -= values[idx];
		tree.delete_val(idx, values[idx]);

		auto expected_max = *std::max_element(cumsum.begin(), cumsum.end());
		auto actual_max = tree.root->cum_max;
		auto expected_min = *std::min_element(cumsum.begin(), cumsum.end());
		auto actual_min = tree.root->cum_min;
		ASSERT_NEAR(actual_max, expected_max,1e-9);
		ASSERT_NEAR(actual_min, expected_min,1e-9);
	}
}

int main(int argc, char **argv) {
	 testing::InitGoogleTest(&argc, argv);

	 return RUN_ALL_TESTS();
}
