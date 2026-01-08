#include "stdafx.h"
#include "CustomJewelStack.h"
#include "Item.h"
#include "ItemManager.h"
#include "ItemStack.h"
#include "DSProtocol.h"
#include "Util.h"

CCustomJewelStack gCustomJewelStack;

CCustomJewelStack::CCustomJewelStack()
{

}

CCustomJewelStack::~CCustomJewelStack()
{

}

bool CCustomJewelStack::CheckJewelSplit(LPOBJ lpObj, int SourceSlot, int TargetSlot)
{
	if (INVENTORY_FULL_RANGE(SourceSlot) == 0)
	{
		return 0;
	}

	if (INVENTORY_FULL_RANGE(TargetSlot) == 0)
	{
		return 0;
	}

	if (lpObj->Inventory[SourceSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->Inventory[TargetSlot].IsItem() == 0)
	{
		return 0;
	}

	if (lpObj->Inventory[TargetSlot].m_IsPeriodicItem != 0)
	{
		return 0;
	}

	CItem* lpItemSource = &lpObj->Inventory[SourceSlot];

	if (gItemStack.GetItemMaxStack(lpItemSource->m_Index) == 0) 
	{
		return 0;
	}

	CItem* lpItemTarget = &lpObj->Inventory[TargetSlot];

	if (lpItemSource->m_Index == lpItemTarget->m_Index) {
		int DurSource = (int)lpItemSource->m_Durability;
		int DurTarget = (int)lpItemTarget->m_Durability;

		if ((DurSource + DurTarget) <= gItemStack.GetItemMaxStack(lpItemTarget->m_Index)) 
		{
			if (DurSource == 0) 
			{
				DurSource++;
			}

			if (lpItemTarget->m_Durability == 0) 
			{
				lpItemTarget->m_Durability++;
			}

			lpItemTarget->m_Durability += (float)DurSource;

			lpItemTarget->Convert(lpItemTarget->m_Index, lpItemTarget->m_Option1, lpItemTarget->m_Option2, lpItemTarget->m_Option3, lpItemTarget->m_NewOption, lpItemTarget->m_SetOption, lpItemTarget->m_JewelOfHarmonyOption, lpItemTarget->m_ItemOptionEx, lpItemTarget->m_SocketOption, lpItemTarget->m_SocketOptionBonus);
			return 1;
		}
	}

	return 0;
}

void CCustomJewelStack::UnSplitJewel(CUSTOM_JEWEL_STACK_INFO_RECV * lpMsg, int aIndex)
{
	if (gObjIsConnectedGP(aIndex) == 0) 
	{
		return;
	}

	LPOBJ lpObj = &gObj[aIndex];

	if (lpObj->Interface.use != 0 || lpObj->Interface.type != 0) 
	{
		return;
	}

	if (lpMsg->stack <= 0) 
	{
		return;
	}

	if (INVENTORY_FULL_RANGE(lpMsg->slot) == 0) 
	{
		return;
	}

	CItem* lpItem = &lpObj->Inventory[lpMsg->slot];

	if (lpItem->IsItem() == 0) 
	{
		return;
	}

	if (lpItem->IsLuckyItem() != 0) 
	{
		return;
	}

	if (lpItem->m_IsPeriodicItem != 0) 
	{
		return;
	}

	if ((BYTE)lpItem->m_Durability < lpMsg->stack) 
	{
		return;
	}

	int ItemIndex = lpItem->m_Index;

	int EmptySlots = gItemManager.GetInventoryEmptySlotCount(lpObj);

	int CountJewels = lpMsg->stack;

	if (EmptySlots < lpMsg->stack) 
	{
		CountJewels = EmptySlots;
	}

	lpItem->m_Durability -= (float)CountJewels;

	if (lpItem->m_Durability >= 1) 
	{
		gItemManager.GCItemModifySend(aIndex, lpMsg->slot);
	}
	else 
	{
		gItemManager.InventoryDelItem(aIndex, lpMsg->slot);

		gItemManager.GCItemDeleteSend(aIndex, lpMsg->slot, 1);
	}

	for (int n = 0; n < CountJewels; n++)
	{
		GDCreateItemSend(aIndex, 0xEB, 0, 0, ItemIndex, 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0xFF, 0);
	}
}
