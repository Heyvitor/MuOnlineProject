#include "stdafx.h"
#include "CCRC32.h"
#include "ThemidaSDK.h"
#include "CustomJewel.h"
#include "LuaProtect.h"
#include "FileProtect.h"

#define MACRO_CONFIG_TXT ".\\MainInfo.ini"
#define MCRO_DEFINE_CONFIG "MainInfo"

void GerateConfig();
void GerateCustomJewels();
void EncryptFiles();
void EncryptLua();

static BYTE bBuxCode[3] = { 0xF2, 0x95, 0x54 };

struct MAIN_FILE_INFO
{
	char CustomerName[32];
	char IpAddress[32];
	WORD IpAddressPort;
	char ClientVersion[8];
	char ClientSerial[17];
	char WindowName[32];
	char ScreenShotPath[50];
	int CharListS13;
	char m_PrivateCode[125];
	BYTE LuaCrypt;

};

struct LOAD_CUSTOM_JEWEL_INFO
{
	CUSTOM_JEWEL_INFO CustomJewelInfo[MAX_CUSTOM_JEWEL];
};

int _tmain(int argc, _TCHAR* argv[])
{
	printf("What do you want to do?: \n");

	printf("1 - Generate Config \n");

	printf("2 - Generate Custom Jewels \n");

	printf("3 - Encrypt Files \n");

	printf("4 - Encrypt Lua \n");

	printf("5 - Generate All \n");

	int execute = 0;

	scanf_s("%i", &execute);

	if (execute == 1)
	{
		GerateConfig();
	}
	else if (execute == 2)
	{
		GerateCustomJewels();
	}
	else if (execute == 3)
	{
		EncryptFiles();
	}
	else if (execute == 4)
	{
		EncryptLua();
	}	
	else if (execute == 5)
	{
		GerateConfig();

		GerateCustomJewels();
	}

	printf("Patch Created! \n");

	system("pause");
}


void GerateConfig()// OK
{
	CLEAR_START

	ENCODE_START

	MAIN_FILE_INFO info;

	memset(&info, 0, sizeof(info));

	info.IpAddressPort = GetPrivateProfileInt(MCRO_DEFINE_CONFIG, "IpAddressPort", 44405, MACRO_CONFIG_TXT);

	GetPrivateProfileString(MCRO_DEFINE_CONFIG, "CustomerName", "", info.CustomerName, sizeof(info.CustomerName), MACRO_CONFIG_TXT);

	GetPrivateProfileString(MCRO_DEFINE_CONFIG, "IpAddress", "", info.IpAddress, sizeof(info.IpAddress), MACRO_CONFIG_TXT);

	GetPrivateProfileString(MCRO_DEFINE_CONFIG, "ClientVersion", "", info.ClientVersion, sizeof(info.ClientVersion), MACRO_CONFIG_TXT);

	GetPrivateProfileString(MCRO_DEFINE_CONFIG, "ClientSerial", "", info.ClientSerial, sizeof(info.ClientSerial), MACRO_CONFIG_TXT);

	GetPrivateProfileString(MCRO_DEFINE_CONFIG, "WindowName", "", info.WindowName, sizeof(info.WindowName), MACRO_CONFIG_TXT);

	GetPrivateProfileString(MCRO_DEFINE_CONFIG, "ScreenShotPath", "", info.ScreenShotPath, sizeof(info.ScreenShotPath), MACRO_CONFIG_TXT);

	info.CharListS13 = GetPrivateProfileInt(MCRO_DEFINE_CONFIG, "CharListSeason13", 0, MACRO_CONFIG_TXT);

	GetPrivateProfileString(MCRO_DEFINE_CONFIG, "PrivateCode", "", info.m_PrivateCode, sizeof(info.m_PrivateCode), MACRO_CONFIG_TXT);

	info.LuaCrypt = GetPrivateProfileInt(MCRO_DEFINE_CONFIG, "OnlyCryptedLua", 1, MACRO_CONFIG_TXT);

	for(int n=0;n < sizeof(MAIN_FILE_INFO);n++)
	{
		((BYTE*)&info)[n] ^= bBuxCode[n % 3];
	}

	if (DirectoryExists(".\\Data\\Configs\\"))
	{

	}
	else
	{
		system("mkdir \"Data\\Configs\\\"");
	}

	DeleteFile("Data\\Configs\\Configs.xtm");

	HANDLE file = CreateFile(STRING_CRIPT("Data\\Configs\\Configs.xtm"), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0);


	if(file == INVALID_HANDLE_VALUE)
	{
		return;
	}

	DWORD OutSize = 0;

	if (WriteFile(file, &info, sizeof(MAIN_FILE_INFO), &OutSize, 0) == 0)
	{
		CloseHandle(file);
		return;
	}

	CloseHandle(file);

	ENCODE_END

	CLEAR_END
}


void GerateCustomJewels()
{
	LOAD_CUSTOM_JEWEL_INFO info;

	memset(&info, 0, sizeof(info));

	gCustomJewel.Load("CustomJewel.txt");

	memcpy(info.CustomJewelInfo, gCustomJewel.m_CustomJewelInfo, sizeof(info.CustomJewelInfo));

	for (int n = 0; n < sizeof(LOAD_CUSTOM_JEWEL_INFO); n++)
	{
		((BYTE*)&info)[n] ^= (BYTE)(0xCA ^ LOBYTE(n));
		((BYTE*)&info)[n] -= (BYTE)(0xFC ^ HIBYTE(n));
	}

	if (DirectoryExists(".\\Data\\Configs\\"))
	{

	}
	else
	{
		system("mkdir \"Data\\Configs\\\"");
	}

	DeleteFile("Data\\Configs\\CustomJewel.xtm");

	HANDLE file = CreateFile(".\\Data\\Configs\\CustomJewel.xtm", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, 0);

	if (file == INVALID_HANDLE_VALUE)
	{
		return;
	}

	DWORD OutSize = 0;

	if (WriteFile(file, &info, sizeof(LOAD_CUSTOM_JEWEL_INFO), &OutSize, 0) == 0)
	{
		CloseHandle(file);
		return;
	}

	CloseHandle(file);
}

void EncryptLua() {
	GetPrivateProfileString(MCRO_DEFINE_CONFIG, "PrivateCode", "", gFileProtectLua.m_PrivateCode, sizeof(gFileProtectLua.m_PrivateCode), MACRO_CONFIG_TXT);

	Sleep(20);

	gFileProtectLua.FindFile("LuaScripts\\Decrypted\\", "LuaScripts\\Encrypted\\");
}

void EncryptFiles() {
	GetPrivateProfileString(MCRO_DEFINE_CONFIG, "PrivateCode", "", gFileProtect.m_PrivateCode, sizeof(gFileProtect.m_PrivateCode), MACRO_CONFIG_TXT);

	Sleep(20);

	gFileProtect.FindFile("Files\\Decrypted\\", "Files\\Encrypted\\");
}