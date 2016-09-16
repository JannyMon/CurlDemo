#include "I8H_Http.h"
#include <stdlib.h>

#ifdef WIN32
#include <direct.h>
#else
#include<unistd.h>
#endif

using namespace std;

S8 s8KeyPathString[256] = {0};
S32 OnHttpReq(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time, U32 u32Type);

int main(int argc, char* argv[])
{
    CURLcode crulCode = CURLE_OK;
	char *lpKeyPath = NULL;
    S8* lpInBuffer = (S8*)"{\"type\":0,\"ch\":1,\"param\":{\"ip\":\"192.168.0.216\",\"username\":\"admin\",\"pwd\":\"\"},\"data\":{}}";
	S8* lpOutBuffer = NULL;
	S8 s8Url[256]="www.baidu.com" ;
	S8 s8Header[4] = {0};
	S32 s32Result = 0;
	U32 u32BufferSize = strlen(lpInBuffer) + 1;
	U32 u32OutSize = 0;


#ifdef WIN32
	lpKeyPath=_getcwd(NULL,0);

    strcpy((char*)s8KeyPathString, lpKeyPath);

    strcat((char*)s8KeyPathString, "\\self.pem");
#else
    lpKeyPath = getcwd(NULL,0);

    strcpy((char*)s8KeyPathString, lpKeyPath);

    strcat((char*)s8KeyPathString, "/self.pem");
#endif

	printf("%s\r\n", s8KeyPathString);

    crulCode = curl_global_init(CURL_GLOBAL_ALL);

    s32Result =  OnHttpReq(s8Url, s8Header, s8Header, u32BufferSize, &lpOutBuffer, &u32OutSize, (5 << 16)|1, 0);

	char c = getchar( );

    s32Result =  OnHttpReq((S8*)"http://192.168.0.216:80//goform//frmDevicePara", s8Header, lpInBuffer, u32BufferSize, &lpOutBuffer, &u32OutSize, (5 << 16)|1, 1);

	curl_global_cleanup();

    return 0;
}


S32 OnHttpReq(S8* lpUrl, S8* lpHeader, S8* lpInBuffer,U32 u32InSize, S8** lpOutBuffer, U32* u32OutSize, U32 u32Time, U32 u32Type)
{
    S32 s32Result = -1;

	HTTPSDK::CHttpSession* lpHttp = NULL;

	lpHttp = new HTTPSDK::CHttpSession;

	if(u32Type == 0)
    {
        s32Result = lpHttp->OnGetRequest(lpUrl, lpHeader, lpInBuffer, u32InSize, lpOutBuffer, u32OutSize, u32Time);
    }
    else if(u32Type == 1)
    {
        s32Result = lpHttp->OnPostRequest(lpUrl, lpHeader, lpInBuffer, u32InSize, lpOutBuffer, u32OutSize, u32Time);
    }

	if (s32Result != I8H_ERROR_CODE_NOERROR)
	{
		printf("http request error!\r\n");
	}

	UTILITY_FREE_POINT(lpHttp);

	return s32Result;
}
