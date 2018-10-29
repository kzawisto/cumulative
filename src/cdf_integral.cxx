
#include "GenericAVL.h"
#include<src/cdf_integral.h>
#include <boost/math/special_functions/erf.hpp>
#include<src/cdf_integral.h>
#include <map>
inline double gauss_quantile(double q) {
    return -sqrt(2) * boost::math::erfc_inv(2 * q);
}

std::vector<double> get_cum_value_brute(std::vector<Point> points) {
    std::vector<double> result;
    for (auto p : points) {
        int count = 1;
        for (auto p1 : points) {
            if (p.x < p1.x and p.y < p1.y) {
                count++;
            }
        }
        result.push_back(1.0 - double(count) / points.size());
    }

    return result;
}


struct PointDescription {
	Point p;
	double marginal_x;
    double marginal_y;
    double values;

};
std::vector<double> get_cum_value_sweep(std::vector<Point> points) {
    auto points_cp = points;
    std::vector<int> indices(points.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::stable_sort(indices.begin(), indices.end(), [&points](int i, int j) {
        return points[i].x < points[j].x;
    });
    for (int i = 0; i < points.size(); ++i)
        points_cp[i] = points[indices[i]];

    GenericAVL<Point, PointOrderByY> avl;
    std::map<Point, double, PointOrderByX> values;
    for (int i = points.size() - 1; i >= 0; i--) {
        auto lo_bound = avl.getLowerBound(points_cp[i]);
        values[points_cp[i]] =
            1.0 - double(avl.size() - lo_bound + 1) / points.size();
        avl.insert(points_cp[i]);
    }

    std::vector<double> result;
    for (auto p : points)
        result.push_back(values[p]);
    return result;
}

Point get_rand_point() {
    double x = rand() % 1000000 / 1e6 + 5e-7;
    double y = rand() % 1000000 / 1e6 + 5e-7;
    return Point{.x = gauss_quantile(x), .y = gauss_quantile(y)};
}
void test_cdf_integral() {
    GenericAVL<Point, PointOrderByX> avl;

    std::cout << avl.getLowerBound(Point{1.5, 1.5}) << " ";
    for (int i = 0; i < 1000; ++i)
        avl.insert(get_rand_point());
    std::cout << avl.getLowerBound(Point{1.5, 1.5}) << " ";
    auto p = -sqrt(2) * boost::math::erfc_inv(2 * 0.9985);
    std::cout << p;
    srand(10);
    std::vector<Point> points;
    for (int i = 0; i < 100; ++i)
        points.push_back(get_rand_point());

    auto res = get_cum_value_brute(points);
    auto res2 = get_cum_value_sweep(points);

    auto points2 = points;

    for (int i = 0; i < 100; ++i)
        points2.push_back(get_rand_point());
    auto res3 = get_cum_value_sweep_gen(points, points2);
}
