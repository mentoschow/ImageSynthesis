#include <iostream>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "pixel_based_full_search.h"

using namespace std;
using namespace cv;

#pragma region Global Variable
int TS_nbr = 3;
int TS_in_cyc = 0;
int TS_in_x;
int TS_in_y;
unsigned int TS_out_seed = 1;
int TS_out_x = 100;
int TS_out_y = 100;
#pragma endregion

int main()
{
    Mat intex_cv;  //入力画像
    Mat outtex_cv(TS_out_y, TS_out_x, CV_8UC3);  //出力画像の初期化
    vector <Mat>image_pyr_in;  //入力多重解像度画像の容器
    vector <Mat>image_pyr_out;  //出力多重解像度画像の容器
    Mat intex_pyr;  //入力多重解像度画像
    Mat outtex_pyr;  //出力多重解像度画像
    
    //時間
    clock_t time_start, time_end;
    double  time_sec;
    int     time_m;
    double  time_s;

    printf("main() --> start\n\n");
    time_start = clock();

    //入力画像の読み込み
    intex_cv = imread("./textures/brickwall.jpg");
    TS_in_x = intex_cv.cols;
    TS_in_y = intex_cv.rows;
    namedWindow("input", CV_WINDOW_AUTOSIZE);
    imshow("input", intex_cv);
    printf("intex_cv.cols * rows = %d * %d\n\n", intex_cv.cols, intex_cv.rows);

    //多重解像度
    printf("multi resolution start.\n");
    for (int i = LEVEL; i > 0; i--) {
        printf("level= %d\n", i);
        resize(intex_cv, intex_pyr, Size(), pow(0.5, i - 1), pow(0.5, i - 1));
        resize(outtex_cv, outtex_pyr, Size(), pow(0.5, i - 1), pow(0.5, i - 1));
        image_pyr_in.push_back(intex_pyr);
        image_pyr_out.push_back(outtex_pyr);
    }
    printf("multi resolution completed.\n\n");

    /* 全探索アルゴリズム */
    printf("full search start.\n");
    int isok = 0;
    pixel_based_full_search full_search;  //初回の探索
    full_search.full_search_first(TS_nbr, TS_in_cyc, TS_out_seed, image_pyr_in[0], image_pyr_out[0]);
    for (int i = 1; i < LEVEL; i++) {
        full_search.ts_full_search(TS_nbr,
            TS_in_cyc, image_pyr_in[i], image_pyr_in[i - 1],
            TS_out_seed, image_pyr_out[i], image_pyr_out[i - 1],
            isok, i);
        if (isok != 0) {
            printf("err : main() -> ts_full_search()\n");
            return -1;
        }
    }
    printf("full search completed.\n\n");

    //出力画像
    outtex_cv = image_pyr_out[LEVEL - 1].clone();
    cv::imwrite("./textures/brickwall_out_level5_nbr3.jpg", outtex_cv);
    cv::namedWindow("output", CV_WINDOW_AUTOSIZE);
    cv::imshow("output", outtex_cv);
    printf("outtex_cv.cols * rows = %d * %d\n", outtex_cv.cols, outtex_cv.rows);

    time_end = clock();
    time_sec = (double)(time_end - time_start) / (double)CLOCKS_PER_SEC;
    time_m = (int)(time_sec / 60.0);
    time_s = time_sec - (double)time_m * 60.0;
    printf("main() : time >> %lf sec. = %d min. %lf sec.\n\n", time_sec, time_m, time_s);
    printf("main() <---- end\n");

    cv::waitKey(0);
    return 0;
}
