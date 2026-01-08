#pragma once

#include "User.h"
#include "Protocol.h"

struct CUSTOM_JEWEL_STACK_INFO_RECV
{
	PSBMSG_HEAD header;
	BYTE slot;
	BYTE stack;
};

class CCustomJewelStack {
public:
	CCustomJewelStack();
	~CCustomJewelStack();

	bool CheckJewelSplit(LPOBJ lpObj, int SourceSlot, int TargetSlot);
	void UnSplitJewel(CUSTOM_JEWEL_STACK_INFO_RECV* lpMsg, int aIndex);
};

extern CCustomJewelStack gCustomJewelStack;