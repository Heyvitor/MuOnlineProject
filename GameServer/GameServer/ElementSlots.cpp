#include "stdafx.h"
#include "ElementSlots.h"
#include "Util.h"

CElementSlots gElementSlots;

CElementSlots::CElementSlots()
{
	for (int i = 0; i < 4; i++)
	{
		this->m_ElementEquiped[i].active = 0;
		this->m_ElementEquiped[i].pet = 0;
	}

	this->VisualInventorySwitch = 0;
}

CElementSlots::~CElementSlots()
{

}

bool CElementSlots::CheckIsPet(int slot)
{
	int stack = slot - 236;

	if (stack < 0)
	{
		stack = 0;
	}

	if (stack > 3)
	{
		stack = 3;
	}

	if (!this->m_ElementEquiped[stack].active)
	{
		return false;
	}

	if (this->m_ElementEquiped[stack].pet)
	{
		return true;
	}

	return false;
}

bool CElementSlots::CheckCanEquip(int slot)
{
	if (slot < 236 || slot > 239)
	{
		return true;
	}

	int stack = slot - 236;

	if (stack < 0)
	{
		stack = 0;
	}

	if (stack > 3)
	{
		stack = 3;
	}

	if (!this->m_ElementEquiped[stack].active)
	{
		return false;
	}

	return true;
}

void CElementSlots::SetElementSlot(int stack, int active, int pet)
{
	gElementSlots.m_ElementEquiped[stack].active = active;
	gElementSlots.m_ElementEquiped[stack].pet = pet;
}


void CElementSlots::SetVisualInventorySwitch(int active)
{
	gElementSlots.VisualInventorySwitch = active;
}