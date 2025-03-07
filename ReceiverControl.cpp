#include <startnet.h>
#include <stdio.h>
#include <ctype.h>
#include <system.h> // Found in C:\Nburn\include
#include <utils.h> // Found in C:\Nburn\include
#include <serial.h>
#include <iosys.h>
#include "ReceiverControl.h"
#include "serialburnerdata.h"

unsigned int16 Crc_Update(long data_ptr, unsigned int16 accum) {
	static const unsigned int16 crctab[] = { // this table requires 512 Bytes of storage
			0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
					0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce,
					0xf1ef, 0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294,
					0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd,
					0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
					0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528,
					0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672,
					0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4, 0xb75b,
					0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
					0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802,
					0x3823, 0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969,
					0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71,
					0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e,
					0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87, 0x4ce4,
					0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f,
					0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97,
					0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
					0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59,
					0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d,
					0xf14e, 0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004,
					0x4025, 0x7046, 0x6067, 0x83b9, 0x9398, 0xa3fb, 0xb3da,
					0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3,
					0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb,
					0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2,
					0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
					0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d,
					0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676,
					0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8,
					0x89e9, 0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
					0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c, 0xeb3f,
					0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a, 0x4a75, 0x5a54,
					0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e,
					0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
					0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0,
					0x0cc1, 0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba,
					0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93,
					0x3eb2, 0x0ed1, 0x1ef0 };

	int16 longVal = accum;
	int16 tableIndex = (longVal >> 8) ^ data_ptr;
	int16 tableVal = crctab[tableIndex];

	longVal &= 0x00FF;
	longVal = longVal << 8;
	longVal = longVal ^ tableVal;

	return (longVal);

	//*accum = (*accum << 8) ^ crctab[(*accum >> 8) ^ data_ptr];

	/*  accum is a ptr to type	 unsigned short ie 8 bit
	 *accum dereferences accum i.e. change the value of the information at address *accum which keeps everything
	 as a type unsigned short - Otherwise the shift is treated like a 12 bit shift and shift left of the
	 value FFEEgives FFEE00 instead of the desired EE00*/
}
// =============================================================================================================

unsigned int16 Use_Crc(unsigned char *Buffer, unsigned int BuffLength) {
	unsigned int i;
	unsigned int16 crcValue = 0xFFFF;

	for (i = 0; i < BuffLength; i++) {
		crcValue = Crc_Update(Buffer[i], crcValue);
	}
//iprintf("CRC=%ld\n",crcValue);
	return crcValue;
}
// =============================================================================================================
// Buffer for com data coming in on UART1 (the ladybug port)
#define COM_MSG_BUF_SIZE 100
char comMsgBuffer[COM_MSG_BUF_SIZE];
unsigned int comMsgBufIndex = 0;
unsigned int comMsgBufLength = 0;

// Buffer for com data coming in on UART0 (the debug port)
#define COM_MSG_BUF_SIZE2 100
char comMsgBuffer2[COM_MSG_BUF_SIZE2];
unsigned int comMsgBufIndex2 = 0;
unsigned int comMsgBufLength2 = 0;

int pushCharToComBuf(char pushVal) {
	unsigned int index;
	if (comMsgBufLength < COM_MSG_BUF_SIZE) {
		index = comMsgBufIndex + comMsgBufLength;
		if (index >= COM_MSG_BUF_SIZE
		)
			index -= COM_MSG_BUF_SIZE;
		comMsgBuffer[index] = pushVal;
		comMsgBufLength++;
		return (1);
	} else {
		return (0);
	}
}

int popCharFromComBuf(char *returnVal) {
	if (comMsgBufLength > 0) {
		*returnVal = comMsgBuffer[comMsgBufIndex];
		comMsgBufIndex++;
		if (comMsgBufIndex >= COM_MSG_BUF_SIZE) {
			comMsgBufIndex = 0;
		}
		comMsgBufLength--;
		return (1);
	} else {
		return (0);
	}
}

int pushCharToComBuf2(char pushVal) {
	unsigned int index;
	if (comMsgBufLength2 < COM_MSG_BUF_SIZE2) {
//	   iprintf("Pushing %c\n",pushVal);
		index = comMsgBufIndex2 + comMsgBufLength2;
		if (index >= COM_MSG_BUF_SIZE2
		)
			index -= COM_MSG_BUF_SIZE2;
		comMsgBuffer2[index] = pushVal;
		comMsgBufLength2++;
		return (1);
	} else {
		return (0);
	}
}

