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
struct CumulativeTree {

	Comparator comparator;
	using KeyType = double;
	using ValueType = double;
    struct Node {
        Node *L=nullptr, *R=nullptr;
        Node * father = nullptr;
        KeyType key;
        ValueType value=0;
        int h;
        int counter = 1;

        int childrenCount;
        double sum_of_vals = 0;
        double cum_max = 0;
        double cum_min = 0;

        double cum_right_offset = 0;
        Node(KeyType key, ValueType value)
            : L(nullptr), R(nullptr), key(key), value(value), h(1), childrenCount(0) {}
    };

    static int getH(Node *n) {
        if (!n)
            return 0;
        else
            return n->h;
    }
    static void set_cum_max_min(Node *n) {
    	n->cum_max = n->cum_right_offset;
    	n->cum_min = n->cum_right_offset;
    	if(n->L) {
    		n->cum_max = std::max(n->cum_max, n->L->cum_max);
    		n->cum_min = std::min(n->cum_min, n->L->cum_min);
    	}
    	if(n->R) {
    		n->cum_max = std::max(n->cum_max, n->cum_right_offset + n->R->cum_max);
    		n->cum_min = std::min(n->cum_min, n->cum_right_offset + n->R->cum_min);
    	}
    }
  public:
    Node *root=nullptr;

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
    static void dealloc(Node * node) {
    	if(node) {
    		dealloc(node->L);
    		dealloc(node->R);
    		delete(node);
    	}
    }
    ~CumulativeTree () {
    	dealloc(root);
    }
    static void inorder_transversal(Node * n) {
    	if(n) {
    		inorder_transversal(n->L);
    		std::cout<<"{"<< n->key << " "<<n->value<<"} ";
    		inorder_transversal(n->R);

    	}
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
				n->cum_max = std::max(n->cum_max, n->value);

				n->cum_min = std::min(n->cum_min, n->value);
			}
    		if(n->L) {
    			offset = n->value + n->L->sum_of_vals;
				n->cum_max = std::max(n->cum_max, offset);

				n->cum_min = std::min(n->cum_min, offset);

				n->cum_max = std::max(n->cum_max, n->L->cum_max);

				n->cum_min = std::min(n->cum_min, n->L->cum_min);
    		}
    		if(n->R) {
				n->cum_max = std::max(n->cum_max, offset + n->R->cum_max);
				n->cum_min = std::min(n->cum_min, offset + n->R->cum_min);
    		}
    }

    template<typename Executor>
    static void bfs_transversal(Node *n, Executor & exec) {
    	if(n) {

    		exec(n);
    		bfs_transversal(n->L, exec);
    		bfs_transversal(n->R, exec);
    	}

    }
    template<typename Executor>
    static void dfs_transversal(Node *n, Executor & exec) {
    	if(n) {

    		dfs_transversal(n->L, exec);
    		dfs_transversal(n->R, exec);
    		exec(n);
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





#endif /* SRC_STATIC_CUMULATIVE_TREE_H_ */
