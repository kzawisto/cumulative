/*
 * DiscreteDistribution.h
 *
 *  Created on: Oct 23, 2018
 *      Author: amadeus
 */

#ifndef SRC_DISCRETEDISTRIBUTION_H_
#define SRC_DISCRETEDISTRIBUTION_H_

#include<vector>
#include<algorithm>

template<typename T>
class StaticDiscreteDistr {
	std::vector<T> data;
	template<typename K>
	StaticDiscreteDistr(K begin, K end) {
		for(;begin != end; begin++) {
			data.push_back(*begin);
		}
		std::stable_sort(data.begin(), data.end());
	}

	double cdfinv(T value) {
		double pos1 = std::lower_bound(data.begin(), data.end(), value) - data.begin();
		double pos2 = std::upper_bound(data.begin(), data.end(), value) - data.begin();
		if (pos1 >= data.size()-1) {
			pos1
		}
		if( pos1 == pos2) {

		}
		else {
			return (pos1 + pos2) / 2 / data.size();
		}
	}

};




#endif /* SRC_DISCRETEDISTRIBUTION_H_ */
