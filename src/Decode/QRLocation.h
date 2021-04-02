#pragma once
#include <opencv2/opencv.hpp>
#include <cstdio>
#include <random>
#include <vector>
#include <algorithm>
#include <ctime>
#include "QRMatrix.h"
#include "QRList.h"
#include "GlobalMacroDefine.h"
#define blockNumberOfWhite 2 
#define numberofblock (NumberofColorBlocks + 2*blockNumberOfWhite)
#define numberofpoint numberofblock*4


using namespace cv;
using namespace std;
struct po
{
	int minx, maxx, miny, maxy;
	int i;
	vector<Point> d;
};

class QRLocation
{
	Mat QRLocation_src, QRLocation_src_gray;
	Mat QRLocation_dst, detected_edges;
	Mat QRLocation_fsrc;
	char* ans;
	bool getQR;
	Point2f vertexs_minRect_QR[4]; //��¼��ά����ĸ���

	//��Ե��ǿ����
	void SobelEnhance(const Mat src, Mat& dst);


	//������ת�Ҷ�ͼ�����룬������openCV�Դ���Canny������ȡ��Ե
	//���Ͳ������ڰ�����Ե�պϣ�����֮���������ȡ
	void canny(Mat a);

	string int2str(const int& int_temp);

	//s�б�����ȫ���ĺ�ѡ��λ�㣬����������λ��ļнǺ������������ɸѡ
	//�Ӻ�ѡ�����ҵ�����,���ص�һ�������i��������λ�����i��i+1��i+2
	int find_point(vector<po>& s);

	//����˳ʱ�뷽��ں��ĸ�����
	void find4j();

	//��λ��������������ж�ά���Mat�������Ƿ��ҵ���ά�롣
	//�����ҵ����򽫶�ά����ĸ����㱣����vertexs_minRect_QR�����У����ڽ�ȡ����
	bool location2(Mat a);

	//��������������ͼƬ������location�����л�õ��Ķ�����н�ȡ�ͽ���
	//���ջ�ý�����Ķ�ά�룬�����QRLocation_dst��
	void end_correct(Mat f);

	//����������ͼ�񣬸����������Լ���ά���ڿ������о���
	//���ֺ�������ڵ����ص㣬�жϺڰ���һ���϶࣬�ý϶��һ��������һ�����ɫ
	void read(Mat a);

public:

	//���캯��������һ��ͼƬ�����ж�λ�����Ͷ�ȡ���������public��ans��
	QRLocation(Mat src);

	//public�������ṩ�ӿڸ���磬����һ��QRList
	QRList get();
};

