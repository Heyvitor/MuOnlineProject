#pragma once

#include "DataServerProtocol.h"

#define CHAOS_BOX_GENESIS_SIZE 120
#define MAX_ITEM_OPTION 16

struct SDHP_CHAOS_BOX_GENESIS_ITEM_SAVE_RECV
{
	PSBMSG_HEAD header; // C2:05:60
	WORD index;
	char account[11];
	char name[11];
};

struct SDHP_CHAOS_BOX_GENESIS_GET_ITEM_SAVE_SEND
{
	PSWMSG_HEAD header; // C2:05:32
	WORD index;
	char account[11];
	char name[11];
	BYTE Itens;
	BYTE State;
	int selectedKey;
	int selectedComb;
	BYTE ChaosBoxItem[CHAOS_BOX_GENESIS_SIZE][MAX_ITEM_OPTION];
};

struct SDHP_CHAOS_BOX_GENESS_ITEM_SAVE_RECV
{
	PSWMSG_HEAD header; // C2:05:61
	WORD index;
	char account[11];
	char name[11];
	BYTE ChaosBoxItem[CHAOS_BOX_GENESIS_SIZE][MAX_ITEM_OPTION];
	BYTE State;
	BYTE Clean;
	int selectedKey;
	int selectedComb;
};

class ChaosBoxGenesis {
public:
	ChaosBoxGenesis();
	~ChaosBoxGenesis();

	void GCChaosBoxGenesisGetItemSaveRecv(SDHP_CHAOS_BOX_GENESIS_ITEM_SAVE_RECV * lpMsg, int index);
	void GCChaosBoxGenesisItemSaveRecv(SDHP_CHAOS_BOX_GENESS_ITEM_SAVE_RECV * lpMsg);
};

extern ChaosBoxGenesis gChaosBoxGenesis;