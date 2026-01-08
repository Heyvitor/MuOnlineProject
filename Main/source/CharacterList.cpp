#include "stdafx.h"
#include "CharacterList.h"
#include "Console.h"
#include "Protect.h"
#include "UIMng.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "ZzzOpenglUtil.h"
#include "GlobalText.h"
#include "CharacterManager.h"
#include "Input.h"
#include "UIGuildInfo.h"

static __forceinline bool CharacterList_CheckMouseIn(float x, float y, float w, float h)
{
	return (MouseX >= x && MouseX < (x + w) && MouseY >= y && MouseY < (y + h));
}

// Implementado em `LuaInterface.cpp`:
// int GetWideX() { return GetCenterX(640); }
extern int GetWideX();

static constexpr float CHARACTER_LIST_BASE_X = 365.0f; // mais para esquerda (antes: 470)
static constexpr float CHARACTER_LIST_BASE_Y = 130.0f;  // mais para baixo
static constexpr float CHARACTER_LIST_W = 170.0f;      // background maior
static constexpr float CHARACTER_LIST_H = 42.0f;       // background maior
static constexpr float CHARACTER_LIST_GAP_Y = 46.0f;   // espaçamento maior

static constexpr float CHARACTER_TOPBTN_GAP_X = 6.0f;
static constexpr float CHARACTER_BTN_W = (CHARACTER_LIST_W - CHARACTER_TOPBTN_GAP_X) / 2.0f;
static constexpr float CHARACTER_DELETE_W = CHARACTER_BTN_W;
static constexpr float CHARACTER_DELETE_H = 22.0f;
static constexpr float CHARACTER_DELETE_Y = (CHARACTER_LIST_BASE_Y - (CHARACTER_DELETE_H + 8.0f));
static constexpr float CHARACTER_CREATE_W = CHARACTER_BTN_W;
static constexpr float CHARACTER_CREATE_H = CHARACTER_DELETE_H;
static constexpr float CHARACTER_CREATE_Y = CHARACTER_DELETE_Y;

static constexpr float CHARACTER_ENTER_X = 80.0f;
static constexpr float CHARACTER_ENTER_Y = 400.0f;
static constexpr float CHARACTER_ENTER_W = CHARACTER_LIST_W;
static constexpr float CHARACTER_ENTER_H = CHARACTER_LIST_H;

static void CharacterList_RequestDeleteSelected()
{
	// Implementa a mesma lógica do CharSelMainWin::DeleteCharacter()
	const int selectedIndex = gCharacterList.GetSelectedIndex();
	if (selectedIndex < 0 || CharactersClient[selectedIndex].Object.Live == 0)
	{
		return;
	}

	// garante que o resto do client esteja sincronizado com o selecionado
	SelectedHero = selectedIndex;
	SelectedCharacter = selectedIndex;

	if (CharactersClient[selectedIndex].GuildStatus != G_NONE)
		CUIMng::Instance().PopUpMsgWin(MESSAGE_DELETE_CHARACTER_GUILDWARNING);
	else if (CharactersClient[selectedIndex].CtlCode & (CTLCODE_02BLOCKITEM | CTLCODE_10ACCOUNT_BLOCKITEM))
		CUIMng::Instance().PopUpMsgWin(MESSAGE_DELETE_CHARACTER_ID_BLOCK);
	else
		CUIMng::Instance().PopUpMsgWin(MESSAGE_DELETE_CHARACTER_CONFIRM);
}

CCharacterList gCharacterList;

CCharacterList::CCharacterList()
{
	this->SelectedButton = -1;
	this->ClickedButton = -1;

	// Initialize messages (using Portuguese as default, can be extended for other languages)
	this->messages.emptyCharacter = "Sem personagem";
}

CCharacterList::~CCharacterList()
{

}

int CheckCreateCharacterWindow() 
{
	CUIMng& rUIMng = CUIMng::Instance();

	return rUIMng.m_CharMakeWin.IsShow();
}

void CCharacterList::Init()
{
	// Set configuration values directly
	this->MaxCharacters = MAX_CHARACTER_LIST;
	this->MountAddSize = MOUNT_ADD_SIZE;
	this->FlyingAddSize = FLYING_ADD_SIZE;
	this->FlyingAddHeight = FLYING_ADD_HEIGHT;
}

void CCharacterList::OpenCharacterSceneData()
{
	// CharSelMainWin removido: esta tela é controlada apenas pelo CharacterList
}

