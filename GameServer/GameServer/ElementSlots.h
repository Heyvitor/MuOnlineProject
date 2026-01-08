#pragma once

struct ELEMENT_SLOTS_INFO 
{
	int active;
	int pet;
};

class CElementSlots 
{
public:
	CElementSlots();
	~CElementSlots();

	bool CheckIsPet(int slot);
	bool CheckCanEquip(int slot);
	void SetElementSlot(int stack, int active, int pet);
	void SetVisualInventorySwitch(int active);
public:
	ELEMENT_SLOTS_INFO m_ElementEquiped[4];

	int VisualInventorySwitch;
};

extern CElementSlots gElementSlots;