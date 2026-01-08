#include "stdafx.h"
#include "TradeX.h"
#include "QueryManager.h"
#include "SocketManager.h"

CTradeX gTradeX;

CTradeX::CTradeX()
{

}

CTradeX::~CTradeX()
{

}

void CTradeX::GCTradeXGetItemSave(SDHP_TRADEX_GET_ITEM_SAVE_RECV * lpMsg, int index)
{
	SDHP_TRADEX_GET_ITEM_SAVE_SEND pMsg;

	pMsg.header.set(0x05, 0x41, sizeof(pMsg));

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account, lpMsg->account, sizeof(pMsg.account));

	memcpy(pMsg.name, lpMsg->name, sizeof(pMsg.name));

	if (gQueryManager.ExecQuery("SELECT account,items,targetname,state FROM TRADE_X WHERE account='%s' and name='%s'", lpMsg->account, lpMsg->name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();

		pMsg.Itens = 0;
	}
	else {
		gQueryManager.GetAsBinary("items", pMsg.TradeXItem[0], sizeof(pMsg.TradeXItem));

		gQueryManager.GetAsString("targetname", pMsg.target, sizeof(pMsg.target));

		pMsg.State = gQueryManager.GetAsInteger("state");

		pMsg.Itens = 1;

		gQueryManager.Close();
	}

	gSocketManager.DataSend(index, (BYTE*)&pMsg, sizeof(pMsg));
}

void CTradeX::GDTradeXItemSaveRecv(SDHP_TRADEX_ITEM_SAVE_RECV * lpMsg)
{
	if (gQueryManager.ExecQuery("SELECT account FROM TRADE_X WHERE account='%s' and name='%s'", lpMsg->account, lpMsg->name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();

		gQueryManager.ExecQuery("INSERT INTO TRADE_X (account,name, state, date) VALUES ('%s', '%s', %d, getdate())", lpMsg->account, lpMsg->name, lpMsg->State);
		gQueryManager.Close();

		gQueryManager.ExecQuery("UPDATE TRADE_X SET items=CONVERT(varbinary(%d),REPLICATE(char(0xFF),%d)) WHERE account='%s' AND name='%s'", sizeof(lpMsg->TradeXItem), sizeof(lpMsg->TradeXItem), lpMsg->account, lpMsg->name);
		gQueryManager.Close();

		gQueryManager.BindParameterAsBinary(1, lpMsg->TradeXItem[0], sizeof(lpMsg->TradeXItem));
		gQueryManager.ExecQuery("UPDATE TRADE_X SET items=?,targetname='%s' WHERE account='%s' and name='%s'", lpMsg->target, lpMsg->account, lpMsg->name);
		gQueryManager.Close();
	}
	else {
		gQueryManager.Close();

		if (lpMsg->Clean == 0) {
			gQueryManager.ExecQuery("DELETE FROM TRADE_X WHERE account='%s' and name='%s'", lpMsg->account, lpMsg->name);
			gQueryManager.Close();
		}
		else {
			gQueryManager.BindParameterAsBinary(1, lpMsg->TradeXItem[0], sizeof(lpMsg->TradeXItem));
			gQueryManager.ExecQuery("UPDATE TRADE_X SET items=?,targetname='%s', state=%d WHERE account='%s' and name='%s'", lpMsg->target, lpMsg->State, lpMsg->account, lpMsg->name);
			gQueryManager.Close();
		}
	}
}
