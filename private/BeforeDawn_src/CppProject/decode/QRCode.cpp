#include "QRCode.h"

QRCode::QRCode()
{
	buffer = NULL;
}
QRCode::QRCode(DataBuffer* dataBuffer)
{
	setBuffer(dataBuffer);
}
void QRCode::setBuffer(DataBuffer* dataBuffer)
{
	buffer = dataBuffer;
}


QRList QRCode::locationQR(Mat img)
{
	//��λ��ά���λ�ã�δ��ɣ�
	QRList test;
	return test;
}

bool QRCode::decode(Mat img)
{
	if (buffer == NULL)
		return false;

	QRList QRlist = locationQR(img);
	for (int i = 0; i < QRlist.size(); i++)
	{

	}

	return true;
}