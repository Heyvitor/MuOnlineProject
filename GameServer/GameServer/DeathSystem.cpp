#include "StdAfx.h"
#include "DeathSystem.h"
#include "ItemManager.h"
#include "GameMain.h"
#include "Util.h"
#include "MemScript.h"
#include "ServerInfo.h"
#include "LuaGameServer.h"

CDeathSystem gDeathSystem;

CDeathSystem::CDeathSystem()
{
	this->Clear();
}

CDeathSystem::~CDeathSystem()
{

}

void CDeathSystem::Clear()
{
	for (int i = 0; i < MAX_DEATH_DESCRIPTIONS; i++)
	{
		this->m_DeathDescriptionsInfo[i].Index = -1;
		this->m_DeathDescriptionsInfo[i].ItemIndex = -1;
	}

	this->m_LoadDeathDescriptions = 0;
}

void CDeathSystem::Load(char * path)
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->Clear();
	this->m_LoadDeathDescriptions = 0;

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			int section = lpMemScript->GetNumber();

			while (true)
			{
				if (section == 0)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					DEATH_DESCRIPTIONS_INFO info;

					memset(&info, 0, sizeof(info));

					info.Index = this->m_LoadDeathDescriptions;

					info.ItemIndex = lpMemScript->GetNumber();

					lpMemScript->GetAsString();

					lpMemScript->GetAsNumber();

					this->SetInfo(info, this->m_LoadDeathDescriptions);

					this->m_LoadDeathDescriptions++;
				}
				else if (section == 1)
				{
					if (strcmp("end", lpMemScript->GetAsString()) == 0)
					{
						break;
					}

					lpMemScript->GetString();
				}
				else
				{
					break;
				}
			}
		}
	}
	catch (...)
	{
		printf(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

void CDeathSystem::SetInfo(DEATH_DESCRIPTIONS_INFO lpInfo, int index)
{
	if (index < 0 || index >= MAX_DEATH_DESCRIPTIONS)
	{
		return;
	}

	this->m_DeathDescriptionsInfo[index] = lpInfo;
}

bool CDeathSystem::CheckIsItem(int ItemIndex)
{
	for (int i = 0; i < this->m_LoadDeathDescriptions; i++)
	{
		if (this->m_DeathDescriptionsInfo[i].Index != -1)
		{
			if (this->m_DeathDescriptionsInfo[i].ItemIndex == ItemIndex)
			{
				return 1;
			}
		}
	}

	return 0;
}

void CDeathSystem::InventoryItemCountRecv(DEATH_SYSTEM_INVENTORY_RECV * lpMsg)
{
	LPOBJ lpObj = &gObj[lpMsg->index];

	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		return;
	}

	for (int n = 0; n < lpMsg->count; n++)
	{
		DEATH_SYSTEM_INVENTORY_INFO* lpInfo = (DEATH_SYSTEM_INVENTORY_INFO*)(((BYTE*)lpMsg) + sizeof(DEATH_SYSTEM_INVENTORY_RECV) + (sizeof(DEATH_SYSTEM_INVENTORY_INFO)*n));

		if (INVENTORY_RANGE(lpInfo->slot) == 0)
		{
			continue;
		}

		if (lpObj->Inventory[lpInfo->slot].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->Inventory[lpInfo->slot].m_Serial != lpInfo->serial)
		{
			continue;
		}

		if (lpObj->Inventory[lpInfo->slot].m_LoadDeathSystem != 0)
		{
			continue;
		}

		lpObj->Inventory[lpInfo->slot].m_LoadDeathSystem = 1;

		lpObj->Inventory[lpInfo->slot].m_DeathSystemKills = lpInfo->kills;
	}

	this->SendInformation(&gObj[lpMsg->index], 0);
}

void CDeathSystem::WarehouseItemCountRecv(DEATH_SYSTEM_WAREHOUSE_RECV* lpMsg)
{
	LPOBJ lpObj = &gObj[lpMsg->index];

	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		return;
	}

	for (int n = 0; n < lpMsg->count; n++)
	{
		DEATH_SYSTEM_WAREHOUSE_INFO* lpInfo = (DEATH_SYSTEM_WAREHOUSE_INFO*)(((BYTE*)lpMsg) + sizeof(DEATH_SYSTEM_WAREHOUSE_RECV) + (sizeof(DEATH_SYSTEM_WAREHOUSE_INFO) * n));

		if (WAREHOUSE_RANGE(lpInfo->slot) == 0)
		{
			continue;
		}

		if (lpObj->Warehouse[lpInfo->slot].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->Warehouse[lpInfo->slot].m_Serial != lpInfo->serial)
		{
			continue;
		}

		if (lpObj->Warehouse[lpInfo->slot].m_LoadDeathSystem != 0)
		{
			continue;
		}

		lpObj->Warehouse[lpInfo->slot].m_LoadDeathSystem = 1;

		lpObj->Warehouse[lpInfo->slot].m_DeathSystemKills = lpInfo->kills;

	}

	this->SendInformation(&gObj[lpMsg->index], 1);
}

