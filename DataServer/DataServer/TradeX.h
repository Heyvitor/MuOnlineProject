#pragma once

#include "DataServerProtocol.h"

#define TRADEX_SIZE 120

struct SDHP_TRADEX_GET_ITEM_SAVE_RECV
{
	PSBMSG_HEAD header; // C2:05:31
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_TRADEX_GET_ITEM_SAVE_SEND
{
	PSWMSG_HEAD header; // C2:05:32
	WORD index;
	char account[11];
	char name[11];
	char target[11];
	BYTE Itens;
	BYTE State;
	BYTE TradeXItem[TRADEX_SIZE][16];
};

struct SDHP_TRADEX_ITEM_SAVE_RECV
{
	PSWMSG_HEAD header; // C2:05:30
	WORD index;
	char account[11];
	char name[11];
	char target[11];
	BYTE TradeXItem[TRADEX_SIZE][16];
	BYTE State;
	BYTE Clean;
};

class CTradeX {
public:
	CTradeX();
	virtual ~CTradeX();

	void GCTradeXGetItemSave(SDHP_TRADEX_GET_ITEM_SAVE_RECV * lpMsg, int index);
	void GDTradeXItemSaveRecv(SDHP_TRADEX_ITEM_SAVE_RECV * lpMsg);
};

extern CTradeX gTradeX;