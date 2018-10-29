
import ks2d.ext as ks
import numpy as np
x1,x2,y1,y2 = [np.random.randn(10000) for _ in range(4)]

print(
    ks.get_2d_ks_stat(x1,y1, x2, y2))