void CCharacterList::MoveCharacterList()
{
	if (SceneFlag == CHARACTER_SCENE)
	{
		gCharacterList.UpdateProc(gCharacterList.MaxCharactersAccount);
	}
}

static __forceinline bool CharacterList_CanDeleteSelected()
{
	// Só permite deletar se existir um personagem realmente selecionado e vivo
	const int selectedIndex = gCharacterList.GetSelectedIndex();
	if (selectedIndex < 0)
	{
		return false;
	}

	return (CharactersClient[selectedIndex].Object.Live != 0);
}

static __forceinline bool CharacterList_CanCreateCharacter(int maxCharacter)
{
	const int maxSlots = ((maxCharacter > 5) ? 5 : maxCharacter);
	for (int i = 0; i < maxSlots; ++i)
	{
		if (!CharactersClient[i].Object.Live)
		{
			return true;
		}
	}
	return false;
}

static void CharacterList_RequestCreateCharacter()
{
	CUIMng& rUIMng = CUIMng::Instance();
	rUIMng.ShowWin(&rUIMng.m_CharMakeWin);
}

static __forceinline bool CharacterList_CanEnterSelected()
{
	const int selectedIndex = gCharacterList.GetSelectedIndex();
	return (selectedIndex > -1 && CharactersClient[selectedIndex].Object.Live != 0);
}

static void CharacterList_RequestEnterGame()
{
	// Mantém a seleção do CharacterList (ClickedButton) como fonte da verdade.
	const int selectedIndex = gCharacterList.GetSelectedIndex();
	if (selectedIndex < 0 || CharactersClient[selectedIndex].Object.Live == 0)
	{
		return;
	}

	SelectedHero = selectedIndex;
	SelectedCharacter = selectedIndex;
	::StartGame();
}

void CCharacterList::RenderCharacterList()
{
	if (SceneFlag == CHARACTER_SCENE)
	{
		gCharacterList.RenderProc(gCharacterList.MaxCharactersAccount);
	}
}

void CCharacterList::SetCharacterPosition(int slot)
{
	if (slot == 0)
	{
		// Position for first character
		CharactersClient[slot].Object.Position[0] = 8590.0f;
		CharactersClient[slot].Object.Position[1] = 18785.0f;
		CharactersClient[slot].Object.Position[2] = 75.0f;
		CharactersClient[slot].Object.Angle[2] = 75.0f;
	}
	else
	{
		// Position for other characters
		CharactersClient[slot].Object.Position[0] = 0.0f;
		CharactersClient[slot].Object.Position[1] = 19210.0f;
		CharactersClient[slot].Object.Position[2] = 175.0f;
		CharactersClient[slot].Object.Angle[2] = 75.0f;
	}

	CharactersClient[slot].Object.Scale = 1.15f;
}

