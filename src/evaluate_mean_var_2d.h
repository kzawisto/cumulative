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
#include<algorithm>
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
	friend std::ostream & operator<<(std::ostream & os, OptimalSpline s) {
		os <<"OptimalSpline{"<<"selection:"<<s.selection<<", remainder:"<<
				s.remainder<<", X:"<<s.X<<", Y:"<<s.Y<<", loglik:"<<s.loglik<<"}\n";
		return os;
	}

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

struct MeanVar2dEvaluator {
	long sweep_position = 0;
	std::vector<double > featuresX_ord_Y, featuresY_ord_Y, response_ord_y;
	std::vector<double> featuresX_ord_X;
	MeanAndVariance<double> total_var;
	SplineTree spline_tree;
	long stride_lim=1000;
	long stride_decr=5;
	MeanVar2dEvaluator(std::vector<double> featuresX, std::vector<double> featuresY, std::vector<double> response) {
		IdxSort ord_by_y(featuresY);
		IdxSort ord_by_x(featuresX);
		std::vector<MeanAndVariance<double>> results_all(featuresY.size());
		 featuresX_ord_Y = ord_by_y.reorder(featuresX);
		 featuresX_ord_X = ord_by_x.reorder(featuresX);
		 featuresY_ord_Y = ord_by_y.reorder(featuresY);
		 response_ord_y = ord_by_y.reorder(response);
		 spline_tree = populate_tree(featuresX_ord_Y, response_ord_y);
		 total_var = get_mean_and_variance(response.begin(), response.end());
	}
	void sweep_forward() {
		spline_tree.delete_val(featuresX_ord_Y[sweep_position], response_ord_y[sweep_position]);
		sweep_position++;
	}
	void sweep_backward() {
		sweep_position--;
		spline_tree.insert_val(featuresX_ord_Y[sweep_position], response_ord_y[sweep_position]);
	}
	MeanAndVariance<double> evaluate_at_indices(long idx_x, long idx_y) {
		assert(idx_y == sweep_position);
		return spline_tree.query_var_above_or_eq(featuresX_ord_X[idx_x]);
	}
	std::pair<long, long> evaluate_at_positions(std::vector<long> X_indices, std::vector<long> Y_indices) {

		IdxSort ord_by_y(Y_indices);
		auto y_sorted = ord_by_y.reorder(Y_indices);
		auto x_sorted = ord_by_y.reorder(X_indices);
		long i_max = -1;
		double loglik_max = -1e9;
		while(y_sorted[0]< sweep_position) {
			sweep_backward();
		}
		for(size_t i = 0;i < Y_indices.size();++i) {
			while(y_sorted[0]< sweep_position) {
				sweep_backward();
			}
			while(y_sorted[i]> sweep_position) {
				sweep_forward();
			}

			auto res = evaluate_at_indices(x_sorted[i], y_sorted[i]);
			auto loglik = evaluate_spline_loglik(total_var, res);
			std::cout<<loglik<<" ";
			if(loglik > loglik_max) {
				loglik_max=loglik;
				i_max = i;
			}
		}
		std::cout<<"\nchosen:"<<x_sorted[i_max]<<" "<< y_sorted[i_max]<<"\n";

		return {x_sorted[i_max], y_sorted[i_max]};
	}
	std::vector<long> get_n_indices(long fr, long to, long stride) {
		std::vector<long> vec;
		for(long i =fr;i < to-1;i+= stride) {
			vec.push_back(i);
		}
		vec.push_back(to-1);
		return vec;
	}
	OptimalSpline run_optim() {
		auto N = featuresX_ord_X.size();

		long stride = N / std::min<long>(stride_lim, (long)(std::sqrt(N))+0.5);
		long frX = 0, toX = N;
		long frY = 0, toY = N;
		std::pair<long,long> result;
		while(true) {

			std::vector<long> x_ind = get_n_indices(frX, toX, stride);
			std::vector<long> y_ind = get_n_indices(frY, toY, stride);
			result = evaluate_at_positions(x_ind, y_ind);

			frX = std::max<long>( result.first - stride, 0l);
			frY = std::max<long>( result.second - stride, 0);
			toX = std::min<long>( result.first + stride, N);
			toY = std::min<long>( result.second + stride, N);
			if(stride == 1) {
				break;
			}
			stride = stride / stride_decr;
			if(stride > 1)
				stride = 1;

		}

		while(result.second< sweep_position) {
			sweep_backward();
		}
		while(result.second> sweep_position) {
			sweep_forward();
		}

		OptimalSpline s;
		s.selection = evaluate_at_indices(result.first, result.second);
		s.remainder = decombine_variances_ex(total_var, s.selection);
		s.X = featuresX_ord_X[result.first];
		s.Y = featuresX_ord_X[result.first];
		s.loglik = evaluate_spline_loglik(total_var, s.selection);
		return s;
	}
};


#endif /* SRC_EVALUATE_MEAN_VAR_2D_H_ */
