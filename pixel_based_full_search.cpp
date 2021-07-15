#include "pixel_based_full_search.h"
#include <stdio.h>
#include <stdlib.h>

int pixel_based_full_search::ts_full_search(int nbr,
    int in_cyc, Mat input, Mat input_before,
    unsigned int out_seed, Mat output, Mat output_before,
    int isok, int i)
{
	/* �������� */
	int in_x_min, in_x_max;
	int in_y_min, in_y_max;
    isok = 0;
    printf("loop: %d\n", i + 1);

	// ���̓e�N�X�`���̒T���͈�
	// ��łȂ�
	if (in_cyc == 0) {
		in_x_min = nbr;
		in_x_max = input.cols - nbr;
		in_y_min = nbr;
		in_y_max = input.rows;
	}
	// ��ł���
	else {
		in_x_min = 0;
		in_x_max = input.cols;
		in_y_min = 0;
		in_y_max = input.rows;
	}

	// �o�̓e�N�X�`���̏�����
	srand(out_seed);
	for (int oy = 0; oy < output.rows; oy++) {
		for (int ox = 0; ox < output.cols; ox++) {
			for (int c = 0; c < 3; c++) {
                output.at<Vec3b>(oy, ox)[c] = input.at<Vec3b>(rand() % input.rows, rand() % input.cols)[c];
			}
		}
	}

	/* �o�̓e�N�X�`���̍��� */

	// �ߗ׉�f�Q���̉�f��
	int pxn = ((2 * nbr + 1) * (2 * nbr + 1)) / 2;
	printf("ts_full_search():nbr = %d, pxn = %d\n", nbr, pxn);

	// �ő� SSD (sum of squared differences) �l
	int ssd_max = 3 * 255 * 255 * pxn;

    // �o�̓e�N�X�`���̉�f�̃��[�v
    for (int oy = 0; oy < output.rows; oy++) {
        for (int ox = 0; ox < output.cols; ox++) {

            // �ŏ� SSD �l�̏�����
            int ssd_min = ssd_max;

            // ���̓e�N�X�`���̑I����f�̏�����
            int ix_s = -1;
            int iy_s = -1;

            // ���̓e�N�X�`����̒T��
            for (int iy = in_y_min; iy < in_y_max; iy++) {
                for (int ix = in_x_min; ix < in_x_max; ix++) {

                    // �ߗ׉�f�Q�� SSD �l�̌v�Z
                    int s;
                    int ssd = 0;
                    for (int ny = (-nbr); ny <= 0; ny++) {
                        for (int nx = (-nbr); nx <= nbr; nx++) {
                            // �ߗ׉�f�Q�̒��S�ɗ�����v�Z�I��
                            if ((ny == 0) && (nx == 0)) {
                                break;
                            }
                            // �v�Z
                            for (int c = 0; c < 3; c++) {
                                s = (int)output.at<Vec3b>((oy + ny + output.rows) % output.rows,(ox + nx + output.cols) % output.cols)[c]
                                    - (int)input.at<Vec3b>((iy + ny + input.rows) % input.rows,(ix + nx + input.cols) % input.cols)[c];
                                ssd += (s * s);
                            }
                        }
                    }

                    // �ŏ� SSD �l�Ɠ��̓e�N�X�`���̑I����f�̍X�V
                    if (ssd_min > ssd) {
                        ssd_min = ssd;
                        ix_s = ix;
                        iy_s = iy;
                    }                                    

                } // ix
            } // iy

            /* �O��i�K�̉摜�̑Ή��ʒu�̉�f��SSD�����߂�A�����`�̋ߗ׉�f�Q��p���� */
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

            // �o�̓e�N�X�`���̉�f�F
            if (ssd_min == ssd_max) {  // ���̓e�N�X�`���̉�f���I������Ă��Ȃ�
                for (int c = 0; c < 3; c++) {
                    output.at<Vec3b>(oy, ox)[c] = 0;
                }
            }
            /* �O��i�K�̉摜�̑Ή��ʒu�̉�f��SSD���������ł���΂��̉�f�̐F���g�� */
            else if (ssd_min > new_ssd) {
                for (int c = 0; c < 3; c++) {
                    output.at<Vec3b>(oy, ox)[c] = input_before.at<Vec3b>(new_iy_s, new_ix_s)[c];
                }
            }
            else {                      // ���̓e�N�X�`���̉�f���I������Ă���
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
    /* �������� */
    int in_x_min, in_x_max;
    int in_y_min, in_y_max;
    printf("loop: 1\n");

    // ���̓e�N�X�`���̒T���͈�
    // ��łȂ�
    if (in_cyc == 0) {
        in_x_min = nbr;
        in_x_max = input.cols - nbr;
        in_y_min = nbr;
        in_y_max = input.rows;
    }
    // ��ł���
    else {
        in_x_min = 0;
        in_x_max = input.cols;
        in_y_min = 0;
        in_y_max = input.rows;
    }

    // �o�̓e�N�X�`���̏�����
    srand(out_seed);
    for (int oy = 0; oy < output.rows; oy++) {
        for (int ox = 0; ox < output.cols; ox++) {
            for (int c = 0; c < 3; c++) {
                output.at<Vec3b>(oy, ox)[c] = input.at<Vec3b>(rand() % input.rows, rand() % input.cols)[c];
            }
        }
    }

    /* �o�̓e�N�X�`���̍��� */

    // �ߗ׉�f�Q���̉�f��
    int pxn = ((2 * nbr + 1) * (2 * nbr + 1)) / 2;
    printf("ts_full_search():nbr = %d, pxn = %d\n", nbr, pxn);

    // �ő� SSD (sum of squared differences) �l
    int ssd_max = 3 * 255 * 255 * pxn;

    // �o�̓e�N�X�`���̉�f�̃��[�v
    for (int oy = 0; oy < output.rows; oy++) {
        for (int ox = 0; ox < output.cols; ox++) {

            // �ŏ� SSD �l�̏�����
            int ssd_min = ssd_max;

            // ���̓e�N�X�`���̑I����f�̏�����
            int ix_s = -1;
            int iy_s = -1;

            // ���̓e�N�X�`����̒T��
            for (int iy = in_y_min; iy < in_y_max; iy++) {
                for (int ix = in_x_min; ix < in_x_max; ix++) {

                    // �ߗ׉�f�Q�� SSD �l�̌v�Z
                    int s;
                    int ssd = 0;
                    for (int ny = (-nbr); ny <= 0; ny++) {
                        for (int nx = (-nbr); nx <= nbr; nx++) {
                            // �ߗ׉�f�Q�̒��S�ɗ�����v�Z�I��
                            if ((ny == 0) && (nx == 0)) {
                                break;
                            }
                            // �v�Z
                            for (int c = 0; c < 3; c++) {
                                s = (int)output.at<Vec3b>((oy + ny + output.rows) % output.rows, (ox + nx + output.cols) % output.cols)[c]
                                    - (int)input.at<Vec3b>((iy + ny + input.rows) % input.rows, (ix + nx + input.cols) % input.cols)[c];
                                ssd += (s * s);
                            }
                        }
                    }

                    // �ŏ� SSD �l�Ɠ��̓e�N�X�`���̑I����f�̍X�V
                    if (ssd_min > ssd) {
                        ssd_min = ssd;
                        ix_s = ix;
                        iy_s = iy;
                    }

                } // ix
            } // iy          

            // �o�̓e�N�X�`���̉�f�F
            if (ssd_min == ssd_max) {  // ���̓e�N�X�`���̉�f���I������Ă��Ȃ�
                for (int c = 0; c < 3; c++) {
                    output.at<Vec3b>(oy, ox)[c] = 0;
                }
            }
            else {                      // ���̓e�N�X�`���̉�f���I������Ă���
                for (int c = 0; c < 3; c++) {
                    output.at<Vec3b>(oy, ox)[c] = input.at<Vec3b>(iy_s, ix_s)[c];
                }
            }

        }
    }
}
