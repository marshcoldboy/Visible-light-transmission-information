#include <opencv2/opencv.hpp>
#include <iostream>
#include <random>
#include <vector>
#include "find_tb.h"
#define ll long long
using namespace cv;
using namespace std;
#define NumberofColorBlocks 100
#define Numbmerofpixels 5
// �ݶ�ÿ��200��ɫ�飬ÿ��ɫ��3�����أ�һ�߹�600������
// �ṩһ����Ӧ��ͼƬ�ڰ�ɫ���01�����������Ӧ��ͼƬ

Mat pic()
{	
	int matrix[NumberofColorBlocks+10][NumberofColorBlocks+10];
	vector<Vec3b> rgb_pixels;  //���� rgb���ص��б�
	rgb_pixels.push_back({ 255,255,255 });//0--��ɫ
	rgb_pixels.push_back({ 0,0,0 });//1--��ɫ
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
			if (matrix[i][j] == -1)
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
	for (i = 0; i * Numbmerofpixels < img.rows; i++)
	{
		for (j = 0; j * Numbmerofpixels < img.cols; j++)
		{
			for (p = 0; p < Numbmerofpixels; p++)
			{
				for (q = 0; q < Numbmerofpixels; q++)
				{
					img.at<Vec3b>(i * Numbmerofpixels + p, j * Numbmerofpixels + q) = rgb_pixels[matrix[i][j]];
				}
			}
		}
	}
	imshow("img", img);
	waitKey();
	return img;
}

void show(Mat img)
{
	imshow("show", img);
	waitKey();
}

Mat src, src_gray;
Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold=40;
int const max_lowThreshold = 100;
int ratioo = 3;
int kernel_size = 3;
char* window_name = (char*)"Edge Map";
Mat fsrc;	

/**
 * @���� CannyThreshold
 * @��飺 trackbar �����ص� - Canny��ֵ�������1:3
 */
void CannyThreshold(int, void*)
{
	/// ʹ�� 3x3�ں˽���
	blur(src_gray, detected_edges, Size(3, 3));//����Ҷ�ͼsrc_gray������˲����ͼƬ��detected_edges
	GaussianBlur(src_gray, detected_edges, Size(7, 7), 0, 0);
	
	equalizeHist(detected_edges, detected_edges);//ֱ��ͼ���⻯���������ػҶ�ֵ�Ķ�̬��Χ���ﵽ��ǿͼ������Աȶȵ�Ч��
	//threshold(detected_edges, detected_edges, 150, 255, 0);
	/// ����Canny����, ��Ե��¼��ԭͼ��
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratioo, kernel_size);

	/// ʹ�� Canny���������Ե��Ϊ������ʾԭͼ��
	dst = Scalar::all(0);

	src.copyTo(dst, detected_edges);
	imshow(window_name, dst);imwrite("out11.jpg", dst);
}

/**
 * @���� canny
 * @��飺 ����һ��Mat����ø�ͼ��Ե
 */
void canny(Mat a)
{
	src = a;
	GaussianBlur(src, src, Size(9, 9), 0, 0);

	int col = src.cols, row = src.rows;
	while (col > 1500 || row > 1500)
	{
		col *= 0.99;
		row *= 0.99;
	}
	resize(src, src, Size(col, row));
	src.copyTo(fsrc);

	dst.create(src.size(), src.type());//������img��ͬ��С��ͼ
	cvtColor(src, src_gray ,COLOR_BGR2GRAY);//ת�Ҷ�ͼ
	show(src_gray);
	namedWindow(window_name, WINDOW_AUTOSIZE);
	//createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);
	CannyThreshold(0, 0);
	waitKey(0);
}

struct po
{
	int minx, maxx, miny, maxy;
	vector<Point> d;
};

bool po_cmp(po a,po b)
{
	if (a.maxx - a.minx != b.maxx - b.minx)
	{
		return a.maxx - a.minx > b.maxx - b.minx;
	}
	else
		return a.maxy - a.miny > b.maxy - b.miny;
}//�Ժ�ѡ��λ���������

string int2str(const int& int_temp)
{
	string string_temp;
	stringstream stream;
	stream << int_temp;
	string_temp = stream.str();   //�˴�Ҳ������ stream>>string_temp  
	return string_temp;
}

