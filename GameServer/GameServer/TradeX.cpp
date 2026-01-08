#include "stdafx.h"
#include "TradeX.h"
#include "ServerInfo.h"
#include "Map.h"
#include "Trade.h"
#include "Notice.h"
#include "Util.h"
#include "GameMain.h"
#include "DeathSystem.h"
#include "LuaGameServer.h"
#include "Log.h"
#include "Message.h"
#include "Viewport.h"

CTradeX gTradeX;

CTradeX::CTradeX()
{
}

CTradeX::~CTradeX()
{
}

void CTradeX::ClearTradeX(LPOBJ lpObj)
{
	for (int n = 0; n < TRADEX_SIZE; n++)
	{
		lpObj->TradeX[n].Clear();
	}

	memset(lpObj->TradeXMap, 0xFF, TRADEX_SIZE);
}

void CTradeX::TradeXRequestSend(LPOBJ lpObj, LPOBJ lpTarget)
{
	if (gServerInfo.m_TradeXSwitch == 0)
	{
		return;
	}

	if (gObjIsConnectedGP(lpObj->Index) == 0)
	{
		return;
	}

	if (gObjIsConnectedGP(lpTarget->Index) == 0)
	{
		return;
	}

	if (lpObj->Interface.use != 0 || lpTarget->Interface.use != 0
		|| lpObj->Interface.type != INTERFACE_NONE || lpTarget->Interface.type != INTERFACE_NONE)
	{
		return;
	}

	if (lpObj->TradeXState != 0 || lpTarget->TradeXState != 0) {
		return;
	}

	if (lpObj->Index == lpTarget->Index) {
		return;
	}

	if (CA_MAP_RANGE(lpTarget->Map) != 0 || DS_MAP_RANGE(lpTarget->Map) != 0 || BC_MAP_RANGE(lpTarget->Map) != 0 || CC_MAP_RANGE(lpTarget->Map) != 0
#if(GAMESERVER_UPDATE>=251)
		|| IT_MAP_RANGE(lpTarget->Map) != 0 || DA_MAP_RANGE(lpTarget->Map) != 0 || DG_MAP_RANGE(lpTarget->Map) != 0 || IG_MAP_RANGE(lpTarget->Map) != 0
#endif
		)
	{
		gTrade.GCTradeResponseSend(lpObj->Index, 0, lpTarget->Name, 0, 0);
		return;
	}

	if (lpObj->X < (lpTarget->X - 6) || lpObj->X >(lpTarget->X + 6) || lpObj->Y < (lpTarget->Y - 6) || lpObj->Y >(lpTarget->Y + 6))
	{
		gTrade.GCTradeResponseSend(lpObj->Index, 0, lpTarget->Name, 0, 0);
		return;
	}

	if (lpObj->PShopOpen != 0 || lpTarget->PShopOpen != 0)
	{
		gTrade.GCTradeResponseSend(lpObj->Index, 0, lpTarget->Name, 0, 0);
		return;
	}

	if ((lpTarget->Option & 1) == 0)
	{
		gTrade.GCTradeResponseSend(lpObj->Index, 0, lpTarget->Name, 0, 0);
		return;
	}

	if (gLuaGameServer.PlayerSendTradeX(lpObj->Index, lpTarget->Index))
	{
		gTrade.GCTradeResponseSend(lpObj->Index, 0, lpTarget->Name, 0, 0);
		return;
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_TRADEX;
	lpObj->Interface.state = 0;
	lpObj->InterfaceTime = GetTickCount();

	lpObj->TargetNumber = lpTarget->Index;
	lpObj->UsingTradeX = 1;
	lpObj->TradeXState = 0;
	lpObj->CoinsRate = 0;
	lpObj->CoinsExchange = 0;
	std::memcpy(lpObj->TradeXTarget, lpTarget->Name, sizeof(lpObj->TradeXTarget));

	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, gMessage.GetMessage(902));

	lpTarget->Interface.use = 1;
	lpTarget->Interface.type = INTERFACE_TRADEX;
	lpTarget->Interface.state = 0;
	lpTarget->InterfaceTime = GetTickCount();

	lpTarget->TargetNumber = lpObj->Index;
	lpTarget->UsingTradeX = 1;
	lpTarget->TradeXState = 0;
	lpTarget->CoinsRate = 0;
	lpTarget->CoinsExchange = 0;
	std::memcpy(lpTarget->TradeXTarget, lpObj->Name, sizeof(lpTarget->TradeXTarget));

	gTrade.GCTradeRequestSend(lpTarget->Index, lpObj->Name);
}

