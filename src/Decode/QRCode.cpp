#include "QRCode.h"

char matrixCheck[NumberofColorBlocks][NumberofColorBlocks];
char xorMatrix[NumberofColorBlocks][NumberofColorBlocks];
//��λ��
const int bigAnchorPoint[8][8] = {
	1, 1, 1, 1, 1, 1, 1, 0,
	1, 0, 0, 0, 0, 0, 1, 0,
	1, 0, 1, 1, 1, 0, 1, 0,
	1, 0, 1, 1, 1, 0, 1, 0,
	1, 0, 1, 1, 1, 0, 1, 0,
	1, 0, 0, 0, 0, 0, 1, 0,
	1, 1, 1, 1, 1, 1, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0 };
//С��λ��
const int smallAnchorPoint[5][5] = {
	1, 1, 1, 1, 1,
	1, 0, 0, 0, 1,
	1, 0, 1, 0, 1,
	1, 0, 0, 0, 1,
	1, 1, 1, 1, 1 };
//�汾��Ϣ��101 1010 1010 0101��
const char version[16] = {
	1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1 };


QRCode::QRCode()
{
	byte = 0;
	QRCodeBasic();
	buffer = NULL;
}
QRCode::QRCode(DataBuffer* dataBuffer)
{
	byte = 0;
	QRCodeBasic();
	setBuffer(dataBuffer);
}
void QRCode::setBuffer(DataBuffer* dataBuffer)
{
	buffer = dataBuffer;
}

QRList QRCode::locationQR(Mat& img)
{
	//��λ��ά���λ��
	QRLocation location(img);
	return location.get();
}

bool QRCode::decode(Mat img)
{
	if (buffer == NULL)
		return false;

	int totalQR = 0; //�ܼƶ�ά������
	int nowQR = 0; //��ǰ�ǵڼ��Ŷ�ά��
	QRList QRlist = locationQR(img);
	for (int i = 0; i < QRlist.size(); i++)
	{
		if (versionCheck(QRlist[i]) && sizeCheck(QRlist[i]))
		{
			Xor(QRlist[i]);
			int x = 1;
			int y = 1;
			if (EffectivenessCheck(QRlist[i], x, y))
				read(QRlist[i], x, y);
		}
	}

	return true;
}
void QRCode::fixPoint(int& x, int& y)
{
	//xΪ�У�yΪ��
	//�Զ�����һ�����ҵ�������ʵĿ�λ��һ������ͻ���Ϊ0,0
	x++;
	if (x >= NumberofColorBlocks)
	{
		x = 0;
		y++;
	}
	if (y >= NumberofColorBlocks)
	{
		x = NumberofColorBlocks - 1;
		y = NumberofColorBlocks - 1;
		return;
	}
	if (matrixCheck[x][y] != -1)
		fixPoint(x, y);
}

