#pragma once

#include "protocol.h"
#include "user.h"

#define MAX_DEATH_DESCRIPTIONS 200

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

struct DEATH_SYSTEM_INVENTORY_INFO
{
	BYTE slot;
	DWORD kills;
	DWORD serial;
};

struct DEATH_SYSTEM_WAREHOUSE_RECV
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

struct PMSG_DEATH_SYSTEM_SEND
{
	PSWMSG_HEAD h;
	BYTE count;
	BYTE Type;
};

struct PMSG_DEATH_SYSTEM_INFO
{
	WORD ItemIndex;
	BYTE Slot;
	BYTE Top;
	DWORD kills;
};

struct DEATH_DESCRIPTIONS_INFO
{
	int Index;
	WORD ItemIndex;
};

struct DEATH_SYSTEM_UPDATE_KILL_SEND
{
	PSWMSG_HEAD header;

	DWORD serial;
	DWORD kills;
	BYTE Type;
};

struct DEATH_SYSTEM_TOP_KILL_RECV
{
	PSWMSG_HEAD header;

	DWORD serial;
	DWORD kills;
};

class CDeathSystem
{
public:

	CDeathSystem();
	virtual ~CDeathSystem();

	void Clear();
	void Load(char * path);
	void SetInfo(DEATH_DESCRIPTIONS_INFO lpInfo, int index);
	bool CheckIsItem(int ItemIndex);

	void InventoryItemCountRecv(DEATH_SYSTEM_INVENTORY_RECV * lpMsg);
	void WarehouseItemCountRecv(DEATH_SYSTEM_WAREHOUSE_RECV * lpMsg);
	void ChaosBoxGenesisItemCountRecv(DEATH_SYSTEM_WAREHOUSE_RECV * lpMsg);
	void TradeXItemCountRecv(DEATH_SYSTEM_WAREHOUSE_RECV * lpMsg);
	void DGReceiveTopSerial(DEATH_SYSTEM_TOP_KILL_RECV* lpMsg);

	void GDInventoryGetInformation(int aIndex);
	void GDWarehouseGetInformation(int aIndex);
	void GDChaosBoxGenesisGetInformation(int aIndex);
	void GDTradeXGetInformation(int aIndex);

	void SendInformation(LPOBJ lpObj, int Type);

	void SendInventory(LPOBJ lpObj);
	void SendWarehouse(LPOBJ lpObj);
	void SendTrade(LPOBJ lpObj);
	void SendTradeTarget(LPOBJ lpObj);
	void SendTradeTargetX(LPOBJ lpObj);
	void SendChaosBox(LPOBJ lpObj);
	void SendChaosBoxGenesis(LPOBJ lpObj);
	void SendTradeX(LPOBJ lpObj);
	void SendPlayerShop(int aIndex, int bIndex);

	void DeathIncrease(int aIndex, int Slot, int Kills);
	void DeathDecrease(int aIndex, int Slot, int Kills);
	int GetDeath(int aIndex, int Slot);

private:
	int m_LoadDeathDescriptions;
	DEATH_DESCRIPTIONS_INFO m_DeathDescriptionsInfo[MAX_DEATH_DESCRIPTIONS];
};

extern CDeathSystem gDeathSystem;