void CTradeX::TradeXResponseRecv(PMSG_TRADE_RESPONSE_RECV * lpMsg, LPOBJ lpObj, LPOBJ lpTarget)
{
	if (lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_TRADEX || lpObj->Interface.state != 0)
	{
		return;
	}

	if (lpTarget->Interface.use == 0 || lpTarget->Interface.type != INTERFACE_TRADEX || lpTarget->Interface.state != 0)
	{
		return;
	}

	if (lpObj->TradeXState != 0 || lpTarget->TradeXState != 0) {
		return;
	}

	if (lpObj->Index == lpTarget->Index) {
		return;
	}

	if (OBJECT_RANGE(lpObj->TargetNumber) == 0 || OBJECT_RANGE(lpTarget->TargetNumber) == 0) {
		goto CLEAR_JUMP;
	}

	if (CA_MAP_RANGE(lpTarget->Map) != 0 || DS_MAP_RANGE(lpTarget->Map) != 0 || BC_MAP_RANGE(lpTarget->Map) != 0 || CC_MAP_RANGE(lpTarget->Map) != 0
#if(GAMESERVER_UPDATE>=251)
		|| IT_MAP_RANGE(lpTarget->Map) != 0 || DA_MAP_RANGE(lpTarget->Map) != 0 || DG_MAP_RANGE(lpTarget->Map) != 0 || IG_MAP_RANGE(lpTarget->Map) != 0
#endif
		)
	{
		goto CLEAR_JUMP;
	}

	if (lpObj->X < (lpTarget->X - 6) || lpObj->X >(lpTarget->X + 6) || lpObj->Y < (lpTarget->Y - 6) || lpObj->Y >(lpTarget->Y + 6))
	{
		goto CLEAR_JUMP;
	}

	if (lpObj->PShopOpen != 0 || lpTarget->PShopOpen != 0)
	{
		goto CLEAR_JUMP;
	}

	if (lpMsg->response == 0)
	{
		goto CLEAR_JUMP;
	}

	if (lpObj->Transaction == 1)
	{
		goto CLEAR_JUMP;
	}

	this->ClearTradeX(lpObj);

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_TRADEX;
	lpObj->Interface.state = 1;
	lpObj->TradeXOk = 0;
	lpObj->TradeXTimeItem = 0;
	lpObj->CoinsRate = 0;
	lpObj->CoinsExchange = 0;

	gTrade.GCTradeResponseSend(lpObj->Index, 1, lpTarget->Name, lpTarget->Level, lpTarget->GuildNumber, 1);

	this->ClearTradeX(lpTarget);

	lpTarget->Interface.use = 1;
	lpTarget->Interface.type = INTERFACE_TRADEX;
	lpTarget->Interface.state = 1;
	lpTarget->TradeXOk = 0;
	lpTarget->TradeXTimeItem = 0;
	lpTarget->CoinsRate = 0;
	lpTarget->CoinsExchange = 0;

	gTrade.GCTradeResponseSend(lpTarget->Index, 1, lpObj->Name, lpObj->Level, lpObj->GuildNumber, 1);
	return;

CLEAR_JUMP:

	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;
	lpObj->TargetNumber = -1;
	lpObj->UsingTradeX = 0;

	gTrade.GCTradeResponseSend(lpObj->Index, 0, lpTarget->Name, 0, 0);

	lpTarget->Interface.use = 0;
	lpTarget->Interface.type = INTERFACE_NONE;
	lpTarget->Interface.state = 0;
	lpTarget->TargetNumber = -1;
	lpTarget->UsingTradeX = 0;

	gTrade.GCTradeResponseSend(lpTarget->Index, 0, lpObj->Name, 0, 0);
}