void CCharacterList::UpdateProc(int maxCharacter)
{
	if (CheckCreateCharacterWindow() != 0)
	{
		return;
	}

	CInput& rInput = CInput::Instance();
	CUIMng& rUIMng = CUIMng::Instance();

	this->SelectedButton = -1;
	// NÃO resetar ClickedButton aqui: ele é a seleção persistente (senão DELETE/ENTRAR perdem o selecionado).

	// Mostra apenas personagens criados e no máximo 5
	const int maxSlots = ((maxCharacter > 5) ? 5 : maxCharacter);
	float addY = 0.0f;
	const float frame_cx = (float)(2 * GetWideX());

	// Se o selecionado atual não existe mais (ex.: após DELETE), re-seleciona o primeiro vivo.
	if (this->ClickedButton >= 0 && CharactersClient[this->ClickedButton].Object.Live == 0)
	{
		this->ClickedButton = -1;
	}

	// Se não existir nenhum personagem criado, abre a janela de criação (comportamento do CharSelMainWin::UpdateDisplay)
	{
		bool bNobodyCharacter = true;
		for (int i = 0; i < maxSlots; ++i)
		{
			if (CharactersClient[i].Object.Live != 0)
			{
				bNobodyCharacter = false;
				break;
			}
		}

		if (bNobodyCharacter)
		{
			if (!rUIMng.m_CharMakeWin.IsShow())
			{
				rUIMng.ShowWin(&rUIMng.m_CharMakeWin);
			}
			return;
		}
	}

	// Ao abrir a tela: se ainda não há seleção, seleciona automaticamente o primeiro personagem criado
	if (this->ClickedButton < 0)
	{
		for (int i = 0; i < maxSlots; ++i)
		{
			if (CharactersClient[i].Object.Live != 0)
			{
				this->ClickedButton = i;
				SelectedHero = i;
				SelectedCharacter = i;
				break;
			}
		}
	}

	// ESC (teclado) abre o MENU igual o botão MENU (CharSelMainWin.cpp:164-169)
	if (PressKey(VK_ESCAPE))
	{
		if (!(rUIMng.m_MsgWin.IsShow() || rUIMng.m_CharMakeWin.IsShow()
			|| rUIMng.m_SysMenuWin.IsShow() || rUIMng.m_OptionWin.IsShow()))
		{
			::PlayBuffer(SOUND_CLICK01);
			rUIMng.SetSysMenuWinShow(true);
			rUIMng.ShowWin(&rUIMng.m_SysMenuWin);
			return;
		}
	}

	// Botão Delete (acima do background da lista)
	{
		const float x = CHARACTER_LIST_BASE_X + frame_cx;
		const float y = CHARACTER_DELETE_Y;
		const float xCreate = x;
		const float xDelete = x + CHARACTER_CREATE_W + CHARACTER_TOPBTN_GAP_X;

		const bool canCreate = CharacterList_CanCreateCharacter(maxSlots);
		const bool canDelete = CharacterList_CanDeleteSelected();

		// Botão CRIAR
		if (CharacterList_CheckMouseIn(xCreate, y, CHARACTER_CREATE_W, CHARACTER_CREATE_H))
		{
			if (rInput.IsLBtnDn())
			{
				if (canCreate)
				{
					::PlayBuffer(SOUND_CLICK01);
					CharacterList_RequestCreateCharacter();
				}
				return;
			}
		}

		// Botão DELETE
		if (CharacterList_CheckMouseIn(xDelete, y, CHARACTER_DELETE_W, CHARACTER_DELETE_H))
		{
			if (rInput.IsLBtnDn())
			{
				if (canDelete)
				{
					::PlayBuffer(SOUND_CLICK01);
					CharacterList_RequestDeleteSelected();
				}
				return;
			}
		}
	}

	// Botão ENTRAR (posição fixa solicitada)
	{
		const float x = CHARACTER_ENTER_X + frame_cx;
		const float y = CHARACTER_ENTER_Y;
		if (CharacterList_CheckMouseIn(x, y, CHARACTER_ENTER_W, CHARACTER_ENTER_H) && rInput.IsLBtnDn())
		{
			if (CharacterList_CanEnterSelected())
			{
				CharacterList_RequestEnterGame();
			}
			return;
		}
	}

	for (int i = 0; i < maxSlots; i++)
	{
		// Não mostra / não interage com slots vazios
		if (CharactersClient[i].Object.Live == 0)
		{
			continue;
		}

		const int x = (int)(CHARACTER_LIST_BASE_X + frame_cx);
		const int y = (int)(CHARACTER_LIST_BASE_Y + addY);

		if (CharacterList_CheckMouseIn((float)x, (float)y, CHARACTER_LIST_W, CHARACTER_LIST_H))
		{
			this->SelectedButton = i;

			// Segurando o botão: rotaciona o personagem
			if (MouseLButton)
			{
				this->CharacterRotate(i);
			}

			// 1 clique já seleciona
			if (rInput.IsLBtnDn())
			{
				::PlayBuffer(SOUND_CLICK01);

				if (CharactersClient[i].Object.Live)
				{
					this->ClickedButton = i;
					CharactersClient[i].Action = 207;

					SelectedHero = i;
					SelectedCharacter = i;

					// Mantém apenas o selecionado visível e com "zoom" fixo
					for (int n = 0; n < this->MaxCharacters; n++)
					{
						if (n == i)
						{
							CharactersClient[n].Object.Visible = true;
							this->ChangeCharacterView(n, 8590.0f, 18785.0f, 60.0f);
						}
						else
						{
							CharactersClient[n].Object.Visible = false;
							// manda pra fora da tela/cena só por segurança (além de Visible=false)
							this->ChangeCharacterView(n, 0.0f, 0.0f, 60.0f);
						}
					}
				}
				else
				{
					CUIMng& rUIMng = CUIMng::Instance();
					rUIMng.ShowWin(&rUIMng.m_CharMakeWin);
				}
			}
		}

		addY += CHARACTER_LIST_GAP_Y;
	}

	// Mantém SEMPRE o zoom/posição fixos do selecionado e evita "desselecionar" ao clicar fora:
	// (alguns fluxos do client podem mexer em SelectedHero; aqui a seleção do CharacterList é a fonte da verdade).
	if (this->ClickedButton >= 0 && CharactersClient[this->ClickedButton].Object.Live != 0)
	{
		SelectedHero = this->ClickedButton;
		SelectedCharacter = this->ClickedButton;

		for (int n = 0; n < this->MaxCharacters; ++n)
		{
			if (n == this->ClickedButton)
			{
				CharactersClient[n].Object.Visible = true;
				this->ChangeCharacterView(n, 8590.0f, 18785.0f, 60.0f);
			}
			else
			{
				CharactersClient[n].Object.Visible = false;
				this->ChangeCharacterView(n, 0.0f, 0.0f, 60.0f);
			}
		}
	}
}

