#include "stdafx.h"
#include "DarkSpiritNew.h"
#include "Util.h"

CCustomDarkSpirit gCustomDarkSpirit;

CCustomDarkSpirit::CCustomDarkSpirit()
{
}

CCustomDarkSpirit::~CCustomDarkSpirit()
{
}

void CCustomDarkSpirit::SetDarkSpirit(int ItemIndex, int type)
{
	if (type == 2)
	{
		DARK_SPIRIT_INFO info;

		info.ItemIndex = ItemIndex;

		gCustomDarkSpirit.m_DarkSpiritInfo.insert(std::make_pair(ItemIndex, info));
	}
}

bool CCustomDarkSpirit::checkIsDarkSpirit(int ItemIndex)
{
	if (ItemIndex == -1) 
	{
		return 0;
	}

	std::map<int, DARK_SPIRIT_INFO>::iterator it = this->m_DarkSpiritInfo.find(ItemIndex);

	if (it == this->m_DarkSpiritInfo.end()) 
	{
		return 0;
	}

	return 1;
}


int CCustomDarkSpirit::checkDarkSpirit(int ItemIndex)
{
	if (ItemIndex == -1)
	{
		return 0;
	}

	std::map<int, DARK_SPIRIT_INFO>::iterator it = this->m_DarkSpiritInfo.find(ItemIndex);

	if (it == this->m_DarkSpiritInfo.end())
	{
		return it->second.ItemIndex;
	}

	return -1;
}
