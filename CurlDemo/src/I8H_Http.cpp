#include "I8H_Http.h"
extern S8 s8KeyPathString[256];

namespace HTTPSDK
{


CHttpSession::CHttpSession( )
	:m_lpCurl(NULL)
	,m_lpHead(NULL)
	,m_lpBody(NULL)
{
	UTILITY_CLEAN_ARRAY(m_s8UserName, sizeof(m_s8UserName));
	UTILITY_CLEAN_ARRAY(m_s8Password, sizeof(m_s8Password));
	UTILITY_CLEAN_ARRAY(m_s8SHash, sizeof(m_s8SHash));
	UTILITY_CLEAN_ARRAY(m_s8XHash, sizeof(m_s8XHash));
	UTILITY_CLEAN_ARRAY(m_s8CHash, sizeof(m_s8CHash));
}



CHttpSession::~CHttpSession( )
{
	UTILITY_FREE_POINT(m_lpHead);

	if(m_lpBody != NULL)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);
	}

	UTILITY_FREE_POINT(m_lpBody) ;

}

S8 *CHttpSession::GetUserName( )
{
	return m_s8UserName ;
}


S8 *CHttpSession::GetPassword( )
{
	return m_s8Password ;
}

VOID CHttpSession::SetUser(S8 *lpUserName, S8 *lpPassword)
{
	CHttpSession::SafeCopyString(lpUserName, m_s8UserName, sizeof(m_s8UserName)) ;

	CHttpSession::SafeCopyString(lpPassword, m_s8Password, sizeof(m_s8Password)) ;

}

S8 * CHttpSession::GetSHash()
{
	return m_s8SHash;
}


S8 * CHttpSession::GetCHash()
{
	return m_s8CHash;
}


S8 * CHttpSession::GetXHash()
{
	return m_s8XHash;
}


VOID CHttpSession::SetHashCode(const S8* lpSHash, const S8* lpCHash, const S8* lpXHash)
{
	if(lpSHash != NULL)
	{
		CHttpSession::SafeCopyString(lpSHash, m_s8SHash, sizeof(m_s8SHash));
	}

	if(lpCHash != NULL)
	{
		CHttpSession::SafeCopyString(lpCHash, m_s8CHash, sizeof(m_s8CHash));
	}

	if(lpXHash != NULL)
	{
		CHttpSession::SafeCopyString(lpXHash, m_s8XHash, sizeof(m_s8XHash));
	}
}


VOID CHttpSession::SafeCopyString(const S8 *lpSrc, S8 *lpDest, S32 DestLen)
{
	S32 SrcLen = 0 ;

	if(lpSrc == NULL || lpDest == NULL)
	{
		return ;
	}

	SrcLen = (S32)strlen(lpSrc) ;

	if(SrcLen >= DestLen)
	{
		memcpy(lpDest, lpSrc, DestLen - 1) ;

		lpDest[DestLen - 1] = '\0' ;
	}
	else
	{
		if(SrcLen >= 1)
		{
			strcpy(lpDest, lpSrc) ;
		}
		else
		{
			memset(lpDest, 0, DestLen) ;
		}
	}

}


