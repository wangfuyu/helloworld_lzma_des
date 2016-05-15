/***********************************************************
*
* Copyright (C), 2010-2014, TP-LINK TECHNOLOGIES CO., LTD.
* All rights reserved.
*
* File name  : cloud-encrypt.c 
* Version	 : 1.0 
* Description: 
*
* Author	 : Wang FuYu <wangfuyu@tp-link.net>
* Create Date: 2015-04-01
* 
* History	 : 
*------------------------------------------------------------
*
*01, 01Apr15, Wang FuYu, create file.
************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "cloud-encrypt.h"
#include "helloworld.h"

#if 0
/******************************************************************************
* FUNCTION		: getEncryptHashKey()
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: get encrypt key from device id
* INPUT			: 
* OUTPUT		: encrypt key(8 bytes)
* RETURN		: 
******************************************************************************/ 
int getEncryptHashKey(char key[])
{
	memcpy(key, ENCYRPT_KEY_DEVID, ENCRYPT_KEY_LEN);
	MSG_PRINT("encrypt key is %s.\n", key);
	
	return ENCRYPT_OK;
}

/******************************************************************************
* FUNCTION		: encryptFile()
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: encrypt buf content by DES
* INPUT			: 
* OUTPUT		: 
* RETURN		: 
******************************************************************************/ 
unsigned char *encryptFile(unsigned char *srcBuf, int srcLen, int *destLen)
{
	int ret = 0;
	int dstLen = 0;
	int retLen = 0;
	char hashKey[ENCRYPT_KEY_LEN + 1] = {0};
	unsigned char *dstBuf = NULL;

	if (NULL == srcBuf || 0 >= srcLen)
	{
		MSG_ERR("src buf is NULL or src len is invalid!\n");
		return NULL;
	}

	memcpy(hashKey, ENCYRPT_KEY_DEVID, ENCRYPT_KEY_LEN);
	MSG_PRINT("encrypt key is %s.\n", hashKey);
  
	dstLen = (srcLen + 7) - (srcLen + 7) % 8 + 8;
	dstBuf = (unsigned char*)malloc(dstLen);
	if (NULL == dstBuf)
	{
		MSG_ERR("malloc dstBuf failed!\n");
		return NULL; 
	}
	memset(dstBuf, 0, dstLen);

	MSG_PRINT("dst len is %d.\n", dstLen);
	retLen = des_min_do(srcBuf, srcLen, dstBuf, dstLen, hashKey, DES_ENCRYPT);
	if (0 >= retLen)
	{
		MSG_ERR("des_min_do failed!\n");
		FREE_P(dstBuf);
		return NULL;
	}

	*destLen = retLen;
	MSG_LOG("des successfully, destLen is %d, dstBuf is %s.\n", *destLen, dstBuf);
	return dstBuf;
} 

/******************************************************************************
* FUNCTION		: decryptFile()
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: decrypt config data
* INPUT			: 
* OUTPUT		: 
* RETURN		: 
******************************************************************************/ 
unsigned char *decryptFile(unsigned char *srcBuf, int srcLen, int *destLen)
{
	int ret = 0;
	int dstLen = 0;
	int retLen = 0;
	char hashKey[ENCRYPT_KEY_LEN] = {0};
	unsigned char *dstBuf = NULL;

	if (NULL == srcBuf || 0 >= srcLen || NULL == destLen)
	{
		MSG_ERR("NULL == srcDecryptBuf || 0 >= srcLen || NULL == destLen!\n");
		return NULL;
	}

	memcpy(hashKey, ENCYRPT_KEY_DEVID, ENCRYPT_KEY_LEN);
	MSG_PRINT("encrypt key is %s.\n", hashKey);

	dstLen = (srcLen + 7) - (srcLen + 7) % 8 + 8;
	dstBuf = (unsigned char*)malloc(dstLen);
	if (NULL == dstBuf)
	{
		MSG_ERR("malloc dstBuf failed!\n");
		return NULL; 
	}
	memset(dstBuf, 0, dstLen);

	retLen = des_min_do(srcBuf, srcLen, dstBuf, dstLen, hashKey, DES_DECRYPT);
	if (0 >= retLen)
	{
		MSG_ERR("des_min_do failed!\n");
		FREE_P(dstBuf);
		return NULL;
	}

	*destLen = retLen;

	MSG_LOG("destLen is %d, decrypt msg successfully!\n", *destLen);
	return dstBuf;
}

