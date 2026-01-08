#include "stdafx.h"
#include "Pets.h"
#include "MemScript.h"
#include "Util.h"
//#include "DarkSpiritNew.h"

CCustomPet gCustomPet;

CCustomPet::CCustomPet() // OK
{
	this->m_CustomPetInfo.clear();
}


CCustomPet::~CCustomPet() // OK
{

}

void CCustomPet::Load(char* path) // OK
{
	CMemScript* lpMemScript = new CMemScript;

	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}

	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}

	this->m_CustomPetInfo.clear();

	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}

			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}

			CUSTOM_PET_INFO info;

			memset(&info, 0, sizeof(info));

			static int CustomItemIndexCount = 0;

			info.Index = CustomItemIndexCount++;
			info.ItemIndex = lpMemScript->GetNumber();
			info.IsMontaria = lpMemScript->GetAsNumber();
			info.IsImortal = lpMemScript->GetAsNumber();
			info.IncLife = lpMemScript->GetAsNumber();
			info.IncMana = lpMemScript->GetAsNumber();
			info.IncDamageRate = lpMemScript->GetAsNumber();
			info.IncAttackSpeed = lpMemScript->GetAsNumber();
			info.IncDoubleDamageRate = lpMemScript->GetAsNumber();
			info.IncTripleDamageRate = lpMemScript->GetAsNumber();
			info.Experience = lpMemScript->GetAsNumber();
			info.IncResistIgnoreDefense = lpMemScript->GetAsNumber();
			info.IncResistIgnoreSD = lpMemScript->GetAsNumber();
			info.IncResistCriticalDamage = lpMemScript->GetAsNumber();
			info.IncResisteExcellentDamage = lpMemScript->GetAsNumber();
			info.IncBlockStuck = lpMemScript->GetAsNumber();
			info.IncReflectRate = lpMemScript->GetAsNumber();
			info.IncSD = lpMemScript->GetAsNumber();
			info.IncBP = lpMemScript->GetAsNumber();

			this->m_CustomPetInfo.push_back(info);

			gCustomDarkSpirit.SetDarkSpirit(info.ItemIndex, info.IsMontaria);
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}

	delete lpMemScript;
}

int CCustomPet::CheckCustomPetIsMontaria(int ItemIndex) // OK
{
	for (std::vector<CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.begin(); it != this->m_CustomPetInfo.end(); it++)
	{
		if (it->ItemIndex == ItemIndex)
		{
			return it->IsMontaria;
		}
	}
	return 0;
}

bool CCustomPet::CheckCustomPetByItem(int ItemIndex) // OK
{
	for (std::vector<CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.begin(); it != this->m_CustomPetInfo.end(); it++)
	{
		if (it->ItemIndex == ItemIndex)
		{
			return 1;
		}
	}

	return 0;
}

int CCustomPet::CheckCustomPetIsImortal(int ItemIndex) // OK
{
	for (std::vector<CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.begin(); it != this->m_CustomPetInfo.end(); it++)
	{
		if (it->ItemIndex == ItemIndex)
		{
			return it->IsImortal;
		}
	}
	return 0;
}

int CCustomPet::GetCustomPetDamageRate(int ItemIndex) // OK
{
	for (std::vector<CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.begin(); it != this->m_CustomPetInfo.end(); it++)
	{
		if (it->ItemIndex == ItemIndex)
		{
			return it->IncDamageRate;
		}
	}
	return 0;
}

