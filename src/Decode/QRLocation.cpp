#include "QRLocation.h"

bool po_cmp(po a, po b)
{
	if (a.maxx - a.minx != b.maxx - b.minx)
	{
		return a.maxx - a.minx > b.maxx - b.minx;
	}
	else
		return a.maxy - a.miny > b.maxy - b.miny;
}//�Ժ�ѡ��λ���������

double cross(Point before, Point next, Point mid)//before->mid    mid->next  ������ˣ�ƽ���ı��������
{
	double x1, y1, x2, y2;
	x1 = mid.x - before.x;
	x2 = next.x - mid.x;
	y1 = mid.y - before.y;
	y2 = next.y - mid.y;
	return (x1 * y2) - (x2 * y1);
}

double dis(Point a, Point b)
{
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

Point base;
bool cmp(Point a, Point b)//���ݸõ����׼������ߺ�ˮƽ�����ɵļн�����
{
	double tana, tanb;
	if (a.x - base.x == 0)
		tana = (a.y - base.y) * 1000;//�˴�Ĭ��x��ֵ��С��С��0.001
	else
		tana = (a.y - base.y) / (a.x - base.x);
	if (b.x - base.x == 0)
		tanb = (b.y - base.y) * 1000;
	else
		tanb = (b.y - base.y) / (b.x - base.x);
	if (tana != tanb)
		return tana < tanb;
	else
		return dis(a, base) < dis(b, base);
}

void findbase(vector<Point>& a)
{
	int cnt = a.size(), t = 0;
	base = a[0];
	for (int i = 0; i < cnt; i++)
	{
		if (a[i].x < base.x || (a[i].x == base.x && a[i].y < base.y))
		{
			base = a[i];
			t = i;
		}
	}
	swap(a[t], a[0]);
}

vector<Point> find_tubao(vector<Point>& a)
{
	vector<Point> result;
	result.push_back(a[0]);
	int top = 0, cnt = a.size();
	for (int i = 1; i < cnt; i++)
	{
		while (top > 0 && cross(result[top - 1], a[i], result[top]) >= 0)
		{
			top--;
			result.pop_back();
		}
		result.push_back(a[i]); top++;
	}
	return result;
}

//���ϴ����Ϊ��͹������غ�����������������ĵ㼯����һ��ɸѡ


void QRLocation::SobelEnhance(const Mat src, Mat& dst)
{
	Mat sobelx(src.size(), CV_16SC1);///��Ե���󣬻��и�ֵ��Ҳ���д���255��ֵ�����������ΪCV_16SC1�з�������
	Mat sobely(src.size(), CV_16SC1);
	Mat img_edgeFiltex(src.size(), CV_8UC1);///���ͼ��������ΪCV_8UC1������ֵ�ض�
	Mat img_edgeFiltey(src.size(), CV_8UC1);
	Sobel(src, sobelx, CV_16SC1, 1, 0, 3);
	convertScaleAbs(sobelx, img_edgeFiltex);
	Sobel(src, sobely, CV_16SC1, 0, 1, 3);
	convertScaleAbs(sobely, img_edgeFiltey);
	///���ؼ�Ȩ
	addWeighted(img_edgeFiltex, 1, img_edgeFiltey, 1, 0, dst);
}


void QRLocation::canny(Mat a)
{
	QRLocation_src = a;
	QRLocation_src.copyTo(QRLocation_fsrc);

	if (QRLocation_src.type() != CV_8UC1)
		cvtColor(QRLocation_src, QRLocation_src_gray, COLOR_BGR2GRAY);//ת�Ҷ�ͼ
	else
		QRLocation_src.copyTo(QRLocation_src_gray);


	int edgeThresh = 1;
	int lowThreshold = 50;
	int const max_lowThreshold = 100;
	int ratioo = 3;
	int kernel_size = 3;

	equalizeHist(detected_edges, detected_edges);//ֱ��ͼ���⻯���������ػҶ�ֵ�Ķ�̬��Χ���ﵽ��ǿͼ������Աȶȵ�Ч��


	GaussianBlur(QRLocation_src_gray, detected_edges, Size(7, 7), 0, 0);//��˹���룬����˴�СӦ����ͼƬ�ߴ������Ӧ����
	threshold(QRLocation_src_gray, detected_edges, 150, 255, THRESH_BINARY);
	Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratioo, kernel_size);

	cv::dilate(detected_edges, detected_edges, cv::Mat());//����
	cv::dilate(detected_edges, detected_edges, cv::Mat());//����
}

