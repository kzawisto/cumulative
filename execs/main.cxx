
#include "src/GenericAVL.h"
#include <boost/math/special_functions/erf.hpp>
#include <map>
//#include<cp_tools/cpp_utility.h>
#include<memory>
#include<src/cdf_integral.h>

#include<src/ks_stat.h>

struct Node;
using NodeSP = std::shared_ptr<Node>;
struct Node {

    int value;
    NodeSP  L = nullptr, R = nullptr;
};
template<typename T>
std::map<T , int> get_heights(T root ) {
    std::map<T , int>   m1 ;
    std::vector<T> stack{root}; 
    while(not stack.empty()) {
        T node = stack.back();
        stack.pop_back();
        int h = m1[node];
        if(node->L) {
           m1[node->L] = h+1;
           stack.push_back(node->L);
        }
        if(node->R) {
           m1[node->R] = h+1;
           stack.push_back(node->R);
        }

    }
    return m1;

}
inline void test_nodes() {
    auto ptr3 = NodeSP(new Node{12, nullptr, nullptr});
    auto ptr2 = NodeSP(new Node{10, nullptr, ptr3});
    auto ptr = NodeSP(new Node{0, ptr2, nullptr});
    auto heights = get_heights(NodeSP(new Node{ 1,ptr , nullptr}));

    for(auto it: heights) {
        std::cout<<it.first->value<<","<<it.second<<"\n";
    }
}
inline void test_kolmogorov() {
    srand(11);
    std::vector<Point> points, points2;
    for (int i = 0; i < 1000; ++i) {
        points.push_back(get_rand_point());
        points2.push_back(get_rand_point());
    }
    double k1 = kolmogorov_simple(points, points2);
    double k2 = kolmogorov_incorrect(points, points2);
	auto kstat = get_kstat_ex(points, points2);
    double k3 = std::max(std::abs(kstat._max), std::abs(kstat._min));
    std::cout<<"correct: "<<k1<<" incorrect:"<<k2<<" bst:" <<k3;
}
int main() {
//    test_cdf_integral();
	test_kolmogorov();
}