void CCustomPet::CalcCustomPetOption(LPOBJ lpObj, bool flag)
{
	if (flag != 0)
	{
		return;
	}
	for (std::vector<CUSTOM_PET_INFO>::iterator it = this->m_CustomPetInfo.begin(); it != this->m_CustomPetInfo.end(); it++)
	{
		CItem* SlotPrincipal = &lpObj->Inventory[8];
		CItem* SlotPrincipal1 = &lpObj->Inventory[236];
		CItem* SlotPrincipal2 = &lpObj->Inventory[237];

		if (it->IsMontaria < 2)
		{
			if (it->ItemIndex != SlotPrincipal->m_Index)
			{
				continue;
			}

			lpObj->AddLife += it->IncLife;

			lpObj->AddMana += it->IncMana;

			lpObj->PhysiSpeed += it->IncAttackSpeed;

			lpObj->MagicSpeed += it->IncAttackSpeed;

			lpObj->DoubleDamageRate += it->IncDoubleDamageRate;

			lpObj->ResistDoubleDamageRate += it->IncDoubleDamageRate;

			lpObj->TripleDamageRate += it->IncTripleDamageRate;
			
			lpObj->ExperienceRate += it->Experience;

			lpObj->MasterExperienceRate += it->Experience;
			
			lpObj->ResistIgnoreDefenseRate += it->IncResistIgnoreDefense;
			
			lpObj->ResistIgnoreShieldGaugeRate += it->IncResistIgnoreSD;
			
			lpObj->ResistCriticalDamageRate += it->IncResistCriticalDamage;
			
			lpObj->ResistExcellentDamageRate += it->IncResisteExcellentDamage;
			
			lpObj->ResistStunRate += it->IncBlockStuck;
			
			lpObj->DamageReflect += it->IncReflectRate;

			lpObj->AddShield += it->IncSD;

			lpObj->AddBP += it->IncBP;
		}

		if (it->IsMontaria == 2)
		{
			if (it->ItemIndex != SlotPrincipal1->m_Index)
			{
				continue;
			}

			lpObj->AddLife += it->IncLife;

			lpObj->AddMana += it->IncMana;

			lpObj->PhysiSpeed += it->IncAttackSpeed;

			lpObj->MagicSpeed += it->IncAttackSpeed;

			lpObj->DoubleDamageRate += it->IncDoubleDamageRate;

			lpObj->ResistDoubleDamageRate += it->IncDoubleDamageRate;

			lpObj->TripleDamageRate += it->IncTripleDamageRate;

			lpObj->ExperienceRate += it->Experience;

			lpObj->MasterExperienceRate += it->Experience;

			lpObj->ResistIgnoreDefenseRate += it->IncResistIgnoreDefense;

			lpObj->ResistIgnoreShieldGaugeRate += it->IncResistIgnoreSD;

			lpObj->ResistCriticalDamageRate += it->IncResistCriticalDamage;

			lpObj->ResistExcellentDamageRate += it->IncResisteExcellentDamage;

			lpObj->ResistStunRate += it->IncBlockStuck;

			lpObj->DamageReflect += it->IncReflectRate;

			lpObj->AddShield += it->IncSD;

			lpObj->AddBP += it->IncBP;
		}

		if (it->IsMontaria == 3)
		{
			if (it->ItemIndex != SlotPrincipal2->m_Index)
			{
				continue;
			}

			lpObj->AddLife += it->IncLife;

			lpObj->AddMana += it->IncMana;

			lpObj->PhysiSpeed += it->IncAttackSpeed;

			lpObj->MagicSpeed += it->IncAttackSpeed;

			lpObj->DoubleDamageRate += it->IncDoubleDamageRate;

			lpObj->ResistDoubleDamageRate += it->IncDoubleDamageRate;

			lpObj->TripleDamageRate += it->IncTripleDamageRate;

			lpObj->ExperienceRate += it->Experience;

			lpObj->MasterExperienceRate += it->Experience;

			lpObj->ResistIgnoreDefenseRate += it->IncResistIgnoreDefense;

			lpObj->ResistIgnoreShieldGaugeRate += it->IncResistIgnoreSD;

			lpObj->ResistCriticalDamageRate += it->IncResistCriticalDamage;

			lpObj->ResistExcellentDamageRate += it->IncResisteExcellentDamage;

			lpObj->ResistStunRate += it->IncBlockStuck;

			lpObj->DamageReflect += it->IncReflectRate;

			lpObj->AddShield += it->IncSD;

			lpObj->AddBP += it->IncBP;
		}
	}

}