Point2f vertexs_minRect_QR[4];//��¼��ά���ĸ������
void location(Mat a)
{
	canny(a);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	//hierarchyΪ2ά�ģ�hierarchy[i][k]�����i�������ĵ�k����������Ӧcontours�е���ţ���������Ϊ-1
	//	k==0--������ͬ���ĺ�һ������
	//	k==1--������ͬ����ǰһ������
	//	k==2--��������һ��������
	//	k==3--�������ĸ�����
	//����ͼ����������������ʽ����������������������ģʽ�������ƽ� 
	findContours(detected_edges, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<int> vector_contours_filter;//���д�Ŷ�λ��ĺ�ѡ
	for (int k, c, i = 0; i < contours.size(); i++)
	{
		k = i;
		c = 0;
		while (hierarchy[k][2] != -1)//��ʾ��������
		{
			k = hierarchy[k][2];
			c = c + 1;
		}

		//----------------------Index����һ�Σ�c >= 5 -> c >= 4��-------------------------//

		if (c >= 4)			//�д��ڵ���5���������ģ���Ϊ��λ���ѡ
		{
			vector_contours_filter.push_back(i);
		}
	}


	vector<Point> vertex_minRect4;//װ��������λ�㹲12�����������
	if ((vector_contours_filter.size() < 3))//���û���ҵ��㹻�Ķ�λ��
	{
		printf("The picture does not meet the requirements\n");
		printf("cnt=	%d\n", vector_contours_filter.size());
		return;
	}
	int all = vector_contours_filter.size();
	printf("all point: %d\n", all);
	vector<po>s;//�洢��λ����Ϣ
	/*string start = "out", end = ".jpg";*/
	for (int j = 0; j < all; j++)
	{
		
		po q;
		q.minx = 1e9, q.miny = 1e9, q.maxx = 0, q.maxy = 0;
		q.d = find_tubao(contours[vector_contours_filter[j]]);
		//q.d = contours[vector_contours_filter[j]];
		int cnt = q.d.size();
		if (cnt < 4)continue;
		for (int i = 0; i < cnt; i++)//��ȷ�������ľ��ζ��㶪��ȥ
		{
			//int temp = contours[vector_contours_filter[0]].size() / 4;
			q.minx = min(q.minx, q.d[i].x);
			q.miny = min(q.miny, q.d[i].y);
			q.maxx = max(q.maxx, q.d[i].x);
			q.maxy = max(q.maxy, q.d[i].y);
			//printf("%d %d\n", contours[vector_contours_filter[j]][i].x, contours[vector_contours_filter[j]][i].y);
		}
		//printf("\n%d\n", vector_contours_filter[0]);
		/*Mat temp;
		temp = src(Range(q.miny, q.maxy), Range(q.minx, q.maxx));
		string path = start + int2str(j) + end;
		imwrite(path, temp);*/
		//show(temp);

		
		//q.d = contours[vector_contours_filter[j]];
		s.push_back(q);
	}
	printf("all : %d\n", all);
	sort(s.begin(), s.end(), po_cmp);//�����������
	int ans = -1;
	if (all > 3)
	{
		for (int i = 0; i + 2 < all; i++)
		{
			int p1x, p2x, p3x, p1y, p2y, p3y;
			p1x = s[i].maxx - s[i].minx;
			p2x = s[i + 1].maxx - s[i + 1].minx;
			p3x = s[i + 2].maxx - s[i + 2].minx;
			p1y = s[i].maxy - s[i].miny;
			p2y = s[i + 1].maxy - s[i + 1].miny;
			p3y = s[i + 2].maxy - s[i + 2].miny;
			if (abs(p3x - p1x) <= 6 && abs(p3y - p1y) <= 6&& abs(p3x - p2x) <= 6 && abs(p3y - p2y) <= 6)
			{
				printf("%d %d %d %d\n",s[i+1].maxx,s[i+1].minx, s[i + 1].maxy, s[i + 1].miny);
				ans = i;
				break;
			}
		}

		if (ans == -1)
		{
			printf("not match\n");
			return;
		}
	}
	else
		ans = 0;
	printf("ans:%d\n", ans);
	for (int i=ans;i<ans+3;i++)
	{
		//cout << "cnt:	" << i << " minx= " << s[i].minx << " maxx= " << s[i].maxx << " miny= " << s[i].miny << " maxy= " << s[i].maxy << endl;

		int cnt = s[i].d.size(); printf("cnt: %d\n", cnt);
		for (int j = 0; j < cnt; j++)
		{
			line(src, s[i].d[j], s[i].d[(j+1)%cnt], Scalar(0, 255, 0), 1, 8);
			vertex_minRect4.push_back(s[i].d[j]);
			//printf("%d %d\n", s[i].d[j].x, s[i].d[j].y);
		}
	}
	Point P1, P2; ll maxdis = 0;
	int cnt = vertex_minRect4.size(); 
	for (int yi = 0; yi < cnt; yi++)//�ҵ�������Զ����
	{
		for (int yt = yi + 1; yt < cnt; yt++)
		{
			ll te = (vertex_minRect4[yi].x - vertex_minRect4[yt].x) * (vertex_minRect4[yi].x - vertex_minRect4[yt].x) + (vertex_minRect4[yi].y - vertex_minRect4[yt].y) * (vertex_minRect4[yi].y - vertex_minRect4[yt].y);
			if (te > maxdis)
			{
				maxdis = te;
				P1 = vertex_minRect4[yi];
				P2 = vertex_minRect4[yt];
			}
		}
	}
	//������Զ������е�Ϊ��ά����е�
	for (int yi = 0; yi < cnt; yi++)
	{
		Point temp;
		temp.x = (P1.x + P2.x) - vertex_minRect4[yi].x;
		temp.y = (P1.y + P2.y) - vertex_minRect4[yi].y;
		vertex_minRect4.push_back(temp);
	}//�����е㣬ȫ���Գƹ�ȥ�����Եõ����ĸ��ǵ�λ��
	
	Point2f point_minRect12[4];

	RotatedRect rect12 = minAreaRect(vertex_minRect4);//����С��Χ����
	//vertex_minRect4�д�Ŷ�λ���ȫ������

	rect12.points(point_minRect12);
	for (int j = 0; j < 4; j++)
	{
		vertexs_minRect_QR[j] = point_minRect12[j];
		line(src, point_minRect12[j], point_minRect12[(j + 1) % 4], Scalar(0, 0, 255), 1, 8);
	}
	
	Point2f midPoint_rect;
	for (int j = 0; j < 4; j++)
	{
		midPoint_rect.x += (point_minRect12[j].x / 4);
		midPoint_rect.y += (point_minRect12[j].y / 4);
	}
	circle(src, midPoint_rect, 10, Scalar(0, 255, 0), 3, 8);
	src.copyTo(fsrc);

	int col = src.cols, row = src.rows;
	while (col > 1000 || row > 1000)
	{
		col *= 0.99;
		row *= 0.99;
	}
	resize(src, src, Size(col, row));
	
	show(src);
}

//�Ѿ����ĽǺʹ����ĽǶ�Ӧ
void find4j()
{
	Point2f mid = Point2f(0, 0);
	for (int i = 0; i < 4; i++)
	{
		mid.x += vertexs_minRect_QR[i].x / 4;
		mid.y += vertexs_minRect_QR[i].y / 4;
	}
	for (int i = 0; i < 4; i++)
	{
		if (vertexs_minRect_QR[i].x < mid.x && vertexs_minRect_QR[i].y < mid.y)
		{
			swap(vertexs_minRect_QR[i], vertexs_minRect_QR[2]);
		}
		if (vertexs_minRect_QR[i].x < mid.x && vertexs_minRect_QR[i].y > mid.y)
		{
			swap(vertexs_minRect_QR[i], vertexs_minRect_QR[1]);
		}
		if (vertexs_minRect_QR[i].x > mid.x && vertexs_minRect_QR[i].y < mid.y)
		{
			swap(vertexs_minRect_QR[i], vertexs_minRect_QR[3]);
		}
		if (vertexs_minRect_QR[i].x > mid.x && vertexs_minRect_QR[i].y > mid.y)
		{
			swap(vertexs_minRect_QR[i], vertexs_minRect_QR[0]);
		}
	}
}

void correct()
{
	//int lenth =1+ sqrt((vertexs_minRect_QR[0].x - vertexs_minRect_QR[1].x)* (vertexs_minRect_QR[0].x - vertexs_minRect_QR[1].x)+ (vertexs_minRect_QR[0].y - vertexs_minRect_QR[1].y)+ (vertexs_minRect_QR[0].y - vertexs_minRect_QR[1].y));
	int lenth = 500;
	Point2f vertex_warp[4];
	vertex_warp[1] = Point2f(0, float(lenth - 1));
	vertex_warp[2] = Point2f(0, 0);
	vertex_warp[3] = Point2f(float(lenth - 1), 0);
	vertex_warp[0] = Point2f(float(lenth - 1), float(lenth));
	find4j();
	Mat transform = getPerspectiveTransform(vertexs_minRect_QR, vertex_warp);
	warpPerspective(fsrc, dst, transform, Size(lenth, lenth));
	//show(fsrc);
	show(dst);
	imwrite("out.jpg", dst);
}

int main()
{
	//src=pic();
	src = imread("C:\\Users\\xc\\Desktop\\1616081160493.jpg", 1);
	//src = imread("out.jpg");
	//canny(src);
	if (src.empty()) {
		//����Ƿ��ȡͼ��
		cout << "Error! Input image cannot be read...\n";
		return -1;
	}

	location(src);
	correct();
}
