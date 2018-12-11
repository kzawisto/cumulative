
#pragma once

#include<vector>
#include<np_bridge/np_bridge.h>
#include<cp_tools/numeric_utility.h>
template<typename T>
std::vector<T> view_to_vec(const np_bridge::NumpyArrayView<T> & v) {
	std::vector<double> vec;
	vec.reserve(v.size());
	for(long i = 0;i < v.size();++i) {
		vec.push_back(v.c_at(i));
	}
	return vec;
}

template<typename T>
std::vector<T> obj_to_vec(PyObject *obj) {
	np_bridge::NumpyArrayView<T> view((PyArrayObject*)obj);
	return view_to_vec<T>(view);

}
template<typename T>
struct typeval {

};

PyObject * py_serialize(long i) {
	return PyLong_FromLong(i);
}
PyObject * py_serialize(double i) {
	return PyFloat_FromDouble(i);
}
template<typename T>
PyObject * py_serialize(MeanAndVariance<T> v) {
	auto dic = PyDict_New();
	set_dict_key(dic, "mean", v.mean);
	set_dict_key(dic, "variance", v.variance);
	set_dict_key(dic, "count", v.count);
	return dic;
}
template<typename T>
void set_dict_key(PyObject * dict, std::string key, T va) {
	auto obj = py_serialize(va);
	assert(obj != nullptr);
	PyDict_SetItemString(dict, key.c_str(), obj);
	Py_XDECREF(obj);
}