S32 CHttpSession::OnGetRequest(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time)
{
	U32 u32WaitTime = 0;
	U32 u32TryTimes = 0;
	CURLcode crulCode = CURLE_OK;

	if(lpUrl == NULL || lpHeader == NULL || lpInBuffer == NULL || lpOutBuffer == NULL || u32InSize <= 0 || u32Time <= 0 || u32OutSize == NULL)
	{
		return I8H_ERROR_CODE_PARAMETER_ERROR;
	}

	m_lpHead = new HttpHead;

	m_lpBody = new HttpBody;

	if(m_lpHead == NULL || m_lpBody == NULL)
	{
		UTILITY_FREE_POINT(m_lpHead);
		UTILITY_FREE_POINT(m_lpBody);
		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}

	memset(m_lpHead, 0, sizeof(HttpHead));

	memset(m_lpBody, 0, sizeof(HttpBody));

	u32WaitTime  = (u32Time >> 16) & 0xFFFF;

	u32TryTimes = u32Time & 0xFFFF;

	m_lpBody->u32TotalSize = 1024*1024;

	m_lpCurl = curl_easy_init( );

	if(m_lpCurl == NULL )
	{
		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}

	m_lpBody->lpBuffer = new S8[m_lpBody->u32TotalSize];

	if(m_lpBody->lpBuffer == NULL)
	{
		curl_easy_cleanup(m_lpCurl);

		m_lpCurl = NULL;

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}

	memset(m_lpBody->lpBuffer, 0, m_lpBody->u32TotalSize* sizeof(S8));

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_CONNECTTIMEOUT, u32WaitTime);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_FRESH_CONNECT, TRUE);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_URL, lpUrl);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt( m_lpCurl, CURLOPT_HEADERFUNCTION, HeadCallBack );

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt( m_lpCurl, CURLOPT_HEADERDATA, this );

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_WRITEDATA, (void*)m_lpBody);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_WRITEFUNCTION, BodyCallBack); //回调http的响应数据

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	while(u32TryTimes)
	{
		crulCode = curl_easy_perform(m_lpCurl);

		if(crulCode == CURLE_OK)
		{
			break;
		}
		else
		{
			u32TryTimes--;
		}
	}

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	if(m_lpBody->u8Full == 1)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_NOENOUGH_BUF;
	}

	if(m_lpCurl != NULL)
	{
		curl_easy_cleanup(m_lpCurl);

		m_lpCurl = NULL;
	}

	if(m_lpHead->u16ResultCode == 404)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		return I8H_ERROR_CODE_COMMAND_UNSUPPORT;
	}
	else
	{
		*lpOutBuffer = m_lpBody->lpBuffer;

		*u32OutSize = m_lpBody->u32Offset;

		if (m_lpHead->u16ResultCode == 200)
		{
			return I8H_ERROR_CODE_NOERROR;
		}

		return I8H_ERROR_CODE_COMMAND_UNSUPPORT;

	}

}

S32 CHttpSession::OnPostRequest(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time)
{
	U32 u32WaitTime = 0;
	U32 u32TryTimes = 0;
	CURLcode crulCode = CURLE_OK;

	if(lpUrl == NULL || lpHeader == NULL || lpInBuffer == NULL || lpOutBuffer == NULL || u32InSize <= 0 || u32Time <= 0 || u32OutSize == NULL)
	{
		return I8H_ERROR_CODE_PARAMETER_ERROR;
	}

	m_lpHead = new HttpHead;

	m_lpBody = new HttpBody;

	if(m_lpHead == NULL || m_lpBody == NULL)
	{
		UTILITY_FREE_POINT(m_lpHead);
		UTILITY_FREE_POINT(m_lpBody);
		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}

	memset(m_lpHead, 0, sizeof(HttpHead));

	memset(m_lpBody, 0, sizeof(HttpBody));

	u32WaitTime  = (u32Time >> 16) & 0xFFFF;

	u32TryTimes = u32Time & 0xFFFF;

	m_lpBody->u32TotalSize = 1024*1024;

	m_lpCurl = curl_easy_init( );

	if(m_lpCurl == NULL )
	{
		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}

	m_lpBody->lpBuffer = new S8[m_lpBody->u32TotalSize];

	if(m_lpBody->lpBuffer == NULL)
	{
		curl_easy_cleanup(m_lpCurl);

		m_lpCurl = NULL;

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}

	memset(m_lpBody->lpBuffer, 0, m_lpBody->u32TotalSize* sizeof(S8));

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_CONNECTTIMEOUT, u32WaitTime);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_FRESH_CONNECT, TRUE);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_URL, lpUrl);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt( m_lpCurl, CURLOPT_HEADERFUNCTION, HeadCallBack );

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt( m_lpCurl, CURLOPT_HEADERDATA, this );

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_POST, 1L);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_POSTFIELDS, lpInBuffer);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_POSTFIELDSIZE, u32InSize);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_WRITEDATA, (void*)m_lpBody);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_WRITEFUNCTION, BodyCallBack); //回调http的响应数据

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	while(u32TryTimes)
	{
		crulCode = curl_easy_perform(m_lpCurl);

		if(crulCode == CURLE_OK)
		{
			break;
		}
		else
		{
			u32TryTimes--;
		}
	}

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	if(m_lpBody->u8Full == 1)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_NOENOUGH_BUF;
	}

	if(m_lpCurl != NULL)
	{
		curl_easy_cleanup(m_lpCurl);

		m_lpCurl = NULL;
	}

	if(m_lpHead->u16ResultCode == 404)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		return I8H_ERROR_CODE_COMMAND_UNSUPPORT;
	}
	else
	{
		*lpOutBuffer = m_lpBody->lpBuffer;

		*u32OutSize = m_lpBody->u32Offset;

		if (m_lpHead->u16ResultCode == 200)
		{
			return I8H_ERROR_CODE_NOERROR;
		}

		return I8H_ERROR_CODE_COMMAND_UNSUPPORT;

	}

}