void CDeathSystem::ChaosBoxGenesisItemCountRecv(DEATH_SYSTEM_WAREHOUSE_RECV* lpMsg)
{
#ifdef CHAOS_MACHINE_GENESIS
	LPOBJ lpObj = &gObj[lpMsg->index];

	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		return;
	}

	for (int n = 0; n < lpMsg->count; n++)
	{
		DEATH_SYSTEM_WAREHOUSE_INFO* lpInfo = (DEATH_SYSTEM_WAREHOUSE_INFO*)(((BYTE*)lpMsg) + sizeof(DEATH_SYSTEM_WAREHOUSE_RECV) + (sizeof(DEATH_SYSTEM_WAREHOUSE_INFO) * n));

		if (CHAOS_BOX_GENESIS_RANGE(lpInfo->slot) == 0)
		{
			continue;
		}

		if (lpObj->ChaosBoxGenesis[lpInfo->slot].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBoxGenesis[lpInfo->slot].m_Serial != lpInfo->serial)
		{
			continue;
		}

		if (lpObj->ChaosBoxGenesis[lpInfo->slot].m_LoadDeathSystem != 0)
		{
			continue;
		}

		lpObj->ChaosBoxGenesis[lpInfo->slot].m_LoadDeathSystem = 1;

		lpObj->ChaosBoxGenesis[lpInfo->slot].m_DeathSystemKills = lpInfo->kills;

	}
#endif
}

void CDeathSystem::TradeXItemCountRecv(DEATH_SYSTEM_WAREHOUSE_RECV* lpMsg)
{
#if(GAMESERVER_UPDATE==201 || GAMESERVER_UPDATE == 603)
	LPOBJ lpObj = &gObj[lpMsg->index];

	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		return;
	}

	for (int n = 0; n < lpMsg->count; n++)
	{
		DEATH_SYSTEM_WAREHOUSE_INFO* lpInfo = (DEATH_SYSTEM_WAREHOUSE_INFO*)(((BYTE*)lpMsg) + sizeof(DEATH_SYSTEM_WAREHOUSE_RECV) + (sizeof(DEATH_SYSTEM_WAREHOUSE_INFO) * n));

		if (TRADE_RANGE(lpInfo->slot) == 0)
		{
			continue;
		}

		if (lpObj->TradeX[lpInfo->slot].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->TradeX[lpInfo->slot].m_Serial != lpInfo->serial)
		{
			continue;
		}

		if (lpObj->TradeX[lpInfo->slot].m_LoadDeathSystem != 0)
		{
			continue;
		}

		lpObj->TradeX[lpInfo->slot].m_LoadDeathSystem = 1;

		lpObj->TradeX[lpInfo->slot].m_DeathSystemKills = lpInfo->kills;

	}
#endif
}

