//#include <QtCore/QCoreApplication>
#include <stdio.h>
//#include <tchar.h>
#include <stdint.h>
#include <vector>
//#include <windows.h>
#include <algorithm>
#include <memory>
//#include <direct.h>
#include <unistd.h>
#include <string.h>

uint32_t width = 180;
uint32_t height = 180;
const uint16_t depth = 24;

uint8_t ARMOUT = 0;
uint8_t ARMIN  = 0;

uint8_t TEST_ONE  = 0;
uint8_t TEST_TWO = 0;

#define FILE_TEST 0
#define RAW2B 1
#define LTOUTFIRST 0
//#define TEST_ONE 0
//#define MAX_PATH 180

#define BI_RGB       0         //不压缩（最常用）
#define BI_RLE8      1         //8比特游程编码（BLE），只用于8位位图
#define BI_RLE4      2         //4比特游程编码（BLE），只用于4位位图
#define BI_BITFIELDS 3         //比特域（BLE），只用于16/32位位图

#define NONE         "\033[m" 
#define RED          "\033[0;32;31m"


char file_name[256] =  "image180180_armout.bmp";
char fbmpR_name[256] = "output0_buffer.hex";
char fbmpG_name[256] = "output1_buffer.hex";
char fbmpB_name[256] = "output2_buffer.hex";
char bmp_prefix[] = ".bmp";	

#pragma pack(2) // 让编译器做2字节对齐 
typedef struct tagBITMAPFILEHEADER
{ 
    uint16_t bfType;         //2Bytes，必须为"BM"，即0x424D 才是Windows位图文件
    uint32_t bfSize;         //4Bytes，整个BMP文件的大小
    uint16_t bfReserved1;    //2Bytes，保留，为0
    uint16_t bfReserved2;    //2Bytes，保留，为0
    uint32_t bfOffBits;      //4Bytes，文件起始位置到图像像素数据的字节偏移量
} BITMAPFILEHEADER;
#pragma pack() // 取消2字节对齐，恢复为默认对齐

typedef struct _tagBMP_INFOHEADER
{
    uint32_t  biSize;           //4Bytes，INFOHEADER结构体大小，存在其他版本I NFOHEADER，用作区分
    uint32_t  biWidth;          //4Bytes，图像宽度（以像素为单位）
    uint32_t  biHeight;         //4Bytes，图像高度，+：图像存储顺序为Bottom2Top，-：Top2Bottom
    uint16_t  biPlanes;         //2Bytes，图像数据平面，BMP存储RGB数据，因此总为1
    uint16_t  biBitCount;       //2Bytes，图像像素位数
    uint32_t  biCompression;    //4Bytes，0：不压缩，1：RLE8，2：RLE4
    uint32_t  biSizeImage;      //4Bytes，4字节对齐的图像数据大小
    uint32_t  biXPelsPerMeter;  //4 Bytes，用象素/米表示的水平分辨率
    uint32_t  biYPelsPerMeter;  //4 Bytes，用象素/米表示的垂直分辨率
    uint32_t  biClrUsed;        //4 Bytes，实际使用的调色板索引数，0：使用所有的调色板索引
    uint32_t  biClrImportant;   //4 Bytes，重要的调色板索引数，0：所有的调色板索引都重要
}BITMAPINFOHEADER;

void createBMP(uint8_t* bitmapBits, uint32_t width, uint32_t height, uint16_t depth, const char* fileName)
{
        uint32_t headersSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
        uint32_t pixelDataSize = height * ((width * (depth / 8)));
		
		printf("pixelDataSize = %d\n",pixelDataSize);
		printf("headersSize = %d\n",headersSize);
		
		printf("sizeof(BITMAPFILEHEADER) = %ld\n",sizeof(BITMAPFILEHEADER));
		printf("sizeof(BITMAPINFOHEADER) = %ld\n",sizeof(BITMAPINFOHEADER));

        BITMAPFILEHEADER bmpFileHeader = {};
        bmpFileHeader.bfType = 0x4D42;
        bmpFileHeader.bfOffBits = headersSize;
        bmpFileHeader.bfSize = headersSize + pixelDataSize;

        BITMAPINFOHEADER bmpInfoHeader = {};
        bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfoHeader.biBitCount = depth;
        bmpInfoHeader.biClrImportant = 0;
        bmpInfoHeader.biClrUsed = 0;
        bmpInfoHeader.biCompression = BI_RGB;
        bmpInfoHeader.biHeight = height;
        bmpInfoHeader.biWidth = width;
        bmpInfoHeader.biPlanes = 1;
        bmpInfoHeader.biSizeImage = pixelDataSize;

        //WCHAR wfile[64];
        //swprintf(wfile, L"%S", fileName);

        FILE* hFile = fopen(fileName, "wb");
        if (!hFile)
            return;

        fwrite(&bmpFileHeader, 1, sizeof(bmpFileHeader), hFile);
        fwrite(&bmpInfoHeader, 1, sizeof(bmpInfoHeader), hFile);
        fwrite(bitmapBits,     1, bmpInfoHeader.biSizeImage, hFile);

        fclose(hFile);
}