S32 CHttpSession::OnRequest4Https(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time, VOID* lpUser, U32 u32Type)
{
	U32 u32WaitTime = 0;
	U32 u32TryTimes = 0;
	S8 	s8Param[128]={0};
	char* lpDigest = strstr(lpUrl,"/digest/");
	char* lpBasic = strstr(lpUrl,"/basic/");
	CURLcode crulCode = CURLE_OK;
	struct curl_slist *lpCurlList = NULL;

	if(lpUrl == NULL  || lpHeader == NULL || lpInBuffer == NULL || lpOutBuffer == NULL || u32InSize < 0 || u32Time <= 0 || u32OutSize == NULL )
	{
		return I8H_ERROR_CODE_PARAMETER_ERROR;
	}

	if(lpDigest != NULL && lpBasic != NULL)
	{
		return I8H_ERROR_CODE_PARAMETER_ERROR;
	}

	m_lpHead = new HttpHead;

	m_lpBody = new HttpBody;

	if(m_lpHead == NULL || m_lpBody == NULL)
	{
		UTILITY_FREE_POINT(m_lpHead);
		UTILITY_FREE_POINT(m_lpBody);
		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}

	memset(m_lpHead, 0, sizeof(HttpHead));

	memset(m_lpBody, 0, sizeof(HttpBody));

	u32WaitTime  = (u32Time >> 16) & 0xFFFF;

	u32TryTimes = u32Time & 0xFFFF;

	m_lpBody->u32TotalSize = 1024*1024;

	m_lpCurl = curl_easy_init( );

	if(m_lpCurl == NULL )
	{
		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}

	m_lpBody->lpBuffer = new S8[m_lpBody->u32TotalSize];

	if(m_lpBody->lpBuffer == NULL)
	{
		curl_easy_cleanup(m_lpCurl);

		m_lpCurl = NULL;

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}

	memset(m_lpBody->lpBuffer, 0, m_lpBody->u32TotalSize* sizeof(S8));

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_CONNECTTIMEOUT, u32WaitTime);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_FRESH_CONNECT, TRUE);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_URL, lpUrl);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt( m_lpCurl, CURLOPT_HEADERFUNCTION, HeadCallBack );

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt( m_lpCurl, CURLOPT_HEADERDATA, this);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	if(u32Type != 0)
	{
		crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_POST, 1L);

		if(crulCode != CURLE_OK)
		{
			UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

			UTILITY_FREE_POINT(m_lpHead);

			UTILITY_FREE_POINT(m_lpBody);

			if(m_lpCurl != NULL)
			{
				curl_easy_cleanup(m_lpCurl);

				m_lpCurl = NULL;
			}

			return I8H_ERROR_CODE_CALL3THAPI_ERROR;
		}

		crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_POSTFIELDS, lpInBuffer);

		if(crulCode != CURLE_OK)
		{
			UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

			UTILITY_FREE_POINT(m_lpHead);

			UTILITY_FREE_POINT(m_lpBody);

			if(m_lpCurl != NULL)
			{
				curl_easy_cleanup(m_lpCurl);

				m_lpCurl = NULL;
			}

			return I8H_ERROR_CODE_CALL3THAPI_ERROR;
		}

		crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_POSTFIELDSIZE, u32InSize);

		if(crulCode != CURLE_OK)
		{
			UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

			UTILITY_FREE_POINT(m_lpHead);

			UTILITY_FREE_POINT(m_lpBody);

			if(m_lpCurl != NULL)
			{
				curl_easy_cleanup(m_lpCurl);

				m_lpCurl = NULL;
			}

			return I8H_ERROR_CODE_CALL3THAPI_ERROR;
		}
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_WRITEDATA, (void*)m_lpBody);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_WRITEFUNCTION, BodyCallBack); //回调http的响应数据

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_CAINFO, s8KeyPathString);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_SSL_VERIFYPEER, TRUE);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_SSLVERSION , CURL_SSLVERSION_TLSv1_1);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_SSL_VERIFYHOST, FALSE);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	sprintf(s8Param, "%s:%s", GetUserName(), GetPassword());

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_USERPWD, s8Param);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	if(lpDigest != NULL)
	{
		crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST);

		if(crulCode != CURLE_OK)
		{
			UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

			UTILITY_FREE_POINT(m_lpHead);

			UTILITY_FREE_POINT(m_lpBody);

			if(m_lpCurl != NULL)
			{
				curl_easy_cleanup(m_lpCurl);

				m_lpCurl = NULL;
			}

			return I8H_ERROR_CODE_CALL3THAPI_ERROR;
		}
	}

	if(lpBasic!= NULL)
	{
		crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);

		if(crulCode != CURLE_OK)
		{
			UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

			UTILITY_FREE_POINT(m_lpHead);

			UTILITY_FREE_POINT(m_lpBody);

			if(m_lpCurl != NULL)
			{
				curl_easy_cleanup(m_lpCurl);

				m_lpCurl = NULL;
			}

			return I8H_ERROR_CODE_CALL3THAPI_ERROR;
		}
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_USERAGENT, "curl/7.47.1");

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	if((strlen(GetSHash( )) != 0) && (strlen(GetCHash( )) != 0) && (strlen(GetXHash( )) != 0))
	{
		lpCurlList = curl_slist_append(lpCurlList, GetSHash( ));

		lpCurlList = curl_slist_append(lpCurlList, GetCHash( ));

		lpCurlList = curl_slist_append(lpCurlList, GetXHash( ));

		curl_easy_setopt(m_lpCurl, CURLOPT_HTTPHEADER, lpCurlList);
	}


	while(u32TryTimes)
	{
		crulCode = curl_easy_perform(m_lpCurl);

		if(crulCode == CURLE_OK)
		{
			break;
		}
		else
		{
			u32TryTimes--;
		}

	}

	if(lpCurlList != NULL)
	{
		curl_slist_free_all(lpCurlList);
	}

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(crulCode  == CURLE_SSL_CONNECT_ERROR)
		{
			return I8H_ERROR_CODE_CONNECT_SSL_ERROR;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	if(m_lpBody->u8Full == 1)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_NOENOUGH_BUF;
	}

	if(m_lpCurl != NULL)
	{
		curl_easy_cleanup(m_lpCurl);

		m_lpCurl = NULL;
	}

	if(m_lpHead->u16ResultCode == 404)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		return I8H_ERROR_CODE_COMMAND_UNSUPPORT;
	}
	else if(m_lpHead->u16ResultCode == 403)
	{
		UTILITY_FREE_ARRAY(m_lpBody->lpBuffer);

		UTILITY_FREE_POINT(m_lpHead);

		UTILITY_FREE_POINT(m_lpBody);

		return I8H_ERROR_CODE_SERVER_FORBIDDEN;
	}
	else
	{
		if(m_lpHead->u16ResultCode == 200)
		{

		}

		*lpOutBuffer = m_lpBody->lpBuffer;

		*u32OutSize = m_lpBody->u32Offset;

		return I8H_ERROR_CODE_NOERROR;
	}

}

