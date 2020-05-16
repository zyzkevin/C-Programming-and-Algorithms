#include <iostream>
#include <fstream>
#include <cstring>
#pragma pack(1)

using namespace std;

typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned long LONG;

struct tagBITMAPFILEHEADER{
WORD bfType; // 位图文件的类型，必须为“BM” 
DWORD bfSize; // 位图文件的大小，以字节为单位 
WORD bfReserved1; // 位图文件保留字，必须为0 
WORD bfReserved2; // 位图文件保留字，必须为0 
DWORD bfOffBits; // 位图数据的起始位置，以相对于位图文件头的偏移量表示，以字节为单位 
}; //该结构占据14个字节。

struct tagBITMAPINFOHEADER{ 

    DWORD biSize; // 本结构所占用字节数 
    LONG biWidth; // 位图的宽度，以像素为单位 
    LONG biHeight; // 位图的高度，以像素为单位 
    WORD biPlanes; // 目标设备的平面数不清，必须为1 
    WORD biBitCount; // 每个像素所需的位数，必须是1(双色), 4(16色)，8(256色)或24(真彩色)之一 
    DWORD biCompression; // 位图压缩类型，必须是 0(不压缩),1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一 
    DWORD biSizeImage; // 位图的大小，以字节为单位
    LONG biXPelsPerMeter; // 位图水平分辨率，每米像素数 
    LONG biYPelsPerMeter; // 位图垂直分辨率，每米像素数 
    DWORD biClrUsed;// 位图实际使用的颜色表中的颜色数 
    DWORD biClrImportant;// 位图显示过程中重要的颜色数 
} ; //该结构占据40个字节。

struct tagRGB3 { 
    BYTE blue;// 蓝色的亮度(值范围为0-255) 
    BYTE green; // 绿色的亮度(值范围为0-255) 
    BYTE red; // 红色的亮度(值范围为0-255) 
    };

struct tagRGB4 { 
    BYTE rgbBlue;// 蓝色的亮度(值范围为0-255) 
    BYTE rgbGreen; // 绿色的亮度(值范围为0-255) 
    BYTE rgbRed; // 红色的亮度(值范围为0-255) 
    BYTE alpha;
    };
    
typedef int FXPT2DOT30;
typedef struct tagCIEXYZ{
    FXPT2DOT30 ciexyzX;
    FXPT2DOT30 ciexyzY;
    FXPT2DOT30 ciexyzZ;
}

CIEXYZ, * LPCIEXYZ ;

typedef struct tagCIEXYZTRIPLE{
    CIEXYZ ciexyzRed;
    CIEXYZ ciexyzGreen;
    CIEXYZ ciexyzBlue;
}CIEXYZTRIPLE;

struct mem32{
    DWORD        bV5RedMask;
    DWORD        bV5GreenMask;
    DWORD        bV5BlueMask;
    DWORD        bV5AlphaMask;
    DWORD        bV5CSType;
    CIEXYZTRIPLE bV5Endpoints;
    DWORD        bV5GammaRed;
    DWORD        bV5GammaGreen;
    DWORD        bV5GammaBlue;
    DWORD        bV5Intent;
    DWORD        bV5ProfileData;
    DWORD        bV5ProfileSize;
    DWORD        bV5Reserved;
};


int make4( const tagBITMAPINFOHEADER & a)
{
    int l = (a.biWidth * a.biBitCount) / 8;
    if (4 - (l % 4) == 4)
        return 0;
    else return 4 - (l % 4);
}

void rotate32(ifstream & srcFile, ofstream & destFile, const tagBITMAPFILEHEADER & head, const tagBITMAPINFOHEADER & info);