void writeData2Bmp(uint8_t* bitmapBits, uint16_t data_len, FILE* hFile)
{
	//int ret = 0;
	//printf(".writeData2Bmp0..................!\n");
	fseek(hFile, 0L, /*SEEK_END*/2);//put file point to the file end.
	//printf(".writeData2Bmp1..................!\n");
	fwrite(bitmapBits, 1, data_len, hFile);
	//printf(".writeData2Bmp2..................!\n");
}

int createBmp_with_fileheader(uint32_t width, uint32_t height, uint16_t depth, const char* fileName)
{
        uint32_t headersSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
        uint32_t pixelDataSize = height * ((width * (depth / 8)));
		
		printf("pixelDataSize = %d\n",pixelDataSize);
		printf("headersSize = %d\n",headersSize);
		
		printf("sizeof(BITMAPFILEHEADER) = %ld\n",sizeof(BITMAPFILEHEADER));
		printf("sizeof(BITMAPINFOHEADER) = %ld\n",sizeof(BITMAPINFOHEADER));

        BITMAPFILEHEADER bmpFileHeader = {};
        bmpFileHeader.bfType = 0x4D42;
        bmpFileHeader.bfOffBits = headersSize;
        bmpFileHeader.bfSize = headersSize + pixelDataSize;

        BITMAPINFOHEADER bmpInfoHeader = {};
        bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmpInfoHeader.biBitCount = depth;
        bmpInfoHeader.biClrImportant = 0;
        bmpInfoHeader.biClrUsed = 0;
        bmpInfoHeader.biCompression = BI_RGB;
        bmpInfoHeader.biHeight = height;
        bmpInfoHeader.biWidth = width;
        bmpInfoHeader.biPlanes = 1;
        bmpInfoHeader.biSizeImage = pixelDataSize;

        //WCHAR wfile[64];
        //swprintf(wfile, L"%S", fileName);

        FILE* hFile = fopen(fileName, "wb");
        if (!hFile)
		{
			printf("Can't creat file %s\n", fileName);
		    return -1;
		}

        fwrite(&bmpFileHeader, 1, sizeof(bmpFileHeader), hFile);
        fwrite(&bmpInfoHeader, 1, sizeof(bmpInfoHeader), hFile);
        //fwrite(bitmapBits,     1, bmpInfoHeader.biSizeImage, hFile);

        fclose(hFile);
		
		printf("Creat file %s successful!\n", fileName);
		return 0;
}


unsigned int matoi(char* str, int count)
{
    unsigned int tmp = 0;
    int num = count;
    int i=0;
    //printf("num = %d\n",num);
    for(i=0; i< num; i++)
    {
        if(str[i]<=0x39)
            tmp |= (str[i]-0x30)<<((num-1-i)*4);
        else
        {
            switch(str[i])
            {
                case 'a':
                case 'A':
                   tmp |= (0xa)<<((num-1-i)*4);
                   break;
                case 'b':
                case 'B':
                   tmp |= (0xb)<<((num-1-i)*4);
                   break;
                case 'c':
                case 'C':
                   tmp |= (0xc)<<((num-1-i)*4);
                   break;
                case 'd':
                case 'D':
                   tmp |= (0xd)<<((num-1-i)*4);
                   break;
                case 'e':
                case 'E':
                   tmp |= (0xe)<<((num-1-i)*4);
                   break;
                case 'f':
                case 'F':
                   tmp |= (0xf)<<((num-1-i)*4);
                   break;
                default:
                   break;
            }
        }
    }
    //printf("ungigend int size = %d\n",sizeof(unsigned int));
    return tmp;
}