int popCharFromComBuf2(char *returnVal) {
	if (comMsgBufLength2 > 0) {
		*returnVal = comMsgBuffer2[comMsgBufIndex2];
		comMsgBufIndex2++;
		if (comMsgBufIndex2 >= COM_MSG_BUF_SIZE2) {
			comMsgBufIndex2 = 0;
		}
		comMsgBufLength2--;
		return (1);
	} else {
		return (0);
	}
}

#define EXT_MSG_TYPE_ST 1
#define EXT_MSG_TYPE_SD 2
#define EXT_MSG_TYPE_SW 3
#define EXT_MSG_TYPE_SV 4
#define EXT_MSG_TYPE_SY 5
#define EXT_MSG_TYPE_SG 6
#define EXT_MSG_TYPE_SP 7
#define EXT_MSG_TYPE_SH 8
#define EXT_MSG_TYPE_SK 9
#define EXT_MSG_TYPE_SM 10
#define EXT_MSG_TYPE_SX 11
#define EXT_MSG_TYPE_TY 12
#define EXT_MSG_TYPE_TP 13

unsigned char comMsg[30];
int comMsgCount = 0;
int comMsgLength = 0;
int badMsgCount = 0;
int comMsgType;
int dataReceivedFlag = 0;

unsigned char comMsg2[30];
int comMsgCount2 = 0;
int comMsgLength2 = 0;
int badMsgCount2 = 0;
int comMsgType2;
int dataReceivedFlag2 = 0;

void readUART1ComMsg() {
	char oneChar;
	unsigned char oneByte1, oneByte2;

	while (comMsgBufLength > 0) {
		if (comMsgBufLength > 0) {
			popCharFromComBuf(&oneChar);
//         iprintf("Popped one char %d\n",oneChar);

			comMsg[comMsgCount] = oneChar;
//         iprintf("Read an %c...comMsgCount=%d\n",oneChar,comMsgCount);
			if (comMsgCount == 0) {
				if (oneChar == 's' || oneChar == 't') {
					comMsgCount++;
				} else {
					badMsgCount++;
				}
			} else if (comMsgCount == 1) {
				comMsgCount++;
				if (comMsg[0] == 's') {
					switch (oneChar) {
					case 't':
						comMsgLength = 14;
						comMsgType = EXT_MSG_TYPE_ST;
						break;
					case 'd':
						comMsgLength = 19;
						comMsgType = EXT_MSG_TYPE_SD;
						break;
					case 'w':
						comMsgLength = 9;
						comMsgType = EXT_MSG_TYPE_SW;
						break;
					case 'v':
						comMsgLength = 12;
						comMsgType = EXT_MSG_TYPE_SV;
						break;
					case 'y':
						comMsgLength = 8;
						comMsgType = EXT_MSG_TYPE_SY;
						break;
					case 'g':
						comMsgLength = 20;
						comMsgType = EXT_MSG_TYPE_SG;
						break;
					case 'p':
						comMsgLength = 5;
						comMsgType = EXT_MSG_TYPE_SP;
						break;
					case 'h':
						comMsgLength = 12;
						comMsgType = EXT_MSG_TYPE_SH;
						break;
					case 'k':
						comMsgLength = 7;
						comMsgType = EXT_MSG_TYPE_SK;
						break;
					case 'm':
						comMsgLength = 7;
						comMsgType = EXT_MSG_TYPE_SM;
						break;
					case 'x':
						comMsgLength = 5;
						comMsgType = EXT_MSG_TYPE_SX;
						break;
					default:
						comMsgLength = 0;
						badMsgCount++;
						break;
					}
				} else if (comMsg[0] == 't') {
					switch (oneChar) {
					case 'y':
						comMsgLength = 5;
						comMsgType = EXT_MSG_TYPE_TY;
						break;
					case 'p':
						comMsgLength = 17;
						comMsgType = EXT_MSG_TYPE_TP;
						break;
					default:
						comMsgCount = 0;
						badMsgCount++;
						break;
					}
				}
			} else {
				comMsgCount++;
				if (comMsgCount >= comMsgLength) {
					comMsgCount = 0;
//               iprintf("found message %d...checking CRC\n",comMsgType);
					long unsigned int lCRC_value = Use_Crc(comMsg,
							comMsgLength - 2);
					oneByte1 = (unsigned char) ((lCRC_value >> 8) & 0x00ff);
					oneByte2 = (unsigned char) (lCRC_value & 0x00ff);
//               iprintf("comparing first CRC byte %d to %d\n",oneByte1,comMsg[comMsgLength-2]);
//               iprintf("comparing second CRC byte %d to %d\n",oneByte2,comMsg[comMsgLength-1]);
					if (oneByte1 == comMsg[comMsgLength - 2]
							&& oneByte2 == comMsg[comMsgLength - 1]) {
//                   iprintf("message good...processing\n");
						dataReceivedFlag = 1;
						processComMsg();
					}
				}
			}
		}
	}
}

