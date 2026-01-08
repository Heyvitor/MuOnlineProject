#include "stdafx.h"
#include "DeathSystem.h"
#include "QueryManager.h"
#include "SocketManager.h"
#include "Util.h"

CDeathSystem gDeathSystem;

CDeathSystem::CDeathSystem()
{

}

CDeathSystem::~CDeathSystem()
{

}

void CDeathSystem::GDInventoryGetItemCount(DEATH_SYSTEM_INVENTORY_RECV * lpMsg, int index)
{
	this->DGSendTopKillSerial(index);

	BYTE send[6096];

	DEATH_SYSTEM_INVENTORY_SEND pMsg;

	pMsg.header.set(0x18, 0x40, 0);

	int size = sizeof(pMsg);

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account, lpMsg->account, sizeof(pMsg.account));

	pMsg.count = 0;

	DEATH_SYSTEM_INVENTORY_INFO info;

	for (int n = 0; n < lpMsg->count; n++)
	{
		DEATH_SYSTEM_INVENTORY_DS_INFO* lpInfo = (DEATH_SYSTEM_INVENTORY_DS_INFO*)(((BYTE*)lpMsg) + sizeof(DEATH_SYSTEM_INVENTORY_RECV) + (sizeof(DEATH_SYSTEM_INVENTORY_DS_INFO)*n));

		if (gQueryManager.ExecQuery("SELECT Deaths FROM DeathSystem WHERE m_Serial=%d", lpInfo->serial) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
		{
			gQueryManager.Close();

			gQueryManager.ExecQuery("INSERT INTO DeathSystem ([m_Serial] ,[Deaths]) VALUES (%d,0)", lpInfo->serial, 0);
			gQueryManager.Close();

			info.slot = lpInfo->slot;
			info.serial = lpInfo->serial;
			info.kills = 0;
		}
		else
		{
			info.slot = lpInfo->slot;
			info.serial = lpInfo->serial;
			info.kills = (DWORD)gQueryManager.GetAsInteger("Deaths");

			gQueryManager.Close();
		}

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	gSocketManager.DataSend(index, send, size);
}

void CDeathSystem::GDWarehouseGetItemCount(DEATH_SYSTEM_INVENTORY_RECV * lpMsg, int index)
{
	this->DGSendTopKillSerial(index);

	BYTE send[6096];

	DEATH_SYSTEM_WAREHOUSE_SEND pMsg;

	pMsg.header.set(0x18, 0x41, 0);

	int size = sizeof(pMsg);

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account, lpMsg->account, sizeof(pMsg.account));

	pMsg.count = 0;

	DEATH_SYSTEM_WAREHOUSE_INFO info;

	for (int n = 0; n < lpMsg->count; n++)
	{
		DEATH_SYSTEM_INVENTORY_DS_INFO* lpInfo = (DEATH_SYSTEM_INVENTORY_DS_INFO*)(((BYTE*)lpMsg) + sizeof(DEATH_SYSTEM_INVENTORY_RECV) + (sizeof(DEATH_SYSTEM_INVENTORY_DS_INFO)*n));

		if (gQueryManager.ExecQuery("SELECT Deaths FROM DeathSystem WHERE m_Serial=%d", lpInfo->serial) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
		{
			gQueryManager.Close();

			gQueryManager.ExecQuery("INSERT INTO DeathSystem ([m_Serial] ,[Deaths]) VALUES (%d,0)", lpInfo->serial, 0);
			gQueryManager.Close();

			info.slot = lpInfo->slot;
			info.serial = lpInfo->serial;
			info.kills = 0;
		}
		else
		{
			info.slot = lpInfo->slot;
			info.serial = lpInfo->serial;
			info.kills = (DWORD)gQueryManager.GetAsInteger("Deaths");

			gQueryManager.Close();
		}

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	gSocketManager.DataSend(index, send, size);
}

void CDeathSystem::GDChaosBoxGenesisGetItemCount(DEATH_SYSTEM_INVENTORY_RECV * lpMsg, int index)
{
	this->DGSendTopKillSerial(index);

	BYTE send[6096];

	DEATH_SYSTEM_WAREHOUSE_SEND pMsg;

	pMsg.header.set(0x18, 0x43, 0);

	int size = sizeof(pMsg);

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account, lpMsg->account, sizeof(pMsg.account));

	pMsg.count = 0;

	DEATH_SYSTEM_WAREHOUSE_INFO info;

	for (int n = 0; n < lpMsg->count; n++)
	{
		DEATH_SYSTEM_INVENTORY_DS_INFO* lpInfo = (DEATH_SYSTEM_INVENTORY_DS_INFO*)(((BYTE*)lpMsg) + sizeof(DEATH_SYSTEM_INVENTORY_RECV) + (sizeof(DEATH_SYSTEM_INVENTORY_DS_INFO)*n));

		if (gQueryManager.ExecQuery("SELECT Deaths FROM DeathSystem WHERE m_Serial=%d", lpInfo->serial) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
		{
			gQueryManager.Close();

			gQueryManager.ExecQuery("INSERT INTO DeathSystem ([m_Serial] ,[Deaths]) VALUES (%d,0)", lpInfo->serial, 0);
			gQueryManager.Close();

			info.slot = lpInfo->slot;
			info.serial = lpInfo->serial;
			info.kills = 0;
		}
		else
		{
			info.slot = lpInfo->slot;
			info.serial = lpInfo->serial;
			info.kills = (DWORD)gQueryManager.GetAsInteger("Deaths");

			gQueryManager.Close();
		}

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	gSocketManager.DataSend(index, send, size);
}

void CDeathSystem::GDTradeXGetItemCount(DEATH_SYSTEM_INVENTORY_RECV * lpMsg, int index)
{
	this->DGSendTopKillSerial(index);

	BYTE send[6096];

	DEATH_SYSTEM_WAREHOUSE_SEND pMsg;

	pMsg.header.set(0x18, 0x44, 0);

	int size = sizeof(pMsg);

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account, lpMsg->account, sizeof(pMsg.account));

	pMsg.count = 0;

	DEATH_SYSTEM_WAREHOUSE_INFO info;

	for (int n = 0; n < lpMsg->count; n++)
	{
		DEATH_SYSTEM_INVENTORY_DS_INFO* lpInfo = (DEATH_SYSTEM_INVENTORY_DS_INFO*)(((BYTE*)lpMsg) + sizeof(DEATH_SYSTEM_INVENTORY_RECV) + (sizeof(DEATH_SYSTEM_INVENTORY_DS_INFO)*n));

		if (gQueryManager.ExecQuery("SELECT Deaths FROM DeathSystem WHERE m_Serial=%d", lpInfo->serial) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
		{
			gQueryManager.Close();

			gQueryManager.ExecQuery("INSERT INTO DeathSystem ([m_Serial] ,[Deaths]) VALUES (%d,0)", lpInfo->serial, 0);
			gQueryManager.Close();

			info.slot = lpInfo->slot;
			info.serial = lpInfo->serial;
			info.kills = 0;
		}
		else
		{
			info.slot = lpInfo->slot;
			info.serial = lpInfo->serial;
			info.kills = (DWORD)gQueryManager.GetAsInteger("Deaths");

			gQueryManager.Close();
		}

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	gSocketManager.DataSend(index, send, size);
}

void CDeathSystem::UpdateKillsSword(DEATH_SYSTEM_UPDATE_KILL_RECV * lpMsg, int index)
{	
	gQueryManager.ExecQuery("UPDATE DeathSystem SET Deaths = %d where m_Serial = '%d'", lpMsg->kills, lpMsg->serial);
	gQueryManager.Close();
	this->DGSendTopKillSerial(index);
}

void CDeathSystem::DGSendTopKillSerial(int index)
{
	
	DEATH_SYSTEM_TOP_KILL_SEND pMsg;

	pMsg.header.set(0x18, 0x45, 0);

	int size = sizeof(pMsg);

	int topserial = 1;

	int mortes = 0;

	if (gQueryManager.ExecQuery("SELECT TOP 1 m_Serial, Deaths from DeathSystem ORDER BY Deaths desc, m_Serial ASC") == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();
	}
	else
	{ 
		topserial = gQueryManager.GetAsInteger("m_Serial");
		mortes = gQueryManager.GetAsInteger("Deaths");
		gQueryManager.Close();
	}


	pMsg.serial = (DWORD)topserial;
	pMsg.mortes = (DWORD)mortes;

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	gSocketManager.DataSend(index, (BYTE*)&pMsg, size);
}
