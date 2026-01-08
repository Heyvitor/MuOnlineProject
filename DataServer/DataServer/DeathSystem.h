#pragma once

#include "DataServerProtocol.h"

struct DEATH_SYSTEM_INVENTORY_RECV
{
	PSWMSG_HEAD header;
	WORD index;
	char account[11];
	BYTE count;
};

struct DEATH_SYSTEM_INVENTORY_DS_INFO
{
	BYTE slot;
	DWORD serial;
};

struct DEATH_SYSTEM_INVENTORY_SEND
{
	PSWMSG_HEAD header;
	WORD index;
	char account[11];
	BYTE count;
};

struct DEATH_SYSTEM_INVENTORY_INFO
{
	BYTE slot;
	DWORD kills;
	DWORD serial;
};

struct DEATH_SYSTEM_WAREHOUSE_SEND
{
	PSWMSG_HEAD header;
	WORD index;
	char account[11];
	BYTE count;
};

struct DEATH_SYSTEM_WAREHOUSE_INFO
{
	BYTE slot;
	DWORD kills;
	DWORD serial;
};

struct DEATH_SYSTEM_UPDATE_KILL_RECV
{
	PSWMSG_HEAD header;

	DWORD serial;
	DWORD kills;
	BYTE Type;
};

struct DEATH_SYSTEM_TOP_KILL_SEND
{
	PSWMSG_HEAD header;
	DWORD serial;
	DWORD mortes;
};

class CDeathSystem
{
public:
	CDeathSystem();
	virtual ~CDeathSystem();

	void GDInventoryGetItemCount(DEATH_SYSTEM_INVENTORY_RECV * lpMsg, int index);
	void GDWarehouseGetItemCount(DEATH_SYSTEM_INVENTORY_RECV * lpMsg, int index);
	void GDChaosBoxGenesisGetItemCount(DEATH_SYSTEM_INVENTORY_RECV * lpMsg, int index);
	void GDTradeXGetItemCount(DEATH_SYSTEM_INVENTORY_RECV * lpMsg, int index);
	void DGSendTopKillSerial(int index);
	void UpdateKillsSword(DEATH_SYSTEM_UPDATE_KILL_RECV * lpMsg, int index);
};

extern CDeathSystem gDeathSystem;