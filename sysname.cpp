#include "sysname.h"
#include <windows.h>
using namespace std;

char _getservername_buff[1024];
string getServerName()
{
    unsigned long sz=1024;
    memset(_getservername_buff,0,1024);
    GetComputerName(_getservername_buff,&sz);
    string ans(_getservername_buff);
    memset(_getservername_buff,0,1024);
    sz=1024;
    GetUserName(_getservername_buff,&sz);
    ans=ans+"-"+_getservername_buff;
    return ans;
}
