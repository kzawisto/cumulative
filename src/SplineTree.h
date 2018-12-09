/*
 * SplineTree.h
 *
 *  Created on: 8 gru 2018
 *      Author: krystian
 */

#ifndef SRC_SPLINETREE_H_
#define SRC_SPLINETREE_H_

#include<cp_tools/numeric_utility.h>
#include<cp_tools/ostream_ops.h>
#include "src/tree_primitives.h"
#include<cmath>

using namespace cp_tools;
template<typename T>
MeanAndVariance<T> decombine_variances(MeanAndVariance<T> one, MeanAndVariance<T> oth) {
	long n2 = (one.count - oth.count);
	double mean_new = (one.count * one.mean - oth.mean * oth.count) / n2;
	double variance_new = (
			((one.count-1) * one.variance - (oth.count - 1)*oth.variance - oth.count * sq(one.mean - oth.mean)
					- n2 *sq(one.mean - mean_new)
					)/(n2 - 1	)
			);
	return MeanAndVariance<T>{
		.mean = mean_new, .variance = variance_new, .count = n2
	};
}

template<typename T>
MeanAndVariance<T> decombine_variances_ex(MeanAndVariance<T> one, MeanAndVariance<T> oth) {
	if(oth.count==0){
		return one;
	}
	if(one.count==2 and oth.count == 1) {
		auto result = decombine_variances(one, oth);
		result.variance = 0;
		return result;
	}
	if(one.count==1 and oth.count == 1) {
		assert(std::abs(oth.mean - one.mean) < 1e-7);
		return MeanAndVariance<T>{0,0,0};
	}
	if(one.count==0) {
		throw std::logic_error("subtracting from empty");
	}
	return decombine_variances(one, oth);
}
template<typename T>
MeanAndVariance<T> combine_variances_ex(MeanAndVariance<T> one, MeanAndVariance<T> oth) {
	if(oth.count==0){
		return one;
	}
	if(one.count==0) {
		return oth;
	}
	return combine_variance(one, oth);
}

struct SplineTreeComparator {
	bool operator() (double i , double j) const {
		return i < j;
	}
};
struct SplineTree {

	SplineTreeComparator cmp;
	using KeyType = double;
	using ValueType = double;
    struct Node {
        Node *L=nullptr, *R=nullptr;
        KeyType key;
        ValueType value=0;
        MeanAndVariance<ValueType> mean_var, intr_mean_var;
        Node(KeyType key, ValueType value)
            : L(nullptr), R(nullptr), key(key), value(value), mean_var{value, 0, 1}, intr_mean_var(mean_var) {


        }

        Node(const Node &n): Node(n.key, n.value){


        }
		void accept_extra_value(KeyType k, ValueType v) {
			intr_mean_var = combine_variances_ex(intr_mean_var, MeanAndVariance<ValueType> {v, 0, 1});
		}
    };

    Node * root = nullptr;
    static void dealloc(Node * node) {
    	if(node) {
    		dealloc(node->L);
    		dealloc(node->R);
    		delete(node);
    	}
    }

    ~SplineTree() {
    	dealloc(root);
    }

    SplineTree() {

    }

    SplineTree(const SplineTree & other) {
    	root=copy_node(other.root);
    }

    SplineTree(SplineTree && other) {
    	root = other.root;
    	other.root = nullptr;
    }

    void operator=(const SplineTree & other) {
    	dealloc_node(root);
    	root=copy_node(other.root);
    }

    void operator=(SplineTree && other) {
    	dealloc_node(root);
    	root=other.root;
    	other.root = nullptr;
    }

    void insert(double key, double value) {
    	root = insert_node(root, cmp, key, value);
    }

    void delete_val_impl(Node *n, KeyType key, ValueType value) const {
    	if(n) {
    	   if (cmp(key, n->key)) {
				delete_val_impl(n->L, key, value);
			}
			else if(cmp(n->key, key)) {
				delete_val_impl(n->R, key, value);
			}
			else {
				 n->intr_mean_var = decombine_variances_ex(n->intr_mean_var, MeanAndVariance<ValueType>{value, 0,1});
			}

    		n->mean_var = n->intr_mean_var;
    		if(n->L)
				   n->mean_var = combine_variances_ex(n->mean_var, n->L->mean_var);
    		if(n->R)
				n->mean_var = combine_variances_ex(n->mean_var, n->R->mean_var);
    	}
    }

    void delete_val(KeyType key, ValueType value) {
    	delete_val_impl(root, key, value);
    }
    static void set_mean_var_values_impl(Node * n) {
    	if(n) {
    		n->mean_var = n->intr_mean_var;
    		if(n->L) {
    			set_mean_var_values_impl(n->L);
    			n->mean_var = combine_variances_ex(n->mean_var, n->L->mean_var);
    		}
    		if(n->R) {
    			set_mean_var_values_impl(n->R);
    			n->mean_var = combine_variances_ex(n->mean_var, n->R->mean_var);
    		}
    	}
    }
    MeanAndVariance<ValueType> query_var_above_or_eq_impl(Node*n,KeyType q) {
    	if(not n) {
    		return MeanAndVariance<ValueType>{0,0,0};
    	}

		 if(cmp(n->key, q)) {
    		return query_var_above_or_eq_impl(n->R, q);
    	} else {
    		auto mv = n->intr_mean_var;
    		if(n->R)
    			mv = combine_variances_ex(mv, n->R->mean_var);
    		return combine_variances_ex(mv,query_var_above_or_eq_impl(n->L, q));
    	}
    }
    MeanAndVariance<ValueType> query_var_below_impl(Node*n,KeyType q) {
    	if(not n) {
    		return MeanAndVariance<ValueType>{0,0,0};
    	}
		 if(cmp(q, n->key)) {
    		auto mv = n->intr_mean_var;
    		if(n->R)
    			mv = combine_variances_ex(mv, n->R->mean_var);
    		return combine_variances_ex(mv,query_var_below_impl(n->L, q));
    	} else {
    		return query_var_below_impl(n->R, q);
    	}
    }
	MeanAndVariance<ValueType> query_var_below(KeyType q) {
		return query_var_below_impl(root,q);
	}
	MeanAndVariance<ValueType> query_var_above_or_eq(KeyType q) {
		return query_var_above_or_eq_impl(root,q);
	}
	MeanAndVariance<ValueType> query_interval(KeyType fr, KeyType to) {
		assert(fr <=to);
		return decombine_variances_ex(
				query_var_above_or_eq(fr), query_var_above_or_eq(to)
		);

	}

    void set_mean_var_values() {
    	set_mean_var_values_impl(root);
    }


};



#endif /* SRC_SPLINETREE_H_ */
