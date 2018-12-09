/*
 * evaluate_mean_var_2d.h
 *
 *  Created on: 9 gru 2018
 *      Author: krystian
 */

#ifndef SRC_EVALUATE_MEAN_VAR_2D_H_
#define SRC_EVALUATE_MEAN_VAR_2D_H_

#include<cp_tools/numeric_utility.h>
#include<cp_tools/ostream_ops.h>
#include <src/IdxSort.h>
#include <src/SplineTree.h>

#include "src/cdf_integral.h"
#include "src/static_cumulative_tree.h"
#include "src/tree_primitives.h"
#include<cmath>

using namespace cp_tools;

using std::size_t;

template<typename T>
bool float_cmp(T x, T y, T eps=1e-6) {
    T maxXY = std::max( std::fabs(x) , std::fabs(y) ) ;
    return std::fabs(x - y) <= eps * maxXY ;
}
namespace cp_tools {
template<typename T>
bool operator==(const MeanAndVariance<T>  one, const MeanAndVariance<T>  other) {
	return float_cmp(one.mean, other.mean) and float_cmp(one.variance, other.variance) and one.count == other.count;
}
}
inline
SplineTree populate_tree(std::vector<double> features, std::vector<double> response) {

	IdxSort s_features(features);
	features = s_features.reorder(features);
	response = s_features.reorder(response);
	auto order_vec = TreeOrder::tree_order_indices({0, features.size()-1});
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
	std::vector<MeanAndVariance<double>> results_all(featuresY.size());
	auto featuresX_ord_Y = ord_by_y.reorder(featuresX);
	auto featuresY_ord_Y = ord_by_y.reorder(featuresY);
	auto response_ord_y = ord_by_y.reorder(response);
	for(long i =0;i<featuresY_ord_Y.size()-1; ++i) {
		if(featuresY_ord_Y[i]> featuresY_ord_Y[i+1]) {
			throw std::runtime_error("");
		}
	}
	for(std::size_t i = 0;i < featuresY.size();i++) {
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
		if(results[i].count==1) {
			results[i].variance = 0;
		}
	}
	return results;
}

struct OptimalSpline {
	MeanAndVariance<double> selection, remainder;
	double X, Y, loglik;
	long idx;
};

double evaluate_spline_loglik(MeanAndVariance<double>total_mv, MeanAndVariance<double> spline_mv) {
	auto remainder_mv = decombine_variances_ex(total_mv, spline_mv);
	return -(remainder_mv.variance * remainder_mv.count + spline_mv.variance * spline_mv.count);
}

inline
OptimalSpline
get_optimal_spline(std::vector<double> featuresX, std::vector<double> featuresY, std::vector<double> response) {

	MeanAndVariance<double> total = get_mean_and_variance(response.begin(), response.end());
	auto results = evaluate_mean_var_2d(featuresX, featuresY, response);
	auto loglik = std::vector<double> (results.size());
	double max =-1e99; long idx = -1;
	for(size_t i = 0;i < results.size();++i) {
		double loglik = evaluate_spline_loglik(total, results[i]);
		if(loglik > max) {
			max = loglik; idx = i;
		}
	}

	OptimalSpline s;

	s.selection = results[idx];
	s.remainder = decombine_variances_ex(total, s.selection);
	s.X = featuresX[idx];
	s.Y = featuresY[idx];
	s.idx = idx;
	s.loglik = max;
	return s;

}


#endif /* SRC_EVALUATE_MEAN_VAR_2D_H_ */