void CCharacterList::RenderCharacterStatus(float posX, float posY, int index)
{
	char szLevel[16] = { 0 };
	sprintf_s(szLevel, "%d", (int)CharactersClient[index].Level);

	// Tamanho padrão (como estava)
	g_pRenderText->SetFont(g_hFontBold);

	// RenderColorRadius() usa DisableTexture() -> precisamos re-habilitar textura pro texto aparecer
	EnableAlphaTest();
	// RenderColorRadius também altera o glColor (preto/alpha). Reset para branco antes de desenhar texto/bitmap.
	glColor4f(1.f, 1.f, 1.f, 1.f);

	// Nome (top, centralizado) - no lugar onde era a classe
	if (this->ClickedButton == index)
	{
		g_pRenderText->SetTextColor(0, 255, 0, 255);
	}
	else
	{
		g_pRenderText->SetTextColor(255, 255, 255, 255);
	}
	g_pRenderText->RenderText((int)posX, (int)(posY + 0), CharactersClient[index].ID, (int)CHARACTER_LIST_W, 0, 3, 0);

	// Classe (bottom, centralizado) - onde era o nome
	g_pRenderText->SetTextColor(255, 255, 255, 255);
	g_pRenderText->RenderText((int)posX, (int)(posY + 18), gCharacterManager.GetCharacterClassText(CharactersClient[index].Class), (int)CHARACTER_LIST_W, 0, 3, 0);

	// Level (bottom-right)
	g_pRenderText->SetTextColor(255, 189, 25, 255);
	g_pRenderText->RenderText((int)(posX + (CHARACTER_LIST_W - 20.0f)), (int)(posY + 18), szLevel, (int)CHARACTER_LIST_W, 0, 7, 0);
}

void CCharacterList::ChangeCharacterView(int index, float posX, float posY, float posZ)
{
	CharactersClient[index].Object.Position[0] = posX;
	CharactersClient[index].Object.Position[1] = posY;
	CharactersClient[index].Object.Position[2] = posZ;
	CharactersClient[index].Object.Angle[0] = 0.30f;
	CharactersClient[index].Object.Angle[1] = 0.30f;
	CharactersClient[index].Object.Angle[2] = 75.0f;
}

void CCharacterList::CharacterRotate(int index)
{
	CharactersClient[index].Object.Angle[2] += 5.0f;
}