void CDeathSystem::DGReceiveTopSerial(DEATH_SYSTEM_TOP_KILL_RECV* lpMsg)
{
	//gServerInfo.m_CounterItemTopKills = lpMsg->kills;
	//
	//if (gServerInfo.m_CounterItemTopSerial != lpMsg->serial) 
	//{
	//	gServerInfo.m_CounterItemTopSerial = lpMsg->serial;
	//	
	//	gLuaGameServer.UpdateTopKill();
	//
	//	for (int n = 9000; n < MAX_OBJECT; n++)
	//	{
	//		LPOBJ lpObj = &gObj[n];
	//
	//		if (lpObj->Connected > OBJECT_LOGGED) {
	//			this->SendInformation(lpObj, 0);
	//			this->SendInformation(lpObj, 1);
	//			this->SendInformation(lpObj, 2);
	//			this->SendInformation(lpObj, 3);
	//			this->SendInformation(lpObj, 8);
	//		}
	//	}
	//}	
}

void CDeathSystem::GDInventoryGetInformation(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[2096];

	DEATH_SYSTEM_INVENTORY_RECV pMsg;

	pMsg.header.set(0x18, 0x40, 0);

	int size = sizeof(pMsg);

	pMsg.index = aIndex;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	pMsg.count = 0;

	DEATH_SYSTEM_INVENTORY_DS_INFO info;

	for (int n = 0; n < INVENTORY_SIZE; n++)
	{
		if (lpObj->Inventory[n].IsItem() == 0)
		{
			continue;
		}

		if (this->CheckIsItem(lpObj->Inventory[n].m_Index) == 0)
		{
			continue;
		}

		info.slot = n;

		info.serial = lpObj->Inventory[n].m_Serial;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if (pMsg.count > 0)
	{
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send, &pMsg, sizeof(pMsg));

		gDataServerConnection.DataSend(send, size);
	}
}

void CDeathSystem::GDWarehouseGetInformation(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[2096];

	DEATH_SYSTEM_INVENTORY_RECV pMsg;

	pMsg.header.set(0x18, 0x41, 0);

	int size = sizeof(pMsg);

	pMsg.index = aIndex;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	pMsg.count = 0;

	DEATH_SYSTEM_INVENTORY_DS_INFO info;

	for (int n = 0; n < WAREHOUSE_SIZE; n++)
	{
		if (lpObj->Warehouse[n].IsItem() == 0)
		{
			continue;
		}

		if (this->CheckIsItem(lpObj->Warehouse[n].m_Index) == 0)
		{
			continue;
		}

		info.slot = n;

		info.serial = lpObj->Warehouse[n].m_Serial;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if (pMsg.count > 0)
	{
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send, &pMsg, sizeof(pMsg));

		gDataServerConnection.DataSend(send, size);
	}
}

void CDeathSystem::GDChaosBoxGenesisGetInformation(int aIndex)
{
#ifdef CHAOS_MACHINE_GENESIS
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[2096];

	DEATH_SYSTEM_INVENTORY_RECV pMsg;

	pMsg.header.set(0x18, 0x43, 0);

	int size = sizeof(pMsg);

	pMsg.index = aIndex;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	pMsg.count = 0;

	DEATH_SYSTEM_INVENTORY_DS_INFO info;

	for (int n = 0; n < CHAOS_BOX_GENESIS_SIZE; n++)
	{
		if (lpObj->ChaosBoxGenesis[n].IsItem() == 0)
		{
			continue;
		}

		if (this->CheckIsItem(lpObj->ChaosBoxGenesis[n].m_Index) == 0)
		{
			continue;
		}

		info.slot = n;

		info.serial = lpObj->ChaosBoxGenesis[n].m_Serial;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if (pMsg.count > 0)
	{
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send, &pMsg, sizeof(pMsg));

		gDataServerConnection.DataSend(send, size);
	}
#endif
}

void CDeathSystem::GDTradeXGetInformation(int aIndex)
{
#if(GAMESERVER_UPDATE==201 || GAMESERVER_UPDATE == 603)
	LPOBJ lpObj = &gObj[aIndex];

	BYTE send[2096];

	DEATH_SYSTEM_INVENTORY_RECV pMsg;

	pMsg.header.set(0x18, 0x44, 0);

	int size = sizeof(pMsg);

	pMsg.index = aIndex;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	pMsg.count = 0;

	DEATH_SYSTEM_INVENTORY_DS_INFO info;

	for (int n = 0; n < TRADEX_SIZE; n++)
	{
		if (lpObj->TradeX[n].IsItem() == 0)
		{
			continue;
		}

		if (this->CheckIsItem(lpObj->TradeX[n].m_Index) == 0)
		{
			continue;
		}

		info.slot = n;

		info.serial = lpObj->TradeX[n].m_Serial;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if (pMsg.count > 0)
	{
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send, &pMsg, sizeof(pMsg));

		gDataServerConnection.DataSend(send, size);
	}
#endif
}

