#include "stdafx.h"
#include "FileProtect.h"

CFileProtect gFileProtect;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileProtect::CFileProtect() // OK
{
	this->m_buff = 0;
	this->m_size = 0;
	memset(this->m_path,0,sizeof(this->m_path));
}

CFileProtect::~CFileProtect() // OK
{

}

bool CFileProtect::LoadFile(char* path) // OK
{
	HANDLE file = CreateFile(path,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_ARCHIVE,0);

	if(file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	this->m_size = GetFileSize(file,0);

	if(this->m_buff != 0)
	{
		delete[] this->m_buff;
		this->m_buff = 0;
	}

	this->m_buff = new char[this->m_size];

	if(this->m_buff == 0)
	{
		CloseHandle(file);
		return 0;
	}

	DWORD OutSize = 0;

	if(ReadFile(file,this->m_buff,this->m_size,&OutSize,0) == 0)
	{
		CloseHandle(file);
		return 0;
	}

	CloseHandle(file);
	return 1;
}

bool CFileProtect::EncryptFile(char* path, char * n_path) // OK
{
	if(this->LoadFile(path) == 0)
	{
		return 0;
	}

	if(this->m_size >= sizeof(gFileProtectHeader) && memcmp(this->m_buff,gFileProtectHeader,sizeof(gFileProtectHeader)) == 0)
	{
		return 0;
	}

	HANDLE file = CreateFile(n_path,GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_ARCHIVE,0);

	if(file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	char* buff = new char[this->m_size+sizeof(gFileProtectHeader)];

	memcpy(&buff[0],gFileProtectHeader,sizeof(gFileProtectHeader));

	memcpy(&buff[sizeof(gFileProtectHeader)],this->m_buff,this->m_size);

	for(int n=0;n < ((int)this->m_size);n++)
	{
		buff[sizeof(gFileProtectHeader) + n] ^= gFileProtectXorTable[n % sizeof(gFileProtectXorTable)];
		buff[sizeof(gFileProtectHeader) + n] += gFileProtectXorTable[this->m_PrivateCode[n%strlen(this->m_PrivateCode)] % sizeof(gFileProtectXorTable)];
	}

	DWORD OutSize = 0;

	if(WriteFile(file,buff,(this->m_size+sizeof(gFileProtectHeader)),&OutSize,0) == 0)
	{
		CloseHandle(file);
		return 0;
	}

	CloseHandle(file);
	return 1;
}


void CFileProtect::FindFile(const char * path,const char * n_path)
{
	char wildcard_path[256];

	wsprintf(wildcard_path,"%s*",path);

	WIN32_FIND_DATA data;

	HANDLE file = FindFirstFile(wildcard_path,&data);

	if(file == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do
	{
		if((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			char file_name[256];
			char n_file_name[256];
			wsprintf(file_name,"%s%s",path,data.cFileName);
			wsprintf(n_file_name,"%s%s",n_path,data.cFileName);

			printf("%s encrypted \n", n_file_name);

			this->EncryptFile(file_name, n_file_name);
			continue;
		}

		if(strcmp(data.cFileName,".") != 0 && strcmp(data.cFileName,"..") != 0)
		{
			char next_path[256];
			char n_next_path[256];
			wsprintf(next_path,"%s%s\\",path,data.cFileName);
			wsprintf(n_next_path,"%s%s\\",n_path,data.cFileName);
			this->FindFile(next_path, n_next_path);
			continue;
		}
	}
	while(FindNextFile(file,&data) != 0);
}