#include "stdafx.h"
#include "Util.h"
#include "GameMain.h"
#include "HackCheck.h"
#include "ItemManager.h"
#include "ServerInfo.h"
#include "SocketManager.h"
#include "ThemidaSDK.h"
#include "Viewport.h"
#include "Log.h"

std::mt19937 seed;
std::uniform_int_distribution<int> dist;
short RoadPathTable[MAX_ROAD_PATH_TABLE] = {-1,-1,0,-1,1,-1,1,0,1,1,0,1,-1,1,-1,0};

int SafeGetItem(int index) // OK
{
	return CHECK_ITEM(index);
}

void CheckComputerHardwareId() // OK
{
#if(PROTECT_STATE==1)

	CLEAR_START

	VM_START

	DWORD VolumeSerialNumber;

	if (GetVolumeInformation("C:\\", 0, 0, &VolumeSerialNumber, 0, 0, 0, 0) == 0)
	{
		while (true)
		{
			TerminateProcess(GetCurrentProcess(), 0);
		}
	}

	UUID uuid;

	UuidCreateSequential(&uuid);

	SYSTEM_INFO SystemInfo;

	GetSystemInfo(&SystemInfo);

	DWORD ComputerHardwareId1 = VolumeSerialNumber ^ 0x12B586FE;

	DWORD ComputerHardwareId2 = *(DWORD*)(&uuid.Data4[2]) ^ 0x5D78A569;

	DWORD ComputerHardwareId3 = ((*(WORD*)(&uuid.Data4[6]) & 0xFFFF) | (SystemInfo.wProcessorArchitecture << 16)) ^ 0xF45BC123;

	DWORD ComputerHardwareId4 = ((SystemInfo.wProcessorLevel & 0xFFFF) | (SystemInfo.wProcessorRevision << 16)) ^ 0xB542D8E1;

	char ComputerHardwareId[35] = { 0 };

	wsprintf(ComputerHardwareId, "%08X-%08X-%08X-%08X", ComputerHardwareId1, ComputerHardwareId2, ComputerHardwareId3, ComputerHardwareId4);

	VM_END

	CLEAR_END

	if (strcmp(HARDWAREID, ComputerHardwareId) != 0)
	{
		while (true)
		{
			TerminateProcess(GetCurrentProcess(), 0);
		}
	}

#endif
}

flt GetRoundValue(float value) // OK
{
	float integral;

	if(modf(value,&integral) > 0.5f)
	{
		return ceil(value);
	}

	return floor(value);
}

BYTE GetNewOptionCount(BYTE NewOption) // OK
{
	BYTE count = 0;

	for(int n=0;n < MAX_EXC_OPTION;n++)
	{
		if((NewOption & (1 << n)) != 0)
		{
			count++;
		}
	}

	return count;
}

BYTE GetSocketOptionCount(BYTE SocketOption[5]) // OK
{
	BYTE count = 0;

	for(int n=0;n < MAX_SOCKET_OPTION;n++)
	{
		if(SocketOption[n] != 0xFF)
		{
			count++;
		}
	}

	return count;
}

BYTE GetPathPacketDirPos(int px,int py) // OK
{
	if(px <= -1 && py <= -1)
	{
		return 0;
	}
	else if(px <= -1 && py == 0)
	{
		return 7;
	}
	else if(px <= -1 && py >= 1)
	{
		return 6;
	}
	else if(px == 0 && py <= -1)
	{
		return 1;
	}
	else if(px == 0 && py >= 1)
	{
		return 5;
	}
	else if(px >= 1 && py <= -1)
	{
		return 2;
	}
	else if(px >= 1 && py == 0)
	{
		return 3;
	}
	else if(px >= 1 && py >= 1)
	{
		return 4;
	}

	return 0;
}

void PacketArgumentDecrypt(char* out_buff,char* in_buff,int size) // OK
{
	BYTE XorTable[3] = {0xFC,0xCF,0xAB};

	for(int n=0;n < size;n++)
	{
		out_buff[n] = in_buff[n]^XorTable[n%3];
	}
}

void ErrorMessageBox(char* message,...) // OK
{
	VM_START

	char buff[256];

	memset(buff,0,sizeof(buff));

	va_list arg;
	va_start(arg,message);
	vsprintf_s(buff,message,arg);
	va_end(arg);

	MessageBox(0,buff,"Error",MB_OK | MB_ICONERROR);

	VM_END

	ExitProcess(0);
}

