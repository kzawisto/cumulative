/*
 * tree_primitives.h
 *
 *  Created on: 8 gru 2018
 *      Author: krystian
 */

#ifndef SRC_TREE_PRIMITIVES_H_
#define SRC_TREE_PRIMITIVES_H_


template<typename Node>
void dealloc_node(Node * node) {
    	if(node) {
    		dealloc_node(node->L);
    		dealloc_node(node->R);
    		delete(node);
    	}
}
template<typename Node, typename Executor>
static void bfs_transversal(Node *n, Executor & exec) {
	if(n) {
		exec(n);
		bfs_transversal(n->L, exec);
		bfs_transversal(n->R, exec);
	}

}
template<typename Node, typename Executor>
static void dfs_transversal(Node *n, Executor & exec) {
	if(n) {

		dfs_transversal(n->L, exec);
		dfs_transversal(n->R, exec);
		exec(n);
	}

}

template<typename Node>
Node * copy_node(const Node * n) {
	if(not n)
		return nullptr;
	Node * copied_n = new Node(*n);
	copied_n->L = copy_node(n->L);
	copied_n->R = copy_node(n->R);
	return copied_n;
}

template<typename Node, typename KeyType, typename ValueType, typename Comparator>
Node *insert_node(Node *n, Comparator & comparator, KeyType key, ValueType value) {
	if (!n)
		return new Node(key, value);
	if (comparator(key, n->key)) {
		n->L = insert_node(n->L,comparator, key, value);
	}
	else if(comparator(n->key, key)) {
		n->R = insert_node(n->R,comparator, key, value);
	}
	else {
		n->accept_extra_value(key, value);
	}
	return n;
}
#endif /* SRC_TREE_PRIMITIVES_H_ */