/************************************************************
 Function   : lzmaCompData
 Author     : Wang FuYu <wangfuyu@tp-link.net>
 Description: compress data
 Input      : 
 Output     : 
 Return     :  
************************************************************/
int desEncData(char *srcFileName, char *dstFileName)
{
	int ret = -1;
	int error = 0;
	int srcLen = 0;
	int dstLen = 0;
	FILE *srcFd = NULL;
	FILE *dstFd = NULL;
	unsigned char *srcBuf = NULL;
	unsigned char *dstBuf = NULL;

	if (NULL == srcFileName || NULL == dstFileName)
	{
		MSG_PRINT("srcFileName or dstFileName is NULL!\n");
		return -1;
	}

	ret = getFileContentLen(srcFileName, &srcLen);
	if (0 != ret)
	{
		MSG_PRINT("get file<%s> length failed.\n", srcFileName);
		return -1;
	}

	srcBuf = (unsigned char*)malloc(sizeof(unsigned char) * srcLen);
	if (NULL == srcBuf)  
	{  
		MSG_PRINT("No enough memory!\n");
		if (srcBuf) 
		{
			FREE_P(srcBuf);
		}
		
		return -1;  
	}	

	ret = writeFilesToBuf(srcFileName, srcBuf, srcLen);
	if (0 != ret)
	{
		MSG_PRINT("wirte %s to buf failed.\n", srcFileName);
		return -1;
	}
	
	MSG_PRINT("source file, len is %d, content is\n%s\n", srcLen, srcBuf);
	dstBuf = encryptFile(srcBuf, srcLen, &dstLen);
	if (0 >= dstLen)
	{
		MSG_PRINT("des encrypt data failed, error is %d.\n", error);
		FREE_P(srcBuf);
		FREE_P(dstBuf);
		return -1;
	}
	MSG_PRINT("After des encrypt, len is %d, data is:\n%s\n", dstLen, dstBuf);

	ret = writeBufToFile(dstFileName, dstBuf, dstLen);
	if (0 != ret)
	{
		MSG_PRINT("wirte buf to %s failed.\n", dstFileName);
		FREE_P(srcBuf);
		FREE_P(dstBuf);
		return -1;
	}
	
	FREE_P(srcBuf);
	FREE_P(dstBuf);
	return 0;
}

/************************************************************
 Function   : desDecData
 Author     : Wang FuYu <wangfuyu@tp-link.net>
 Description: uncompress data
 Input      : 
 Output     : 
 Return     :  
************************************************************/
int desDecData(char *srcFileName, char *dstFileName)
{
	int ret = -1;
	int error = 0;
	int srcLen = 0;
	int dstLen = 0;
	FILE *srcFd = NULL;
	FILE *dstFd = NULL;
	unsigned char *srcBuf = NULL;
	unsigned char *dstBuf = NULL;

	if (NULL == srcFileName || NULL == dstFileName)
	{
		MSG_PRINT("srcFileName or dstFileName is NULL!\n");
		return -1;
	}

	ret = getFileContentLen(srcFileName, &srcLen);
	if (0 != ret)
	{
		MSG_PRINT("get file<%s> length failed.\n", srcFileName);
		return -1;
	}

	srcBuf = (unsigned char*)malloc(sizeof(unsigned char) * srcLen);
	if (NULL == srcBuf)  
	{  
		MSG_PRINT("No enough memory for src buf!\n");
		return -1;
	}	

	ret = writeFilesToBuf(srcFileName, srcBuf, srcLen);
	if (0 != ret)
	{
		MSG_PRINT("wirte %s to buf failed.\n", srcFileName);
		FREE_P(srcBuf);
		return -1;
	}
	
	MSG_PRINT("des decrypt file, srcLen is %d, dstLen is %d.\n", srcLen, dstLen);
	dstBuf =  decryptFile(srcBuf, srcLen, &dstLen);
	if (0 >= dstLen)
	{
		MSG_PRINT("des decrypt data failed, error is %d.\n", error);
		FREE_P(srcBuf);
		FREE_P(dstBuf);
		return -1;
	}
	MSG_PRINT("After des decrypt, len is %d, data is:\n%s\n", dstLen, dstBuf);

	ret = writeBufToFile(dstFileName, dstBuf, dstLen);
	if (0 != ret)
	{
		MSG_PRINT("wirte buf to %s failed.\n", dstFileName);
		FREE_P(srcBuf);
		FREE_P(dstBuf);
		return -1;
	}

	FREE_P(srcBuf);
	FREE_P(dstBuf);
	return 0;
}
#endif

