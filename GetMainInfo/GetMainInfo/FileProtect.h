#pragma once

static const BYTE gFileProtectHeader[8] = { 0x68, 0xA2, 0xD2, 0x20, 0xA4, 0x43, 0x41, 0xDE };
static const BYTE gFileProtectXorTable[16] = { 0x13, 0xA4, 0x85, 0x1F, 0x1E, 0x19, 0x6F, 0xBE, 0x8A, 0x22, 0x29, 0xDE, 0x03, 0x6E, 0x99, 0xB7 };

class CFileProtect
{
public:
	CFileProtect();
	virtual ~CFileProtect();
	bool LoadFile(char* path);
	bool EncryptFile(char* path, char * n_path);
	void FindFile(const char * path, const char * n_path);

private:
	char* m_buff;
	DWORD m_size;
	char m_path[256];

public:
	char m_IpServer[17];
	char m_PrivateCode[125];
};

extern CFileProtect gFileProtect;