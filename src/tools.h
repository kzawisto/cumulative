/*
 * tools.h
 *
 *  Created on: 27 paź 2018
 *      Author: krystian
 */

#ifndef SRC_TOOLS_H_
#define SRC_TOOLS_H_

#include<vector>
#include<iostream>
template<typename T>
std::ostream & operator<<(std::ostream & op, const std::vector<T> & vec) {
	op<<"[";
	if(not vec.empty()) {
		op<<vec.front();
	}
	for(std::size_t i = 1;i < vec.size();++i) {
		op<<","<<vec[i];
	}
	return op<<"]";
}
using vecd = std::vector<double>;
using vecl = std::vector<long>;
inline vecd get_selector(const vecd & vec, const vecl &indices) {
	vecd result(indices.size());
	for(int i =0;i < indices.size();++i){
		result[i] = vec[indices[i]];
	}
	return result;
}


inline vecl get_range_until(long lower, long upper) {
	vecl vec;
	vec.reserve(upper - lower);
	for(;lower < upper;lower++) {
		vec.push_back(lower);
	}
	return vec;
}


#endif /* SRC_TOOLS_H_ */