static void usage(char* name)
{
    printf("\nNAME\n");
    printf("\tConvert 3 hex files to rgb file\n\n");
    printf("SYNOPSIS\n");
    printf("\t%s -i [-f path] \n\n", name);
    printf("DESCRIPTION\n");
    //printf("\t\t f1, f2, f3   hex file\n");
    //printf("\t\t output-file  binary file\n");
	printf("\t\t -i    Convert arm in files!\n");
	printf("\t\t -o    Convert arm out files!\n");
	printf("\t\t -c    Convert isp out files not filter offset bytes!\n");
	printf("\t\t -k    Convert isp out files filter offset bytes!\n");
	printf("\t\t -f    Selectable Input the floder path where hex files in!\n");
	printf("\t\t -u    please input output file name!\n");
	printf("\t\t -w    please input image with pixels ! default 180\n");
	printf("\t\t -h    please input image height!default 180 \n");
	printf("\t\t -H    For usag informaion!\n");
    printf("\n\n");
}

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);
    //std::vector<uint8_t> buffer(width * 3 * 1); //get one row data mem
    //const int radius = 300;
	int ret = 0;
#if FILE_TEST //RAW2B
    const char* file_name = "new_image180180_c.bmp";
    const char* fbmpR_name = "output_r14g14b14_buffer0.hex";
    const char* fbmpG_name = "output_r14g14b14_buffer1.hex";
    const char* fbmpB_name = "output_r14g14b14_buffer2.hex";
