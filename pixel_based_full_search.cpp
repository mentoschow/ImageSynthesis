#include "pixel_based_full_search.h"
#include <stdio.h>
#include <stdlib.h>

int pixel_based_full_search::ts_full_search(int nbr,
    int in_cyc, Mat input, Mat input_before,
    unsigned int out_seed, Mat output, Mat output_before,
    int isok, int i)
{
	/* 初期処理 */
	int in_x_min, in_x_max;
	int in_y_min, in_y_max;
    isok = 0;
    printf("loop: %d\n", i + 1);

	// 入力テクスチャの探索範囲
	// 環状でない
	if (in_cyc == 0) {
		in_x_min = nbr;
		in_x_max = input.cols - nbr;
		in_y_min = nbr;
		in_y_max = input.rows;
	}
	// 環状である
	else {
		in_x_min = 0;
		in_x_max = input.cols;
		in_y_min = 0;
		in_y_max = input.rows;
	}

	// 出力テクスチャの初期化
	srand(out_seed);
	for (int oy = 0; oy < output.rows; oy++) {
		for (int ox = 0; ox < output.cols; ox++) {
			for (int c = 0; c < 3; c++) {
                output.at<Vec3b>(oy, ox)[c] = input.at<Vec3b>(rand() % input.rows, rand() % input.cols)[c];
			}
		}
	}

	/* 出力テクスチャの合成 */

	// 近隣画素群中の画素数
	int pxn = ((2 * nbr + 1) * (2 * nbr + 1)) / 2;
	printf("ts_full_search():nbr = %d, pxn = %d\n", nbr, pxn);

	// 最大 SSD (sum of squared differences) 値
	int ssd_max = 3 * 255 * 255 * pxn;

    // 出力テクスチャの画素のループ
    for (int oy = 0; oy < output.rows; oy++) {
        for (int ox = 0; ox < output.cols; ox++) {

            // 最小 SSD 値の初期化
            int ssd_min = ssd_max;

            // 入力テクスチャの選択画素の初期化
            int ix_s = -1;
            int iy_s = -1;

            // 入力テクスチャ上の探索
            for (int iy = in_y_min; iy < in_y_max; iy++) {
                for (int ix = in_x_min; ix < in_x_max; ix++) {

                    // 近隣画素群の SSD 値の計算
                    int s;
                    int ssd = 0;
                    for (int ny = (-nbr); ny <= 0; ny++) {
                        for (int nx = (-nbr); nx <= nbr; nx++) {
                            // 近隣画素群の中心に来たら計算終了
                            if ((ny == 0) && (nx == 0)) {
                                break;
                            }
                            // 計算
                            for (int c = 0; c < 3; c++) {
                                s = (int)output.at<Vec3b>((oy + ny + output.rows) % output.rows,(ox + nx + output.cols) % output.cols)[c]
                                    - (int)input.at<Vec3b>((iy + ny + input.rows) % input.rows,(ix + nx + input.cols) % input.cols)[c];
                                ssd += (s * s);
                            }
                        }
                    }

                    // 最小 SSD 値と入力テクスチャの選択画素の更新
                    if (ssd_min > ssd) {
                        ssd_min = ssd;
                        ix_s = ix;
                        iy_s = iy;
                    }                                    

                } // ix
            } // iy

            /* 前一段階の画像の対応位置の画素のSSDを求める、正方形の近隣画素群を用いる */
            int new_ix_s = ix_s / 2;
            int new_iy_s = iy_s / 2;
            int new_ox = ox / 2;
            int new_oy = oy / 2;
            int new_s;
            int new_ssd = 0;
            for (int i = new_iy_s - nbr; i < new_iy_s + nbr; i++) {
                for (int j = new_ix_s - nbr; j < new_ix_s + nbr; j++) {
                    for (int c = 0; c < 3; c++) {
                        new_s = (int)output_before.at<Vec3b>((new_oy + i + output_before.rows) % output_before.rows, (new_ox + j + output_before.cols) % output_before.cols)[c]
                            - (int)input_before.at<Vec3b>((new_iy_s + i + input_before.rows) % input_before.rows, (new_ix_s + j + input_before.cols) % input_before.cols)[c];
                        new_ssd += (new_s * new_s);
                    }
                }
            }

            // 出力テクスチャの画素色
            if (ssd_min == ssd_max) {  // 入力テクスチャの画素が選択されていない
                for (int c = 0; c < 3; c++) {
                    output.at<Vec3b>(oy, ox)[c] = 0;
                }
            }
            /* 前一段階の画像の対応位置の画素のSSDが小さいであればその画素の色を使う */
            else if (ssd_min > new_ssd) {
                for (int c = 0; c < 3; c++) {
                    output.at<Vec3b>(oy, ox)[c] = input_before.at<Vec3b>(new_iy_s, new_ix_s)[c];
                }
            }
            else {                      // 入力テクスチャの画素が選択されている
                for (int c = 0; c < 3; c++) {
                    output.at<Vec3b>(oy, ox)[c] = input.at<Vec3b>(iy_s, ix_s)[c];
                }
            }

        }
    }

    return isok;
}

