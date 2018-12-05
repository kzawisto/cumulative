
import ks2d.ext as ks
import numpy as np
#6
np.random.seed(9)
x1,x2,y1,y2 = [np.random.randn(10000) for _ in range(4)]


stat = ks.get_2d_ks_stat(x1,y1, x2, y2)


stat

import pandas as pd
pd.cut(np.random.rand(1000),bins=[0,0.2,0.5,0.7,1.0])


catsxl = pd.cut(x2, bins =[-1e9,x_l,1e9])
catsyl = pd.cut(y2, bins =[-1e9,y_l,1e9])
catsxl1 = pd.cut(x1, bins =[-1e9,x_l,1e9])
catsyl1 = pd.cut(y1, bins =[-1e9,y_l,1e9])
a,b =pd.crosstab(catsxl, catsyl),pd.crosstab(catsxl1, catsyl1)
    
a,b, a.values-b.values


x_h, y_h =stat["max_loc_x"] ,stat["max_loc_y"]
x_l, y_l =stat["min_loc_x"] ,stat["min_loc_y"]
catsxh = pd.cut(x2, bins =[-1e9,x_h,1e9])
catsyh = pd.cut(y2, bins =[-1e9,y_h,1e9])
catsxh1 = pd.cut(x1, bins =[-1e9,x_h,1e9])
catsyh1 = pd.cut(y1, bins =[-1e9,y_h,1e9])
print(
    pd.crosstab(catsxh, catsyh) - pd.crosstab(catsxh1, catsyh1)
)

catsx1 = pd.cut(x1, bins =[-1e9,min(x_h,x_l),max(x_h,x_l),1e9])
catsy1 = pd.cut(y1, bins =[-1e9,min(y_h,y_l),max(y_h,y_l),1e9])

pd.crosstab(catsx1, catsy1)

catsx2 = pd.cut(x2, bins =[-1e9,min(x_h,x_l),max(x_h,x_l),1e9])
catsy2 = pd.cut(y2, bins =[-1e9,min(y_h,y_l),max(y_h,y_l),1e9])

pd.crosstab(catsx2, catsy2)


pd.crosstab(catsx2, catsy2) - pd.crosstab(catsx1, catsy1) 


pd.crosstab(catsx2, catsy2) / pd.crosstab(catsx1, catsy1) 



0.463 - 0.001
#x_h, x_l = max(x_h,x_l), min(x_h,x_l)

#y_h, y_l = max(y_h,y_l), min(y_h,y_l)

#y_l-=0.005

stat


sum((x1 > x_l) & (y1 > y_l) ),sum((x2 > x_l) & (y2 > y_l) )


sum((x1 > x_h) & (y1 > y_h) ),sum((x2 > x_h) & (y2 > y_h) )

sum((x1 > x_h) & (y1 > y_h) ),sum((x2 > x_h) & (y2 > y_h) )


sum((x1 > x_l) & (y1 > y_l) & (y1 < y_h) & (x1<x_h)),sum((x2 > x_l) & (y2 > y_l) & (y2 < y_h) & (x2<x_h))


a,b = sum(((x1 < x_h) & (x1> x_l) & (y1 < y_h) & (y1 > y_l))),sum(((x2 < x_h) & (x2> x_l) & (y2 < y_h) & (y2 > y_l)))
print(
    a,b,a-b
)


a, b=sum(((x1 < x_h) & (y1 < y_h) )), sum(((x2 < x_h)  & (y2 < y_h)))
print(a, b,a-b
)


a,b = sum(((x1 > x_h) & (y1 < y_h) )), sum(((x2 > x_h)  & (y2 < y_h)))
print(
    a,b,a-b
)