#endif

	uint8_t* pData = NULL;//buffer.data();
	int ch;
	uint8_t f_path = 0;
	uint8_t f_outfile = 0;
	uint32_t flen = 0;
	
	uint32_t u16_data_len = 0;
	
	char input_path[256] ={ 0 };
	char outfile_name[128] ={ 0 };
	
	uint32_t seek = 0;
    unsigned int dataR = 0;
    unsigned int dataG = 0;
    unsigned int dataB = 0;
	
	uint8_t dataRL = 0;
	uint8_t dataRH = 0;
	uint8_t dataGL = 0;
	uint8_t dataGH = 0;
	uint8_t dataBL = 0;
	uint8_t dataBH = 0;
	
	//char buff[8]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37};
    char strR[9];
    char strG[9];
    char strB[9];
	
	ARMIN = 0;
	ARMOUT = 0;
    TEST_ONE  = 0;
	TEST_TWO  = 0;
	
	printf("argc %d\n",argc);
	if(argc < 2)
	{
		usage(argv[0]);
        exit(-1);
	}
	
	while ((ch = getopt(argc, argv, "iockf:u:w:h:H?")) != -1)
    {
        switch (ch) {
        case 'i':
            ARMIN = 1;
            break;
        case 'o':
            ARMOUT = 1;
            break;
		case 'c':
            TEST_ONE = 1;
            break;
		case 'k':
            TEST_TWO = 1;
            break;
	    case 'f':
		    f_path = 1;
			sprintf(input_path, "%s", optarg);
			if(strlen(input_path) > 250)
			{
				printf("input_path length is more max length 250!\n");
				usage(argv[0]);
				exit(-1);
			}
			else if(input_path[strlen(input_path) - 1] != '/')
			{
				input_path[strlen(input_path)] = '/';
				//input_path[0][strlen(input_path[0]) + 1] = '\0';
			}
			//printf("case input_path = %s\n", input_path);
			break;
		case 'w':
			width = atoi(optarg);
			break;
		case 'h':
		    height = atoi(optarg);
			break;
		case 'u':
			f_outfile = 1;
			sprintf(outfile_name, "%s", optarg);
			break;
        case 'H':
        case '?':
        default:
            usage(argv[0]);
            exit(-1);
        }   
    }
	
    if(ARMOUT)
	{
		//file_name =  "image180180_armout.bmp";
		//fbmpR_name = "output0_buffer.hex";
		//fbmpG_name = "output1_buffer.hex";
		//fbmpB_name = "output2_buffer.hex";
		
		sprintf(file_name,  "%s%d%d%s", "image", width, height, "_armout.bmp");
		sprintf(fbmpR_name, "%s", "output0_buffer.hex");
		sprintf(fbmpG_name, "%s", "output1_buffer.hex");
		sprintf(fbmpB_name, "%s", "output2_buffer.hex");
	}
    else if(ARMIN)
    {
        //file_name =  "image180180_armin.bmp";
        //fbmpR_name = "input0_buffer.hex";
        //fbmpG_name = "input1_buffer.hex";
        //fbmpB_name = "input2_buffer.hex";
		
		sprintf(file_name,  "%s%d%d%s", "image", width, height, "_armin.bmp");
		sprintf(fbmpR_name, "%s", "input0_buffer.hex");
		sprintf(fbmpG_name, "%s", "input1_buffer.hex");
		sprintf(fbmpB_name, "%s", "input2_buffer.hex");
	}
	else if(TEST_ONE)
    {
		//file_name = "output_image192180_with24bytes.bmp";
        //fbmpR_name = "output_r14g14b14_buffer0.hex";
        //fbmpG_name = "output_r14g14b14_buffer1.hex";
        //fbmpB_name = "output_r14g14b14_buffer2.hex";
		
		sprintf(file_name,  "%s%d%d%s", "image", width, height, "_with24bytes.bmp");
		sprintf(fbmpR_name, "%s", "output_r14g14b14_buffer0.hex");
		sprintf(fbmpG_name, "%s", "output_r14g14b14_buffer1.hex");
		sprintf(fbmpB_name, "%s", "output_r14g14b14_buffer2.hex");
	}
    else if(TEST_TWO)
    {
		//file_name = "output_image192180_no24bytes.bmp";
        //fbmpR_name = "output_r14g14b14_buffer0.hex";
        //fbmpG_name = "output_r14g14b14_buffer1.hex";
        //fbmpB_name = "output_r14g14b14_buffer2.hex";
		
		sprintf(file_name,  "%s%d%d%s", "image",width, height,"_no24bytes.bmp");
		sprintf(fbmpR_name, "%s", "output_r14g14b14_buffer0.hex");
		sprintf(fbmpG_name, "%s", "output_r14g14b14_buffer1.hex");
		sprintf(fbmpB_name, "%s", "output_r14g14b14_buffer2.hex");		
	}
	
	if(f_outfile)
	{
		uint8_t len = strlen(outfile_name);
		uint8_t to = (outfile_name[len-1]^'p') |(outfile_name[len-2]^'m') \
					| (outfile_name[len-3]^'b') | (outfile_name[len-4]^'.');
		
		//printf("to = %d\n", to);
		if( to != 0)
		{
			sprintf(file_name, "%s%s", outfile_name, ".bmp");
			//printf("1 file_name len : %s, %d\n", file_name, len);   
		}
		else
		{
			sprintf(file_name, "%s", outfile_name);
			//printf("2 file_name len : %s, %d\n", file_name, len);
		}
	}
	
	if(f_path)
	{
	    char tmpfile_name[256] = {0};
		char tmpR_name[256] = {0};
		char tmpG_name[256] = {0};
		char tmpB_name[256] = {0};
		
		sprintf(tmpfile_name, "%s%s", input_path, file_name);
		sprintf(tmpR_name, "%s%s", input_path, fbmpR_name);
        sprintf(tmpG_name, "%s%s", input_path, fbmpG_name);
        sprintf(tmpB_name, "%s%s", input_path, fbmpB_name);
		
		strcpy(file_name, tmpfile_name);
		strcpy(fbmpR_name, tmpR_name);
		strcpy(fbmpG_name, tmpG_name);
		strcpy(fbmpB_name, tmpB_name);
	}
    //printf("file_name : %s\n", file_name);

    //first creat file and write file header to file
    ret = createBmp_with_fileheader(width, height, depth, file_name);
	if(ret == -1)
	{
		printf("creat file %s fialed!\n", file_name);
		return ret;
	}
#if 0//get current path
    char Path[MAX_PATH];
    _getcwd(Path,MAX_PATH);

    printf("The Path= %s",Path);
