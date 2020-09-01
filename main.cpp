/*
* 转换ISP输入\输出的RGB\YUV数据文件为bmp图像文件，以验证图像是否正确！
* by 李宁 ramon.li@siengine.com 2020-08-03
*
*/

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <unistd.h>
#include <string.h>

uint32_t width  = 180;
uint32_t height = 180;
const uint16_t depth = 24;

uint8_t arm_out = 0;
uint8_t arm_in  = 0;

uint8_t  custom_c    = 0;
uint8_t  linef_b     = 0;
uint32_t line_offset = 0;

uint8_t per_color_bytes = 2;
uint8_t pix_bits        = 14;
uint8_t color_format    = 9; //CIF mode 0:RAW 1:RGB 2:YUV //9 is default value indicate in ISP output mode

#define RAW2B 1
#define debug 0

#define BI_RGB       0         //不压缩（最常用）
#define BI_RLE8      1         //8比特游程编码（BLE），只用于8位位图
#define BI_RLE4      2         //4比特游程编码（BLE），只用于4位位图
#define BI_BITFIELDS 3         //比特域（BLE），只用于16/32位位图

#define NONE         "\033[m" 
#define RED          "\033[0;32;31m"

#define CLIP(color) (unsigned char)(((color) > 0xFF) ? 0xff : (((color) < 0) ? 0 : (color)))

//char bmp_suffix[]    = ".bmp";	

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
	fseek(hFile, 0L, /*SEEK_END*/2);//put file point to the file end.
	fwrite(bitmapBits, 1, data_len, hFile);
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

/*
*  convert string hex char to unsigned int
*  char* str -- input str
*  count str length
*  return unsigned int value
*/
unsigned int matoi(char* str, int count)
{
    unsigned int tmp = 0;
    int num = count;
    int i=0;

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

    return tmp;
}


/*
* help information
*/
static void usage(char* name)
{
    printf("\nNAME\n");
    printf("\tConvert 3 hex files to rgb file\n\n");
    printf("SYNOPSIS\n");
    printf("\t%s -i [-f path] \n\n", name);
    printf("DESCRIPTION\n");
	printf("\t\t -i    转换ARM给的输入文件!\n");
	printf("\t\t -o    转换ARM输入文件产生的输出文件!\n");
	printf("\t\t -c    转换客户由ISP输出的文件!\n");
	printf("\t\t -f    设置输入文件的路径（可选）!\n");
	printf("\t\t -s    设置客户输出文件的line_offset（可选）\n");
	printf("\t\t -t    设置line offset 中空白数据在行首还是行尾：0代表行尾，1代表行首！\n");
    printf("\t\t -u    设置输出文件的名字!\n");
    printf("\t\t -m    设置输入数据的颜色格式，CIF mode -- 1:RGB888, 2:YUV422_888,CIF模式下请用-r 指明输入文件\n");
	printf("\t\t       默认为 isp raw format, ISP mode -- 9:RAW, 8:RGB888, 7:YUV422_888\n");
	printf("\t\t -r    设置输入文件的R色文件名（如果是YUV格式，则代表Y）!\n");
	printf("\t\t -g    设置输入文件的G色文件名（如果是YUV格式，则代表U）!\n");
	printf("\t\t -b    设置输入文件的B色文件名（如果是YUV格式，则代表V）!\n");
    printf("\t\t -y    设置每个像素，单个颜色分量占用的bit数，例如16, 12, 10, 8, 4. 默认设置为14 !\n");
	printf("\t\t -w    设置要转出图片的宽度（单位：像素），默认是180\n");
	printf("\t\t -h    设置要转出图片的高度（单位：像素），默认是180\n");
	printf("\t\t -H    输出帮助信息!\n");
    printf("\n\n");
}