void CCharacterList::RenderProc(int maxCharacter)
{
	if (CheckCreateCharacterWindow() != 0)
	{
		return;
	}

	const float frame_cx = (float)(2 * GetWideX());
	BeginBitmap();
	EnableAlphaTest();
	glColor3f(1.0f, 1.0f, 1.0f);

	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(0, 0, 0, 0);
	g_pRenderText->SetTextColor(255, 255, 255, 255);

	// Botão Delete (acima da lista)
	{
		const float x = CHARACTER_LIST_BASE_X + frame_cx;
		const float y = CHARACTER_DELETE_Y;
		const float xCreate = x;
		const float xDelete = x + CHARACTER_CREATE_W + CHARACTER_TOPBTN_GAP_X;

		const bool canCreate = CharacterList_CanCreateCharacter(maxCharacter);
		const bool canDelete = CharacterList_CanDeleteSelected();

		// CRIAR
		{
			float alpha = canCreate ? 0.35f : 0.18f;
			if (canCreate && CharacterList_CheckMouseIn(xCreate, y, CHARACTER_CREATE_W, CHARACTER_CREATE_H))
			{
				alpha = 0.45f;
			}

			RenderColorRadius(xCreate, y, CHARACTER_CREATE_W, CHARACTER_CREATE_H, alpha, 1, 7.0f);
			RenderColorRadius(xCreate + 1.0f, y + 1.0f, CHARACTER_CREATE_W - 2.0f, CHARACTER_CREATE_H - 2.0f, 0.06f, 0, 6.0f);

			EnableAlphaTest();
			glColor4f(1.f, 1.f, 1.f, 1.f);
			g_pRenderText->SetFont(g_hFontBold);
			g_pRenderText->SetTextColor(canCreate ? 255 : 170, canCreate ? 255 : 170, canCreate ? 255 : 170, 255);
			g_pRenderText->RenderText((int)xCreate, (int)(y + 5.0f), "CRIAR", (int)CHARACTER_CREATE_W, 0, 3, 0);
		}

		// DELETE
		{
			float alpha = canDelete ? 0.35f : 0.18f;
			if (canDelete && CharacterList_CheckMouseIn(xDelete, y, CHARACTER_DELETE_W, CHARACTER_DELETE_H))
			{
				alpha = 0.45f;
			}

			RenderColorRadius(xDelete, y, CHARACTER_DELETE_W, CHARACTER_DELETE_H, alpha, 1, 7.0f);
			RenderColorRadius(xDelete + 1.0f, y + 1.0f, CHARACTER_DELETE_W - 2.0f, CHARACTER_DELETE_H - 2.0f, 0.06f, 0, 6.0f);

			EnableAlphaTest();
			glColor4f(1.f, 1.f, 1.f, 1.f);
			g_pRenderText->SetFont(g_hFontBold);
			g_pRenderText->SetTextColor(canDelete ? 255 : 170, canDelete ? 255 : 170, canDelete ? 255 : 170, 255);
			g_pRenderText->RenderText((int)xDelete, (int)(y + 5.0f), "Delete", (int)CHARACTER_DELETE_W, 0, 3, 0);
		}
	}

	// Botão ENTRAR
	{
		const float x = CHARACTER_ENTER_X + frame_cx;
		const float y = CHARACTER_ENTER_Y;

		const bool canEnter = CharacterList_CanEnterSelected();
		float alpha = canEnter ? 0.35f : 0.18f;
		if (canEnter && CharacterList_CheckMouseIn(x, y, CHARACTER_ENTER_W, CHARACTER_ENTER_H))
		{
			alpha = 0.45f;
		}

		RenderColorRadius(x, y, CHARACTER_ENTER_W, CHARACTER_ENTER_H, alpha, 1, 7.0f);
		RenderColorRadius(x + 1.0f, y + 1.0f, CHARACTER_ENTER_W - 2.0f, CHARACTER_ENTER_H - 2.0f, 0.06f, 0, 6.0f);

		EnableAlphaTest();
		glColor4f(1.f, 1.f, 1.f, 1.f);
		g_pRenderText->SetFont(g_hFontBold);
		g_pRenderText->SetTextColor(canEnter ? 255 : 170, canEnter ? 255 : 170, canEnter ? 255 : 170, 255);
		g_pRenderText->RenderText((int)x, (int)(y + 12.0f), "ENTRAR", (int)CHARACTER_ENTER_W, 0, 3, 0);
	}

	// Mostra apenas personagens criados e no máximo 5
	const int maxSlots = ((maxCharacter > 5) ? 5 : maxCharacter);
	float addY = 0.0f;

	for (int i = 0; i < maxSlots; i++)
	{
		// Não desenha slot vazio
		if (CharactersClient[i].Object.Live == 0)
		{
			continue;
		}

		const float x = CHARACTER_LIST_BASE_X + frame_cx;
		const float y = CHARACTER_LIST_BASE_Y + addY;

		float alpha = 0.35f;
		if (SelectedHero == i)
		{
			alpha = 0.55f;
		}
		else if (CharacterList_CheckMouseIn(x, y, CHARACTER_LIST_W, CHARACTER_LIST_H))
		{
			alpha = 0.45f;
		}

		RenderColorRadius(x, y, CHARACTER_LIST_W, CHARACTER_LIST_H, alpha, 1, 7.0f);
		RenderColorRadius(x + 1.0f, y + 1.0f, CHARACTER_LIST_W - 2.0f, CHARACTER_LIST_H - 2.0f, 0.06f, 0, 6.0f);

		// "Zoom" via escala (o Z da cena é sobrescrito em `ZzzScene.cpp` a cada frame)
		CharactersClient[i].Object.Scale = (SelectedHero == i) ? 1.45f : 1.15f;
		this->RenderCharacterStatus(x, y + 10.0f, i);

		addY += CHARACTER_LIST_GAP_Y;
	}

	EndBitmap();
	DisableAlphaBlend();
}

// Set max character and pet size at the end
// SetMaxCharacter(MAX_CHARACTER_LIST);
// SetPetSize(MOUNT_ADD_SIZE, FLYING_ADD_SIZE, FLYING_ADD_HEIGHT);