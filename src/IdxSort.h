/*
 * IdxSort.h
 *
 *  Created on: 8 gru 2018
 *      Author: krystian
 */

#ifndef SRC_IDXSORT_H_
#define SRC_IDXSORT_H_
#include <algorithm>
#include <vector>

struct IdxSort {

	std::vector<long> indices;
	IdxSort(std::vector<double> & v) : indices(v.size()){
		for(std::size_t i  =0 ;i < v.size();++i) {
			indices[i]= i;
		}
		std::sort(indices.begin(), indices.end(),[v] (long i, long j) {
			return v[i] < v[j];
		});
	}
	template<typename T>
	std::vector<T> reorder(const std::vector<T> & v) {
		std::vector<T> v1(v.size());
		for(std::size_t i  =0 ;i < v.size();++i) {
			v1[indices[i]] = v[i];
		}
		return v1;
	}
	template<typename T>
	std::vector<T> unreorder(const std::vector<T> & v) {

		std::vector<T> v1(v.size());
		for(std::size_t i  =0 ;i < v.size();++i) {
			v1[i] = v[indices[i]];
		}
		return v1;
	}
};




#endif /* SRC_IDXSORT_H_ */
