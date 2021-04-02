#include "QRCode.h"

//=======================================
//���ݶ���
//=======================================
#define WihtePadding (Numbmerofpixels * 2)        //�ױ�����
#define MarginPadding 30        //��߾�ױ�����
#define BlackSquare 3           //�ڿ�����
#define NumberofColorBlocks 51 //�����С���߳�����Ӧ�������㣺1����С����С��35,2��Ϊ������
#define Numbmerofpixels 5      //�������Ӧ������
#define LinePixels (Numbmerofpixels * NumberofColorBlocks + 2 * WihtePadding + 2 * MarginPadding + 2 * BlackSquare)
#define DataContain (NumberofColorBlocks * NumberofColorBlocks - 192 - 30 - (NumberofColorBlocks - 16) * 2) //���Դ������������217Ϊ��λ��ռ�ã�30Ϊ�汾��Ϣ

char matrix[NumberofColorBlocks][NumberofColorBlocks]; //��ά����Ϣ������һ��û����������飡��
char xorMatrix[NumberofColorBlocks][NumberofColorBlocks]; //����ڸǲ�
vector<Vec3b> rgb_pixels;                             //���� rgb���ص��б�
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
    1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1};

//=======================================
//���岿��
//=======================================

QRCode::QRCode()
{
    init();
    buffer = NULL;
}

QRCode::QRCode(DataBuffer* _buffer)
{
    init();
    setBuffer(_buffer);
}
void QRCode::setBuffer(DataBuffer* _buffer)
{
    buffer = _buffer;
}

void QRCode::init()
{
    //��ʼ��
    rgb_pixels.push_back({ 255, 255, 255 }); //0--��ɫ
    rgb_pixels.push_back({ 0, 0, 0 });       //1--��ɫ
}
bool QRCode::isEnd()
{
    if (buffer != NULL)
        return buffer->isEnd();
    else
        return true;
}

//���ɶ�ά��
Mat QRCode::getQRCode()
{
    //��ʼ��ͼƬ����ֵΪ��ɫ
    Mat img(LinePixels, LinePixels, CV_8UC3, Scalar(255, 255, 255));

    //��д������Ϣ
    QRCodeBasic();
    //cout << "���Դ���Ķ�����λ����" << DataContain << endl;

    //д������
    writeData();

    //�������
    Xor();

    //д��ͼƬ
    for (int i = 0; i < NumberofColorBlocks; i++)
    {
        for (int j = 0; j < NumberofColorBlocks; j++)
        {
            //д�뵥��0��1
            for (int p = 0; p < Numbmerofpixels; p++)
            {
                for (int q = 0; q < Numbmerofpixels; q++)
                {
                    int x = i * Numbmerofpixels + p + WihtePadding + MarginPadding + BlackSquare;
                    int y = j * Numbmerofpixels + q + WihtePadding + MarginPadding + BlackSquare;
                    int color = (matrix[i][j] == -1) ? 0 : matrix[i][j];
                    img.at<Vec3b>(x, y) = rgb_pixels[color];
                }
            }
        }
    }
    for (int i = MarginPadding; i < MarginPadding + BlackSquare; i++)
    {
        for (int j = MarginPadding; j < LinePixels - MarginPadding; j++)
        {
            img.at<Vec3b>(i, j) = 1;
            img.at<Vec3b>(j, i) = 1;
            img.at<Vec3b>(LinePixels - i, j) = 1;
            img.at<Vec3b>(LinePixels - j, LinePixels - i) = 1;
        }
    }
    return img;
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
            matrix[i][j] = -1;
        }
    }

    //���λ��
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            matrix[i][j] = bigAnchorPoint[i][j];
            matrix[NumberofColorBlocks - 1 - i][j] = bigAnchorPoint[i][j];
            matrix[i][NumberofColorBlocks - 1 - j] = bigAnchorPoint[i][j];
        }
    }


    //���λ��
    for (i = 8; i < NumberofColorBlocks - 8; i++)
    {
        matrix[0][i] = (i + 1) % 2;
        matrix[i][0] = (i + 1) % 2;
    }

    //��д�汾��Ϣ
    for (i = 0; i < 16; i++)
    {
        matrix[1][i + 8] = version[i];
        matrix[i + 8][1] = version[i];
    }
}

//д������
void QRCode::writeData()
{
    //��ά��ͷ����ռȥ8���ֽڣ�Ϊ�˷��㲢δ���ñ䳤���룬��������ʱ������Ż�
    const int headCover = 8;
    if (buffer == NULL)
        return;
    unsigned int size = DataContain / 8; //��ǰ��С��ά���д�ֽ���
    unsigned int dataSize = buffer->size();
    unsigned int pointer = buffer->pointer();
    string type = buffer->getFiletype();

    unsigned int writeSize = (dataSize - pointer > size - headCover) ? size - headCover : (dataSize - pointer);
    int x = 1; //����д��λ��ָ��
    int y = 1;
    unsigned short page = pointer / (size - headCover); //��ǰҳ��
    unsigned short pageTotle = dataSize / (size - headCover); //�ܹ�ҳ��

    unsigned short pow = 1;
    for (int i = 0; i < 16; i++)
    {
        fixPoint(x, y);
        matrix[x][y] = ((page & pow) == pow) ? 1 : 0;
        pow *= 2;
    }
    pow = 1;
    for (int i = 0; i < 16; i++)
    {
        fixPoint(x, y);
        matrix[x][y] = ((pageTotle & pow) == pow) ? 1 : 0;
        pow *= 2;
    }

    unsigned int pow2 = 1;
    for (int i = 0; i < 32; i++)
    {
        fixPoint(x, y);
        matrix[x][y] = ((writeSize & pow2) == pow2) ? 1 : 0;
        pow2 *= 2;
    }

    while (writeSize-- && !buffer->isEnd())
    {
        char tmp = buffer->nextChar();
        char pow3 = 1;
        for (int i = 0; i < 8; i++)
        {
            fixPoint(x, y);
            matrix[x][y] = ((tmp & pow3) == pow3) ? 1 : 0;
            pow3 *= 2;
        }
    }
    printf("Current pages:%d\tTotal pages:%d\n", page, pageTotle);
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
    if (matrix[x][y] != -1)
        fixPoint(x, y);
}


void QRCode::Xor()
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
                int compare = (matrix[i][j] == -1) ? 0 : matrix[i][j];
                if (compare != xorMatrix[i][j])
                    matrix[i][j] = 1;
                else
                    matrix[i][j] = 0;
            }
        }
    }
}
