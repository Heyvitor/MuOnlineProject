#pragma once

#include "Protocol.h"

class CCharacterList {
public:
	CCharacterList();
	~CCharacterList();

	void Init();
	static void OpenCharacterSceneData();
	static void MoveCharacterList();
	static void RenderCharacterList();
	void SetCharacterPosition(int slot);
	int GetSelectedIndex() const { return this->ClickedButton; }

private:
	// Constants from Lua
	static const int MAX_CHARACTER_LIST = 5;
	static constexpr float MOUNT_ADD_SIZE = 0.40f;
	static constexpr float FLYING_ADD_SIZE = 0.20f;
	static constexpr float FLYING_ADD_HEIGHT = 80.0f;

	// Button states
	int SelectedButton;
	int ClickedButton;

	// Character messages
	struct CharacterMessages {
		const char* emptyCharacter;
	};

	CharacterMessages messages;

public:
	float MountAddSize;
	float FlyingAddSize;
	float FlyingAddHeight;
	int MaxCharacters;
	int MaxCharactersAccount;

private:
	// Helper functions
	void UpdateProc(int maxCharacter);
	void RenderProc(int maxCharacter);
	void RenderCharacterStatus(float posX, float posY, int index);
	void ChangeCharacterView(int index, float posX, float posY, float posZ);
	void CharacterRotate(int index);
};

extern CCharacterList gCharacterList;