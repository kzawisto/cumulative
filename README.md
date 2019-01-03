
Evaluation of bivariate 2D Kolmogorov statistics in n log n complexity - you can process few mln samples few orders of magnitude faster than existing

implementations in e.g. "Numerical Recipes".

For details of the algorithm see 

https://github.com/kzawisto/cumulative/blob/master/docs/cumulative.pdf

Package contains few others statistical tests: bivariate Cramer von Mises test, and a bivariate variance-based statistics.

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

## Usage:

```

import ks2d.ext as ks
import numpy as np
np.random.seed(9)
x1,x2,y1,y2 = [np.random.randn(10000) for _ in range(4)]


stat = ks.get_2d_ks_stat(x1,y1, x2, y2)
print(stat)
```