void pixel_based_full_search::full_search_first(int nbr, int in_cyc, unsigned int out_seed, Mat input, Mat output)
{
    /* 初期処理 */
    int in_x_min, in_x_max;
    int in_y_min, in_y_max;
    printf("loop: 1\n");

    // 入力テクスチャの探索範囲
    // 環状でない
    if (in_cyc == 0) {
        in_x_min = nbr;
        in_x_max = input.cols - nbr;
        in_y_min = nbr;
        in_y_max = input.rows;
    }
    // 環状である
    else {
        in_x_min = 0;
        in_x_max = input.cols;
        in_y_min = 0;
        in_y_max = input.rows;
    }

    // 出力テクスチャの初期化
    srand(out_seed);
    for (int oy = 0; oy < output.rows; oy++) {
        for (int ox = 0; ox < output.cols; ox++) {
            for (int c = 0; c < 3; c++) {
                output.at<Vec3b>(oy, ox)[c] = input.at<Vec3b>(rand() % input.rows, rand() % input.cols)[c];
            }
        }
    }

    /* 出力テクスチャの合成 */

    // 近隣画素群中の画素数
    int pxn = ((2 * nbr + 1) * (2 * nbr + 1)) / 2;
    printf("ts_full_search():nbr = %d, pxn = %d\n", nbr, pxn);

    // 最大 SSD (sum of squared differences) 値
    int ssd_max = 3 * 255 * 255 * pxn;

    // 出力テクスチャの画素のループ
    for (int oy = 0; oy < output.rows; oy++) {
        for (int ox = 0; ox < output.cols; ox++) {

            // 最小 SSD 値の初期化
            int ssd_min = ssd_max;

            // 入力テクスチャの選択画素の初期化
            int ix_s = -1;
            int iy_s = -1;

            // 入力テクスチャ上の探索
            for (int iy = in_y_min; iy < in_y_max; iy++) {
                for (int ix = in_x_min; ix < in_x_max; ix++) {

                    // 近隣画素群の SSD 値の計算
                    int s;
                    int ssd = 0;
                    for (int ny = (-nbr); ny <= 0; ny++) {
                        for (int nx = (-nbr); nx <= nbr; nx++) {
                            // 近隣画素群の中心に来たら計算終了
                            if ((ny == 0) && (nx == 0)) {
                                break;
                            }
                            // 計算
                            for (int c = 0; c < 3; c++) {
                                s = (int)output.at<Vec3b>((oy + ny + output.rows) % output.rows, (ox + nx + output.cols) % output.cols)[c]
                                    - (int)input.at<Vec3b>((iy + ny + input.rows) % input.rows, (ix + nx + input.cols) % input.cols)[c];
                                ssd += (s * s);
                            }
                        }
                    }

                    // 最小 SSD 値と入力テクスチャの選択画素の更新
                    if (ssd_min > ssd) {
                        ssd_min = ssd;
                        ix_s = ix;
                        iy_s = iy;
                    }

                } // ix
            } // iy          

            // 出力テクスチャの画素色
            if (ssd_min == ssd_max) {  // 入力テクスチャの画素が選択されていない
                for (int c = 0; c < 3; c++) {
                    output.at<Vec3b>(oy, ox)[c] = 0;
                }
            }
            else {                      // 入力テクスチャの画素が選択されている
                for (int c = 0; c < 3; c++) {
                    output.at<Vec3b>(oy, ox)[c] = input.at<Vec3b>(iy_s, ix_s)[c];
                }
            }

        }
    }
}