//��Ӷ�ά��Ļ�����Ϣ����λ�㡢����ͼ����ʽ�㣩
void QRCode::QRCodeBasic()
{
	int i, j;
	//ȫ������-1������û�����ݣ�-1���ᱻ����Ϊ0�����ͼƬ�
	for (i = 0; i < NumberofColorBlocks; i++)
	{
		for (j = 0; j < NumberofColorBlocks; j++)
		{
			matrixCheck[i][j] = -1;
		}
	}

	//���λ��
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 8; j++)
		{
			matrixCheck[i][j] = bigAnchorPoint[i][j];
			matrixCheck[NumberofColorBlocks - 1 - i][j] = bigAnchorPoint[i][j];
			matrixCheck[i][NumberofColorBlocks - 1 - j] = bigAnchorPoint[i][j];
		}
	}


	//���λ��
	for (i = 8; i < NumberofColorBlocks - 8; i++)
	{
		matrixCheck[0][i] = (i + 1) % 2;
		matrixCheck[i][0] = (i + 1) % 2;
	}

	//��д�汾��Ϣ
	for (i = 0; i < 16; i++)
	{
		matrixCheck[1][i + 8] = version[i];
		matrixCheck[i + 8][1] = version[i];
	}
}
bool QRCode::versionCheck(QRMatrix& matrix)
{
	bool version1 = true;
	bool version2 = true;
	for (int i = 0; i < 16; i++)
	{
		if (matrix.at(3, i + 11) != version[i])
			version1 = false;
		if (matrix.at(i + 11, 3) != version[i])
			version1 = false;
	}
	return version1 || version2;
}
bool QRCode::sizeCheck(QRMatrix& matrix)
{
	bool check1 = true;
	bool check2 = true;
	//��λ����
	for (int i = 8; i < NumberofColorBlocks - 8; i++)
	{
		if (matrix.at(2, i + 2) != (i + 1) % 2)
			check1 = false;
		if (matrix.at(i + 2, 2) != (i + 1) % 2)
			check2 = false;
	}

	return check1 || check2;
}
void QRCode::Xor(QRMatrix& matrix)
{
	for (int i = 0; i < NumberofColorBlocks; i++)
	{
		for (int j = 0; j < NumberofColorBlocks; j++)
		{
			const int block = 5; //�����С
			int x = i / block;
			int y = j / block;
			xorMatrix[i][j] = (char)((x + y + 1) % 2);
		}
	}

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			xorMatrix[i][j] = -1;
			xorMatrix[NumberofColorBlocks - 1 - i][j] = -1;
			xorMatrix[i][NumberofColorBlocks - 1 - j] = -1;
		}
	}

	for (int i = 8; i < NumberofColorBlocks - 8; i++)
	{
		xorMatrix[0][i] = -1;
		xorMatrix[i][0] = -1;
	}

	//��д�汾��Ϣ
	for (int i = 0; i < 16; i++)
	{
		xorMatrix[1][i + 8] = -1;
		xorMatrix[i + 8][1] = -1;
	}


	for (int i = 0; i < NumberofColorBlocks; i++)
	{
		for (int j = 0; j < NumberofColorBlocks; j++)
		{
			if (xorMatrix[i][j] != -1)
			{
				int compare = (matrix.at(i + 2, j + 2) == -1) ? 0 : matrix.at(i + 2, j + 2);
				if (compare != xorMatrix[i][j])
					matrix.at(i + 2, j + 2) = 1;
				else
					matrix.at(i + 2, j + 2) = 0;
			}
		}
	}
}
bool QRCode::EffectivenessCheck(QRMatrix& matrix, int& x, int& y)
{
	unsigned int writeSize = 0;
	unsigned short page = 0; //��ǰҳ��
	unsigned short pageTotle = 0; //�ܹ�ҳ��

	unsigned int pow = 1;
	for (int i = 0; i < 16; i++)
	{
		fixPoint(x, y);
		if (matrix.at(x + 2, y + 2) == 1)
			page += pow;
		pow *= 2;
	}
	pow = 1;
	for (int i = 0; i < 16; i++)
	{
		fixPoint(x, y);
		if (matrix.at(x + 2, y + 2) == 1)
			pageTotle += pow;
		pow *= 2;
	}
	pow = 1;
	for (int i = 0; i < 32; i++)
	{
		fixPoint(x, y);
		if (matrix.at(x + 2, y + 2) == 1)
			writeSize += pow;
		pow *= 2;
	}
#ifdef DEBUG
	printf("reading:page:%d\ttotal page:%d\twrite byte:%d\n", page, pageTotle, writeSize);
#endif

	buffer->setTotal((buffer->getTotal() < pageTotle) ? pageTotle : buffer->getTotal());
	if (page >= buffer->getNow()) 
	{
#ifdef DEBUG
		printf("Valid read!!!\n");
#endif
		if(page - buffer->getNow() > 0)
			printf("lost %d pages!!\n", page - buffer->getNow());
		buffer->setNow(page + 1);
		byte = writeSize;
		return true;
	}
	else
	{
#ifdef DEBUG
		printf("ignore\n");
#endif
		return false;
	}
}
void QRCode::read(QRMatrix& matrix, int& x, int& y)
{
	while (byte--) 
	{
		char tmp = 0;
		char pow = 1;
		for (int i = 0; i < 8; i++)
		{
			fixPoint(x, y);
			if (matrix.at(x + 2, y + 2) == 1)
				tmp += pow;
			pow *= 2;
		}
		buffer->append(tmp);
	}
}