string QRLocation::int2str(const int& int_temp)
{
	string string_temp;
	stringstream stream;
	stream << int_temp;
	string_temp = stream.str();   //�˴�Ҳ������ stream>>string_temp  
	return string_temp;
}


int QRLocation::find_point(vector<po>& s)
{
	int all = s.size();
	int col = QRLocation_src.cols, row = QRLocation_src.rows;
	for (int i = 0; i + 2 < all; i++)
	{
		int p1x, p2x, p3x, p1y, p2y, p3y, mid1x, mid1y, mid2x, mid2y, mid3x, mid3y;
		p1x = s[i].maxx - s[i].minx;
		p2x = s[i + 1].maxx - s[i + 1].minx;
		p3x = s[i + 2].maxx - s[i + 2].minx;
		p1y = s[i].maxy - s[i].miny;
		p2y = s[i + 1].maxy - s[i + 1].miny;
		p3y = s[i + 2].maxy - s[i + 2].miny;
		mid1x = (s[i].maxx + s[i].minx) / 2;
		mid1y = (s[i].maxy + s[i].miny) / 2;
		mid2x = (s[i + 1].maxx + s[i + 1].minx) / 2;
		mid2y = (s[i + 1].maxy + s[i + 1].miny) / 2;
		mid3x = (s[i + 2].maxx + s[i + 2].minx) / 2;
		mid3y = (s[i + 2].maxy + s[i + 2].miny) / 2;
		//printf("%d %d\n", col,row);
		double ab, bc, ac;
		ab = sqrt((mid2x - mid1x) * (mid2x - mid1x) + (mid2y - mid1y) * (mid2y - mid1y));
		bc = sqrt((mid2x - mid3x) * (mid2x - mid3x) + (mid2y - mid3y) * (mid2y - mid3y));
		ac = sqrt((mid3x - mid1x) * (mid3x - mid1x) + (mid3y - mid1y) * (mid3y - mid1y));
		double cosa, cosb, cosc;

		cosa = ((mid2x - mid1x) * (mid3x - mid1x) + (mid2y - mid1y) * (mid3y - mid1y)) / ab / ac;
		cosb = ((mid1x - mid2x) * (mid3x - mid2x) + (mid1y - mid2y) * (mid3y - mid2y)) / ab / bc;
		cosc = ((mid2x - mid3x) * (mid1x - mid3x) + (mid2y - mid3y) * (mid1y - mid3y)) / bc / ac;

		if (abs(p3x - p1x) <= col / 150 && abs(p3y - p1y) <= row / 150 && abs(p3x - p2x) <= col / 150 && abs(p3y - p2y) <= row / 150)
		{
		}
		else continue;
		double p = 0.25;
		if (cosa<p && cosa>-p || cosb<p && cosb>-p || cosc<p && cosc>-p)
		{
			return i;
			break;
		}
		else continue;
	}


	return -1;

}


void QRLocation::find4j()
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
			swap(vertexs_minRect_QR[i], vertexs_minRect_QR[3]);
		}
		if (vertexs_minRect_QR[i].x < mid.x && vertexs_minRect_QR[i].y > mid.y)
		{
			swap(vertexs_minRect_QR[i], vertexs_minRect_QR[2]);
		}
		if (vertexs_minRect_QR[i].x > mid.x && vertexs_minRect_QR[i].y < mid.y)
		{
			swap(vertexs_minRect_QR[i], vertexs_minRect_QR[0]);
		}
		if (vertexs_minRect_QR[i].x > mid.x && vertexs_minRect_QR[i].y > mid.y)
		{
			swap(vertexs_minRect_QR[i], vertexs_minRect_QR[1]);
		}
	}
}


