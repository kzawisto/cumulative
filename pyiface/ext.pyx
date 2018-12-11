

cimport cython
from numpy cimport import_array
import_array()
     
cdef extern from "pyiface/ks_test_iface.h":
    cdef object ks_test(object, object, object, object)
  
cdef extern from "pyiface/spline_iface.h":
    cdef object get_optimal_spline(object, object, object)  
@cython.embedsignature(True)
def get_2d_ks_stat(x1, y1, x2, y2):
    return ks_test(x1, y1, x2, y2)

@cython.embedsignature(True)
def get_2d_optimal_spline(X, y, response):
    return get_optimal_spline(X, y, response)