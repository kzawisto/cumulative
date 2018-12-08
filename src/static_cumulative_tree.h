/*
 * rich_avl.h
 *
 *  Created on: Oct 24, 2018
 *      Author: amadeus
 */

#ifndef SRC_STATIC_CUMULATIVE_TREE_H_
#define SRC_STATIC_CUMULATIVE_TREE_H_

#include<algorithm>
#include<iostream>

#include<boost/optional.hpp>
#include<utility>
#include<deque>
#include<src/tree_primitives.h>


struct Comparator {
	bool operator() (double i , double j) const {
		return i < j;
	}
};
struct TreeOrder {

struct Interval {
	boost::optional<std::pair<long, long>> left, right;
	long middle;


};


static Interval split_interval(std::pair<long, long> pair1) {
	Interval in;
	in.middle =(pair1.first + pair1.second) / 2;

	if(in.middle > pair1.first)
		in.left = std::make_pair(pair1.first, in.middle-1);
	if(in.middle < pair1.second)
		in.right = std::make_pair(in.middle+1, pair1.second);
	return in;

}

static std::vector<long> tree_order_indices(std::pair<long, long> indices_range) {

	std::deque<std::pair<long, long>> spaces;
	spaces.push_back(indices_range);
	std::vector<long> result;
	while(not spaces.empty()) {
		auto p = spaces.front();
		spaces.pop_front();
		auto interval =split_interval(p);
		result.push_back(interval.middle);
		if(interval.left) {
			spaces.push_back(interval.left.get());
		}
		if(interval.right) {
			spaces.push_back(interval.right.get());
		}
	}
	return result;

}
static std::vector<double> tree_order(std::vector<double> sorted_vector) {

	std::deque<std::pair<long, long>> spaces;
	spaces.push_back({0, sorted_vector.size()-1});
	std::vector<double> result;
	while(not spaces.empty()) {
		auto p = spaces.front();
		spaces.pop_front();
		auto interval =split_interval(p);
		result.push_back(sorted_vector[interval.middle]);
		if(interval.left) {
			spaces.push_back(interval.left.get());
		}
		if(interval.right) {
			spaces.push_back(interval.right.get());
		}
	}
	return result;

}
};
inline
void set_max_and_loc(double & old_max, double & old_loc, double max_candidate, double candicate_loc) {
	if(old_max < max_candidate) {
		old_max = max_candidate;
		old_loc = candicate_loc;
	}
}

inline
void set_min_and_loc(double & old_min, double & old_loc, double min_candidate, double candicate_loc) {
	if(old_min > min_candidate) {
		old_min = min_candidate;
		old_loc = candicate_loc;
	}
}

struct CumulativeTree {

	Comparator comparator;
	using KeyType = double;
	using ValueType = double;
    struct Node {
        Node *L=nullptr, *R=nullptr;
        KeyType key;
        ValueType value=0;
        int counter = 1;

        double sum_of_vals = 0;
        double cum_max = 0;
        double cum_min = 0;
        double loc_max=0;
        double loc_min=0;

        double cum_right_offset = 0;
        Node(KeyType key, ValueType value)
            : L(nullptr), R(nullptr), key(key), value(value) {}

        Node(const Node &n){
        	L = nullptr;
        	R = nullptr;
        	key = n.key;
        	value = n.value;
        	counter = n.counter;
			sum_of_vals = n.sum_of_vals;
			cum_max = n.cum_max;
			cum_min = n.cum_min;
			loc_max= n.loc_max;
			loc_min= n.loc_min;
			cum_right_offset = n.cum_right_offset;
        }
        void accept_extra_value(KeyType key, ValueType v) {
        	value += v;
        	counter ++;
        }
    };

  public:
    Node *root=nullptr;
    CumulativeTree() {

    }

    CumulativeTree(const CumulativeTree & tree) {
    	root = copy_node(tree.root);

    	std::cerr<<"copy ";
    }

    CumulativeTree(CumulativeTree && tree) {
    	root = tree.root;
    	tree.root = nullptr;
    	std::cerr<<"move ";
    }


