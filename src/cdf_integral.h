
#pragma once
#include<vector>
#include<algorithm>

#include "GenericAVL.h"
#include<map>
#include<set>
#include<scalable/scalable.h>
struct Point {
    double x, y;
    friend std::ostream &operator<<(std::ostream &op, Point p) {
        op << "{x=" << p.x << ", y=" << p.y << "}";

        return op;
    }
};

Point get_rand_point();

struct PointOrderByX {
    bool operator()(Point a, Point b) {
        return a.x < b.x or (a.x == b.x and a.y < b.y);
    }
};
struct PointOrderByY {
    bool operator()(Point a, Point b) {
        return a.y < b.y or (a.y == b.y and a.x < b.x);
    }
};


void test_cdf_integral();

std::vector<double> get_cum_value_sweep(std::vector<Point> points);


struct LabelledPoint {
	Point p;
	int label = 0;
};

inline
std::vector<Point> point_cross_product(std::vector<Point> point) {
	std::set<double> xs, ys;
	std::vector<Point> result;
	for(auto p: point) {
		xs.insert(p.x);
		ys.insert(p.y);
	}
	for(auto x: xs){
		for(auto y: ys) {
			result.push_back(Point{x,y});
		}
	}
	return result;
}
inline
std::vector<double> get_cum_value_sweep_gen(std::vector<Point> empirical_dist, std::vector<Point> evaluation_points) {

	std::vector<LabelledPoint> que;
	for(auto p : empirical_dist) que.push_back(LabelledPoint{p, 1});
	for(auto p : evaluation_points) que.push_back(LabelledPoint{p, 0});
	std::vector<double> result(evaluation_points.size());
    std::vector<int> indices(que.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::stable_sort(indices.begin(), indices.end(), [&que](int i, int j) {
        return que[i].p.x < que[j].p.x;
    });

    GenericAVL<Point, PointOrderByY> avl;
    std::vector<double> values(evaluation_points.size());
    for (int i = indices.size() - 1; i >= 0; i--) {
    	auto point = que[indices[i]];
    	if(point.label == 0) {
			auto lo_bound = avl.getLowerBound(point.p);
			values[indices[i]-empirical_dist.size()] =  1.0 - double(avl.size() - lo_bound + 1) / empirical_dist.size();
    	}
        if(point.label==1) {
        	avl.insert(que[indices[i]].p);
        }
    }
    return values;
}
inline double kolmogorov_simple(std::vector<Point> distrib1,std::vector<Point> distrib2) {
	std::vector<Point>  cross_d1 = point_cross_product(distrib1);
	std::vector<Point>  cross_d2 = point_cross_product(distrib2);
	std::vector<Point> evaluation_points;
	evaluation_points.insert(evaluation_points.end(), cross_d1.begin(), cross_d1.end());
	evaluation_points.insert(evaluation_points.end(), cross_d2.begin(), cross_d2.end());
	auto vals1 = get_cum_value_sweep_gen(distrib1, evaluation_points);
	auto vals2 = get_cum_value_sweep_gen(distrib2, evaluation_points);

	double max_val = 0;
	for(unsigned i =0;i < evaluation_points.size();++i) {
		max_val = std::max(max_val, std::abs(vals1[i]-vals2[i]));
	}
	return max_val;
}

inline double kolmogorov_incorrect(std::vector<Point> distrib1,std::vector<Point> distrib2) {
	std::vector<Point> evaluation_points;
	evaluation_points.insert(evaluation_points.end(), distrib1.begin(), distrib1.end());
	evaluation_points.insert(evaluation_points.end(), distrib2.begin(), distrib2.end());
	auto vals1 = get_cum_value_sweep_gen(distrib1, evaluation_points);
	auto vals2 = get_cum_value_sweep_gen(distrib2, evaluation_points);

	double max_val = 0;
	for(unsigned i =0;i < evaluation_points.size();++i) {
		max_val = std::max(max_val, std::abs(vals1[i]-vals2[i]));
	}
	return max_val;
}
