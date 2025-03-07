#define int16 long int

unsigned int16 Use_Crc(unsigned char *Buffer, unsigned int BuffLength);
unsigned int16 Crc_Update(long data_ptr, unsigned int16 accum);
int pushCharToComBuf(char pushVal);
int popCharFromComBuf(char *returnVal);
int pushCharToComBuf2(char pushVal);
int popCharFromComBuf2(char *returnVal);
void readUART0ComMsg();
void readUART1ComMsg();
void writeUART1ComMsg(void *);
void processComMsg();
void processComMsg2();
void writeRSMsg();
void writeCDMsg();
void writeCTMsg();
//void AjaxCallback(int sock, const char* url);
void sendCSCommand();

#define USE_OS_FOR_WRITE_UART1 1





