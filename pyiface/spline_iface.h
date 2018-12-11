/*
 * spline_iface.h
 *
 *  Created on: 11 gru 2018
 *      Author: krystian
 */



#include"src/evaluate_mean_var_2d.h"
#include<Python.h>
#include<pyiface/commons.h>

PyObject * py_serialize(const OptimalSpline & s) {
	auto dic = PyDict_New();
	set_dict_key(dic, "selection", s.selection);
	set_dict_key(dic, "remainder", s.remainder);
	set_dict_key(dic, "loglik", s.loglik);
	set_dict_key(dic, "X", s.X);
	set_dict_key(dic, "Y", s.Y);
	set_dict_key(dic, "idx", s.idx);
	return dic;
}


PyObject * get_optimal_spline(PyObject * featuresX, PyObject * featuresY, PyObject * response) {

	auto fX = obj_to_vec<double>(featuresX);
	auto fY = obj_to_vec<double>(featuresY);
	auto fr = obj_to_vec<double>(response);
	MeanVar2dEvaluator eva(fX, fY, fr);
	OptimalSpline spline = eva.run_optim();
	return py_serialize(spline);
}