int main(int argc, char *argv[])
{
	//int ret = 0;
	int ch;
	
	uint8_t  f_path    = 0; //0:using current path
	uint8_t  f_outfile = 0; //0:using default output file name 1:indicate output filename
	uint8_t  f_infile  = 0; //0:using default input file name  1:indicate input filename 
	uint8_t  line_offset_flg = 0;
	
	uint32_t flen      = 0;
	uint32_t line_blank = 0;
	uint32_t u16_data_len = 0;
	uint32_t expect_infile_size = 0;
	
	uint8_t* pData0 = NULL;//buffer.data();
	uint8_t* pData1 = NULL;
	uint8_t* pData2 = NULL;
	
	char input_path[256]   ={ 0 };
	char infile_r[128]     ={ 0 };
	char infile_g[128]     ={ 0 };
	char infile_b[128]     ={ 0 };
	char outfile_name[128] ={ 0 };
	
	uint32_t seek = 0;
	uint32_t seekuv = 0;
	
    uint32_t dataR = 0;
    uint32_t dataG = 0;
    uint32_t dataB = 0;
	
	uint8_t dataY0 = 0;
    uint8_t dataY1 = 0;
    uint8_t dataU = 0;
	uint8_t dataV = 0;
	
	uint8_t dataRL = 0;
	uint8_t dataRH = 0;
	uint8_t dataGL = 0;
	uint8_t dataGH = 0;
	uint8_t dataBL = 0;
	uint8_t dataBH = 0;
	
	uint8_t addre_line_len = 0;
	
	uint8_t w_s = 0; //表示设置宽度
	uint8_t h_s = 0; //表示设置高度
	
	//char buff[8]={0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37};
    char strR[9];
    char strG[9];
    char strB[9];
	
	arm_in   = 0;
	arm_out  = 0;
    custom_c = 0;
	linef_b  = 0;
	line_offset = per_color_bytes * width;
	
	//program output bmp files' default name
    char file_name0[256]  = "image180180_armout0.bmp";
    char file_name1[256]  = "image180180_armout1.bmp";
    char file_name2[256]  = "image180180_armout2.bmp";

    //program input data files from ISP or CIF
    char fbmpR_name[256]  = "output0_buffer.hex";
    char fbmpG_name[256]  = "output1_buffer.hex";
    char fbmpB_name[256]  = "output2_buffer.hex";
	
	printf("argc %d\n",argc);
	if(argc < 2)
	{
		usage(argv[0]);
        exit(-1);
	}
	
#if 0	
	printf("\t\t -i    转换ARM的输入文件 !\n");
	printf("\t\t -o    转换ARM输入文件产生的输出文件 !\n");
	printf("\t\t -c    转换客户由ISP输出的文件 !\n");
	printf("\t\t -f    设置输入文件的路径（可选）!\n");
	printf("\t\t -s    设置客户输出文件的line_offset（可选）!\n");
	printf("\t\t -t    设置line offset 中空白数据在行首还是行尾：0代表行尾，1代表行首 !\n");
    printf("\t\t -u    设置输出文件的名字 !\n");
    printf("\t\t -m    设置输入数据的颜色格式，CIF mode -- 0:RAW, 1:RGB888, 2:YUV422_888,CIF模式下请用-r 指明输入文件"\n);
	printf("\t\t       默认为 isp raw format, ISP mode -- 9:RAW, 8:RGB888, 7:YUV422_888\n");
	printf("\t\t -r    设置输入文件的R色文件名（如果是YUV格式，则代表Y）!\n");
	printf("\t\t -g    设置输入文件的G色文件名（如果是YUV格式，则代表U）!\n");
	printf("\t\t -b    设置输入文件的B色文件名（如果是YUV格式，则代表V）!\n");
    printf("\t\t -y    设置每个像素，单个颜色分量占用的bit数，例如16, 12, 10, 8, 4. 默认设置为14 !\n");
	printf("\t\t -w    设置要转出图片的宽度（单位：像素），默认是180 !\n");
	printf("\t\t -h    设置要转出图片的高度（单位：像素），默认是180 !\n");
	printf("\t\t -H    输出帮助信息!\n");
#endif
	
	while ((ch = getopt(argc, argv, "iocf:s:t:u:m:r:g:b:y:w:h:H?")) != -1)
    {
        switch (ch) {
        case 'i':
            arm_in = 1;
            break;
        case 'o':
            arm_out = 1;
            break;
		case 'c':
            custom_c = 1;
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
		case 's':
		    line_offset_flg = 1;
            line_offset = atoi(optarg);
            break;
		case 't':
            linef_b = atoi(optarg);
			if(linef_b != 1)
			{
				linef_b = 0;
			}
            break;
		case 'u':
			f_outfile = 1;
			sprintf(outfile_name, "%s", optarg);
			break;
		case 'm':
		    color_format = atoi(optarg);
			break;
		case 'r':
			f_infile |= 1;
			sprintf(infile_r, "%s", optarg);
			break;
		case 'g':
			f_infile |= 1<<1;
			sprintf(infile_g, "%s", optarg);
			break;
		case 'b':
			f_infile |= 1<<2;
			sprintf(infile_b, "%s", optarg);
			break;
		case 'y':
		    pix_bits = atoi(optarg);
			break;
		case 'w':
			w_s  = 1;
			width = atoi(optarg);
			break;
		case 'h':
		    h_s = 1;
		    height = atoi(optarg);
			break;
        case 'H':
        case '?':
        default:
            usage(argv[0]);
            exit(-1);
        }   
    }
	
	if(w_s||h_s)
	{
		if(w_s == 0)
		{
			printf("缺少宽度参数，请检查输入的参数！\n");
			return -1;
		}
		
		if(h_s == 0)
		{
			printf("缺少高度参数，请检查输入的参数！\n");
			return -1;
		}
		
	}
	
	if(color_format <= 9)
	{
		switch(color_format)
		{
			case 0: //CIF mode RAW
			case 1: //CIF mode RGB888
			case 2: //CIF mode YUV422_888
			{
				if(f_infile == 0)
				{
				    printf("缺少输入文件，请检查输入的参数！\n");
				    return -1;
				}
				if(f_outfile == 0)
				{
				    sprintf(file_name0, "%s%d%s", "cif_output_", color_format, ".bmp");
				}
				break;
			}
			case 9: //ISP mode RAW
			case 8: //ISP mode GRB888
			case 7: //ISP mode YUV422_888
			    break;
			default:
			{
				printf("颜色格式设置错误，请检查输入的参数！\n");
				return -1;
			}
		}		
	}
	else
	{
		printf("颜色格式设置错误，请检查输入的参数！\n");
	    return -1;
	}
	
    if(arm_out)
	{
		sprintf(file_name0,  "%s%d%d%s", "image", width, height, "_armout.bmp");
		sprintf(fbmpR_name, "%s", "output0_buffer.hex");
		sprintf(fbmpG_name, "%s", "output1_buffer.hex");
		sprintf(fbmpB_name, "%s", "output2_buffer.hex");
	}
    else if(arm_in)
    {		
		sprintf(file_name0,  "%s%d%d%s", "image", width, height, "_armin.bmp");
		sprintf(file_name1,  "%s%d%d%s", "image", width, height, "_armin1.bmp");
		sprintf(file_name2,  "%s%d%d%s", "image", width, height, "_armin2.bmp");
		sprintf(fbmpR_name, "%s", "input0_buffer.hex");
		sprintf(fbmpG_name, "%s", "input1_buffer.hex");
		sprintf(fbmpB_name, "%s", "input2_buffer.hex");
	}
	else if(custom_c)
    {
		sprintf(file_name0,  "%s%d%d%s", "image", width, height, "_customs.bmp");
		sprintf(fbmpR_name, "%s", "output_r14g14b14_buffer0.hex");
		sprintf(fbmpG_name, "%s", "output_r14g14b14_buffer1.hex");
		sprintf(fbmpB_name, "%s", "output_r14g14b14_buffer2.hex");
	}
	
	if(f_outfile)
	{
		uint8_t len = strlen(outfile_name);
		uint8_t to = (outfile_name[len-1]^'p') |(outfile_name[len-2]^'m') \
					| (outfile_name[len-3]^'b') | (outfile_name[len-4]^'.');
		
		if( to != 0)
		{
			sprintf(file_name0, "%s%s", outfile_name, ".bmp");
			//printf("1 file_name0 len : %s, %d\n", file_name0, len);   
		}
		else
		{
			sprintf(file_name0, "%s", outfile_name);
			//printf("2 file_name0 len : %s, %d\n", file_name0, len);
		}
	}
	
	if(f_infile)
	{
		if((color_format >= 0)&&(color_format < 3))
		{
			sprintf(fbmpR_name, "%s", infile_r);
		}
		else
		{
			if(f_infile != 0x07)
			{
				printf("缺少输入文件，请检查输入的参数！\n");
				return -1;
			}
			else
			{
				sprintf(fbmpR_name, "%s", infile_r);
				sprintf(fbmpG_name, "%s", infile_g);
				sprintf(fbmpB_name, "%s", infile_b);
			}
		}
	}

	if(f_path)
	{
	    char tmpfile_name[256] = {0};
		char tmpR_name[256] = {0};
		char tmpG_name[256] = {0};
		char tmpB_name[256] = {0};
		
		sprintf(tmpfile_name, "%s%s", input_path, file_name0);
		sprintf(tmpR_name, "%s%s", input_path, fbmpR_name);
        sprintf(tmpG_name, "%s%s", input_path, fbmpG_name);
        sprintf(tmpB_name, "%s%s", input_path, fbmpB_name);
			
		strcpy(file_name0, tmpfile_name);
		strcpy(fbmpR_name, tmpR_name);
		strcpy(fbmpG_name, tmpG_name);
		strcpy(fbmpB_name, tmpB_name);
		
		if(arm_in)
		{
			memset(tmpfile_name, 0, 256);
			sprintf(tmpfile_name, "%s%s", input_path, file_name1);
			strcpy(file_name1, tmpfile_name);
			
			memset(tmpfile_name, 0, 256);
			sprintf(tmpfile_name, "%s%s", input_path, file_name2);
			strcpy(file_name2, tmpfile_name);
		}
	}
    
	//printf("file_name0 :  %s\n", file_name0);
	//printf("fbmpR_name : %s\n", fbmpR_name);
	//printf("fbmpG_name : %s\n", fbmpG_name);
	//printf("fbmpB_name : %s\n", fbmpB_name);
	//return 0;

    //first creat file and write file header to file
    /*int ret = createBmp_with_fileheader(width, height, depth, file_name0);
	if(ret == -1)
	{
		printf("creat file %s fialed!\n", file_name0);
		return ret;
	}*/
	
#if 0//get current path
    #define MAX_PATH 256
    char Path[MAX_PATH];
    _getcwd(Path,MAX_PATH);
    printf("The Path= %s",Path);
#endif

    expect_infile_size = (height*width/2)*9 + 8;
	if(color_format == 7) //yuv422_888  u v data length = y data length/2
	{	
        expect_infile_size =  expect_infile_size / 2;
	}
	
    FILE *fpR = fopen(fbmpR_name, "rb" );
    if(fpR == NULL)
    {
        printf("Open %s file failed!\n", fbmpR_name);
        return -1;
    }
    fseek(fpR,0L,SEEK_END); /* 定位到文件末尾 */
	flen=ftell(fpR);
	printf("%s file lenth %d!\n", fbmpR_name, flen);
	
	if(flen < expect_infile_size )
	{
		printf(RED);
		printf("错误！%s 实际数据大小少于预期！\n", fbmpR_name);
		printf(NONE);
		
        fclose(fpR);
        return -1;		
	}
	char *pdataR = (char *)malloc(sizeof(char) * flen);
    
	if(pdataR == NULL)
	{
		printf("Alloc data space fialed file = %s, len = %d!\n", fbmpR_name, flen);
		fclose(fpR);
		return -1;
	}
    
	memset(pdataR, 0, sizeof(char)*flen);
	fseek(fpR,0L, 0);
	fread(pdataR, 1, flen, fpR);
	fclose(fpR);
	
	//find frist line length address line
	addre_line_len = 0;
	while(addre_line_len < flen)
    {
	    if((pdataR[addre_line_len] == 0x0a)||(pdataR[addre_line_len] == 0x0d))
		{
			addre_line_len = addre_line_len + 1;
			printf("address line length is %d\n", addre_line_len);
			break;
		}
		addre_line_len++;
	}
	
    char *pdataG = NULL;
	char *pdataB = NULL;
	
    if((color_format > 6)&&(color_format < 10))
	{
		FILE *fpG = fopen(fbmpG_name, "rb" );
		if(fpG == NULL)
		{
			printf("Open %s file failed!\n", fbmpG_name);
			free(pdataR);
			return -1;
		}
		fseek(fpG,0L,SEEK_END); /* 定位到文件末尾 */
		flen=ftell(fpG);
		printf("%s file lenth %d!\n", fbmpG_name, flen);
		
		if(flen < expect_infile_size )
		{
			printf(RED);
			printf("错误！%s 实际数据大小少于预期！\n", fbmpR_name);
			printf(NONE);
			free(pdataR);
			fclose(fpG);
			return -1;		
		}
		
		pdataG = (char *)malloc(sizeof(char) * flen);
		if(pdataG == NULL)
		{
			printf("Alloc data space fialed file = %s, len = %d!\n", fbmpG_name, flen);
			free(pdataR);
			fclose(fpG);
			return -1;
		}		
		memset(pdataG, 0, sizeof(char)*flen);
		fseek(fpG,0L, 0);
		fread(pdataG, sizeof(char), flen, fpG);
		fclose(fpG);

		FILE *fpB = fopen(fbmpB_name, "rb" );
		if(fpB == NULL)
		{
			printf("Open %s file failed!\n", fbmpB_name);
			free(pdataR);
			free(pdataG);
			return -1;
		}
		fseek(fpB,0L,SEEK_END); /* 定位到文件末尾 */
		flen=ftell(fpB);
		printf("%s file lenth %d!\n", fbmpB_name, flen);
		
		if(flen < expect_infile_size )
		{
			printf(RED);
			printf("错误！%s 实际数据大小少于预期！\n", fbmpR_name);
			printf(NONE);
			free(pdataR);
			free(pdataG);
			fclose(fpB);
			return -1;		
		}
		
		pdataB = (char *)malloc(sizeof(char) * flen);
		if(pdataB == NULL)
		{
			printf("Alloc data space fialed file = %s, len = %d!\n", fbmpB_name, flen);
			free(pdataR);
			free(pdataG);
			fclose(fpB);
			return -1;
		}
		memset(pdataB, 0, sizeof(char)*flen);
		fseek(fpB,0L, 0);
		fread(pdataB, sizeof(char), flen, fpB);
		fclose(fpB);
	}

	
	pData0 = (uint8_t *)malloc(sizeof(uint8_t) * width * height * 3);
	if(pData0 == NULL)
	{
		printf("Alloc pData0 space fialed len = %d!\n", width * height * 3);
		free(pdataR);

		if(pdataG != NULL)
		{
			free(pdataG);
		}
		
		if(pdataB != NULL)
		{
		    free(pdataB);
		}
		
		return -1;
	}
	memset(pData0, 0, sizeof(uint8_t) * width * height * 3);
	
	if(arm_in)
	{
	    pData1 = (uint8_t *)malloc(sizeof(uint8_t) * width * height * 3);
		if(pData1 == NULL)
		{
			printf("Alloc pData1 space fialed len = %d!\n", width * height * 3);
			free(pdataR);
			
			if(pdataG != NULL)
			{
				free(pdataG);
			}			
			
			if(pdataB != NULL)
			{
				free(pdataB);
			}
			
			free(pData0);
			return -1;
		}
		memset(pData1, 0, sizeof(uint8_t) * width * height * 3);
		
	    pData2 = (uint8_t *)malloc(sizeof(uint8_t) * width * height * 3);
		if(pData2 == NULL)
		{
			printf("Alloc pData2 space fialed len = %d!\n", width * height * 3);
			free(pdataR);
			
			if(pdataG != NULL)
			{
				free(pdataG);
			}
			if(pdataB != NULL)
			{
			    free(pdataB);
			}
			
			free(pData0);
			free(pData1);
			return -1;
		}
		memset(pData2, 0, sizeof(uint8_t) * width * height * 3);
	}

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

    u16_data_len = 0;
	uint32_t i;
	uint32_t j;
	uint32_t count = width/2;
	
	if(pix_bits <= 8)
	{
		per_color_bytes = 1;
	}
	else if(pix_bits <= 16)
	{
		per_color_bytes = 2;
	}
	else if(pix_bits <= 24)
	{
		per_color_bytes = 3;
	}
	else if(pix_bits <= 32)
	{
		per_color_bytes = 4;
	}
    
	if(line_offset_flg == 0)
	{
		line_blank = 0;
	}
	
	if(line_offset_flg == 1)
	{
		if(line_offset < (per_color_bytes * width))
		{
			line_blank = 0;
			printf(RED);
			printf("警告line_offset小于每行字节数!\n");
			printf("line_offset =  %d\n", line_offset);
			printf("per_line_bytes =  %d\n", per_color_bytes * width);
			printf(NONE);			
		}
		else
		{
			line_blank = line_offset - (per_color_bytes * width);
			line_blank = line_blank/per_color_bytes/2;
		}
	}
	
	printf("1....height=%d...width=%d............!\n", height, count);
    if((color_format > 6)&&(color_format < 10))
	{   
		for (i = height; i > 0; i--)
		{
			if(color_format == 7)//yuv422_888
			{
				for (j = 0; j < count; j++)
				{
					seek = 9*((i-1)*(line_blank + count) + j) + addre_line_len + (linef_b * line_blank)*9;
					seekuv = 9*((i-1)*(line_blank + count) + j)/2 + addre_line_len + (linef_b * line_blank)*9/2;
					//seekuv = 9*((i-1)*(line_blank/2 + count/2) + j/2) + addre_line_len + (linef_b * line_blank/2)*9;
				
					memset(strR, 0, 9);
					memset(strG, 0, 9);
					memset(strB, 0, 9);

					memcpy(strR, &pdataR[seek], 8);
					memcpy(strG, &pdataG[seekuv], 8);
					memcpy(strB, &pdataB[seekuv], 8);
					
					dataR = matoi(strR,8);
					dataG = matoi(strG,8);
					dataB = matoi(strB,8);
					
					dataV = (uint8_t)(0x000000ff&(dataB>>((j%2?0:1)*16)));
					dataU = (uint8_t)(0x000000ff&(dataG>>((j%2?0:1)*16)));
					dataY0 = (uint8_t)(0x000000ff&(dataR>>16));
					dataY1 = (uint8_t)(dataR&0x000000ff);
					
					//R = Y + 1.4075 *（V-128）
					//G = Y – 0.3455 *（U –128） – 0.7169 *（V –128）
					//B = Y + 1.779 *（U – 128）
					
					dataBH = CLIP(dataY0 + 1.779*(dataU-128));
					dataBL = CLIP(dataY1 + 1.779*(dataU-128));
					
					dataGH = CLIP(dataY0 - 0.3455*(dataU-128) - 0.7169*(dataV -128));
					dataGL = CLIP(dataY1 - 0.3455*(dataU-128) - 0.7169*(dataV -128));

					dataRH = CLIP(dataY0 + 1.4075*(dataV -128));
					dataRL = CLIP(dataY1 + 1.4075*(dataV -128));

					pData0[u16_data_len + 2] = dataBH;
					pData0[u16_data_len + 1] = dataGH;
					pData0[u16_data_len + 0] = dataRH;

					u16_data_len += 3;

					pData0[u16_data_len + 2] = dataBL;
					pData0[u16_data_len + 1] = dataGL;
					pData0[u16_data_len + 0] = dataRL;

					u16_data_len += 3;
				}
			}
			else
			{
				for (j = 0; j < count; j++)
				{
					if(arm_out|arm_in) //2 column  1st:address(4 bytes)  2end:data(4 bytes 2 pixels 14bit of 1 pixel) 1 line 384 bytes
					{
						seek = 18*(i-1)*count + (2*j + 1)*9;
					}

					if(custom_c)  // 1 column 1st row:address "@2000000" other rows are data  (4 bytes 2 pixels 14bit of 1 pixel) 1 line 360 bytes
					{
						seek = 9*((i-1)*(line_blank + count) + j) + addre_line_len + (linef_b * line_blank)*9;
					}
					
					memset(strR, 0, 9);
					memset(strG, 0, 9);
					memset(strB, 0, 9);

					memcpy(strR, &pdataR[seek], 8);
					memcpy(strG, &pdataG[seek], 8);
					memcpy(strB, &pdataB[seek], 8);
					
					dataR = matoi(strR,8);
					dataG = matoi(strG,8);
					dataB = matoi(strB,8);
					
					dataBH = (uint8_t)(0x000000ff&(dataB>>16));
					dataBL = (uint8_t)(dataB&0x000000ff);
					
					dataGH = (uint8_t)(0x000000ff&(dataG>>16));
					dataGL = (uint8_t)(dataG&0x000000ff);

					dataRH = (uint8_t)(0x000000ff&(dataR>>16));
					dataRL = (uint8_t)(dataR&0x000000ff);
#if debug
					if(i==1 && j <10)
					{
						printf("\nj: %d\n", j);
						printf("strR: %s\n",strR);
						printf("dataR: %08x\n",dataR);
						printf("dataRH: %02x\n",dataRH);
						printf("dataRL: %02x\n",dataRL);
						
						printf("strG: %s\n",strG);
						printf("dataG: %08x\n",dataG);
						printf("dataGH: %02x\n",dataGH);
						printf("dataGL: %02x\n",dataGL);
						
						printf("strB: %s\n",strB);
						printf("dataB: %08x\n",dataB);
						printf("dataBH: %02x\n",dataBH);
						printf("dataBL: %02x\n",dataBL);
						printf("--------------------------\n");
					}
#endif					
					if(arm_in)
					{
						pData0[u16_data_len + 2] = dataRH;
						pData0[u16_data_len + 1] = dataRH;
						pData0[u16_data_len + 0] = dataRH;
						
						pData1[u16_data_len + 2] = dataGH;
						pData1[u16_data_len + 1] = dataGH;
						pData1[u16_data_len + 0] = dataGH;
						
						pData2[u16_data_len + 2] = dataBH;
						pData2[u16_data_len + 1] = dataBH;
						pData2[u16_data_len + 0] = dataBH;

						u16_data_len += 3;
						
						pData0[u16_data_len + 2] = dataRL;
						pData0[u16_data_len + 1] = dataRL;
						pData0[u16_data_len + 0] = dataRL;
						
						pData1[u16_data_len + 2] = dataGL;
						pData1[u16_data_len + 1] = dataGL;
						pData1[u16_data_len + 0] = dataGL;
						
						pData2[u16_data_len + 2] = dataBL;
						pData2[u16_data_len + 1] = dataBL;
						pData2[u16_data_len + 0] = dataBL;

						u16_data_len += 3;
					}
					else
					{
						pData0[u16_data_len + 2] = dataBH;
						pData0[u16_data_len + 1] = dataGH;
						pData0[u16_data_len + 0] = dataRH;

						u16_data_len += 3;

						pData0[u16_data_len + 2] = dataBL;
						pData0[u16_data_len + 1] = dataGL;
						pData0[u16_data_len + 0] = dataRL;

						u16_data_len += 3;
					}
				}
			}
		}
	}
	else//cif mode 1 rgb888 2 yuv422_888 
	{
		for (i = height; i > 0; i--)
		{
			for (j = 0; j < count; j++)
			{
				if(color_format == 1)  //RGB888 1 column 1st row:address "@2000000" other rows are data 
				{
					seek = 9*((i-1)*(line_blank*2 + count*2) + j*2) + addre_line_len + (linef_b * line_blank*2)*9;
					/*
					*  per pixel  bit31----------bit23-----------bit15----------bit7--------bit0
					*                   R7---R0         G7---R0        B7---B0      00000000
					*/
					memset(strR, 0, 9);
					memset(strG, 0, 9);
					//memset(strB, 0, 9);
					
					memcpy(strR, &pdataR[seek], 8);
					memcpy(strG, &pdataR[seek+9], 8);
					//memcpy(strB, &pdataB[seek], 8);
					
					dataR = matoi(strR, 8);
					dataG = matoi(strG, 8);
					//dataB = matoi(strB,8);
					
					dataBH = (uint8_t)(0x000000ff&(dataR>>16));
					dataBL = (uint8_t)(0x000000ff&(dataG>>16));
					
					dataGH = (uint8_t)(0x000000ff&(dataR>>8));
					dataGL = (uint8_t)(0x000000ff&(dataG>>8));

					dataRH = (uint8_t)(0x000000ff&(dataR));
					dataRL = (uint8_t)(0x000000ff&(dataG));
					
					if(i==1 && j <10)
					{
						printf("j: %d\n", j);
						printf("strR: %s\n", strR);
						printf("dataR: %08x\n", dataR);
						printf("dataRH: %02x\n", dataRH);
						printf("dataGH: %02x\n", dataGH);
						printf("dataBH: %02x\n", dataBH);
						
						printf("strG: %s\n", strG);
						printf("dataG: %08x\n", dataG);
						printf("dataRL: %02x\n", dataRL);
						printf("dataGL: %02x\n", dataGL);
						printf("dataBL: %02x\n", dataBL);
						
						printf("--------------------------\n");
					}
					
					pData0[u16_data_len + 2] = dataBH;
					pData0[u16_data_len + 1] = dataGH;
					pData0[u16_data_len + 0] = dataRH;

					u16_data_len += 3;
					
					pData0[u16_data_len + 2] = dataBL;
					pData0[u16_data_len + 1] = dataGL;
					pData0[u16_data_len + 0] = dataRL;

					u16_data_len += 3;
				}
				
				if(color_format == 2)  //yuv422_888 1 column 1st row:address "@2000000" other rows are data
				{
					seek = 9*((i-1)*(line_blank + count) + j) + addre_line_len + (linef_b * line_blank*2)*9;
					/*
					*  per pixel  bit31----------bit23-----------bit15----------bit7--------bit0
					*                   Yn7---Yn0       V7---V0   Y(n+1)7---Y(n+1)0  U7---U0
					*/
					memset(strR, 0, 9);
					//memset(strG, 0, 9);
					//memset(strB, 0, 9);
					
					memcpy(strR, &pdataR[seek], 8);
					//memcpy(strG, &pdataG[seek], 8);
					//memcpy(strB, &pdataB[seek], 8);
					
					dataR = matoi(strR,8);
					//dataG = matoi(strG,8);
					//dataB = matoi(strB,8);
					dataU= (uint8_t)(0x000000ff&(dataR>>24));
					dataY1 = (uint8_t)(0x000000ff&(dataR>>16));
					dataV = (uint8_t)(0x000000ff&(dataR>>8));
					dataY0  = (uint8_t)(0x000000ff&(dataR));
					
					//R = Y + 1.4075 *（V-128）
					//G = Y – 0.3455 *（U –128） – 0.7169 *（V –128）
					//B = Y + 1.779 *（U – 128）
					
					dataBH = CLIP(dataY0 + 1.779*(dataU-128));
					dataBL = CLIP(dataY1 + 1.779*(dataU-128));
					
					dataGH = CLIP(dataY0 - 0.3455*(dataU-128) - 0.7169*(dataV -128));
					dataGL = CLIP(dataY1 - 0.3455*(dataU-128) - 0.7169*(dataV -128));

					dataRH = CLIP(dataY0 + 1.4075*(dataV -128));
					dataRL = CLIP(dataY1 + 1.4075*(dataV -128));
					
					pData0[u16_data_len + 2] = dataBH;
					pData0[u16_data_len + 1] = dataGH;
					pData0[u16_data_len + 0] = dataRH;

					u16_data_len += 3;
					
					pData0[u16_data_len + 2] = dataBL;
					pData0[u16_data_len + 1] = dataGL;
					pData0[u16_data_len + 0] = dataRL;

					u16_data_len += 3;
				}
			}
			//printf("h:%d-w:%d : %d\n",i, j, u16_data_len);
		}
	}
#if RAW2B
    //createBMP((uint8_t*)buffer.data(), width, height, depth, file_name0);
	createBMP((uint8_t*)pData0, width, height, depth, file_name0);
	if(arm_in)
	{
		createBMP((uint8_t*)pData1, width, height, depth, file_name1);
		createBMP((uint8_t*)pData2, width, height, depth, file_name2);
	}
#endif

    free(pdataR);
	if(pdataG != NULL)
	{
	    free(pdataG);
	}
	
	if(pdataB != NULL)
	{
	    free(pdataB);
	}
	
	free(pData0);
	
	if(arm_in)
	{
		free(pData1);
		free(pData2);
		printf(RED "Creat file %s done!\n" NONE,file_name0);
		printf(RED "Creat file %s done!\n" NONE,file_name1);
		printf(RED "Creat file %s done!\n" NONE,file_name2);
		
	}
	else
	{
		printf(RED "Creat file %s done!\n" NONE,file_name0);
	}

    return 0;
}