#endif

    FILE *fpR = fopen(fbmpR_name, "rb+" );
    if(fpR == NULL)
    {
        printf("Open %s file failed!\n", fbmpR_name);
        return -1;
    }
    fseek(fpR,0L,SEEK_END); /* 定位到文件末尾 */
	flen=ftell(fpR);
	printf("%s file lenth %d!\n", fbmpR_name, flen);
	char *pdataR = (char *)malloc(sizeof(char) * flen);
    if(pdataR == NULL)
	{
		printf("Alloc data space fialed file = %s, len = %d!\n", fbmpR_name, flen);
		return -1;
	}
    memset(pdataR, 0, sizeof(char)*flen);
	fseek(fpR,0L, 0);
	fread(pdataR, 1, flen, fpR);
	fclose(fpR);


	FILE *fpG = fopen(fbmpG_name, "rb+" );
    if(fpG == NULL)
    {
        printf("Open %s file failed!\n", fbmpG_name);
        return -1;
    }
	fseek(fpG,0L,SEEK_END); /* 定位到文件末尾 */
	flen=ftell(fpG);
	printf("%s file lenth %d!\n", fbmpG_name, flen);
	char *pdataG = (char *)malloc(sizeof(char) * flen);
    if(pdataG == NULL)
	{
		printf("Alloc data space fialed file = %s, len = %d!\n", fbmpG_name, flen);
		return -1;
	}		
	memset(pdataG, 0, sizeof(char)*flen);
	fseek(fpG,0L, 0);
	fread(pdataG, sizeof(char), flen, fpG);
    fclose(fpG);

    FILE *fpB = fopen(fbmpB_name, "rb+" );
    if(fpB == NULL)
    {
        printf("Open %s file failed!\n", fbmpB_name);
        return -1;
    }
	fseek(fpB,0L,SEEK_END); /* 定位到文件末尾 */
	flen=ftell(fpB);
	printf("%s file lenth %d!\n", fbmpB_name, flen);
	char *pdataB = (char *)malloc(sizeof(char) * flen);
    if(pdataB == NULL)
	{
		printf("Alloc data space fialed file = %s, len = %d!\n", fbmpB_name, flen);
		return -1;
	}
	memset(pdataB, 0, sizeof(char)*flen);
	fseek(fpB,0L, 0);
	ret = fread(pdataB, sizeof(char), flen, fpB);
    fclose(fpB);
	
	pData = (uint8_t *)malloc(sizeof(uint8_t) * width * height * 3);
	if(pData == NULL)
	{
		printf("Alloc pData space fialed len = %d!\n", width * height * 3);
		return -1;
	}
	memset(pData, 0, sizeof(uint8_t) * width * height * 3);
#if FILE_TEST //put read data to a file
                //FILE* fpstrR;
                //fpstrR=fopen("bmpstrR.hex","w+");
                //FILE* fpstrG;
                //fpstrG=fopen("bmpstrG.hex","w+");
                FILE* fpstrB;
                fpstrB=fopen("bmpstrB.hex","w+");
#endif

#if 0
    printf("%s first 72 bytes!\n", fbmpR_name);
	uint32_t count = width/2;
    for(uint32_t m=height-1; m< height; m++)
    {
        //fseek(fpR, (2*i + 1)*9, /*SEEK_CUR*/0);
        //fread(strR, sizeof(char), 8, fpR);
		/*if(m==0)
		{
            count = 9;
		}
		else
		{
			count = 9;
		}*/
		
        printf("L%d: ", m);		
        for(uint32_t n=0; n<count; n++)
        {
			seek = 9*(m*count + n) + 8;
			
            printf("%d ", seek/*pdataB[seek]*/);
			if(n%9 == 0)
			{
				printf("\n");
			}	
        }

        //unsigned int dataR = matoi(strR,8);
        //printf("data = 0x%08x \n",dataR);
    }
	return 0;
#endif
    //printf("1...................!\n");
