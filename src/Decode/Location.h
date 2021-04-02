#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include "QRMatrix.h"
#define numberofblock 55
#define numberofpoint 825
using namespace cv;
using namespace std;
class Location
{
private:
	double cross(Point before, Point next, Point mid);
	double dis(Point a, Point b);
	Point base;//��׼�㣬��Ϊ͹����һ�㣬���������е㶼�ڸõ��ұ�
	bool cmp(Point a, Point b);
	void findbase(vector<Point>& a);
	vector<Point> find_tubao(vector<Point>& a);
	
	Mat src, src_gray;
	Mat dst, detected_edges;

	int edgeThresh = 1;
	int lowThreshold = 50;
	int const max_lowThreshold = 100;
	int ratioo = 3;
	int kernel_size = 3;
	char* window_name = (char*)"Edge Map";
	Mat fsrc;
	int ccc = 1;//Ϊ1ʱ����������հף�Ϊ0ʱ����

	void SobelEnhance(const Mat src, Mat& dst);
	void CannyThreshold(int, void*);
	void canny(Mat a);
	struct po
	{
		int minx, maxx, miny, maxy;
		int i;
		vector<Point> d;
	};
	bool po_cmp(po a, po b);
	string int2str(const int& int_temp);
	int find_point(vector<po>& s);

	Point2f vertexs_minRect_QR[4];//��¼��ά���ĸ������

	void find4j();

	bool location(Mat a);

	void end_correct(Mat f);

	void read(Mat a);
public:
	Mat source;
	Location(Mat source);
	QRMatrix res;
	int ans[numberofblock + 5][numberofblock + 5];
};