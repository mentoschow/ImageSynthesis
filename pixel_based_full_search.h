#pragma once
#ifndef _TS_TEXSYN_H
#define _TS_TEXSYN_H

#define TS_IN_XMAX 100
#define TS_IN_YMAX 100

#define TS_OUT_XMAX 400
#define TS_OUT_YMAX 400

#define LEVEL 5  //‘½d‰ğ‘œ“x‚ÌƒŒƒxƒ‹

#endif
#include <opencv2/opencv.hpp>

using namespace cv;

class pixel_based_full_search
{
public:
	int ts_full_search(int nbr, 
		int in_cyc, Mat input, Mat input_before, 
		unsigned int out_seed, Mat output, Mat output_before,
		int isok, int i);
	void full_search_first(int nbr, int in_cyc, unsigned int out_seed, Mat input, Mat output);
};