long unsigned statusTuneFreq;

extern int rssiA, rssiB, vuL, vuR;
char statusByte1, statusByte2;
extern int lockA, lockB;
long extSTMsgTime = 0;

int receivedChannelNumber = 0;
int requestedChannelNumber = 0;
int receivedFilterNumber = 0;
int receivedSquelchMode = 0;
int receivedMonoStereoMode = 0;
int receivedToneNoLock = 0;
int receivedCalTone = 0;
int controlPowerValue = 0;
long unsigned int txID = 0;
long unsigned int receiverSerialNum = 0;
char receiverFirmwareVersion[5];
long unsigned int displayedTxID = 0;
long unsigned int txVoltage = 0;
long unsigned int txRSSI = 0;
long unsigned int displayedTxVoltage = 0;
int subchanPersistCount = 0;
int receivedTxStatus;
extern int chanLock[];
extern int allChannelsFlag[];
extern NV_SettingsStruct NV_Settings;

/*
 long long unsigned int freqList[8]={
 0x058800,
 0x059800,
 0x05A800,
 0x05B800,
 0x05C800,
 0x05D800,
 0x05E800,
 0x05F800
 };
 */

int bad_sv_counter=0;
void processComMsg() {
	long unsigned int testlongval;
	if (comMsgType == EXT_MSG_TYPE_ST) {

		extSTMsgTime = GetPreciseTime(); // mark time that message was received

		statusTuneFreq = (long unsigned) comMsg[2];
		statusTuneFreq <<= 8;
		statusTuneFreq |= (long unsigned) comMsg[3];
		statusTuneFreq <<= 8;
		statusTuneFreq |= (long unsigned) comMsg[4];

		receivedChannelNumber = 0;
		if (statusTuneFreq == NV_Settings.freq[0]) {
			receivedChannelNumber = 1;
		}
		if (statusTuneFreq == NV_Settings.freq[1]) {
			receivedChannelNumber = 2;
		}
		if (statusTuneFreq == NV_Settings.freq[2]) {
			receivedChannelNumber = 3;
		}
		if (statusTuneFreq == NV_Settings.freq[3]) {
			receivedChannelNumber = 4;
		}
		if (statusTuneFreq == NV_Settings.freq[4]) {
			receivedChannelNumber = 5;
		}
		if (statusTuneFreq == NV_Settings.freq[5]) {
			receivedChannelNumber = 6;
		}
		if (statusTuneFreq == NV_Settings.freq[6]) {
			receivedChannelNumber = 7;
		}
		if (statusTuneFreq == NV_Settings.freq[7]) {
			receivedChannelNumber = 8;
		}

		receivedFilterNumber = (int) (comMsg[5]);
		receivedFilterNumber &= 0x07;

		receivedMonoStereoMode = 1;
		if ((comMsg[5] & 0x40) == 0) {
			receivedMonoStereoMode = 2; // 1:mono mode, 0:stereo mode
		}
		receivedSquelchMode = 1;
		if ((comMsg[5] & 0x20) != 0) {
			receivedSquelchMode = 2; // 2:extended range, 1:noise reduce
		}
		receivedTxStatus = 0;
		if ((comMsg[5] & 0x08) != 0) {
			receivedTxStatus = 1; // 1:transmitting, 0:not transmitting
		}

		receivedToneNoLock = 0;
		if ((comMsg[6] & 0x02) != 0) {
			receivedToneNoLock = 1;
		}

		receivedCalTone = 0;
		if ((comMsg[5] & 0x10) != 0) {
			receivedCalTone = 1;
		}

//		iprintf("received squelch mode=%d\n",receivedSquelchMode);

		lockA = 0;
		lockB = 0;
		if ((comMsg[8] & 0x80) == 0) {
			lockA = 1;
		}
		if ((comMsg[9] & 0x80) == 0) {
			lockB = 1;
		}
		if (lockA == 1 || lockB == 1) {
			chanLock[receivedChannelNumber - 1] = 2;
		} else {
			chanLock[receivedChannelNumber - 1] = 1;
		}
		allChannelsFlag[receivedChannelNumber - 1] = 1;
		rssiA = comMsg[8] & 0x7f;
		if (lockA == 1) {
			rssiA -= 127;
		} else {
			rssiA = -rssiA;
		}
		rssiB = comMsg[9] & 0x7f;
		if (lockB == 1) {
			rssiB -= 127;
		} else {
			rssiB = -rssiB;
		}
		statusByte1 = comMsg[5];
		statusByte2 = comMsg[6];

		vuL = comMsg[10];
		vuR = comMsg[11];

	}
	if (comMsgType == EXT_MSG_TYPE_SV) {
		if (comMsg[4] ^ comMsg[5] == comMsg[6] && (comMsg[4]!=0 || comMsg[5]!=0)) {
			bad_sv_counter=0;
			iprintf("\n--------------------------------------------");
	        iprintf("\n%d, %d, %d\n", comMsg[4], comMsg[5], comMsg[6]);
			iprintf("cheksum worked\n");
			// compute transmit id from message bytes 4 and 5
			testlongval = (comMsg[4] & 0x001f);
			testlongval &= 0x00ff;
			testlongval <<= 8;
			testlongval |= (comMsg[5] & 0x00ff);

			if ((comMsg[4] & 0x00C0) != 0) {
				txRSSI = testlongval;
			} else {
				txID = testlongval;

			}
			txVoltage = comMsg[7];

		} else {
			bad_sv_counter++;
			if (bad_sv_counter>6) {
				bad_sv_counter=0;
				testlongval = (comMsg[4] & 0x001f);
				testlongval &= 0x00ff;
				testlongval <<= 8;
				testlongval |= (comMsg[5] & 0x00ff);

				if ((comMsg[4] & 0x00C0) != 0) {
					txRSSI = testlongval;
				} else {
					txID = testlongval;

				}
				txVoltage = comMsg[7];

			}
		}
	}

	/* 		if (txID==0) { // this section allows the TdID to persist on the screen when there is a week signal
	 if (subchanPersistCount<5) {
	 subchanPersistCount++;
	 return;
	 }
	 } else {
	 subchanPersistCount=0;
	 }
	 if (txID!=displayedTxID || (txID!=0 && txVoltage!=displayedTxVoltage)) {
	 displayedTxID=txID;
	 displayedTxVoltage=txVoltage;
	 }
	 */

	if (comMsgType == EXT_MSG_TYPE_SD) {
		receiverSerialNum = (long unsigned int) comMsg[2];
		receiverSerialNum *= 10;
		receiverSerialNum += (long unsigned int) comMsg[3];
		receiverSerialNum *= 10;
		receiverSerialNum += (long unsigned int) comMsg[4];
		receiverFirmwareVersion[0] = (char) comMsg[5];
		receiverFirmwareVersion[1] = (char) comMsg[6];
		receiverFirmwareVersion[2] = (char) comMsg[7];
		receiverFirmwareVersion[3] = (char) comMsg[8];
		receiverFirmwareVersion[4] = (char) 0;
	}

}

