/*
 * test_variance.cxx
 *
 *  Created on: 7 gru 2018
 *      Author: krystian
 */



#include<gtest/gtest.h>
#include<cp_tools/numeric_utility.h>
#include<cp_tools/ostream_ops.h>
#include <src/IdxSort.h>
#include <src/SplineTree.h>
#include<eigen3/Eigen/Dense>

#include "src/cdf_integral.h"
#include "src/static_cumulative_tree.h"
#include "src/tree_primitives.h"

using namespace cp_tools;

using std::size_t;

inline
SplineTree populate_tree(std::vector<double> features, std::vector<double> response) {

	IdxSort s_features(features);
	features = s_features.reorder(features);
	response = s_features.reorder(response);
	auto order_vec = TreeOrder::tree_order_indices({0, features.size()});
	SplineTree tr;
	for(auto a: order_vec) {
		tr.insert(features[a], response[a]);
	}
	tr.set_mean_var_values();
	return tr;
}
inline
std::vector<MeanAndVariance<double>>
evaluate_mean_var_2d(std::vector<double> featuresX, std::vector<double> featuresY, std::vector<double> response) {
	auto spline_tree = populate_tree(featuresX, response);
	IdxSort ord_by_y(featuresY);
	std::vector<MeanAndVariance<double>> results_all(featuresY);
	auto featuresX_ord_Y = ord_by_y.reorder(featuresX);
	auto featuresY_ord_Y = ord_by_y.reorder(featuresY);
	auto response_ord_y = ord_by_y.reorder(response);
	for(std::size_t i = 0;i < featuresY.size();i--) {
		results_all[i] = spline_tree.query_var_above_or_eq(featuresX_ord_Y[i]);
		spline_tree.delete_val(featuresX_ord_Y[i], response_ord_y[i]);
	}
	return ord_by_y.unreorder(results_all);
}

inline
std::vector<MeanAndVariance<double>>
evaluate_mean_var_2d_naive(std::vector<double> f1, std::vector<double> f2, std::vector<double> response) {

	std::vector<MeanAndVariance<double>> results(f1.size());
	for(size_t i=0;i< f1.size();++i) {
		std::vector<double> chosen_values;
		for(size_t j =0;j < f1.size(); ++j) {
			if(f1[j]>= f1[i] and f2[j]>= f2[i]) {
				chosen_values.push_back(response[j]);
			}
		}
		results[i] = get_mean_and_variance(chosen_values.begin(), chosen_values.end());
	}
	return results;

}

TEST(test, spline_tree) {

	std::vector<double> features ;
	std::vector<double> response ;
	srand(10);
	for(int i =0;i <1000;++i) {
		features.push_back(i);
		response.push_back(gauss_quantile(rand() % 1000000  / 1000000.0));
	}
	IdxSort s_features(features);
	features = s_features.reorder(features);
	response = s_features.reorder(response);
	auto order_vec = TreeOrder::tree_order_indices({0, 1000});
	SplineTree tr;
	for(auto a: order_vec) {
		tr.insert(features[a], response[a]);
	}
	tr.set_mean_var_values();
	std::cout<<tr.query_var_above_or_eq(400)<<"\n";
	std::cout<<get_mean_and_variance(response.begin() + 400, response.end())<<"\n";
}
TEST(test, two_dim_lookup) {

	std::vector<double> featuresX ;
	std::vector<double> featuresY ;
	std::vector<double> response ;
	srand(10);
	for(int i =0;i <1000;++i) {
		featuresX.push_back(i);
		response.push_back(gauss_quantile(rand() % 1000000  / 1000000.0));
		featuresY.push_back(gauss_quantile(rand() % 1000000  / 1000000.0));
	}

	auto spline_tree = populate_tree(featuresX, response);
	IdxSort ord_by_y(featuresY);
	std::vector<MeanAndVariance<double>> results_all(featuresY);
	auto featuresX_ord_Y = ord_by_y.reorder(featuresX);
	auto featuresY_ord_Y = ord_by_y.reorder(featuresY);
	auto response_ord_y = ord_by_y.reorder(response);
	for(std::size_t i = 0;i < featuresY.size();i--) {
		results_all[i] = spline_tree.query_var_above_or_eq(featuresX_ord_Y[i]);
		spline_tree.delete_val(featuresX_ord_Y[i], response_ord_y[i]);
	}
	ord_by_y.unreorder(results_all);

	//response = s_features.reorder(response);
}
TEST(test, combined_variance) {

	using namespace cp_tools;
	std::vector<double> vec,vec2,vec3;
	srand(1);
	for(unsigned i =0;i < 30; i++) {
		vec3.push_back((rand() % 10000) / 10000.0);
	}
	for(unsigned i =0;i < 20; i++) {
		vec2.push_back((rand() % 10000) / 10000.0);
	}
	vec.insert(vec.end(), vec3.begin(), vec3.end());
	vec.insert(vec.end(), vec2.begin(), vec2.end());
	auto mv2 =  get_mean_and_variance(vec2.begin(), vec2.end());
	auto mv3 =  get_mean_and_variance(vec3.begin(), vec3.end());
	auto mv = combine_variance(mv2, mv3);
	auto mv_actual = get_mean_and_variance(vec.begin(), vec.end());
	ASSERT_NEAR(mv.mean, mv_actual.mean, 1e-7);
	ASSERT_NEAR(mv.variance, mv_actual.variance, 1e-7);
	std::cout<<decombine_variances(mv, mv2)<<"\n";
	std::cout<<mv3;

};

TEST(test, idx_sort) {
	std::vector<double> vec {1,5,3,2,8,7};
	IdxSort s(vec);
	auto vec2 = s.reorder(vec);
	auto expected = std::vector<double>{ 1,2,3,5,7,8};
	ASSERT_EQ(expected, vec2);

}

