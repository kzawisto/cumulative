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

#include <src/evaluate_mean_var_2d.h>
#include<pyiface/commons.h>


PyObject * ks_test(PyObject * x1, PyObject * y1, PyObject * x2, PyObject * y2) {

	std::vector<double> vec_x1 = obj_to_vec<double>(x1), vec_x2 = obj_to_vec<double>(x2);
	std::vector<double> vec_y1 = obj_to_vec<double>(y1), vec_y2 = obj_to_vec<double>(y2);
	if(vec_x1.size() != vec_y1.size())throw std::logic_error("wrong dimmensions at first argument.");
	if(vec_x2.size() != vec_y2.size())throw std::logic_error("wrong dimmensions at second argument.");
	auto result = get_kstat(vec_x1, vec_y1, vec_x2, vec_y2);
	PyObject *d = PyDict_New();
	set_dict_key(d, "max", result._max);
	set_dict_key(d, "min", result._min);
	 set_dict_key(d, "max_loc_y", result.max_loc_y);
	set_dict_key(d,  "max_loc_x", result.max_loc_x);
	set_dict_key(d, "min_loc_y", result.min_loc_y);
	set_dict_key(d, "min_loc_x", result.min_loc_x);
	return d;
}


#endif /* PYIFACE_KS_TEST_IFACE_H_ */