S32 CHttpSession::OnHeartBeatRequest(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time)
{
	U32 u32WaitTime = 0;
	CURLcode crulCode = CURLE_OK;

	if(lpUrl == NULL || lpHeader == NULL || lpInBuffer == NULL || lpOutBuffer == NULL || u32InSize <= 0 || u32Time <= 0 || u32OutSize == NULL)
	{
		return I8H_ERROR_CODE_PARAMETER_ERROR;
	}

	m_lpHead = new HttpHead;

	if(m_lpHead == NULL)
	{
		UTILITY_FREE_POINT(m_lpHead);

		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}

	memset(m_lpHead, 0, sizeof(HttpHead));

	u32WaitTime  = (u32Time >> 16) & 0xFFFF;

	m_lpCurl = curl_easy_init( );

	if(m_lpCurl == NULL )
	{
		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_CONNECTTIMEOUT, u32WaitTime);

	if(crulCode != CURLE_OK)
	{

		UTILITY_FREE_POINT(m_lpHead);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_URL, lpUrl);

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_POINT(m_lpHead);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt( m_lpCurl, CURLOPT_HEADERFUNCTION, HeadCallBack );

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_POINT(m_lpHead);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt( m_lpCurl, CURLOPT_HEADERDATA, this );

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_POINT(m_lpHead);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_WRITEFUNCTION, BodyCallBack); //回调http的响应数据

	if(crulCode != CURLE_OK)
	{
		UTILITY_FREE_POINT(m_lpHead);

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_perform(m_lpCurl);

	UTILITY_FREE_POINT(m_lpHead);

	if(m_lpCurl != NULL)
	{
		curl_easy_cleanup(m_lpCurl);

		m_lpCurl = NULL;
	}

	if(crulCode != CURLE_OK)
	{
		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}
	else
	{
		return I8H_ERROR_CODE_NOERROR;
	}

}