void LogAdd(eLogColor color,char* text,...) // OK
{
	tm today;
	time_t ltime;
	time(&ltime);

	if(localtime_s(&today,&ltime) != 0)
	{
		return;
	}

	char time[32];

	if(asctime_s(time,sizeof(time),&today) != 0)
	{
		return;
	}

	char temp[1024];

	va_list arg;
	va_start(arg,text);
	vsprintf_s(temp,text,arg);
	va_end(arg);

	char log[1024];

	wsprintf(log,"%.8s %s",&time[11],temp);

	gServerDisplayer.LogAddText(color,log,strlen(log));
}

bool DataSend(int aIndex,BYTE* lpMsg,DWORD size) // OK
{
	return gSocketManager.DataSend(aIndex,lpMsg,size);
}

void DataSendAll(BYTE* lpMsg,int size) // OK
{
	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnected(n) != 0)
		{
			DataSend(n,lpMsg,size);
		}
	}
}

bool DataSendSocket(SOCKET socket,BYTE* lpMsg,DWORD size) // OK
{
	if(socket == INVALID_SOCKET)
	{
		return 0;
	}

	#if(ENCRYPT_STATE==1)

	EncryptData(lpMsg,size);

	#endif

	int count=0,result=0;

	while(size > 0)
	{
		if((result=send(socket,(char*)&lpMsg[count],size,0)) == SOCKET_ERROR)
		{
			if(WSAGetLastError() != WSAEWOULDBLOCK)
			{
				return 0;
			}
		}
		else
		{
			count += result;
			size -= result;
		}
	}

	return 1;
}

void MsgSendV2(LPOBJ lpObj,BYTE* lpMsg,int size) // OK
{
	for(int n=0;n < MAX_VIEWPORT;n++)
	{
		if(lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_USER)
		{
			DataSend(lpObj->VpPlayer2[n].index,lpMsg,size);
		}
	}
}

void CloseClient(int aIndex) // OK
{
	gSocketManager.Disconnect(aIndex);
}

void PostMessage1(char* name,char* message,char* text) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	char buff[256] = {0};

	wsprintf(buff,message,name,text);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n],buff);
		}
	}

	#else

	char buff[256] = {0};

	wsprintf(buff,message,text);

	int size = strlen(buff);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_WHISPER_SEND pMsg;

	pMsg.header.set(0x02,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}

	#endif
}

void PostMessage2(char* name,char* message,char* text) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	char buff[256] = {0};

	wsprintf(buff,message,name,text);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n],buff);
		}
	}

	#else

	char buff[256] = {'~'};

	wsprintf(&buff[1],message,text);

	int size = strlen(buff);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_SEND pMsg;

	pMsg.header.set(0x00,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}

	#endif
}

void PostMessage3(char* name,char* message,char* text) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	char buff[256] = {0};

	wsprintf(buff,message,name,text);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n],buff);
		}
	}

	#else

	char buff[256] = {'@'};

	wsprintf(&buff[1],message,text);

	int size = strlen(buff);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_SEND pMsg;

	pMsg.header.set(0x00,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}

	#endif
}

void PostMessage4(char* name,char* message,char* text) // OK
{
	#if(GAMESERVER_UPDATE>=701)

	char buff[256] = {0};

	wsprintf(buff,message,name,text);

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			GCNewMessageSend(&gObj[n],buff);
		}
	}

	#else

	char buff[256] = {'$'};

	wsprintf(&buff[1],message,text);

	int size = strlen(buff);

	size = ((size>MAX_CHAT_MESSAGE_SIZE)?MAX_CHAT_MESSAGE_SIZE:size);

	PMSG_CHAT_SEND pMsg;

	pMsg.header.set(0x00,(sizeof(pMsg)-(sizeof(pMsg.message)-(size+1))));

	memcpy(pMsg.name,name,sizeof(pMsg.name));

	memcpy(pMsg.message,buff,size);

	pMsg.message[size] = 0;

	for(int n=OBJECT_START_USER;n < MAX_OBJECT;n++)
	{
		if(gObjIsConnectedGP(n) != 0)
		{
			DataSend(n,(BYTE*)&pMsg,pMsg.header.size);
		}
	}

	#endif
}

void SetLargeRand() // OK
{
	seed = std::mt19937(std::random_device()());
	dist = std::uniform_int_distribution<int>(0,2147483647);
}

long GetLargeRand() // OK
{
	return dist(seed);
}