void CTradeX::TradeXOkButtonRecv(PMSG_TRADE_OK_BUTTON_RECV * lpMsg, LPOBJ lpObj, LPOBJ lpTarget)
{
	if (lpObj->Interface.use == 0 || lpObj->Interface.type != INTERFACE_TRADEX || lpObj->Interface.state == 0)
	{
		return;
	}

	if (lpTarget->Interface.use == 0 || lpTarget->Interface.type != INTERFACE_TRADEX || lpTarget->Interface.state == 0)
	{
		return;
	}

	if (lpMsg->tradeX == 1) {
		if (lpMsg->Cancel == 25) {
			lpObj->TradeXState = 2;
			lpTarget->TradeXState = 2;

			lpObj->TargetNumber = -1;
			lpTarget->TargetNumber = -1;

			gTradeX.TradeXSendState(lpObj->Index, 4);
			gTradeX.TradeXSendState(lpTarget->Index, 4);
			return;
		}
	}

	if ((GetTickCount() - lpObj->TradeXTimeItem) < 9000
		|| (GetTickCount() - lpTarget->TradeXTimeItem) < 9000)
	{
		return;
	}

	if (lpObj->TradeXState != 0 || lpTarget->TradeXState != 0) {
		return;
	}

	if (OBJECT_RANGE(lpObj->TargetNumber) == 0 || OBJECT_RANGE(lpTarget->TargetNumber) == 0) {
		return;
	}

	if (lpMsg->flag == 0 && lpObj->TradeXOk != 0)
	{
		lpObj->TradeXOk = 0;
	}

	if (lpMsg->flag == 1 && lpObj->TradeXOk == 0)
	{
		lpObj->TradeXOk = 1;
	}

	gTrade.GCTradeOkButtonSend(lpTarget->Index, lpObj->TradeXOk, 1);

	if (lpObj->TradeXOk == 0 || lpTarget->TradeXOk == 0)
	{
		return;
	}

	if (gLuaGameServer.PlayerTradeXOk(lpObj->Index, lpTarget->Index)) {
		this->TradeXSendState(lpObj->Index, 5);
		this->TradeXSendState(lpTarget->Index, 5);
		return;
	}

	this->ExchangeTradeXItem(lpObj, lpTarget);

	//Source
	lpObj->TradeXState = 1;
	this->TradeXSendState(lpObj->Index, 3);
	this->TradeXListSend(lpObj->Index, lpObj, 0);
	this->TradeXListSend(lpObj->Index, lpTarget, 1);
	this->GDTradeXItemSaveSend(lpObj->Index, 0);

#if (GAMESERVER_UPDATE >= 201)
	//gViewport.GCViewportUpdatePlayer(lpObj);
#endif

	//Target
	lpTarget->TradeXState = 1;
	this->TradeXSendState(lpTarget->Index, 3);
	this->TradeXListSend(lpTarget->Index, lpTarget, 0);
	this->TradeXListSend(lpTarget->Index, lpObj, 1);
	this->GDTradeXItemSaveSend(lpTarget->Index, 0);

#if (GAMESERVER_UPDATE >= 201)
	//gViewport.GCViewportUpdatePlayer(lpTarget);
#endif

	lpObj->TargetNumber = -1;
	lpTarget->TargetNumber = -1;
}

void CTradeX::TradeXItemAddSend(int aIndex, BYTE slot, BYTE * ItemInfo)
{
	PMSG_TRADEX_ITEM_ADD_SEND pMsg;

	pMsg.header.set(0xB1, 0x15, sizeof(pMsg));

	pMsg.slot = slot;

	memcpy(pMsg.ItemInfo, ItemInfo, sizeof(pMsg.ItemInfo));

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CTradeX::TradeXGetItemSaveSend(LPOBJ lpObj)
{
	if (gObjIsAccountValid(lpObj->Index, lpObj->Account) == 0)
	{
		return;
	}

	SDHP_TRADEX_GET_ITEM_SAVE_SEND pMsg;

	pMsg.header.set(0x05, 0x41, sizeof(pMsg));

	pMsg.index = lpObj->Index;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}

void CTradeX::TradeXGetItemSaveRecv(SDHP_TRADEX_GET_ITEM_SAVE_RECV * lpMsg)
{
	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		LogAdd(LOG_RED, "[TradeXGetItemSaveRecv] Invalid Account [%d](%s)", lpMsg->index, lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}

	LPOBJ lpObj = &gObj[lpMsg->index];

	if (_stricmp(lpMsg->name, lpObj->Name) != 0) {
		return;
	}

	if (lpMsg->Itens == 0) {
		return;
	}

	BYTE Clean = 0;

	for (int n = 0; n < TRADEX_SIZE; n++)
	{
		CItem item;

		lpObj->TradeX[n].Clear();

		if (gItemManager.ConvertItemByte(&item, lpMsg->TradeXItem[n]) != 0) {
			gItemManager.TradeXAddItem(lpObj->Index, item, n); 
			Clean++;
		}
	}

	if (Clean == 0) {
		return;
	}

	lpObj->UsingTradeX = 1;

	lpObj->TradeXState = lpMsg->State;

	std::memcpy(lpObj->TradeXTarget, lpMsg->target, sizeof(lpObj->TradeXTarget));

	this->TradeXSendState(lpObj->Index, 1, lpObj->TradeXState);

	gDeathSystem.GDTradeXGetInformation(lpObj->Index);
}

void CTradeX::GDTradeXItemSaveSend(int aIndex, int Check)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->UsingTradeX == 0)
	{
		return;
	}

	BYTE Clean = 0;

	SDHP_TRADEX_ITEM_SAVE_SEND pMsg;

	pMsg.header.set(0x05, 0x40, sizeof(pMsg));

	pMsg.index = aIndex;

	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));

	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));

	memcpy(pMsg.target, lpObj->TradeXTarget, sizeof(pMsg.target));

	pMsg.State = lpObj->TradeXState;

	for (int n = 0; n < TRADEX_SIZE; n++) {
		gItemManager.DBItemByteConvert(pMsg.TradeXItem[n], &lpObj->TradeX[n]);

		if (lpObj->TradeX[n].IsItem()) {
			Clean++;
		}
	}

	if (Check == 0) {
		Clean = 1;
	}

	pMsg.Clean = Clean;

	gDataServerConnection.DataSend((BYTE*)&pMsg, sizeof(pMsg));
}

