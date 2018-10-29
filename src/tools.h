/*
 * tools.h
 *
 *  Created on: 27 paź 2018
 *      Author: krystian
 */

#ifndef SRC_TOOLS_H_
#define SRC_TOOLS_H_

#include<vector>
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
