/*
 * ks_test_iface.h
 *
 *  Created on: Oct 29, 2018
 *      Author: amadeus
 */

#ifndef PYIFACE_KS_TEST_IFACE_H_
#define PYIFACE_KS_TEST_IFACE_H_

#include<np_bridge/np_bridge.h>
#include<src/ks_stat.h>

PyObject * ks_test(PyObject * x1, PyObject * y1, PyObject * x2, PyObject * y2) {
	np_bridge::NumpyArrayView<double> view_x1((PyArrayObject*)x1),
			view_y1((PyArrayObject*)y1),
			view_x2((PyArrayObject*)x2),
			view_y2((PyArrayObject*)y2);
	if(view_x1.size() != view_y1.size())throw std::logic_error("wrong dimmensions at first argument.");
	if(view_x2.size() != view_y2.size())throw std::logic_error("wrong dimmensions at second argument.");
	std::vector<double> vec_x1, vec_x2;
	std::vector<double> vec_y1, vec_y2;
	for(long i = 0;i < view_x1.size(); i++) {
		vec_x1.push_back(view_x1.at(i));
		vec_y1.push_back(view_y1.at(i));
	}
	for(long i = 0;i < view_x1.size(); i++) {
		vec_x2.push_back(view_x2.at(i));
		vec_y2.push_back(view_y2.at(i));
	}

	auto result = get_kstat(vec_x1, vec_y1, vec_x2, vec_y2);
	PyObject *d = PyDict_New();
	PyDict_SetItemString(d, "max", PyFloat_FromDouble(result._max));
	PyDict_SetItemString(d, "min", PyFloat_FromDouble(result._min));
	PyDict_SetItemString(d, "max_loc_y", PyFloat_FromDouble(result.max_loc_y));
	PyDict_SetItemString(d, "max_loc_x", PyFloat_FromDouble(result.max_loc_x));
	PyDict_SetItemString(d, "min_loc_y", PyFloat_FromDouble(result.min_loc_y));
	PyDict_SetItemString(d, "min_loc_x", PyFloat_FromDouble(result.min_loc_x));
	return d;
}



#endif /* PYIFACE_KS_TEST_IFACE_H_ */