void CDeathSystem::SendInformation(LPOBJ lpObj, int Type)
{
	if (Type == 0)
	{
		this->SendInventory(lpObj);
	}
	else if (Type == 1)
	{
		this->SendWarehouse(lpObj);
	}
	else if (Type == 2)
	{
		this->SendTrade(lpObj);
	}
	else if (Type == 3)
	{
		this->SendChaosBox(lpObj);
	}
	else if (Type == 6)
	{
		this->SendTradeX(lpObj);
	}
	else if (Type == 8)
	{
		this->SendChaosBoxGenesis(lpObj);
	}
}

void CDeathSystem::SendInventory(LPOBJ lpObj)
{
	BYTE send[4096];

	PMSG_DEATH_SYSTEM_SEND pMsg;

	int size = sizeof(pMsg);

	pMsg.h.set(0xFB, 0x2C, sizeof(pMsg));

	pMsg.count = 0;
	pMsg.Type = 0;

	PMSG_DEATH_SYSTEM_INFO info;

	for (int i = 0; i < INVENTORY_SIZE; i++)
	{
		if (lpObj->Inventory[i].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->Inventory[i].m_LoadDeathSystem == 0)
		{
			continue;
		}

		info.ItemIndex = lpObj->Inventory[i].m_Index;
		info.Slot = i;

		info.Top = 0;

		info.kills = lpObj->Inventory[i].m_DeathSystemKills;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}

	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(lpObj->Index, send, size);
}

void CDeathSystem::SendWarehouse(LPOBJ lpObj)
{
	BYTE send[4096];

	PMSG_DEATH_SYSTEM_SEND pMsg;

	int size = sizeof(pMsg);

	pMsg.h.set(0xFB, 0x2C, sizeof(pMsg));

	pMsg.count = 0;
	pMsg.Type = 1;

	PMSG_DEATH_SYSTEM_INFO info;

	for (int i = 0; i < WAREHOUSE_SIZE; i++)
	{
		if (lpObj->Warehouse[i].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->Warehouse[i].m_LoadDeathSystem == 0)
		{
			continue;
		}

		info.ItemIndex = lpObj->Warehouse[i].m_Index;
		info.Slot = i;

		info.Top = 0;

		info.kills = lpObj->Warehouse[i].m_DeathSystemKills;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}

	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(lpObj->Index, send, size);
}

void CDeathSystem::SendTrade(LPOBJ lpObj)
{
	BYTE send[4096];

	PMSG_DEATH_SYSTEM_SEND pMsg;

	int size = sizeof(pMsg);

	pMsg.h.set(0xFB, 0x2C, sizeof(pMsg));

	pMsg.count = 0;
	pMsg.Type = 2;

	PMSG_DEATH_SYSTEM_INFO info;

	for (int i = 0; i < TRADE_SIZE; i++)
	{
		if (lpObj->Trade[i].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->Trade[i].m_LoadDeathSystem == 0)
		{
			continue;
		}

		info.ItemIndex = lpObj->Trade[i].m_Index;
		info.Slot = i;
		info.Top = 0;
		info.kills = lpObj->Trade[i].m_DeathSystemKills;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}

	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(lpObj->Index, send, size);
}

void CDeathSystem::SendTradeTarget(LPOBJ lpObj)
{
	BYTE send[4096];

	PMSG_DEATH_SYSTEM_SEND pMsg;

	int size = sizeof(pMsg);

	pMsg.h.set(0xFB, 0x2C, sizeof(pMsg));

	pMsg.count = 0;
	pMsg.Type = 4;

	PMSG_DEATH_SYSTEM_INFO info;

	for (int i = 0; i < TRADE_SIZE; i++)
	{
		if (lpObj->Trade[i].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->Trade[i].m_LoadDeathSystem == 0)
		{
			continue;
		}

		info.ItemIndex = lpObj->Trade[i].m_Index;
		info.Slot = i;
		info.Top = 0;

		info.kills = lpObj->Trade[i].m_DeathSystemKills;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}

	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(lpObj->TargetNumber, send, size);
}