int msgTypeCounter = 0;
extern int channelScanFlag;
extern int singleScanFlag;
extern int savedChannel;

void writeUART1ComMsg(void *) {
	long x;
	int doneFlag;
	int i;
#if USE_OS_FOR_WRITE_UART1==1
	while (1) {
#endif
		x = GetPreciseTime();
//	iprintf("Elapsed time since ST message: %ld\n",x-extSTMsgTime);
		if ((x - extSTMsgTime) > (long) 500000) { // if more than a quarter second has passed...
#if 1==1
			if (channelScanFlag == 0) {
#endif
				if (NV_Settings.netburnerMakesRequests) {
					msgTypeCounter++;
					if (msgTypeCounter == 5) {
//				iprintf("Writing CD message\n");
						writeCDMsg();
					} else if (msgTypeCounter >= 6) {
//				iprintf("Writing CT message\n");
						msgTypeCounter = 0;
						writeCTMsg();
					} else {
//				iprintf("Writing RS message\n");
						writeRSMsg();
					}
				}
#if 1==1
			} else {
				requestedChannelNumber++;
				if (requestedChannelNumber > 8)
					requestedChannelNumber = 1;
				if (singleScanFlag == 1) { // if in single scan mode, check to see if all channels are scanned yet
					if (savedChannel == requestedChannelNumber) { // if next channel is saved channel to return to...
						doneFlag = 1;
						for (i = 0; i < 8; i++) { // if any channel not scanned yet, then keep scanning
							if (chanLock[i] == 0) {
								doneFlag = 0; // a channel is not scanned yet
							}
						}
						if (doneFlag == 1) { // if all channels scanned...
							channelScanFlag = 0; // stop scanning
							singleScanFlag = 0; // stop scanning
						}
					}
				}
				sendCSCommand(); // retune to next channel
			}
#endif
		}
		if (NV_Settings.noReceiverDataTimeout==(long)0) {
			dataReceivedFlag=1;
		} else if ((x - extSTMsgTime) > (long)1000* (long)NV_Settings.noReceiverDataTimeout) { // if more than two seconds has passed...
			dataReceivedFlag = 0; // re-initialize data received flag to indicate that there are no status messages being received
			receivedChannelNumber = 0;
			receivedFilterNumber = 0;
			receivedMonoStereoMode = 0;
			receivedSquelchMode = 0;
		}
#if USE_OS_FOR_WRITE_UART1==1
		OSTimeDly(TICKS_PER_SECOND / 5); // Delay for two tenths of a second
//		OSTimeDly(TICKS_PER_SECOND); // Delay for a full second

	}
#endif
}

