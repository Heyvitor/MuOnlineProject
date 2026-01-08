#include "stdafx.h"
#include "ChaosBoxGenesis.h"
#include "QueryManager.h"
#include "SocketManager.h"

ChaosBoxGenesis gChaosBoxGenesis;

ChaosBoxGenesis::ChaosBoxGenesis()
{

}

ChaosBoxGenesis::~ChaosBoxGenesis()
{

}

void ChaosBoxGenesis::GCChaosBoxGenesisGetItemSaveRecv(SDHP_CHAOS_BOX_GENESIS_ITEM_SAVE_RECV * lpMsg, int index)
{
	SDHP_CHAOS_BOX_GENESIS_GET_ITEM_SAVE_SEND pMsg;

	pMsg.header.set(0x05, 0x60, sizeof(pMsg));

	pMsg.index = lpMsg->index;

	memcpy(pMsg.account, lpMsg->account, sizeof(pMsg.account));

	memcpy(pMsg.name, lpMsg->name, sizeof(pMsg.name));

	if (gQueryManager.ExecQuery("SELECT * FROM CHAOS_BOX_GENESIS WHERE Account='%s' and Name='%s'", lpMsg->account, lpMsg->name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();

		pMsg.Itens = 0;
	}
	else {
		gQueryManager.GetAsBinary("Items", pMsg.ChaosBoxItem[0], sizeof(pMsg.ChaosBoxItem));

		pMsg.State = gQueryManager.GetAsInteger("State");

		pMsg.selectedKey = gQueryManager.GetAsInteger("SelectedKey");

		pMsg.selectedComb = gQueryManager.GetAsInteger("SelectedComb");

		pMsg.Itens = 1;

		gQueryManager.Close();
	}

	gSocketManager.DataSend(index, (BYTE*)&pMsg, sizeof(pMsg));
}

void ChaosBoxGenesis::GCChaosBoxGenesisItemSaveRecv(SDHP_CHAOS_BOX_GENESS_ITEM_SAVE_RECV * lpMsg)
{
	if (gQueryManager.ExecQuery("SELECT * FROM CHAOS_BOX_GENESIS WHERE Account='%s' and Name='%s'", lpMsg->account, lpMsg->name) == 0 || gQueryManager.Fetch() == SQL_NO_DATA)
	{
		gQueryManager.Close();

		gQueryManager.ExecQuery("INSERT INTO CHAOS_BOX_GENESIS (Account,Name, State, SelectedKey, SelectedComb) VALUES ('%s', '%s', %d, %d, %d)", lpMsg->account, lpMsg->name, lpMsg->State, lpMsg->selectedKey, lpMsg->selectedComb);
		gQueryManager.Close();

		gQueryManager.ExecQuery("UPDATE CHAOS_BOX_GENESIS SET Items=CONVERT(varbinary(%d),REPLICATE(char(0xFF),%d)) WHERE Account='%s' AND Name='%s'", sizeof(lpMsg->ChaosBoxItem), sizeof(lpMsg->ChaosBoxItem), lpMsg->account, lpMsg->name);
		gQueryManager.Close();

		gQueryManager.BindParameterAsBinary(1, lpMsg->ChaosBoxItem[0], sizeof(lpMsg->ChaosBoxItem));
		gQueryManager.ExecQuery("UPDATE CHAOS_BOX_GENESIS SET Items=? WHERE Account='%s' and Name='%s'", lpMsg->account, lpMsg->name);
		gQueryManager.Close();
	}
	else {
		gQueryManager.Close();

		if (lpMsg->Clean == 0) {
			gQueryManager.ExecQuery("DELETE FROM CHAOS_BOX_GENESIS WHERE Account='%s' and Name='%s'", lpMsg->account, lpMsg->name);
			gQueryManager.Close();
		}
		else {
			gQueryManager.BindParameterAsBinary(1, lpMsg->ChaosBoxItem[0], sizeof(lpMsg->ChaosBoxItem));
			gQueryManager.ExecQuery("UPDATE CHAOS_BOX_GENESIS SET Items=?, State=%d, SelectedKey=%d, SelectedComb=%d WHERE Account='%s' and Name='%s'", lpMsg->State, lpMsg->selectedKey, lpMsg->selectedComb, lpMsg->account, lpMsg->name);
			gQueryManager.Close();
		}
	}
}