void CDeathSystem::SendTradeTargetX(LPOBJ lpObj)
{
#if(GAMESERVER_UPDATE==201 || GAMESERVER_UPDATE == 603)
	BYTE send[4096];

	PMSG_DEATH_SYSTEM_SEND pMsg;

	int size = sizeof(pMsg);

	pMsg.h.set(0xFB, 0x2C, sizeof(pMsg));

	pMsg.count = 0;
	pMsg.Type = 7;

	PMSG_DEATH_SYSTEM_INFO info;

	for (int i = 0; i < TRADEX_SIZE; i++)
	{
		if (lpObj->TradeX[i].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->TradeX[i].m_LoadDeathSystem == 0)
		{
			continue;
		}

		info.ItemIndex = lpObj->TradeX[i].m_Index;
		info.Slot = i;

		info.kills = lpObj->TradeX[i].m_DeathSystemKills;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}

	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(lpObj->TargetNumber, send, size);
#endif
}

void CDeathSystem::SendChaosBox(LPOBJ lpObj)
{
	BYTE send[2096];

	PMSG_DEATH_SYSTEM_SEND pMsg;

	int size = sizeof(pMsg);

	pMsg.h.set(0xFB, 0x2C, sizeof(pMsg));

	pMsg.count = 0;
	pMsg.Type = 3;

	PMSG_DEATH_SYSTEM_INFO info;

	for (int i = 0; i < CHAOS_BOX_SIZE; i++)
	{
		if (lpObj->ChaosBox[i].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBox[i].m_LoadDeathSystem == 0)
		{
			continue;
		}

		info.ItemIndex = lpObj->ChaosBox[i].m_Index;
		info.Slot = i;
		info.Top = 0;
		info.kills = lpObj->ChaosBox[i].m_DeathSystemKills;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}

	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(lpObj->Index, send, size);
}

void CDeathSystem::SendChaosBoxGenesis(LPOBJ lpObj)
{
#ifdef CHAOS_MACHINE_GENESIS
	BYTE send[2096];

	PMSG_DEATH_SYSTEM_SEND pMsg;

	int size = sizeof(pMsg);

	pMsg.h.set(0xFB, 0x2C, sizeof(pMsg));

	pMsg.count = 0;
	pMsg.Type = 8;

	PMSG_DEATH_SYSTEM_INFO info;

	for (int i = 0; i < CHAOS_BOX_GENESIS_SIZE; i++)
	{
		if (lpObj->ChaosBoxGenesis[i].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->ChaosBoxGenesis[i].m_LoadDeathSystem == 0)
		{
			continue;
		}

		info.ItemIndex = lpObj->ChaosBoxGenesis[i].m_Index;
		info.Slot = i;
		info.Top = 0;
		info.kills = lpObj->ChaosBoxGenesis[i].m_DeathSystemKills;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}

	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(lpObj->Index, send, size);
#endif
}

void CDeathSystem::SendTradeX(LPOBJ lpObj)
{
#if(GAMESERVER_UPDATE==201 || GAMESERVER_UPDATE == 603)
	BYTE send[2096];

	PMSG_DEATH_SYSTEM_SEND pMsg;

	int size = sizeof(pMsg);

	pMsg.h.set(0xFB, 0x2C, sizeof(pMsg));

	pMsg.count = 0;
	pMsg.Type = 6;

	PMSG_DEATH_SYSTEM_INFO info;

	for (int i = 0; i < TRADEX_SIZE; i++)
	{
		if (lpObj->TradeX[i].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->TradeX[i].m_LoadDeathSystem == 0)
		{
			continue;
		}

		info.ItemIndex = lpObj->TradeX[i].m_Index;
		info.Slot = i;

		info.kills = lpObj->TradeX[i].m_DeathSystemKills;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}

	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(lpObj->Index, send, size);
#endif
}