S32 CHttpSession::OnUpload4Https(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time)
{
	U32 u32WaitTime = 0;
	CURLcode crulCode = CURLE_OK;
	CURLFORMcode eFormRet = CURL_FORMADD_OK;
	struct curl_httppost *post = NULL;
	struct curl_httppost *last = NULL;
	struct curl_slist *headers = NULL;

	if(lpUrl == NULL || lpHeader == NULL || lpInBuffer == NULL || lpOutBuffer == NULL || u32InSize <= 0 || u32Time <= 0 || u32OutSize == NULL)
	{
		return I8H_ERROR_CODE_PARAMETER_ERROR;
	}

	u32WaitTime  = (u32Time >> 16) & 0xFFFF;

	m_lpCurl = curl_easy_init( );

	if(m_lpCurl == NULL )
	{
		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}


	headers = curl_slist_append(headers, "Expect:");

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_HTTPHEADER, headers);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_VERBOSE, 1);

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_CAINFO, s8KeyPathString);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_SSL_VERIFYPEER, TRUE);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_SSL_VERIFYHOST, FALSE);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	eFormRet = curl_formadd(&post, &last, CURLFORM_COPYNAME, "file", CURLFORM_FILE, lpInBuffer, CURLFORM_END) ;

	if(eFormRet != CURL_FORMADD_OK)
	{

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		if(post != NULL)
		{
			curl_formfree(post);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;

	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_URL, lpUrl);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		if(post != NULL)
		{
			curl_formfree(post);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_HTTPPOST, post);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		if(post != NULL)
		{
			curl_formfree(post);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_CONNECTTIMEOUT, u32WaitTime);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		if(post != NULL)
		{
			curl_formfree(post);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_perform(m_lpCurl);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		if(post != NULL)
		{
			curl_formfree(post);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;

	}

	*lpOutBuffer = NULL;

	*u32OutSize = 0;

	if(m_lpCurl != NULL)
	{
		curl_easy_cleanup(m_lpCurl);

		m_lpCurl = NULL;
	}

	if(post != NULL)
	{
		curl_formfree(post);
	}

	if(headers != NULL)
	{
		curl_slist_free_all(headers);
	}

	return I8H_ERROR_CODE_NOERROR;

}


S32 CHttpSession::OnUpload(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time)
{
	U32 u32WaitTime = 0;
	CURLcode crulCode = CURLE_OK;
	CURLFORMcode eFormRet = CURL_FORMADD_OK;
	struct curl_httppost *post = NULL;
	struct curl_httppost *last = NULL;
	struct curl_slist *headers = NULL;

	if(lpUrl == NULL || lpHeader == NULL || lpInBuffer == NULL || lpOutBuffer == NULL || u32InSize <= 0 || u32Time <= 0 || u32OutSize == NULL)
	{
		return I8H_ERROR_CODE_PARAMETER_ERROR;
	}

	u32WaitTime  = (u32Time >> 16) & 0xFFFF;

	m_lpCurl = curl_easy_init( );

	if(m_lpCurl == NULL )
	{
		return I8H_ERROR_CODE_ALLOC_RESOURCE_ERROR;
	}

	headers = curl_slist_append(headers, "Expect:");

	headers = curl_slist_append(headers,"Cookie:s_browsertype=1;");

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_HTTPHEADER, headers);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);

		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_VERBOSE, 1);

	eFormRet = curl_formadd(&post, &last, CURLFORM_COPYNAME, "file", CURLFORM_FILE, lpInBuffer, CURLFORM_END) ;

	if(eFormRet != CURL_FORMADD_OK)
	{

		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		if(post != NULL)
		{
			curl_formfree(post);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;

	}


	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_URL, lpUrl);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		if(post != NULL)
		{
			curl_formfree(post);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_HTTPPOST, post);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		if(post != NULL)
		{
			curl_formfree(post);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}


	crulCode = curl_easy_setopt(m_lpCurl, CURLOPT_CONNECTTIMEOUT, u32WaitTime);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		if(post != NULL)
		{
			curl_formfree(post);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;
	}

	crulCode = curl_easy_perform(m_lpCurl);

	if(crulCode != CURLE_OK)
	{
		if(m_lpCurl != NULL)
		{
			curl_easy_cleanup(m_lpCurl);

			m_lpCurl = NULL;
		}

		if(headers != NULL)
		{
			curl_slist_free_all(headers);
		}

		if(post != NULL)
		{
			curl_formfree(post);
		}

		return I8H_ERROR_CODE_CALL3THAPI_ERROR;

	}

	*lpOutBuffer = NULL;

	*u32OutSize = 0;

	if(m_lpCurl != NULL)
	{
		curl_easy_cleanup(m_lpCurl);

		m_lpCurl = NULL;
	}

	if(headers != NULL)
	{
		curl_slist_free_all(headers);
	}

	if(post != NULL)
	{
		curl_formfree(post);
	}

	return I8H_ERROR_CODE_NOERROR;

}



