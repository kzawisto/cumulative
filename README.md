
Evaluation of bivariate 2D Kolmogorov statistics in n log n complexity - you can process few mln samples few orders of magnitude faster than existing
implementations in e.g. "Numerical Recipes".

For details of the algorithm see 

https://github.com/kzawisto/cumulative/blob/master/docs/cumulative.pdf

## Dependencies:

```
https://github.com/kzawisto/np_bridge
https://github.com/kzawisto/cp_tools
https://github.com/kzawisto/scalable
``` 
(C++ headers only, copy to /usr/include).

Boost 1.60, Eigen3.3.2, Cython and Numpy for python bindings.
C++14 compliant compiler.

## Installation:

Install outlined dependencies.
In cloned repository run
```
python setup.py install
```