void ChatYellowMessage(int aIndex, char* String)
{
	BYTE* Packet;
	Packet = (BYTE*)malloc(200);
	memset(Packet, 0x00, 200);
	*Packet = 0xC1;
	*(Packet + 2) = 0x02;
	memcpy((Packet + 13), String, strlen(String));
	int Len = (strlen(String) + 0x13);
	*(Packet + 1) = Len;
	DataSend(aIndex, Packet, Len);
	free(Packet);
}

void ChatYellowMessageAll(char* String)
{
	BYTE* Packet;
	Packet = (BYTE*)malloc(200);
	memset(Packet, 0x00, 200);
	*Packet = 0xC1;
	*(Packet + 2) = 0x02;
	memcpy((Packet + 13), String, strlen(String));
	int Len = (strlen(String) + 0x13);
	*(Packet + 1) = Len;
	DataSendAll(Packet, Len);
	free(Packet);
}

void ChatGreenMessage(int aIndex, char* String)
{
	BYTE* Packet;
	Packet = (BYTE*)malloc(1024);
	memset(Packet, 0x00, 1024);
	*Packet = 0xC1;
	*(Packet + 2) = 0x00;
	*(Packet + 13) = 0x40;
	memcpy((Packet + 14), String, strlen(String));
	int Len = (strlen(String) + 0x14);
	*(Packet + 1) = Len;
	DataSend(aIndex, Packet, Len);
	free(Packet);
}

void ChatGreenMessageAll(char* String)
{
	BYTE* Packet;
	Packet = (BYTE*)malloc(1024);
	memset(Packet, 0x00, 1024);
	*Packet = 0xC1;
	*(Packet + 2) = 0x00;
	*(Packet + 13) = 0x40;
	memcpy((Packet + 14), String, strlen(String));
	int Len = (strlen(String) + 0x14);
	*(Packet + 1) = Len;
	DataSendAll(Packet, Len);
	free(Packet);
}

void ChatBlueMessage(int aIndex, char* String)
{
	BYTE* Packet;
	Packet = (BYTE*)malloc(1024);
	memset(Packet, 0x00, 1024);
	*Packet = 0xC1;
	*(Packet + 2) = 0x00;
	*(Packet + 13) = 0x7E;
	memcpy((Packet + 14), String, strlen(String));
	int Len = (strlen(String) + 0x14);
	*(Packet + 1) = Len;
	DataSend(aIndex, Packet, Len);
	free(Packet);
}

void ChatBlueMessageAll(char* String)
{
	BYTE* Packet;
	Packet = (BYTE*)malloc(1024);
	memset(Packet, 0x00, 1024);
	*Packet = 0xC1;
	*(Packet + 2) = 0x00;
	*(Packet + 13) = 0x7E;
	memcpy((Packet + 14), String, strlen(String));
	int Len = (strlen(String) + 0x14);
	*(Packet + 1) = Len;
	DataSendAll(Packet, Len);
	free(Packet);
}

void ChatGoldMessage(int aIndex, char* String)
{
	BYTE* Packet;
	Packet = (BYTE*)malloc(1024);
	memset(Packet, 0x00, 1024);
	*Packet = 0xC1;
	*(Packet + 2) = 0x00;
	*(Packet + 13) = 0x40;
	*(Packet + 14) = 0x40;
	memcpy((Packet + 15), String, strlen(String));
	int Len = (strlen(String) + 0x15);
	*(Packet + 1) = Len;
	DataSend(aIndex, Packet, Len);
	free(Packet);
}

void ChatGoldMessageAll(char* String)
{
	BYTE* Packet;
	Packet = (BYTE*)malloc(1024);
	memset(Packet, 0x00, 1024);
	*Packet = 0xC1;
	*(Packet + 2) = 0x00;
	*(Packet + 13) = 0x40;
	*(Packet + 14) = 0x40;
	memcpy((Packet + 15), String, strlen(String));
	int Len = (strlen(String) + 0x15);
	*(Packet + 1) = Len;
	DataSendAll(Packet, Len);
	free(Packet);
}

void LogAddLua(eLogColor color, char* text, ...)
{
	tm today;
	time_t ltime;
	time(&ltime);

	if (localtime_s(&today, &ltime) != 0)
	{
		return;
	}

	char time[32];

	if (asctime_s(time, sizeof(time), &today) != 0)
	{
		return;
	}

	char temp[1024];

	va_list arg;
	va_start(arg, text);
	vsprintf_s(temp, text, arg);
	va_end(arg);

	gLog.Output(LOG_LUA, temp);
}