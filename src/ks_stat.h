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

void fill_tree(const vecd & x_coordinates_sorted, const vecd & values) {
	CumulativeTree tree;
	auto order_vec = TreeOrder::tree_order_indices({0, x_coordinates_sorted.size()-1});
	for(auto a: order_vec) {
		tree.insert(x_coordinates_sorted[a], values[a]);
	}
}

struct MaxMin {
	double _max=-1e9, _min=1e9;
	void aggregate_max(double max_candidate){
		_max = std::max(_max, max_candidate);
	}

	void aggregate_min(double min_candidate){
		_min = std::min(_min, min_candidate);
	}
};

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
	mm.aggregate_max(tree.root->cum_max);
	mm.aggregate_min(tree.root->cum_min);
	for(int i = indices_by_y.size() - 1; i >= 0;--i) {
		tree.delete_val(all_x[indices_by_y[i]], all_vals[indices_by_y[i]]);
		mm.aggregate_max(tree.root->cum_max);
		mm.aggregate_min(tree.root->cum_min);

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


#endif /* SRC_KS_STAT_H_ */