#if 1
    //FILE *fpbmpFile = fopen(file_name, "ab");
	//printf("1.1..................!\n");
	//if(fpbmpFile == NULL)
	//{
	//    printf("Open %s file failed!\n", file_name);
    //    return -1;
	//}
	//printf("2...................!\n");
    u16_data_len = 0;
	uint32_t i;
	uint32_t j;
	uint32_t count = width/2;
	printf("2....height=%d...width=%d............!\n", height, count);
    for (i = height; i > 0; i--)
    {
        for (j = 0; j < count; j++)
        {
			if(ARMOUT|ARMIN) //2 column  1st:address(4 bytes)  2end:data(4 bytes 2 pixels 14bit of 1 pixel) 1 line 384 bytes
			{
				//seek = 18*(i)*(0 + count) + (2*j + 1)*9;
				seek = 18*(i-1)*count + (2*j + 1)*9; // line offset 384(bytes) - (width)180*2(1(R/G/B) 14bit used 2 byte ) = 24(byte)/4=6
			}

			if(TEST_ONE)  // 1 column 1st row:address "@2000000" other rows are data  (4 bytes 2 pixels 14bit of 1 pixel) 1 line 360 bytes
			{
				seek = 9*(i*count + j) + 8;
				//seek = 9*(i/*-1*/)*(0 + width/2) + j*9 + 8; // param 6 == not using 24bytes every line end(width)180*2(1(R/G/B) 14bit used 2 byte )
			}

			if(TEST_TWO)  // 1 column 1st row:address "@2000000" other rows are data  (4 bytes 2 pixels 14bit of 1 pixel) 1 line 360 bytes
			{
				seek = 9*(i-1)*(6 + count) + j*9 + 8; // param 6 == not using 24bytes every line end(width)180*2(1(R/G/B) 14bit used 2 byte )
			}
#if LTOUTFIRST  // 1 column 1st row:address "@2000000" other rows are data  (4 bytes 2 pixels 14bit of 1 pixel)
			//seek = 9*(i-1)*(6 + width/2) + j*9 + 8; // line offset 384(bytes) - (width)180*2(1(R/G/B) 14bit used 2 byte ) = 24(byte)/4=6
#endif

#if FILE_TEST
			seek = 9*(180-i)*(6 + width/2) + j*9 + 8; //
#endif
            //fseek(fpR, seek, 0);
			//fseek(fpG, seek, 0);
			//fseek(fpB, seek, 0);
			//printf("h:%d-w:%d : seek:%d\n",i, j, seek);
			
			memset(strR, 0, 9);
			memset(strG, 0, 9);
			memset(strB, 0, 9);

            //pdataR += seek;
			//pdataB += seek;
			//pdataG += seek;
			
            memcpy(strR, &pdataR[seek], 8);
			memcpy(strG, &pdataG[seek], 8);
			memcpy(strB, &pdataB[seek], 8);
			//fread(strR, sizeof(char), 8, fpR);
			//fread(strG, sizeof(char), 8, fpG);
			//fread(strB, sizeof(char), 8, fpB);
#if RAW2B
            //pdataR -= seek;
			//pdataB -= seek;
			//pdataG -= seek;
			
			dataR = matoi(strR,8);
			dataG = matoi(strG,8);
			dataB = matoi(strB,8);
			
			dataBH = (uint8_t)(0x000000ff&(dataB>>16));
			dataBL = (uint8_t)(dataB&0x000000ff);
			
			dataGH = (uint8_t)(0x000000ff&(dataG>>16));
			dataGL = (uint8_t)(dataG&0x000000ff);

			dataRH = (uint8_t)(0x000000ff&(dataR>>16));
			dataRL = (uint8_t)(dataR&0x000000ff);			
#endif
			//printf("h:%d-w:%d : 0x%08x\n",i, j, dataB);
			//printf("h:%d-w:%d : 0x%08x\n",i, j, dataR);
			//printf("h:%d-w:%d : 0x%08x\n",i, j, dataG);
#if FILE_TEST //put read data to a file
			//printf("h:%d-w:%d : %s\n",180-i, j, strB);
			//fprintf(fpstrR,"%s\n", strR);
			//fprintf(fpstrG,"%s\n", strG);
			fprintf(fpstrB,"%s\n", strB);
#endif

#if RAW2B
			pData[u16_data_len + 2] = dataBH;
			pData[u16_data_len + 1] = dataGH;
			pData[u16_data_len + 0] = dataRH;

            u16_data_len += 3;

			pData[u16_data_len + 2] = dataBL;
			pData[u16_data_len + 1] = dataGL;
			pData[u16_data_len + 0] = dataRL;

			u16_data_len += 3;
#endif
		}
		printf("h:%d-w:%d : %d\n",i, j, u16_data_len);
    }
	
#if RAW2B
    //createBMP((uint8_t*)buffer.data(), width, height, depth, file_name);
	createBMP((uint8_t*)pData, width, height, depth, file_name);
#endif

#endif

#if FILE_TEST //put read data to a file
      printf("4...................!\n");
     //fclose(fpstrR);
     //fclose(fpstrG);
     fclose(fpstrB);

#endif
    free(pdataR);
	free(pdataG);
	free(pdataB);
	free(pData);
    printf(RED "Creat file %s done!\n" NONE,file_name);

    //return a.exec();
    return 0;
}
