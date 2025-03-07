#ifndef WebFormCodeH
#define WebFormCodeH

//All of the functions exposed to the HTML subsystem must be extern C
//so that the names don't get mangled.
extern "C"
{
  void AjaxCallback(int sock, const char* url);
  void AjaxCallbackReload(int sock, const char* url);
  void AjaxCallbackTxStatus(int sock, const char* url);
  void AjaxCallbackChanLock(int sock, const char* url);
  void AjaxCallbackTxStatus2(int sock, const char* url);
};

int MyDoPost( int sock, char *url, char *pData, char *rxBuffer );
void RegisterPost();

#endif

//#define TIMEOUT_USEC 300000000 // five minute timeout


