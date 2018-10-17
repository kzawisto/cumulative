
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
struct CounterAVL {

    struct Node {
        Node *L, *R;
        double key;
        int h;

        int childrenCount;
        double cum_max = 0;
        double cum_offset = 0;
        Node(double key) : L(NULL), R(NULL), key(key), h(1), childrenCount(0) {}
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
    static Node *insertNode(Node *n, double key) {
        if (!n)
            return new Node(key);
        if (n->key > key)
            n->L = insertNode(n->L, key);
        else if (n->key < key)
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

    static Node *findNode(Node *n, double key) {
        if (!n)
            return n;
        if (key < n->key)
            return findNode(n, key);
    }
    static Node *findMinNode(Node *n) {
        while (n->L)
            n = n->L;
        return n;
    }
    static void copyStuff(Node *fr, Node *to) { to->key = fr->key; }
    static Node *deleteNode(Node *n, double key) {
        if (!n)
            return n;
        if (n->key > key)
            n->L = deleteNode(n->L, key);
        else if (n->key < key)
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

    static bool testOrderedness(const std::vector<int> &vec) {
        int ordered = 1;
        for (unsigned i = 0; i < vec.size() - 1; ++i) {
            ordered &= (vec[i] < vec[i + 1]);
        }
        return ordered;
    }

    static bool randomOrderTest(int N = 100) {
        std::vector<int> vec(100);
        std::iota(vec.begin(), vec.end(), 0);
        std::random_shuffle(vec.begin(), vec.end());

        return true;
    };
    void deconstructTree() {
        while (root) {

            root = deleteNode(root, root->key);
        }
    }

  public:
    CounterAVL() { root = NULL; }
    CounterAVL(double key) { root = insertNode(NULL, key); }
    void insert(double key) { root = insertNode(root, key); }
    void del(double key) { root = deleteNode(root, key); }
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
    int getLowerBound(double k) {
        int offset = 0;
        Node *n = root;

        if (!n or (!n->L && !n->R))
            return 0;
        while (true) {
            int offset1 = getChildrenCount(n->L) + (bool)(n->L);

            if (k < n->key) {
                if (n->L)
                    n = n->L;
                else
                    return offset1 + offset;
            } else if (k > n->key) {
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
    int getUpperBound(double k) {
        int offset = 0;
        Node *n = root;

        if (!n or (!n->L && !n->R))
            return 0;
        while (true) {
            int offset1 = getChildrenCount(n->L) + (bool)(n->L);

            if (k < n->key) {
                if (n->L)
                    n = n->L;
                else
                    return offset1 + offset;
            } else if (k > n->key) {
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

    double getQuantile(double q) {
        int N = floor(q * size() + 0.5);

        if (N >= size())
            N = size() - 1;
        return getNth(N)->key;
    }
    double getQuantileExact(double q) {
        double N = (q * size());
        if (1e-2 < abs(N - floor(N + 0.5))) {
            if ((int)N + 1 < size()) {
                double q1 = N - (int)N;
                return getNth(N)->key * (1 - q1) + getNth(N + 1)->key * q1;
            }
        }
        return getQuantile(q);
    }

    ~CounterAVL() { deconstructTree(); }
};