extern int FD_SerialData; /* The connected data socket */
char outputMsg[15];

void sendCSCommand() {
	int nwr;
	int chan;
	outputMsg[0] = 'c';
	outputMsg[1] = 's';
//	outputMsg[2] = 0x05;

//	iprintf("\n*****************************\nRequested channel: %d\n***********************\n",requestedChannelNumber);
	chan = requestedChannelNumber - 1;
	if (chan > 7)
		chan = 7;
	if (chan < 0)
		chan = 0;
	outputMsg[2] = (char) (0x0FF & (NV_Settings.freq[chan] >> 16));
	outputMsg[3] = (char) (0x0FF & (NV_Settings.freq[chan] >> 8));
	outputMsg[4] = (char) (0x0FF & NV_Settings.freq[chan]);
	outputMsg[5] = 0x80;
	outputMsg[6] = 0xa0;
	if (receivedMonoStereoMode == 1) {
		outputMsg[5] |= 0x40; // set to stereo
	}
	if (receivedSquelchMode == 2) {
		outputMsg[5] |= 0x20; // set to extended range
	}
	outputMsg[5] |= receivedFilterNumber; // set filter
	if (receivedToneNoLock == 1) {
		outputMsg[6] |= 0x02; // set tone no lock on
	}
	if (receivedCalTone == 1) {
		outputMsg[5] |= 0x10; // set cal tone on
	}

	int16 lCRC_value = Use_Crc((unsigned char*) outputMsg, (9 - 2));
	outputMsg[9 - 2] = (char) ((lCRC_value >> 8) & 0x0FF);
	outputMsg[9 - 1] = (char) (lCRC_value & 0x0FF);
	nwr = write(FD_SerialData, outputMsg, 9);

//	iprintf("\ncontrol byte 1=%x, control byte 2 =%x ",outputMsg[5],outputMsg[6]);
//	iprintf("\nreceived tone no lock=%d, received cal tone =%d \n ",receivedToneNoLock,receivedCalTone);

}

void writeRSMsg() {
	outputMsg[0] = 'r';
	outputMsg[1] = 's';
	int16 lCRC_value = Use_Crc((unsigned char*) outputMsg, (4 - 2));
	outputMsg[2] = (char) ((lCRC_value >> 8) & 0x0FF);
	outputMsg[3] = (char) (lCRC_value & 0x0FF);
	int nwr;
	nwr = write(FD_SerialData, outputMsg, 4);
}

