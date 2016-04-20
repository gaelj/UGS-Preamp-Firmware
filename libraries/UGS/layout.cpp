/*
 * layout.cpp
 *
 *  Created on: 16 août 2015
 *      Author: gael
 */

#include "layout.h"


Layout::Layout()
{
	x_max = 800;
	y_max = 480;

	x_min = 8;
	y_min = 8;
	size_m = 3;
	size_d = 2;
	sep = 16;

	icon_size = (int)(base_size * size_m / size_d); // 144
	ss = icon_size + sep; 							// 160

	name = "0";
}

Layout::Layout(String name_, int x_min_, int y_min_, int x_max_, int y_max_, int size_m_, int size_d_, int sep_)
{
	x_max = x_max_;
	y_max = y_max_;

	x_min = x_min_;
	y_min = y_min_;
	size_m = size_m_;
	size_d = size_d_;

	sep = sep_;

	icon_size = (int)(base_size * size_m / size_d);
	ss = icon_size + sep;

	name = name_;
}
