/*
 * test_variance.cxx
 *
 *  Created on: 7 gru 2018
 *      Author: krystian
 */



#include<gtest/gtest.h>
#include <src/evaluate_mean_var_2d.h>

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
	auto one = tr.query_var_above_or_eq(400);
	auto other = get_mean_and_variance(response.begin() + 400, response.end());
	ASSERT_TRUE(one == other);
}
double gaus_var() {
	return gauss_quantile(rand() % 1000000  / 1000000.0);
}
TEST(test, two_dim_lookup) {

	std::vector<double> featuresX ;
	std::vector<double> featuresY ;
	std::vector<double> response ;
	srand(10);
	for(int i =0;i <1000;++i) {
		featuresX.push_back(gaus_var());
		response.push_back(gauss_quantile(rand() % 1000000  / 1000000.0));
		featuresY.push_back(gauss_quantile(rand() % 1000000  / 1000000.0));
	}

	auto result1 = evaluate_mean_var_2d(featuresY, featuresX, response);
	auto result2 = evaluate_mean_var_2d_naive(featuresY, featuresX, response);

/*	std::sort(result1.begin(), result1.end(), [](auto a, auto b) {
		return a.mean<b.mean;
	});
	std::sort(result2.begin(), result2.end(), [](auto a, auto b) {
		return a.mean<b.mean;
	});*/
	for(size_t i =0;i < result1.size();++i) {
		ASSERT_EQ(result1[i],result2[i]);
	}
	std::cout<<"\n";
	std::cout<<result1[0]<<result2[0]<<"\n";
	std::cout<<result1[1]<<result2[1]<<"\n";
	std::cout<<result1[2]<<result2[2]<<"\n";
	std::cout<<result1[3]<<result2[3]<<"\n";
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

TEST(test, idx_sort_reorder) {
	std::vector<double> vec {1,5,3,2,8,7};
	IdxSort s(vec);
	auto vec2 = s.reorder(vec);
	auto expected = std::vector<double>{ 1,2,3,5,7,8};
	ASSERT_EQ(expected, vec2);
}


TEST(test, idx_sort_unreorder) {
	std::vector<double> vec {1,5,3,2,8,7};
	IdxSort s(vec);
	auto vec2 = s.unreorder(s.reorder(vec));
	auto expected = std::vector<double>{ 1,2,3,5,7,8};
	ASSERT_EQ(vec2, vec);
}

TEST(test, idx_sort_large) {
	std::vector<double> vec;
	srand(15);
	for(size_t i = 0;i < 10000;++i) {
		vec.push_back(gaus_var());
	}
	IdxSort s(vec);
	auto vec2 = s.reorder(vec);
	for(size_t i =0;i < vec.size()-1;++i) {
		ASSERT_LE(vec2[i], vec2[i+1]);
	}
	ASSERT_EQ(s.unreorder(vec2), vec);
}

TEST(test, spline_tree_value_removal) {


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
	auto one = tr.query_var_above_or_eq(400);
	auto other = get_mean_and_variance(response.begin() + 400, response.end());
	tr.delete_val(features[550], response[550]);
	tr.delete_val(features[551], response[551]);
	tr.delete_val(features[552], response[552]);
	auto one1 = tr.query_var_above_or_eq(400);
	response.erase(response.begin()+550);
	response.erase(response.begin()+550);
	response.erase(response.begin()+550);
	auto other1 = get_mean_and_variance(response.begin() + 400, response.end());
	ASSERT_TRUE(one1 ==other1);
}