/*
 void writeCYMsg() {
 outputMsg[0] = 'c';
 outputMsg[1] = 'y';
 outputMsg[2] = 0x05;

 switch (receivedChannelNumber) {
 case 1:
 outputMsg[3] = 0x88;
 break;
 case 2:
 outputMsg[3] = 0x98;
 break;
 case 3:
 outputMsg[3] = 0xa8;
 break;
 case 4:
 outputMsg[3] = 0xb8;
 break;
 case 5:
 outputMsg[3] = 0xc8;
 break;
 case 6:
 outputMsg[3] = 0xd8;
 break;
 case 7:
 outputMsg[3] = 0xe8;
 break;
 case 8:
 default:
 outputMsg[3] = 0xf8;
 break;

 }
 outputMsg[4] = 0x00;
 outputMsg[5] = 0x00;

 int16 lCRC_value = Use_Crc((unsigned char*) outputMsg, (8 - 2));
 outputMsg[6] = (char) ((lCRC_value >> 8) & 0x0FF);
 outputMsg[7] = (char) (lCRC_value & 0x0FF);
 int nwr;
 nwr = write(FD_SerialData, outputMsg, 8);
 }
 */
void writeCDMsg() {
	outputMsg[0] = 'c';
	outputMsg[1] = 'd';
	int16 lCRC_value = Use_Crc((unsigned char*) outputMsg, (4 - 2));
	outputMsg[2] = (char) ((lCRC_value >> 8) & 0x0FF);
	outputMsg[3] = (char) (lCRC_value & 0x0FF);
	int nwr;
	nwr = write(FD_SerialData, outputMsg, 4);
}

void writeCTMsg() {
	outputMsg[0] = 'c';
	outputMsg[1] = 't';
	outputMsg[2] = 0;
	outputMsg[3] = 0;
	outputMsg[4] = 0;
	outputMsg[5] = 0;
	outputMsg[6] = 0;
	outputMsg[7] = 0;
	outputMsg[8] = 0;
	outputMsg[9] = 0;

	int16 lCRC_value = Use_Crc((unsigned char*) outputMsg, (12 - 2));
	outputMsg[10] = (char) ((lCRC_value >> 8) & 0x0FF);
	outputMsg[11] = (char) (lCRC_value & 0x0FF);
	int nwr;
	nwr = write(FD_SerialData, outputMsg, 12);
}

void toggleLine17();

#define EXT_MSG_TYPE_RS 100
#define EXT_MSG_TYPE_RX 101
#define EXT_MSG_TYPE_RY 102
#define EXT_MSG_TYPE_RP 103
#define EXT_MSG_TYPE_RQ 104
#define EXT_MSG_TYPE_CS 105
#define EXT_MSG_TYPE_CD 106
#define EXT_MSG_TYPE_CI 107
#define EXT_MSG_TYPE_CW 108
#define EXT_MSG_TYPE_CT 109

#define EXT_MSG_TYPE_CY 110
#define EXT_MSG_TYPE_CG 111
#define EXT_MSG_TYPE_CP 112
#define EXT_MSG_TYPE_CQ 113
#define EXT_MSG_TYPE_CH 114
#define EXT_MSG_TYPE_CJ 115
#define EXT_MSG_TYPE_CK 116
#define EXT_MSG_TYPE_CX 117
#define EXT_MSG_TYPE_CN 118
#define EXT_MSG_TYPE_CM 119
#define EXT_MSG_TYPE_CL 120