bool CTradeX::TradeXCheckIsEmpty(LPOBJ lpObj)
{
	if (lpObj->UsingTradeX == 0 || lpObj->Interface.type != INTERFACE_TRADEX) {
		return 0;
	}

	if (OBJECT_RANGE(lpObj->TargetNumber) != 0) {
		return 0;
	}

	BYTE Clean = 0;

	for (int n = 0; n < TRADEX_SIZE; n++) {
		if (lpObj->TradeX[n].IsItem()) {
			Clean++;
		}
	}

	if (Clean == 0) {
		lpObj->UsingTradeX = 0;
		lpObj->Interface.use = 0;
		lpObj->Interface.type = INTERFACE_NONE;
		lpObj->Interface.state = 0;
		lpObj->TradeXState = 0;

		gTrade.GCTradeResultSend(lpObj->Index, 1);
		this->TradeXSendState(lpObj->Index, 2);
		return 1;
	}

	return 0;
}

bool CTradeX::TradeXCheckIsEmpty2(LPOBJ lpObj)
{
	if (lpObj->UsingTradeX == 0 || lpObj->Interface.type != INTERFACE_TRADEX) 
	{
		return 0;
	}

	BYTE CleanSource = 0;
	BYTE CleanTarget = 0;


	if (OBJECT_RANGE(lpObj->TargetNumber) <= 0) 
	{
		return 0;
	}

	LPOBJ lpTarget = &gObj[lpObj->TargetNumber];

	for (int n = 0; n < TRADEX_SIZE; n++) 
	{
		if (lpObj->TradeX[n].IsItem()) 
		{
			CleanSource++;
		}

		if (lpTarget->TradeX[n].IsItem())
		{
			CleanTarget++;
		}
	}

	if (CleanSource == 0 && CleanTarget == 0)
	{
		lpObj->UsingTradeX = 0;
		lpObj->Interface.use = 0;
		lpObj->Interface.type = INTERFACE_NONE;
		lpObj->Interface.state = 0;
		lpObj->TradeXState = 0;

		gTrade.GCTradeResultSend(lpObj->Index, 1);
		this->TradeXSendState(lpObj->Index, 2);
		//
		lpTarget->UsingTradeX = 0;
		lpTarget->Interface.use = 0;
		lpTarget->Interface.type = INTERFACE_NONE;
		lpTarget->Interface.state = 0;
		lpTarget->TradeXState = 0;

		gTrade.GCTradeResultSend(lpTarget->Index, 1);
		this->TradeXSendState(lpTarget->Index, 2);
		return 1;
	}

	return 0;
}

