#ifndef _I8H_HTTP_H
#define _I8H_HTTP_H

#include <string.h>
#include "curl.h"

#ifndef VOID
typedef void VOID;
#endif

#ifndef U64
typedef unsigned long long U64 ;
#endif

#ifndef U32
typedef unsigned int U32;
#endif

#ifndef U16
typedef unsigned short U16 ;
#endif

#ifndef U8
typedef unsigned char U8 ;
#endif

#ifndef S32
typedef int S32;
#endif

#ifndef S16
typedef short S16 ;
#endif

#ifndef S8
typedef char S8 ;
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#define MAX_HASH_CODE_LEN 64

#define I8H_ERROR_CODE_NOERROR 								0	//!没有错误
#define I8H_ERROR_CODE_FAIL									1   //!失败
#define I8H_ERROR_CODE_PASSWORD_ERROR 						2	//!用户名密码错误
#define I8H_ERROR_CODE_NOENOUGHPRI 							3	//!权限不足
#define I8H_ERROR_CODE_NOINIT 								4	//!没有初始化
#define I8H_ERROR_CODE_HASINIT 	     						5	//!已经初始化
#define I8H_ERROR_CODE_ORDER_ERROR							6	//!调用次序错误
#define I8H_ERROR_CODE_PARAMETER_ERROR 						7  //!参数错误
#define I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR					8  //!资源分配错误
#define I8H_ERROR_CODE_CALL3THAPI_ERROR						9	//!调用第三方接口错误
#define I8H_ERROR_CODE_CHAN_ERROR	 						10	//!通道号错误
#define I8H_ERROR_CODE_CHAN_NOTSUPPORT             			11 	//!通道不支持该操作
#define I8H_ERROR_CODE_OVER_MAXLINK 						12	//!连接到DVR的客户端个数超过最大
#define I8H_ERROR_CODE_VERSIONNOMATCH						13	//!版本不匹配
#define I8H_ERROR_CODE_NETWORK_FAIL							14   //!网络错误(包括连接 发送 接收等错误)
#define I8H_ERROR_CODE_OPENFILE_FAIL						15  //!打开文件出错
#define I8H_ERROR_CODE_UPGRADE_FAIL							16  //!DVR升级失败
#define I8H_ERROR_CODE_DATASERIALIZE_FAIL					17	//!数据序列化错误
#define I8H_ERROR_CODE_DATADESERIALIZE_FAIL					18	//!数据反序列化错误
#define I8H_ERROR_CODE_NOENOUGH_BUF							19	//!缓冲区太小
#define I8H_ERROR_CODE_USERNOTEXIST							20	//!用户不存在
#define I8H_ERROR_CODE_COMMAND_UNSUPPORT					21	//!命令不支持
#define I8H_ERROR_CODE_INVALID_HANDLE						22  //!无效句柄
#define I8H_ERROR_CODE_SERVER_FORBIDDEN						23  //!服务禁止
#define I8H_ERROR_CODE_CONNECT_SSL_ERROR					24  //!SSL连接错误



#define UTILITY_FREE_POINT(x)			\
{									\
	if (NULL != x)					\
{								\
	delete x;					\
	x = NULL;					\
}								\
}

//!检查并释放数组
#define UTILITY_FREE_ARRAY(x)			\
{									\
	if (NULL != x)					\
{								\
	delete[] x;					\
	x = NULL;					\
}								\
}

#define UTILITY_CLEAN_ARRAY(x, size)	\
{									\
	if(NULL != x)					\
{								\
	memset(x, 0, size) ;		\
}								\
}

#define UTILITY_COPY_ARRAY(x, y, size)	\
{									\
	if(NULL != (x) && NULL != (y))	\
{								\
	memcpy((x), (y), size) ;	\
}								\
}

typedef struct _tagHttpHead
{
	U16 u16ResultCode ;
	U16 u16DataType ;
}HttpHead ;


typedef struct _tagHttpBody
{
	S8* lpBuffer;
	U8  u8Full;
	U32 u32Offset;
	U32 u32TotalSize;
}HttpBody;

namespace HTTPSDK
{

class CHttpSession
{
public:
	CHttpSession( ) ;
	~CHttpSession( ) ;

	S8 *GetUserName( ) ;
	S8 *GetPassword( ) ;

	S8 *GetSHash( );
	S8 *GetCHash( );
	S8 *GetXHash( );

	VOID SetUser(S8 *lpUserName, S8 *lpPassword) ;
	VOID SetHashCode(const S8* lpSHash, const S8* lpCHash, const S8* lpXHash);


	S32 OnGetRequest(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time);
	
	S32 OnPostRequest(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time);

	S32 OnRequest4Https(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time, VOID* lpUser, U32 u32Type);

	S32 OnHeartBeatRequest(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time);

	S32 OnUpload(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time);

	S32 OnUpload4Https(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time);

	static size_t BodyCallBack(void *buffer, size_t size, size_t nmemb, void *stream);

	static size_t HeadCallBack(void *buffer, size_t size, size_t nmemb, void *stream);

	static VOID SafeCopyString(const S8 *lpSrc, S8 *lpDest, S32 DestLen) ;

private:
	CHttpSession(const CHttpSession&){ }
	CHttpSession& operator=(const CHttpSession&){return (*this);}

private:
	CURL* m_lpCurl ;
	HttpHead *m_lpHead ;
	HttpBody *m_lpBody ;
	S8  m_s8UserName[32] ;
	S8  m_s8Password[16] ;
	S8 m_s8SHash[MAX_HASH_CODE_LEN];
	S8 m_s8XHash[MAX_HASH_CODE_LEN];
	S8 m_s8CHash[MAX_HASH_CODE_LEN];

};

}


#endif