void readUART0ComMsg() {
	char oneChar;
	unsigned char oneByte1, oneByte2;

//   toggleLine17();
	while (comMsgBufLength2 > 0) {
		if (comMsgBufLength2 > 0) {
			popCharFromComBuf2(&oneChar);
//         iprintf("Popped one char %d\n",oneChar);

			comMsg2[comMsgCount2] = oneChar;
//         iprintf("Read an %c...comMsgCount=%d\n",oneChar,comMsgCount2);
			if (comMsgCount2 == 0) {
				if (oneChar == 'r' || oneChar == 'c') {
					comMsgCount2++;
				} else {
					badMsgCount2++;
				}
			} else if (comMsgCount2 == 1) {
				comMsgCount2++;
				if (comMsg2[0] == 'r') {
					switch (oneChar) {
					case 's':
						comMsgLength2 = 4;
						comMsgType2 = EXT_MSG_TYPE_RS;
						break;
					case 'x':
					case '9':
						comMsgLength2 = 12;
						comMsgType2 = EXT_MSG_TYPE_RX;
						break;
					case 'y':
						comMsgLength2 = 5;
						comMsgType2 = EXT_MSG_TYPE_RY;
						break;
					case 'p':
						comMsgLength2 = 9;
						comMsgType2 = EXT_MSG_TYPE_RP;
						break;
					case 'q':
						comMsgLength2 = 4;
						comMsgType2 = EXT_MSG_TYPE_RQ;
						break;
					default:
						comMsgCount2 = 0;
						badMsgCount2++;
						break;
					}
				} else if (comMsg2[0] == 'c') {
					switch (oneChar) {
					case 's':
						comMsgLength2 = 9;
						comMsgType2 = EXT_MSG_TYPE_CS;
						break;
					case 'd':
						comMsgLength2 = 4;
						comMsgType2 = EXT_MSG_TYPE_CD;
						break;
					case 'i':
						comMsgLength2 = 13;
						comMsgType2 = EXT_MSG_TYPE_CI;
						break;
					case 'w':
						comMsgLength2 = 13;
						comMsgType2 = EXT_MSG_TYPE_CW;
						break;
					case 't':
						comMsgLength2 = 11;
						comMsgType2 = EXT_MSG_TYPE_CT;
						break;
					case 'y':
						comMsgLength2 = 8;
						comMsgType2 = EXT_MSG_TYPE_CY;
						break;
					case 'g':
						comMsgLength2 = 4;
						comMsgType2 = EXT_MSG_TYPE_CG;
						break;
					case 'p':
						comMsgLength2 = 4;
						comMsgType2 = EXT_MSG_TYPE_CP;
						break;
					case 'q':
						comMsgLength2 = 5;
						comMsgType2 = EXT_MSG_TYPE_CQ;
						break;
					case 'h':
						comMsgLength2 = 4;
						comMsgType2 = EXT_MSG_TYPE_CH;
						break;
					case 'j':
						comMsgLength2 = 12;
						comMsgType2 = EXT_MSG_TYPE_CJ;
						break;
					case 'k':
						comMsgLength2 = 4;
						comMsgType2 = EXT_MSG_TYPE_CK;
						break;
					case 'l':
						comMsgLength2 = 7;
						comMsgType2 = EXT_MSG_TYPE_CL;
						break;
					case 'm':
						comMsgLength2 = 4;
						comMsgType2 = EXT_MSG_TYPE_CM;
						break;
					case 'n':
						comMsgLength2 = 7;
						comMsgType2 = EXT_MSG_TYPE_CN;
						break;
					case 'x':
						comMsgLength2 = 5;
						comMsgType2 = EXT_MSG_TYPE_CX;
						break;
					default:
						comMsgCount2 = 0;
						badMsgCount2++;
						break;
					}
				}
			} else {
				comMsgCount2++;
				if (comMsgCount2 >= comMsgLength2) {
					comMsgCount2 = 0;
//               iprintf("found message %d...checking CRC\n",comMsgType);
					long unsigned int lCRC_value = Use_Crc(comMsg2,
							comMsgLength2 - 2);
					oneByte1 = (unsigned char) ((lCRC_value >> 8) & 0x00ff);
					oneByte2 = (unsigned char) (lCRC_value & 0x00ff);
//               iprintf("comparing first CRC byte %d to %d\n",oneByte1,comMsg[comMsgLength-2]);
//               iprintf("comparing second CRC byte %d to %d\n",oneByte2,comMsg[comMsgLength-1]);
					if (oneByte1 == comMsg2[comMsgLength2 - 2]
							&& oneByte2 == comMsg2[comMsgLength2 - 1]) {
//                   iprintf("message good...processing\n");
						dataReceivedFlag2 = 1;
						processComMsg2();
					}
				}
			}
		}
	}
}

extern int dataOnSerialPortFlag;
extern long int dataOnSerialPortTime;

void processComMsg2() {
	int nwr;
	nwr = write(FD_SerialData, (char *) comMsg2, comMsgLength2);
	dataOnSerialPortFlag = 1;
	dataOnSerialPortTime = GetPreciseTime();
	toggleLine17();
}

/*
 *
 ///==============================================================================
 // Example function that can be called via AJAX. This one just writes out the
 // the current timetick value but of course this could be much more elaborate.
 //==============================================================================
 void AjaxCallback(int sock, const char* url)
 {
 char java_script[100];
 sprintf(java_script, " System Tick count:%u", TimeTick);
 writestring(sock, java_script );
 }

 */