size_t CHttpSession::HeadCallBack(void *buffer, size_t size, size_t nmemb, void *userp)
{
    printf("\r\n HeadCallBack:buffer: %s\r\n", (char*)buffer);
	int retCode = 0 ;

	char* lpSrc = NULL;

	CHttpSession* lpHttp  = (CHttpSession*)userp;

	int ret = sscanf((char*)buffer,"HTTP/1.%*c %3d",&retCode);

	if(ret)
	{
		lpHttp->m_lpHead->u16ResultCode = retCode;
	}

	if((lpSrc = strstr((char*)buffer,"S-HASH")) != NULL)
	{
		CHttpSession::SafeCopyString(lpSrc, lpHttp->m_s8SHash, (S32)strlen(lpSrc) );
	}
	else if((lpSrc = strstr((char*)buffer,"C-HASH")) != NULL)
	{
		CHttpSession::SafeCopyString(lpSrc, lpHttp->m_s8CHash, (S32)strlen(lpSrc));
	}
	else if((lpSrc = strstr((char*)buffer,"X-HASH")) != NULL)
	{
		CHttpSession::SafeCopyString(lpSrc, lpHttp->m_s8XHash, (S32)strlen(lpSrc));
	}

	return size * nmemb;
}



size_t CHttpSession::BodyCallBack(void *ptr, size_t size, size_t nmemb, void *stream)
{
    printf("\r\n BodyCallBack:buffer: %s\r\n", (char*)ptr);
	size_t tSize = 0;

	HttpBody* lpWebParam = (HttpBody*)stream;

	if(lpWebParam->lpBuffer != NULL)
	{
		if(lpWebParam->u32Offset + size * nmemb > lpWebParam->u32TotalSize)
		{
			lpWebParam->u8Full = 1;
		}
		else
		{
			memcpy(lpWebParam->lpBuffer + lpWebParam->u32Offset, ptr, size * nmemb);

			tSize = size * nmemb;

			lpWebParam->u32Offset = (U32)tSize + lpWebParam->u32Offset ;
		}
	}

	return size * nmemb;

}


}