void rotate(ifstream & srcFile, ofstream & destFile, tagBITMAPFILEHEADER & head, tagBITMAPINFOHEADER & info)
{

    int h = info.biHeight;
    int w = info.biWidth;
    int size = h * w;

    int rem = make4(info);

    tagRGB3 * pixel = new tagRGB3[size];

    for( int i = 0; i < h; i++)
    {
        srcFile.read( (char*) (pixel + w * i), w * sizeof(tagRGB3));
        srcFile.seekg(rem, ios::cur);
    }

    //rotating
    int newh = w;
    int neww = h;
    int news = neww* newh;

    //editing new info + head
    tagBITMAPFILEHEADER destHead = head;
    tagBITMAPINFOHEADER destInfo = info;

    destInfo.biHeight = (DWORD)newh;
    destInfo.biWidth = (DWORD)neww;

    rem = make4(destInfo);
    destInfo.biSizeImage = newh * (neww + rem);
    destHead.bfSize = (DWORD) (sizeof(destHead) + sizeof(destInfo) + destInfo.biSizeImage);
    
    tagRGB3 * pixelrot = new tagRGB3[news];
    
    for( int i  = 0; i < newh; i++)
        for( int j = 0; j < neww; j++)
        {
            *(pixelrot + i * neww + j) = *( pixel + j * w + newh - i - 1);
        }

    destFile.write((char *) &destHead, sizeof(tagBITMAPFILEHEADER));
    destFile.write((char *) &destInfo, sizeof(tagBITMAPINFOHEADER));

    char* zero = new char[rem + 1];
	memset(zero, 0, rem + 1);


    for(int i = 0; i < newh ; i++){
		destFile.write((char*) (pixelrot + i* neww), sizeof(tagRGB3) * neww);
		destFile.write((char*) zero, rem);
	}

    delete [] pixel;
    delete [] pixelrot;

}

mem32 filling; //stores 32 info

int main( int argc, char * argv[])
{
    if( argc != 3)  
    {
        cout << "Invalid arguments" << endl;
        system("pause");
        return 0;

    }

    tagBITMAPFILEHEADER fileHead;
    tagBITMAPINFOHEADER infoHead;

    ifstream srcFile;
    
    srcFile.open( argv[1],  ios::in | ios::binary);
    if( !srcFile)
    {
        cout << "Source File open error" << endl;
        system("pause");
        return 0;
    }

    ofstream destFile( argv[2], ios::out | ios::binary);
    if( !destFile)
    {
        cout << "Dest File open error" << endl;
        srcFile.close();
        system("pause");
        return 0;
    }

    srcFile.seekg(0, ios::beg);

    srcFile.read( (char*)& fileHead, sizeof( fileHead));
    srcFile.read( (char*) & infoHead, sizeof( infoHead));
    if( infoHead.biBitCount == 32)
        srcFile.read( (char*) & filling, sizeof( filling));

    switch( infoHead.biBitCount)
    {
        case 24: rotate( srcFile, destFile, fileHead, infoHead); break;
        default: rotate32( srcFile, destFile, fileHead, infoHead); break;

    }
    /*

    for( int i = 0; i < h; i++)
    {
        destFile.write( (char*) &pixelrot[i], sizeof( pixel[i]));
        destFile.seekp(rem, ios::cur);
    }
    
    */
    
    srcFile.close();
    destFile.close();

    system("pause");
    return 0;
}


void rotate32(ifstream & srcFile, ofstream & destFile, const tagBITMAPFILEHEADER & head, const tagBITMAPINFOHEADER & info)
{

    int h = info.biHeight;
    int w = info.biWidth;
    int size = h * w;

    int rem = make4(info);

    tagRGB4 * pixel = new tagRGB4[size];

    for( int i = 0; i < h; i++)
    {
        srcFile.read( (char*) (pixel + w * i), w * sizeof(tagRGB4));
        srcFile.seekg(rem, ios::cur);
    }

    //rotating
    int newh = w;
    int neww = h;
    int news = neww* newh;

    //editing new info + head
    tagBITMAPFILEHEADER destHead = head;
    tagBITMAPINFOHEADER destInfo = info;

    destInfo.biHeight = (DWORD)newh;
    destInfo.biWidth = (DWORD)neww;

    rem = make4(destInfo);
    destInfo.biSizeImage = newh * (neww + rem);
    destHead.bfSize = (DWORD) (sizeof(destHead) + sizeof(destInfo) + destInfo.biSizeImage);
    
    tagRGB4 * pixelrot = new tagRGB4[news];
    
    for( int i  = 0; i < newh; i++)
        for( int j = 0; j < neww; j++)
        {
            *(pixelrot + i * neww + j) = *( pixel + j * w + newh - i - 1);
        }

    destFile.write((char *) &destHead, sizeof(tagBITMAPFILEHEADER));
    destFile.write((char *) &destInfo, sizeof(tagBITMAPINFOHEADER));
    destFile.write( (char*) & filling, sizeof( filling));

    char* zero = new char[rem + 1];
	memset(zero, 0, rem + 1);


    for(int i = 0; i < newh ; i++){
		destFile.write((char*) (pixelrot + i* neww), sizeof(tagRGB4) * neww);
		destFile.write((char*) zero, rem);
	}

    delete [] pixel;
    delete [] pixelrot;

}