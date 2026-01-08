#pragma once

struct DARK_SPIRIT_INFO 
{
	int ItemIndex;
};

class CCustomDarkSpirit 
{
public:
	CCustomDarkSpirit();
	~CCustomDarkSpirit();

	void SetDarkSpirit(int ItemIndex, int type);
	bool checkIsDarkSpirit(int ItemIndex);
	int checkDarkSpirit(int ItemIndex);

public:
	std::map<int, DARK_SPIRIT_INFO> m_DarkSpiritInfo;
};

extern CCustomDarkSpirit gCustomDarkSpirit;