bool QRLocation::location2(Mat a)
{
	canny(a);

	vector<vector<Point>> contours;
	
	vector<int> vector_contours_filter;//���д�Ŷ�λ��ĺ�ѡ
	vector<Point> vertex_minRect4;//װ��������λ�㹲12�����������
	vector<po>s;//�洢��λ����Ϣ

	vector<Vec4i> hierarchy;
	//hierarchyΪ2ά�ģ�hierarchy[i][k]�����i�������ĵ�k����������Ӧcontours�е���ţ���������Ϊ-1
	//	k==0--������ͬ���ĺ�һ������
	//	k==1--������ͬ����ǰһ������
	//	k==2--��������һ��������
	//	k==3--�������ĸ�����
	//����ͼ����������������ʽ����������������������ģʽ�������ƽ� 
	findContours(detected_edges, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	
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

	if ((vector_contours_filter.size() < 3))//���û���ҵ��㹻�Ķ�λ��
	{
		printf("The picture does not meet the requirements\n");
		return false;
	}
	int all = vector_contours_filter.size();
	
	for (int j = 0; j < all; j++)
	{
		po q;
		q.minx = 1e9, q.miny = 1e9, q.maxx = 0, q.maxy = 0;
		q.i = vector_contours_filter[j];
		q.d = find_tubao(contours[vector_contours_filter[j]]);
		int cnt = q.d.size();

		if (cnt > 4)
			approxPolyDP(q.d, q.d, 2, true);

		cnt = q.d.size();
		
		if (cnt < 4)continue;
		for (int i = 0; i < cnt; i++)//��ȷ�������ľ��ζ��㶪��ȥ
		{
			q.minx = min(q.minx, q.d[i].x);
			q.miny = min(q.miny, q.d[i].y);
			q.maxx = max(q.maxx, q.d[i].x);
			q.maxy = max(q.maxy, q.d[i].y);
		}
		s.push_back(q);
	}
	sort(s.begin(), s.end(),po_cmp);//�����������
	int ans = -1;
	ans = find_point(s);//����������ѡ��ĵ�һ�������
	if (ans == -1)
	{
		printf("not match\n");
		return false;
	}
	vector<Point>res_contours;
	if (hierarchy[s[ans].i][3] != -1)//�ҵ���λ��ĸ�����
	{
		res_contours = contours[hierarchy[s[ans].i][3]];
	}
	else
	{
		printf("not match\n");
		return false;
	}

	approxPolyDP(res_contours, res_contours, 50, true);

	vector<Point2f>point_minRect12;

	for (int i = 0; i < 4; i++)
	{
		Point2f temp = Point2f((double)res_contours[i].x, (double)res_contours[i].y);
		point_minRect12.push_back(temp);
	}
	for (int j = 0; j < 4; j++)
	{
		vertexs_minRect_QR[j] = point_minRect12[j];
	}

	Point2f midPoint_rect;

	for (int j = 0; j < 4; j++)
	{
		midPoint_rect.x += (point_minRect12[j].x / 4);
		midPoint_rect.y += (point_minRect12[j].y / 4);
	}
	int f = 0;
	find4j();

	f = 6;
	for (int i = ans; i < ans + 3; i++)
	{
		if (s[i].minx < midPoint_rect.x && s[i].miny < midPoint_rect.y)
		{
			f -= 2;
		}
		if (s[i].minx > midPoint_rect.x && s[i].miny < midPoint_rect.y)
		{
			f -= 3;
		}
		if (s[i].minx > midPoint_rect.x && s[i].miny > midPoint_rect.y)
		{
			f -= 0;
		}
		if (s[i].minx < midPoint_rect.x && s[i].miny > midPoint_rect.y)
		{
			f -= 1;
		}
	}
	Point2f temp1 = vertexs_minRect_QR[(2 + f) % 4];
	Point2f temp2 = vertexs_minRect_QR[(3 + f) % 4];
	Point2f temp3 = vertexs_minRect_QR[(0 + f) % 4];
	Point2f temp0 = vertexs_minRect_QR[(1 + f) % 4];

	vertexs_minRect_QR[0] = temp0;
	vertexs_minRect_QR[1] = temp1;
	vertexs_minRect_QR[2] = temp2;
	vertexs_minRect_QR[3] = temp3;
}


void QRLocation::end_correct(Mat f)
{
	int lenth = numberofpoint + 1;
	Point2f vertex_warp[4];
	vertex_warp[1] = Point2f(0, float(lenth - 1));
	vertex_warp[2] = Point2f(0, 0);
	vertex_warp[3] = Point2f(float(lenth - 1), 0);
	vertex_warp[0] = Point2f(float(lenth - 1), float(lenth - 1));
	Mat transform = getPerspectiveTransform(vertexs_minRect_QR, vertex_warp);
	warpPerspective(f, QRLocation_dst, transform, Size(lenth, lenth));//QRLocation_dst��Ϊ�����и������Ķ�ά��
}





void QRLocation::read(Mat a)
{
	cvtColor(a, a, COLOR_RGB2GRAY);
	//threshold(a, a, 170, 255,THRESH_BINARY);
	int pointOfeachBlock = numberofpoint / numberofblock;
	int py = 0; int i, j; int gray_sum = 0;
	for (i = 0; i < numberofblock; i++)
	{
		py = 0;
		for (j = 0; j < numberofblock; j++)
		{
			int cnt = 0; int gray_cnt = 0;
			line(a, Point(i * pointOfeachBlock, (j) * (pointOfeachBlock + py / (numberofblock - 2))), Point((i + 1) * pointOfeachBlock, (j) * (pointOfeachBlock + py / (numberofblock - 2))), Scalar(0, 255, 0));
			line(a, Point(i * pointOfeachBlock, (j) * (pointOfeachBlock + py / (numberofblock - 2))), Point((i) * pointOfeachBlock, (j + 1) * (pointOfeachBlock + py / (numberofblock - 2))), Scalar(0, 255, 0));
			for (int ii = i * pointOfeachBlock; ii < (i + 1) * pointOfeachBlock; ii++)
			{
				uchar* data = a.ptr<uchar>(ii);
				for (int jj = j * (pointOfeachBlock + py / (numberofblock - 2)); jj < (j + 1) * (pointOfeachBlock + py / (numberofblock - 2)); jj++)
				{
					if (data[jj] < 85)
						cnt++;//��¼�����ص�ĸ���
					if (data[jj] >= 85 && data[jj] < 170)
					{
						gray_cnt++;
					}
				}
			}
			if ((double)gray_cnt / (pointOfeachBlock * pointOfeachBlock) > 0.5)
			{
				gray_sum++;
			}
			if ((double)cnt / (pointOfeachBlock * pointOfeachBlock) > 0.5)//�ڿ��
			{
				ans[i * (numberofblock)+j] = 1;
			}
			else
			{
				ans[i * (numberofblock) + j] = 0;
			}
			if (j == 2 && ans[i * numberofblock + j] == 1)
			{
				py = -(double)cnt / pointOfeachBlock;
			}
		}
	}
	if (gray_sum > 15)
		getQR = false;
}


QRLocation::QRLocation(Mat src)//����ͼƬ
{
	getQR = false;
	ans = new char[(numberofblock) * (numberofblock)];
	memset(ans, 0, (numberofblock) * (numberofblock));
	if (src.empty()) {
		//����Ƿ��ȡͼ��
		printf("Error! Input image cannot be read...\n");
	}
	if (location2(src))
	{
		getQR = true;
		end_correct(QRLocation_fsrc);
		read(QRLocation_dst);
	}
}


QRList QRLocation::get()
{
	QRList list;
	if (getQR)
	{
		QRMatrix matrix(ans, numberofblock, numberofblock);
		list.append(matrix);
	}
	return list;
}