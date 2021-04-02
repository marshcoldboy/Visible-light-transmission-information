#include <opencv2/opencv.hpp>
#include <iostream>
#include <random>
#include <vector>
using namespace cv;
using namespace std;
#define NumberofColorBlocks 200
#define Numbmerofpixels 3
// �ݶ�ÿ��200��ɫ�飬ÿ��ɫ��3�����أ�һ�߹�600������
// �ṩһ����Ӧ��ͼƬ�ڰ�ɫ���01�����������Ӧ��ͼƬ
int matrix[NumberofColorBlocks+10][NumberofColorBlocks+10];
vector<Vec3b> rgb_pixels;  //���� rgb���ص��б�

int main()
{
	rgb_pixels.push_back({ 255,255,255 });//0--��ɫ
	rgb_pixels.push_back({0,0,0 });//1--��ɫ
	int all = NumberofColorBlocks * Numbmerofpixels;
	Mat img(all, all, CV_8UC3, Scalar(0, 0, 0));
	int i, j, p, q;
	for (i = 0; i < NumberofColorBlocks; i++)
	{
		for (j = 0; j < NumberofColorBlocks; j++)
		{
			matrix[i][j] = -1;
		}
	}//ȫ������-1������δʹ��
	int bigAnchorPoint[8][8] = {
		1,1,1,1,1,1,1,0,
		1,0,0,0,0,0,1,0,
		1,0,1,1,1,0,1,0,
		1,0,1,0,1,0,1,0,
		1,0,1,1,1,0,1,0,
		1,0,0,0,0,0,1,0,
		1,1,1,1,1,1,1,0,
		0,0,0,0,0,0,0,0
	};//��λ��
	int smallAnchorPoint[5][5] = {
		1,1,1,1,1,
		1,0,0,0,1,
		1,0,1,0,1,
		1,0,0,0,1,
		1,1,1,1,1
	};//С��λ��
	for (i = 0; i < NumberofColorBlocks; i++)
	{
		for (j = 0; j < NumberofColorBlocks; j++)
		{
			if(matrix[i][j] == -1);
			matrix[i][j] = rand() % 2;
		}
	}//���ڴ洢01��Ϣ�Ĳ��֣����ܴ� NumberofColorBlocks*Numbmerofpixels-217 ��bit

	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			matrix[i][j] = bigAnchorPoint[i][j];
			matrix[NumberofColorBlocks - 1 - i][j] = bigAnchorPoint[i][j];
			matrix[i][NumberofColorBlocks - 1 - j] = bigAnchorPoint[i][j];
		}
	}
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 5; j++)
		{
			matrix[NumberofColorBlocks - 1 - i][NumberofColorBlocks - 1 - j] = smallAnchorPoint[i][j];
		}
	}
	for (i = 0; i*Numbmerofpixels < img.rows; i++)
	{
		for (j = 0; j*Numbmerofpixels < img.cols; j++)
		{
			for (p = 0; p < Numbmerofpixels; p++)
			{
				for (q = 0; q < Numbmerofpixels; q++)
				{	
					img.at<Vec3b>(i*Numbmerofpixels+p, j*Numbmerofpixels+q) =rgb_pixels[matrix[i][j]];
				}
			}
		}
	}
	imshow("img", img);
	waitKey();
}

