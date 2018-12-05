/*
 * ks_stat.h
 *
 *  Created on: 27 paź 2018
 *      Author: krystian
 */

#ifndef SRC_KS_STAT_H_
#define SRC_KS_STAT_H_
#include<vector>
#include<cassert>
#include<exception>
#include<src/tools.h>
#include<src/static_cumulative_tree.h>
#include<algorithm>
#include<src/Point.h>
#include <type_traits>

void fill_tree(const vecd & x_coordinates_sorted, const vecd & values) {
	CumulativeTree tree;
	auto order_vec = TreeOrder::tree_order_indices({0, x_coordinates_sorted.size()-1});
	for(auto a: order_vec) {
		tree.insert(x_coordinates_sorted[a], values[a]);
	}
}

struct MaxMin {
	double _max=-1e9, _min=1e9;
	double max_loc_y = -1, max_loc_x = -1;
	double min_loc_y = -1, min_loc_x = -1;

	void aggregate_max(double max_candidate, double loc_x_new, double loc_y_new){
		if(_max < max_candidate) {
			_max = max_candidate;
			max_loc_x = loc_x_new;
			max_loc_y = loc_y_new;
		}
	}

	void aggregate_min(double min_candidate, double loc_x_new, double loc_y_new){
		if(_min > min_candidate) {
			_min = min_candidate;
			min_loc_x = loc_x_new;
			min_loc_y = loc_y_new;
		}
	}
};

template<typename T>
auto rev_at(T & container, decltype(std::declval<T>().size()) idx)-> decltype(std::declval<T>().at(0)){
	return container.at(container.size() - idx);

}
inline
MaxMin get_kstat(const vecd & x_s1,const vecd & y_s1, const vecd & x_s2,const vecd & y_s2) {
	if(x_s1.size() != y_s1.size()) throw std::logic_error("s1 sample dimmensions do not match");
	if(x_s2.size() != y_s2.size()) throw std::logic_error("s2 sample dimmensions do not match");

	double s1_value = 1.0 / x_s1.size();
	double s2_value = - 1.0 / x_s2.size();
	vecd all_x, all_y, all_vals;
	for(long unsigned i = 0;i < x_s1.size();++i) {
		all_x.push_back(x_s1[i]);
		all_y.push_back(y_s1[i]);
		all_vals.push_back(s1_value);
	}

	for(long unsigned i = 0;i < x_s2.size();++i) {
		all_x.push_back(x_s2[i]);
		all_y.push_back(y_s2[i]);
		all_vals.push_back(s2_value);
	}
	vecl indices_by_x = get_range_until(0, all_x.size());
	std::stable_sort(indices_by_x.begin(), indices_by_x.end(), [&all_x](long i, long j) {return all_x[i] < all_x[j];});
	vecd all_x_by_x = get_selector(all_x, indices_by_x);
	vecd all_vals_by_val = get_selector(all_vals, indices_by_x);

	CumulativeTree tree;
	auto order_vec = TreeOrder::tree_order_indices({0, all_x_by_x.size()-1});
	for(auto a: order_vec) {
		tree.insert(all_x_by_x[a], all_vals_by_val[a]);
	}

	CumulativeTree::inorder_transversal_set_cumsum(tree.root,0);
	CumulativeTree::transversal_set_extrema(tree.root);

	vecl indices_by_y = get_range_until(0, all_y.size());
	std::stable_sort(indices_by_y.begin(), indices_by_y.end(), [&all_y](long i, long j) {return all_y[i] < all_y[j];});

	MaxMin mm;
	mm.aggregate_max(tree.root->cum_max, tree.root->loc_max, all_y[rev_at(indices_by_y,1)]);
	mm.aggregate_min(tree.root->cum_min, tree.root->loc_min, all_y[rev_at(indices_by_y,1)]);
	for(int i = indices_by_y.size() - 1; i >= 0;--i) {
		auto idx = indices_by_y[i];
		tree.delete_val(all_x[idx], all_vals[idx]);
		mm.aggregate_max(tree.root->cum_max, tree.root->loc_max, all_y[idx]);
		mm.aggregate_min(tree.root->cum_min, tree.root->loc_min, all_y[idx]);

	}

	return mm;
}


inline MaxMin get_kstat_ex(const std::vector<Point> & s1, const std::vector<Point> & s2){
	vecd x_s1, x_s2, y_s1, y_s2;
	x_s1.reserve(s1.size());
	y_s1.reserve(s1.size());
	x_s2.reserve(s2.size());
	y_s2.reserve(s2.size());
	for(auto p: s1) {
		x_s1.push_back(p.x);
		y_s1.push_back(p.y);
	}
	for(auto p: s2) {
		x_s2.push_back(p.x);
		y_s2.push_back(p.y);
	}
	return get_kstat(x_s1, y_s1, x_s2, y_s2);
}
inline std::vector<double> query_vals(
		std::vector<double> vec_X_vals,
		std::vector<double> vec_Y_vals,
		std::vector<double> vec_vals,
		std::vector<double> vec_X_evaluation,
		std::vector<double> vec_Y_evaluation
) {

}


#endif /* SRC_KS_STAT_H_ */