void CDeathSystem::SendPlayerShop(int aIndex, int bIndex)
{
	LPOBJ lpObj = &gObj[bIndex];

	BYTE send[2096];

	PMSG_DEATH_SYSTEM_SEND pMsg;

	pMsg.h.set(0xFB, 0x2C, 0);

	int size = sizeof(pMsg);

	pMsg.count = 0;
	pMsg.Type = 5;

	PMSG_DEATH_SYSTEM_INFO info;

	for (int i = INVENTORY_EXT4_SIZE; i < INVENTORY_FULL_SIZE; i++)
	{
		if (lpObj->Inventory[i].IsItem() == 0)
		{
			continue;
		}

		if (lpObj->Inventory[i].m_LoadDeathSystem == 0)
		{
			continue;
		}

		LogAdd(LOG_BLUE, "Shop: %d / %d", lpObj->Inventory[i].m_Index, lpObj->Inventory[i].m_DeathSystemKills);

		info.ItemIndex = lpObj->Inventory[i].m_Index;
		info.Slot = i;
		info.Top = 0;
		info.kills = lpObj->Inventory[i].m_DeathSystemKills;

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	pMsg.h.size[0] = SET_NUMBERHB(size);
	pMsg.h.size[1] = SET_NUMBERLB(size);

	memcpy(send, &pMsg, sizeof(pMsg));

	DataSend(aIndex, send, size);
}

int CDeathSystem::GetDeath(int aIndex, int Slot) {
	
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return -1;
	}
	
	if (gObj[aIndex].Inventory[Slot].IsItem() == 0)
	{
		return -1;
	}

	if (this->CheckIsItem(gObj[aIndex].Inventory[Slot].m_Index) == 0) {
		return -1;
	}

	
	return gObj[aIndex].Inventory[Slot].m_DeathSystemKills;
	
}

void CDeathSystem::DeathIncrease(int aIndex, int Slot, int Kills)
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}

	if (gObj[aIndex].Inventory[Slot].IsItem() == 0)
	{
		return;
	}

	if (this->CheckIsItem(gObj[aIndex].Inventory[Slot].m_Index) == 0) {
		return;
	}

	if (gObj[aIndex].Inventory[Slot].m_LoadDeathSystem == 0)
	{
		return;
	}

	gObj[aIndex].Inventory[Slot].m_DeathSystemKills += Kills;

	DEATH_SYSTEM_UPDATE_KILL_SEND pMsg;

	pMsg.header.set(0x18, 0x42, 0);

	int size = sizeof(pMsg);

	pMsg.Type = 1;

	pMsg.serial = gObj[aIndex].Inventory[Slot].m_Serial;

	pMsg.kills = gObj[aIndex].Inventory[Slot].m_DeathSystemKills;

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	gDataServerConnection.DataSend((BYTE*)&pMsg, size);
}

void CDeathSystem::DeathDecrease(int aIndex, int Slot, int Kills)
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return;
	}

	if (gObj[aIndex].Inventory[Slot].IsItem() == 0)
	{
		return;
	}

	if (this->CheckIsItem(gObj[aIndex].Inventory[Slot].m_Index) == 0) {
		return;
	}

	if (gObj[aIndex].Inventory[Slot].m_LoadDeathSystem == 0)
	{
		return;
	}
	
	if (gObj[aIndex].Inventory[Slot].m_DeathSystemKills >= Kills) {
		gObj[aIndex].Inventory[Slot].m_DeathSystemKills -= Kills;
	}
	else
	{
		gObj[aIndex].Inventory[Slot].m_DeathSystemKills = 0;
	}


	DEATH_SYSTEM_UPDATE_KILL_SEND pMsg;

	pMsg.header.set(0x18, 0x42, 0);

	int size = sizeof(pMsg);

	pMsg.Type = 2;

	pMsg.serial = gObj[aIndex].Inventory[Slot].m_Serial;

	pMsg.kills = gObj[aIndex].Inventory[Slot].m_DeathSystemKills;

	pMsg.header.size[0] = SET_NUMBERHB(size);
	pMsg.header.size[1] = SET_NUMBERLB(size);

	gDataServerConnection.DataSend((BYTE*)&pMsg, size);
}