    Node *insertNode(Node *n, KeyType key, double value) {
        if (!n)
            return new Node(key, value);
        if (comparator(key, n->key)) {
            n->L = insertNode(n->L, key, value);
            //set_cum_max_min(n);
        }
        else if(comparator(n->key, key)) {
            n->R = insertNode(n->R, key, value);
            //set_cum_max_min(n);
        }
        else {
        	n->value += value;
        	n->counter ++;
        }
        return n;
    }
    void insert(double key, double value) {
    	root = insertNode(root, key, value);
    }

    ~CumulativeTree () {
    	dealloc_node(root);
    }
    static void inorder_transversal(Node * n) {
    	if(n) {
    		inorder_transversal(n->L);
    		std::cout<<"{"<< n->key << " "<<n->value<<"} ";
    		inorder_transversal(n->R);

    	}
    }
    static double query_upper_bound_impl(Node * n, double value, double offset) {


		double v=  n->value + offset;
    	if(value < n->key) {
    		if(n->L) {
    			return query_upper_bound_impl(n->L, value, offset);
    		} else return offset;
    	}
    	if(n->L) {
			v+= n->L->sum_of_vals;
    	}
    	if(value > n->key) {

    		if(n->R) {
    			return query_upper_bound_impl(n->R, value, v);
    		} else {
    			return v;
    		}
    	}

   		return v;
    }
    double query_upper_bound(double value) {
    	return query_upper_bound_impl(root, value, 0);
    }

    static ValueType inorder_transversal_set_cumsum(Node * n, double offset) {
    	if(n) {
    		double cum_left = inorder_transversal_set_cumsum(n->L, offset);

    		double cum_right = inorder_transversal_set_cumsum(n->R, cum_left + offset + n->value);
    		n->sum_of_vals = cum_left + cum_right + n->value;
    		return cum_left + cum_right + n->value;

    	}
    	return 0;
    }

    static void reset_extrema(Node * n) {
			n->cum_max = -1e+9;
			n->cum_min = 1e+9;
			double offset= n->value;
			if(not n->L) {
				set_max_and_loc(n->cum_max, n->loc_max, n->value, n->key);
				set_min_and_loc(n->cum_min, n->loc_min, n->value, n->key);
			}
			else {
    			offset = n->value + n->L->sum_of_vals;
				set_max_and_loc(n->cum_max, n->loc_max, offset , n->key);
				set_min_and_loc(n->cum_min, n->loc_min, offset, n->key);
				set_max_and_loc(n->cum_max, n->loc_max, n->L->cum_max, n->L->loc_max);
				set_min_and_loc(n->cum_min, n->loc_min, n->L->cum_min, n->L->loc_min);
    		}
    		if(n->R) {
				set_max_and_loc(n->cum_max, n->loc_max, offset + n->R->cum_max, n->R->loc_max);
				set_min_and_loc(n->cum_min, n->loc_min, offset + n->R->cum_min, n->R->loc_min);
    		}
    }

    static void transversal_set_extrema(Node * n) {
    	auto visitor = [] (Node * n){reset_extrema(n);};
    	dfs_transversal(n, visitor);
    }

    void delete_val_impl(Node *n, KeyType key, ValueType value) const {
    	if(n) {
    	   if (comparator(key, n->key)) {
				delete_val_impl(n->L, key, value);
			}
			else if(comparator(n->key, key)) {
				delete_val_impl(n->R, key, value);
			}
			else {
				n->value -= value;
				n->counter--;
			}

			n->sum_of_vals -= value;
    	   reset_extrema(n);
    	}
    }

    void delete_val(KeyType key, ValueType value) {
    	delete_val_impl(root, key, value);
    }
};

template<typename T>
CumulativeTree build_cumulative_tree(T indices, T values) {
	CumulativeTree tree;
	auto order_vec = TreeOrder::tree_order_indices({0, values.size()-1});
	for(auto a: order_vec) {
		tree.insert(indices[a], values[a]);
	}
	CumulativeTree::inorder_transversal_set_cumsum(tree.root,0);
	CumulativeTree::transversal_set_extrema(tree.root);
	return tree;
}


#endif /* SRC_STATIC_CUMULATIVE_TREE_H_ */