/******************************************************************************
* FUNCTION		: desEncryptBuf()
* AUTHOR		: wangfuyu <wangfuyu@tp-link.net>
* DESCRIPTION	: en/decrypt buf content
* INPUT			: 
* OUTPUT		: 
* RETURN		: 
******************************************************************************/ 
unsigned char *desEncryptBuf(unsigned char *srcBuf, int srcLen, int *destLen, int type)
{
	int ret = 0;
	int dstLen = 0;
	int retLen = 0;
	char hashKey[ENCRYPT_KEY_LEN + 1] = {0};
	unsigned char *dstBuf = NULL;

	if (NULL == srcBuf || 0 >= srcLen || NULL == destLen)
	{
		MSG_PRINT("NULL == srcDecryptBuf || 0 >= srcLen || NULL == destLen!\n");
		return NULL;
	}

	memcpy(hashKey, ENCYRPT_KEY_DEVID, ENCRYPT_KEY_LEN);
	MSG_PRINT("des %scrypt key is %s.\n", DES_ENCRYPT == type? "en" : "de", hashKey);

	dstLen = (srcLen + 7) - (srcLen + 7) % 8 + 8;
	dstBuf = (unsigned char*)malloc(dstLen);
	if (NULL == dstBuf)
	{
		MSG_PRINT("malloc dstBuf failed!\n");
		return NULL; 
	}
	memset(dstBuf, 0, dstLen);

	MSG_PRINT("srcLen is %d, dstLen is %d, srcBuf is %s.\n", srcLen, dstLen, srcBuf);
	retLen = des_min_do(srcBuf, srcLen, dstBuf, dstLen, hashKey, type);
	if (0 >= retLen)
	{
		MSG_PRINT("des_min_do failed!\n");
		FREE_P(dstBuf);
		return NULL;
	}

	*destLen = retLen;

	MSG_PRINT("dstLen is %d, destLen is %d, %scrypt msg successfully!\n", dstLen, *destLen, DES_ENCRYPT == type? "en" : "de");
	return dstBuf;
}

/************************************************************
 Function   : desDecData
 Author     : Wang FuYu <wangfuyu@tp-link.net>
 Description: uncompress data
 Input      : 
 Output     : 
 Return     :  
************************************************************/
int desEncryptFile(char *srcFileName, char *dstFileName, int type)
{
	int ret = -1;
	int error = 0;
	int srcLen = 0;
	int dstLen = 0;
	FILE *srcFd = NULL;
	FILE *dstFd = NULL;
	unsigned char *srcBuf = NULL;
	unsigned char *dstBuf = NULL;

	if (NULL == srcFileName || NULL == dstFileName)
	{
		MSG_PRINT("srcFileName or dstFileName is NULL!\n");
		return -1;
	}

	ret = getFileContentLen(srcFileName, &srcLen);
	if (0 != ret)
	{
		MSG_PRINT("get file<%s> length failed.\n", srcFileName);
		return -1;
	}

	srcBuf = (unsigned char*)malloc(sizeof(unsigned char) * srcLen);
	if (NULL == srcBuf)  
	{  
		MSG_PRINT("No enough memory for src buf!\n");
		return -1;
	}	

	ret = writeFilesToBuf(srcFileName, srcBuf, srcLen);
	if (0 != ret)
	{
		MSG_PRINT("wirte %s to buf failed.\n", srcFileName);
		FREE_P(srcBuf);
		return -1;
	}
	
	MSG_PRINT("des en/decrypt file, srcLen is %d, dstLen is %d.\n", srcLen, dstLen);
	dstBuf =  desEncryptBuf(srcBuf, srcLen, &dstLen, 
							CLOUD_DES_ENCRYPT == type ? DES_ENCRYPT : DES_DECRYPT);
	if (0 >= dstLen)
	{
		MSG_PRINT("des en/decrypt data failed, dstLen is %d.\n", dstLen);
		FREE_P(srcBuf);
		FREE_P(dstBuf);
		return -1;
	}
	MSG_PRINT("After des en/decrypt, len is %d, data is:\n%s\n", dstLen, dstBuf);

	ret = writeBufToFile(dstFileName, dstBuf, dstLen);
	if (0 != ret)
	{
		MSG_PRINT("wirte buf to %s failed.\n", dstFileName);
		FREE_P(srcBuf);
		FREE_P(dstBuf);
		return -1;
	}

	FREE_P(srcBuf);
	FREE_P(dstBuf);
	return 0;
} 

/************************************************************
 Function   : desDataProg
 Author     : Wang FuYu <wangfuyu@tp-link.net>
 Description: process data
 Input      : 
 Output     : 
 Return     :  
************************************************************/
int desDataProg(char *src, char *dst, unsigned short type)
{
	int ret = -1;
	
	MSG_PRINT("Enter des data program...\n");
	if (CLOUD_DES_ENCRYPT == type || CLOUD_DES_DECRYPT == type)
	{
		ret = desEncryptFile(src, dst, type);
	}
	else
	{
		MSG_PRINT("type<%d> is not supported.\n");
		return -1;
	}
#if 0
	if (CLOUD_DES_ENCRYPT == type)
	{
		ret = desEncData(src, dst);
	}
	else if (CLOUD_DES_DECRYPT == type)
	{
		ret = desDecData(src, dst);
	}
#endif
	if (0 != ret)
	{
		MSG_PRINT("cloud en/decrypt error.\n");
		return -1;
	}
	
	return 0;
}


