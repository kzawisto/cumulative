/*
 * rich_avl.h
 *
 *  Created on: Oct 24, 2018
 *      Author: amadeus
 */

#ifndef SRC_RICH_AVL_H_
#define SRC_RICH_AVL_H_

#include<algorithm>
struct Comparator {
	bool operator() (double i , double j)  {
		return i < j;
	}
};
struct RichAVL {



	Comparator comparator;
	using KeyType = double;
    struct Node {
        Node *L, *R;
        int key;
        int h;

        int childrenCount;
        double cum_max = 0;
        double cum_min = 0;

        double cum_right_offset = 0;
        Node(int key)
            : L(nullptr), R(nullptr), key(key), h(1), childrenCount(0) {}
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
    Node *root;

    Node *insertNode(Node *n, KeyType key, double value, double right_offset) {
        if (!n)
            return new Node(key);
        if (comparator(key, n->key)) {
        	n->cum_right_offset +=right_offset;
            n->L = insertNode(n->L, key, value, right_offset);
            set_cum_max_min(n);
        }
        else  {
            n->R = insertNode(n->R, key, value, right_offset);
            set_cum_max_min(n);
        }

        return n;
    }
};




#endif /* SRC_RICH_AVL_H_ */