void CTradeX::TradeXListSend(int aIndex, LPOBJ lpObj, BYTE target)
{
	BYTE send[4096];

	PMSG_TRADEX_ITEM_LIST_SEND pMsg;

	pMsg.header.set(0xFB, 0x11, 0);

	int size = sizeof(pMsg);

	pMsg.count = 0;

	pMsg.target = target;

	PMSG_TRADEX_ITEM_LIST info;

	for (int n = 0; n < TRADEX_SIZE; n++)
	{
		if (lpObj->TradeX[n].IsItem() == 0)
		{
			continue;
		}

		info.slot = n;

		gItemManager.ItemByteConvert(info.ItemInfo, lpObj->TradeX[n]);

		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);

		pMsg.count++;
	}

	if (pMsg.count > 0) {
		pMsg.header.size[0] = SET_NUMBERHB(size);
		pMsg.header.size[1] = SET_NUMBERLB(size);

		memcpy(send, &pMsg, sizeof(pMsg));

		DataSend(aIndex, send, size);
	}
}

void CTradeX::TradeXSendState(int aIndex, BYTE state, BYTE transation)
{
	PMSG_TRADEX_STATE_SEND pMsg;

	pMsg.header.set(0xFB, 0x10, sizeof(pMsg));

	pMsg.state = state;

	pMsg.transation = transation;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

void CTradeX::TradeXOpenWindowRecv(PMSG_TRADEX_OPEN_WINDOW_RECV * lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (!lpObj->UsingTradeX) {
		return;
	}

	if (lpObj->Interface.use != 0 || lpObj->Interface.type != INTERFACE_NONE) {
		return;
	}

	if (CA_MAP_RANGE(lpObj->Map) != 0 || DS_MAP_RANGE(lpObj->Map) != 0 || BC_MAP_RANGE(lpObj->Map) != 0 || CC_MAP_RANGE(lpObj->Map) != 0
#if(GAMESERVER_UPDATE>=251)
		|| IT_MAP_RANGE(lpObj->Map) != 0 || DA_MAP_RANGE(lpObj->Map) != 0 || DG_MAP_RANGE(lpObj->Map) != 0 || IG_MAP_RANGE(lpObj->Map) != 0
#endif
		)
	{
		return;
	}

	if (lpObj->PShopOpen != 0)
	{
		return;
	}

	if (lpObj->TradeXState == 2) {
		if (this->TradeXCheckIsEmpty(lpObj)) {
			return;
		}
	}

	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_TRADEX;
	lpObj->Interface.state = 1;

	gTrade.GCTradeResponseSend(lpObj->Index, 1, lpObj->TradeXTarget, 0, 0, 1);

	this->TradeXSendState(lpObj->Index, 0, lpObj->TradeXState);

	this->TradeXListSend(lpObj->Index, lpObj, 0);

	gDeathSystem.SendTradeX(lpObj);

	if (OBJECT_USER_RANGE(lpObj->TargetNumber)) {
		this->TradeXListSend(lpObj->Index, &gObj[lpObj->TargetNumber], 1);
		gDeathSystem.SendTradeTargetX(lpObj);
	}
}

void CTradeX::TradeXHiddenWindowRecv(PMSG_TRADEX_HIDDEN_WINDOW_RECV * lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];

	//for (int i = 0; i < 120; i++)
	//{
	//	if (lpObj->TradeX[i].IsItem() != -1)
	//	{
	//		if (OBJECT_RANGE(lpObj->TargetNumber) != 0)
	//		{
	//			if (lpObj->UsingTradeX == 1) {
	//				lpObj->TradeXState = 2;
	//				gTradeX.TradeXSendState(lpObj->TargetNumber, 4);
	//				gObj[lpObj->TargetNumber].TradeXState = 2;
	//				gObj[lpObj->TargetNumber].TargetNumber = -1;
	//			}
	//			else
	//			{
	//				gTrade.ResetTrade(lpObj->TargetNumber);
	//				gTrade.GCTradeResultSend(lpObj->TargetNumber, 0);
	//				gTrade.ResetTrade(aIndex);
	//			}
	//		}
	//	}
	//}

	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}

	if (!lpObj->UsingTradeX) {
		return;
	}

	if (lpObj->Interface.use == 1 && lpObj->Interface.type != INTERFACE_TRADEX) {
		return;
	}

	if (lpObj->PShopOpen != 0)
	{
		return;
	}

	if (lpObj->Interface.type != INTERFACE_TRADEX) {
		return;
	}

	if (lpObj->TradeXState != 0) 
	{
		if (this->TradeXCheckIsEmpty(lpObj)) {
			return;
		}
	}
	else
	{
		if (this->TradeXCheckIsEmpty2(lpObj)) 
		{
			return;
		}
	}

	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;

	this->TradeXSendState(lpObj->Index, 1);
	gTrade.GCTradeResultSend(lpObj->Index, 1);

