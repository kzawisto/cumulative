

#pragma once
#include <iostream>

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <vector>
//#include<calculus/cpp_utils.h>
//#include<calculus/boost_utils.h>
using namespace std;
template <typename KeyType, typename Comparator = std::less<KeyType>>
struct GenericAVL {

    Comparator comparator;

    struct Node {
        Node *L, *R;
        KeyType key;
        int h;

        int childrenCount;
        double cum_max = 0;
        double cum_offset = 0;
        Node(KeyType key)
            : L(NULL), R(NULL), key(key), h(1), childrenCount(0) {}
    };

  public:
    Node *root;

  private:
    static int getH(Node *n) {
        if (!n)
            return 0;
        else
            return n->h;
    }

    static int getBalance(Node *n) { return getH(n->L) - getH(n->R); }

    static int getChildrenCount(Node *n) {
        if (!n)
            return 0;
        else
            return n->childrenCount;
    }
    static void updateChildrenCount(Node *n) {
        n->childrenCount = getChildrenCount(n->L) + getChildrenCount(n->R) +
                           bool(n->R) + bool(n->L);
    };
    static void updateH(Node *n) {
        n->h = 1 + max(getH(n->L), getH(n->R));
        updateChildrenCount(n);
    }

    static Node *rightRotation(Node *z) {
        Node *x = z->L;
        z->L = x->R;
        x->R = z;
        updateH(z);
        updateH(x);

        return x;
    }
    static Node *leftRotation(Node *z) {
        Node *x = z->R;
        z->R = x->L;
        x->L = z;
        updateH(z);
        updateH(x);
        return x;
    }
    static Node *balanceNode(Node *n) {
        int balance = getBalance(n);
        if (balance > 1 && getBalance(n->L) >= 0)
            return rightRotation(n);
        if (balance > 1 && getBalance(n->L) < 0) {
            n->L = leftRotation(n->L);
            return rightRotation(n);
        }
        if (balance < -1 && getBalance(n->R) <= 0)
            return leftRotation(n);
        if (balance < -1 && getBalance(n->R) > 0) {
            n->R = rightRotation(n->R);
            return leftRotation(n);
        }
        return n;
    }
    Node *insertNode(Node *n, KeyType key) {
        if (!n)
            return new Node(key);
        if (comparator(key, n->key))
            n->L = insertNode(n->L, key);
        else if (comparator(n->key, key))
            n->R = insertNode(n->R, key);
        else {
            n->R = insertNode(n->R, key);
            //    return n;
            // key+=1e-9;
            // n->R = insertNode(n->R , key);
            //  return n;
        }
        updateH(n);
        return balanceNode(n);
    }

    Node *findNode(Node *n, KeyType key) {
        if (!n)
            return n;
        if (comparator(key, n->key))
            return findNode(n, key);
    }
    static Node *findMinNode(Node *n) {
        while (n->L)
            n = n->L;
        return n;
    }
    static void copyStuff(Node *fr, Node *to) { to->key = fr->key; }
    Node *deleteNode(Node *n, KeyType key) {
        if (!n)
            return n;
        if (comparator(key, n->key))
            n->L = deleteNode(n->L, key);
        else if (comparator(n->key, key))
            n->R = deleteNode(n->R, key);
        else {
            if (!n->L && !n->R) {
                delete n;
                return NULL;
            } else if (!n->L || !n->R) {
                if (n->L) {
                    Node *b = n->L;
                    *n = *(n->L);
                    delete b;
                } else {
                    Node *b = n->R;
                    *n = *(n->R);
                    delete b;
                }
            } else {
                Node *t = findMinNode(n->R);
                copyStuff(t, n);
                n->R = deleteNode(n->R, t->key);
            }
        }
        if (!n)
            return n;
        updateH(n);
        return balanceNode(n);
    }

    static vector<Node *> getOrdered(Node *n) {
        vector<Node *> output;
        vector<Node *> dfsstack;
        Node *current = n;
        do {
            while (current) {
                dfsstack.push_back(current);
                current = current->L;
            }
            Node *b = dfsstack.back();
            dfsstack.pop_back();
            output.push_back(b);
            if (b->R) {
                current = b->R;
            }

        } while (!dfsstack.empty() or current);
        return output;
    };

    void deconstructTree() {
        while (root) {

            root = deleteNode(root, root->key);
        }
    }

  public:
    GenericAVL(Comparator cmp) : comparator(cmp) { root = NULL; }

    GenericAVL() { root = NULL; }

    void insert(KeyType key) { root = insertNode(root, key); }
    void del(KeyType key) { root = deleteNode(root, key); }
    int size() {
        if (!root)
            return 0;
        else
            return root->childrenCount + 1;
    }
    Node *getNth(int N) {
        int offset = 0;
        Node *n = root;
        if (!n or (!n->L && !n->R))
            return n;
        while (true) {

            int offset1 = getChildrenCount(n->L) + (bool)(n->L);
            if (offset1 + offset > N) {
                n = n->L;
            } else if (offset1 + offset < N) {
                offset += offset1 + 1;
                n = n->R;
            } else
                return n;
        }
    }
    int getLowerBound(KeyType k) {
        int offset = 0;
        Node *n = root;

        if (!n or (!n->L && !n->R))
            return 0;
        while (true) {
            int offset1 = getChildrenCount(n->L) + (bool)(n->L);

            if (comparator(k, n->key)) {
                if (n->L)
                    n = n->L;
                else
                    return offset1 + offset;
            } else if (comparator(n->key, k)) {
                offset += offset1 + 1;
                if (n->R)
                    n = n->R;
                else
                    return offset;
            } else {
                return offset1 + offset;
            }
        }
    }
    int getUpperBound(KeyType k) {
        int offset = 0;
        Node *n = root;

        if (!n or (!n->L && !n->R))
            return 0;
        while (true) {
            int offset1 = getChildrenCount(n->L) + (bool)(n->L);

            if (comparator(k, n->key)) {
                if (n->L)
                    n = n->L;
                else
                    return offset1 + offset;
            } else if (comparator(k, n->key)) {
                offset += offset1 + 1;
                if (n->R)
                    n = n->R;
                else
                    return offset;
            } else {
                return offset1 + offset + 1;
            }
        }
    }
};
