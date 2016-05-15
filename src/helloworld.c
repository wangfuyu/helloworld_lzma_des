/****************
* hello.c
* The mot simplistic C program ever written.
* An epileptic monkey on crack could write this code.
*****************/

#include "helloworld.h"

char gSrcFileName[MAX_FILENAME_LEN] = {0};
char gDstFileName[MAX_FILENAME_LEN] = {0};
unsigned short gLzmaType = 0;

/******************************************************************************
* FUNCTION		: writeFilesToBuf()
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: read file data to buf
* INPUT			: @filePath:full path of file
				  @buf:buffer for saving file data
				  @len:file data size
* OUTPUT		: 
* RETURN		: 
******************************************************************************/
int getFileContentLen(char* filename, int *len)
{
	int fileLen = 0;
	FILE* fp = NULL;

	if (NULL == filename || NULL == len)
	{
		MSG_PRINT("filename or len is NULL!\n");
		return -1;
	}

	fp = fopen(filename, "rb");
	if (NULL == fp)  
	{  
		MSG_PRINT("Can not open %s!\n", filename);	
		return -1;  
	}

	fseek(fp, 0L, SEEK_END); 
	fileLen = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	fclose(fp);

	*len = fileLen;
	return 0;
}


/******************************************************************************
* FUNCTION		: writeFilesToBuf()
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: read file data to buf
* INPUT			: @filePath:full path of file
				  @buf:buffer for saving file data
				  @len:file data size
* OUTPUT		: 
* RETURN		: 
******************************************************************************/
int writeFilesToBuf(char* filePath, unsigned char* buf, int len)
{
	FILE* fp = NULL;
	int readBytes = 0;

	if (NULL == filePath || NULL == buf || len < 0)
	{
		MSG_PRINT("input paras invalid!\n");
		return -1;
	}

	if (0 == len)
	{
		MSG_PRINT("%s is an empty file, not need to read\n", filePath);
		return 0;
	}

	fp = fopen(filePath, "rb"); 
	if(!fp)
	{		
		return -1;
	}

	readBytes = fread(buf, sizeof(unsigned char), len, fp);
	fclose(fp);

	if (readBytes != len)
	{
		return -1;
	}

	return 0;
}

/******************************************************************************
* FUNCTION		: writeBufToFile()
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: write buf data to file
* INPUT			: 
* OUTPUT		: 
* RETURN		: 
******************************************************************************/
int writeBufToFile(char* filePath, unsigned char* buf, int len)
{
	FILE* fp = NULL;
	int writeBytes = 0;

	if (NULL == filePath || NULL == buf)
	{
		MSG_PRINT("Invalid paras:NULL == filePath || NULL == buf\n");
		return -1;
	}

	fp = fopen(filePath, "wb"); 
	if(!fp)
	{
		MSG_PRINT("open %s failed!\n", filePath);
		return -1;
	}

	writeBytes = fwrite(buf, sizeof(unsigned char), len, fp);
	fclose(fp);

	if (writeBytes != len)
	{
		MSG_PRINT("write files failed! len = %d\n", len);
		return -1;
	}

	return 0;
}

/******************************************************************************
* FUNCTION		: getOpt()
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: 
* INPUT			: 
* OUTPUT		: 
* RETURN		: 
******************************************************************************/
static int getOpt(int argc, char** argv)
{
	int ch = 0; 
	extern char *optarg;

	if (argc < 2)
	{
		MSG_PRINT("argc not big enough!\n");
		return -1;
	}
	   
	while ((ch = getopt(argc, argv, "s:d:t:")) !=  -1)
	{
		switch (ch)
		{
		case 's':
			MSG_PRINT("src: %s\n", optarg);
			strncpy(gSrcFileName, optarg, MAX_FILENAME_LEN);
			break;
		case 'd':
			MSG_PRINT("dst: %s\n", optarg);
			strncpy(gDstFileName, optarg, MAX_FILENAME_LEN);
			break;
		case 't':
			MSG_PRINT("type: %s\n", optarg);
			gLzmaType = (unsigned short)atoi(optarg);
			break;
		default:
			MSG_PRINT("opt: %s, not supported.\n", optarg);
			return -1;
		}
	}

	return 0;
}

/******************************************************************************
* FUNCTION		: main()
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: 
* INPUT			: 
* OUTPUT		: 
* RETURN		: 
******************************************************************************/
int main(int argc, char** argv)
{	
	int ret = -1;
	
	ret = getOpt(argc, argv);
	if (0 != ret)
	{
		MSG_PRINT("get option failed!\n");
		return -1;
	}

	ret = lzmaDataProg(gSrcFileName, gDstFileName, gLzmaType);
	if (0 != ret)
	{
		MSG_PRINT("lzma return fail msg!\n");
		return -1;
	}
#if 0
	ret = desDataProg(gSrcFileName, gDstFileName, gLzmaType);
	if (0 != ret)
	{
		MSG_PRINT("des return fail msg!\n");
		return -1;
	} 
#endif	
	MSG_PRINT("hello world\n");	
    return 0;
}
