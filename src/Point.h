/*
 * Point.h
 *
 *  Created on: 27 paź 2018
 *      Author: krystian
 */

#ifndef SRC_POINT_H_
#define SRC_POINT_H_

struct Point {
    double x, y;
    friend std::ostream &operator<<(std::ostream &op, Point p) {
        op << "{x=" << p.x << ", y=" << p.y << "}";

        return op;
    }
};




#endif /* SRC_POINT_H_ */
