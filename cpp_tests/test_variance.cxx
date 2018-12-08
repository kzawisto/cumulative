/*
 * test_variance.cxx
 *
 *  Created on: 7 gru 2018
 *      Author: krystian
 */



#include<gtest/gtest.h>
#include<cp_tools/numeric_utility.h>
#include <cp_tools/numeric_utility.h>
template<typename T>
T sq(T val) { return val * val; }

using namespace cp_tools;
template <typename T>
std::ostream & operator<<(std::ostream & op, MeanAndVariance<T> mv) {
	op<<"{mean="<<mv.mean<<", var="<<mv.variance<<", count=" << mv.count<<"}";
	return op;
}
template<typename T>
MeanAndVariance<T> combine_variance(MeanAndVariance<T> one, MeanAndVariance<T> another) {
	long count_total = (another.count + one.count);
	double mean_new = (one.mean * one.count + another.mean * another.count) / count_total;
	double variance_new = ((one.count-1) * one.variance + one.count*sq(one.mean - mean_new) +

			(another.count-1) * another.variance + another.count * sq(another.mean - mean_new)) / (count_total-1);
	return MeanAndVariance<T>{
		.mean = mean_new, .variance = variance_new, .count = count_total
	};
}
TEST(test, combined_variance) {

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

};