#if (GAMESERVER_UPDATE >= 201)
	//gViewport.GCViewportUpdatePlayer(lpObj);
#endif
}

void CTradeX::TradeXItemDelSend(int aIndex, BYTE slot)
{
	PMSG_TRADEX_ITEM_DEL_SEND pMsg;

	pMsg.header.set(0xFB, 0x13, sizeof(pMsg));

	pMsg.slot = slot;

	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}

bool CTradeX::ExchangeTradeXItem(LPOBJ lpObj, LPOBJ lpTarget)
{
	static CCriticalSection m_critical;

	m_critical.lock();

	CItem* TradeX_Source = new CItem[TRADEX_SIZE];

	for (int i = 0; i < TRADEX_SIZE; i++) 
	{
		TradeX_Source[i].Clear();
	}

	CItem* TradeX_Target = new CItem[TRADEX_SIZE];

	for (int i = 0; i < TRADEX_SIZE; i++) {
		TradeX_Target[i].Clear();
	}

	//Copy tradex from source
	for (int i = 0; i < TRADEX_SIZE; i++) 
	{
		if (lpObj->TradeX[i].IsItem()) 
		{
			TradeX_Source[i] = lpObj->TradeX[i];
		}
	}

	this->ExchangeTradeItemLog(lpObj, lpTarget);

	//Clear TradeX source
	this->ClearTradeX(lpObj);

#if (GAMESERVER_UPDATE >= 201)
	//gViewport.GCViewportUpdatePlayer(lpObj);
#endif

	//Copy tradex from target
	for (int i = 0; i < TRADEX_SIZE; i++) 
	{
		if (lpTarget->TradeX[i].IsItem()) 
		{
			TradeX_Target[i] = lpTarget->TradeX[i];
		}
	}

	this->ExchangeTradeItemLog(lpTarget, lpObj);

	//Clear TradeX target
	this->ClearTradeX(lpTarget);

#if (GAMESERVER_UPDATE >= 201)
	//gViewport.GCViewportUpdatePlayer(lpTarget);
#endif

	//Copy to source itens from target
	for (int i = 0; i < TRADEX_SIZE; i++) 
	{
		if (TradeX_Target[i].IsItem()) 
		{
			lpObj->TradeX[i] = TradeX_Target[i];
		}
	}

	//Copy to target itens from source
	for (int i = 0; i < TRADEX_SIZE; i++) 
	{
		if (TradeX_Source[i].IsItem()) 
		{
			lpTarget->TradeX[i] = TradeX_Source[i];
		}
	}

	delete[] TradeX_Source;

	delete[] TradeX_Target;

	m_critical.unlock();

	return 1;
}

void CTradeX::ExchangeTradeItemLog(LPOBJ lpObj, LPOBJ lpTarget)
{
	for (int n = 0; n < TRADEX_SIZE; n++)
	{
		if (lpObj->TradeX[n].IsItem() != 0)
		{
			gLog.Output(LOG_TRADEX, "[ExchangeTradeXLog][%s][%s] - (Account: %s, Name: %s, Item: %s, Level: %02d, Serial: %08X, Option1: %01d, Option2: %01d, Option3: %01d, NewOption: %03d, JewelOfHarmonyOption: %03d, ItemOptionEx: %03d, SocketOption: %03d, %03d, %03d, %03d, %03d)", lpObj->Account, lpObj->Name, lpTarget->Account, lpTarget->Name, lpObj->TradeX[n].GetName(), lpObj->TradeX[n].m_Level, lpObj->TradeX[n].m_Serial, lpObj->TradeX[n].m_Option1, lpObj->TradeX[n].m_Option2, lpObj->TradeX[n].m_Option3, lpObj->TradeX[n].m_NewOption, lpObj->TradeX[n].m_JewelOfHarmonyOption, lpObj->TradeX[n].m_ItemOptionEx, lpObj->TradeX[n].m_SocketOption[0], lpObj->TradeX[n].m_SocketOption[1], lpObj->TradeX[n].m_SocketOption[2], lpObj->TradeX[n].m_SocketOption[3], lpObj->TradeX[n].m_SocketOption[4]);
		}
	}
}