#pragma once

#include "User.h"
#include "Trade.h"
#include "Protocol.h"

struct PMSG_TRADEX_ITEM_ADD_SEND
{
	PSBMSG_HEAD header; // C1:B1:15
	BYTE slot;
	BYTE ItemInfo[MAX_ITEM_INFO];
};

//**********************************************//
//********** GameServer -> DataServer **********//
//**********************************************//

struct SDHP_TRADEX_ITEM_SAVE_SEND
{
	PSWMSG_HEAD header; // C2:05:30
	WORD index;
	char account[11];
	char name[11];
	char target[11];
	BYTE TradeXItem[TRADEX_SIZE][MAX_ITEM_BYTE];
	BYTE State;
	BYTE Clean;
};

struct SDHP_TRADEX_GET_ITEM_SAVE_SEND
{
	PSBMSG_HEAD header; // C2:05:31
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_TRADEX_GET_ITEM_SAVE_RECV
{
	PSWMSG_HEAD header; // C2:05:32
	WORD index;
	char account[11];
	char name[11];
	char target[11];
	BYTE Itens;
	BYTE State;
	BYTE TradeXItem[TRADEX_SIZE][MAX_ITEM_BYTE];
};

struct PMSG_TRADEX_STATE_SEND
{
	PSBMSG_HEAD header; // C1:0xFB:0x10
	BYTE state;
	BYTE transation;
};

struct PMSG_TRADEX_ITEM_LIST_SEND
{
	PSWMSG_HEAD header; // C2:0xFB:0x11
	BYTE count;
	BYTE target;
};

struct PMSG_TRADEX_ITEM_LIST
{
	BYTE slot;
	BYTE ItemInfo[MAX_ITEM_INFO];
};

struct PMSG_TRADEX_OPEN_WINDOW_RECV
{
	PSBMSG_HEAD header; // C2:0xFB:0x12
};

struct PMSG_TRADEX_ITEM_DEL_SEND
{
	PSBMSG_HEAD header; // C1:0xFB:0x13
	BYTE slot;
};

struct PMSG_TRADEX_HIDDEN_WINDOW_RECV
{
	PSBMSG_HEAD header; // C2:0xFB:0x14
	BYTE state;
};

class CTradeX {
public:
	CTradeX();
	virtual ~CTradeX();

	void ClearTradeX(LPOBJ lpObj);
	void TradeXRequestSend(LPOBJ lpObj, LPOBJ lpTarget);
	void TradeXResponseRecv(PMSG_TRADE_RESPONSE_RECV* lpMsg, LPOBJ lpObj, LPOBJ lpTarget);
	void TradeXOkButtonRecv(PMSG_TRADE_OK_BUTTON_RECV* lpMsg, LPOBJ lpObj, LPOBJ lpTarget);
	void TradeXItemAddSend(int aIndex, BYTE slot, BYTE* ItemInfo);
	void TradeXGetItemSaveSend(LPOBJ lpObj);
	void TradeXGetItemSaveRecv(SDHP_TRADEX_GET_ITEM_SAVE_RECV * lpMsg);
	void GDTradeXItemSaveSend(int aIndex, int Check);
	bool TradeXCheckIsEmpty(LPOBJ lpObj);
	bool TradeXCheckIsEmpty2(LPOBJ lpObj);
	void TradeXListSend(int aIndex, LPOBJ lpObj, BYTE target);
	void TradeXSendState(int aIndex, BYTE state, BYTE transation = 0);
	void TradeXOpenWindowRecv(PMSG_TRADEX_OPEN_WINDOW_RECV * lpMsg, int aIndex);
	void TradeXHiddenWindowRecv(PMSG_TRADEX_HIDDEN_WINDOW_RECV * lpMsg, int aIndex);
	void TradeXItemDelSend(int aIndex, BYTE slot);
	bool ExchangeTradeXItem(LPOBJ lpObj, LPOBJ lpTarget);
	void ExchangeTradeItemLog(LPOBJ lpObj, LPOBJ lpTarget);
};

extern CTradeX gTradeX;