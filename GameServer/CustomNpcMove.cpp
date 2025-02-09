#include "stdafx.h"
#include "CustomNpcMove.h"
#include "Map.h"
#include "ReadScript.h"
#include "Message.h"
#include "Notice.h"
#include "Util.h"
#include "ResetTable.h"
#include "CommandManager.h"
#include "Serverinfo.h"

CCustomNpcMove gCustomNpcMove;

CCustomNpcMove::CCustomNpcMove()
{
	this->m_CustomNpcMove.clear();

}

CCustomNpcMove::~CCustomNpcMove()
{

}

void CCustomNpcMove::Load(char* path)

{
	CReadScript* lpReadScript = new CReadScript;


	if (lpReadScript == NULL)
	{
		ErrorMessageBox(READ_SCRIPT_ALLOC_ERROR, path);

		return;
	}

	if (!lpReadScript->Load(path))
	{
		ErrorMessageBox(READ_SCRIPT_FILE_ERROR, path);

		delete lpReadScript;

		return;
	}

	this->m_CustomNpcMove.clear();


	int Index = 0;

	try
	{
		eTokenResult token;

		while (true)
		{
			token = lpReadScript->GetToken();

			if (token == TOKEN_END || token == TOKEN_END_SECTION)
			{
				break;
			}

			NPC_MOVE_INFO info;

			info.MonsterClass = lpReadScript->GetNumber();

			info.Map = lpReadScript->GetAsNumber();

			info.X = lpReadScript->GetAsNumber();

			info.Y = lpReadScript->GetAsNumber();

			info.MoveMap = lpReadScript->GetAsNumber();

			info.MoveX = lpReadScript->GetAsNumber();

			info.MoveY = lpReadScript->GetAsNumber();

			info.MinLevel = lpReadScript->GetAsNumber();

			info.MaxLevel = lpReadScript->GetAsNumber();

			info.MinReset = lpReadScript->GetAsNumber();

			info.MaxReset = lpReadScript->GetAsNumber();

			info.AccountLevel = lpReadScript->GetAsNumber();

			info.PkMove = lpReadScript->GetAsNumber();

			info.IsResetNpc = lpReadScript->GetAsNumber(); // Lendo o valor do .txt
			
			this->m_CustomNpcMove.insert(std::pair<int, NPC_MOVE_INFO>(Index, info));


			Index++;
		}

	}
	catch (...)
	{

		ErrorMessageBox(lpReadScript->GetError());
	}

	delete lpReadScript;


}

void CCustomNpcMove::ProcessNpcReset(LPOBJ lpObj, int MonsterClass)
{
	for (auto it = this->m_CustomNpcMove.begin(); it != this->m_CustomNpcMove.end(); it++)
	{
		auto& npc = it->second;

		if (npc.MonsterClass == MonsterClass && npc.IsResetNpc == 1)
		{
			// Validações básicas
			if (lpObj->Connected < OBJECT_ONLINE)
			{
				return;
			}

			if (lpObj->Interface.use != 0 || lpObj->State == OBJECT_DELCMD || lpObj->DieRegen != 0 || lpObj->Teleport != 0)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(75, lpObj->Lang));

				return;
			}

			// Verifica nível e zen
			int requiredLevel = gResetTable.GetResetLevel(lpObj);
			int requiredMoney = gResetTable.GetResetMoney(lpObj);

			if (lpObj->Level < requiredLevel)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(59, lpObj->Lang), gResetTable.GetResetLevel(lpObj));

				return;
			}

			if (lpObj->Money< requiredMoney)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(58, lpObj->Lang), gResetTable.GetResetMoney(lpObj));

				return;
			}

			// Verifica se o personagem excedeu o limite de resets
			if (lpObj->Reset >= gServerInfo.m_CommandResetLimit[lpObj->AccountLevel])
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(62, lpObj->Lang),gServerInfo.m_CommandResetLimit[lpObj->AccountLevel]);
				return;
			}

			// Chama a função de reset do CommandManager
			gCommandManager.CommandReset(lpObj, ""); // Passa um argumento vazio

			// Log de reset
			LogAdd(LOG_BLACK, "[CommandReset][%s][%s] - (Reset: %d)", lpObj->Account, lpObj->Name, lpObj->Reset);

			// Mensagem de sucesso
			gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(81, lpObj->Lang), lpObj->Reset);

			return;
		}
	}
}

bool CCustomNpcMove::GetNpcMove(LPOBJ lpObj, int MonsterClass, int Map, int X, int Y)
{
	for (std::map<int, NPC_MOVE_INFO>::iterator it = this->m_CustomNpcMove.begin(); it != this->m_CustomNpcMove.end(); it++)
	{
		if (it->second.MonsterClass == MonsterClass && it->second.Map == Map && it->second.X == X && it->second.Y == Y)
		{
			// Se for um NPC de Reset, executa a função de reset
			if (it->second.IsResetNpc == 1)
			{
				this->ProcessNpcReset(lpObj, MonsterClass);
				return 1;
			}

			if (it->second.MinLevel != -1 && lpObj->Level < it->second.MinLevel)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(59, lpObj->Lang), it->second.MinLevel);

				return 1;
			}

			if (it->second.MaxLevel != -1 && lpObj->Level > it->second.MaxLevel)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(60, lpObj->Lang), it->second.MaxLevel);

				return 1;
			}

			if (it->second.MinReset != -1 && lpObj->Reset < it->second.MinReset)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(61, lpObj->Lang), it->second.MinReset);

				return 1;
			}

			if (it->second.MaxReset != -1 && lpObj->Reset > it->second.MaxReset)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(62, lpObj->Lang), it->second.MaxReset);

				return 1;
			}

			if (lpObj->AccountLevel < it->second.AccountLevel)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(57, lpObj->Lang));

				return 1;
			}

			if (it->second.PkMove == 0 && lpObj->PKLevel >= PKLVL_OUTLAW)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(66, lpObj->Lang));

				return 1;
			}

			if (lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(65, lpObj->Lang));

				return 1;
			}

			if (it->second.MoveMap == MAP_ATLANS && (lpObj->Inventory[8].m_Index == GET_ITEM(13, 2) || lpObj->Inventory[8].m_Index == GET_ITEM(13, 3))) // Uniria, Dinorant
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(67, lpObj->Lang));

				return 1;
			}

			if (it->second.MoveMap == MAP_ICARUS && (lpObj->Inventory[7].IsItem() == 0 && lpObj->Inventory[8].m_Index != GET_ITEM(13, 3))) // Dinorant
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(68, lpObj->Lang));

				return 1;
			}

			if (it->second.MoveMap == MAP_ICARUS && lpObj->Inventory[8].m_Index == GET_ITEM(13, 2)) // Uniria
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, gMessage.GetTextMessage(67, lpObj->Lang));

				return 1;
			}

			gObjTeleport(lpObj->Index, it->second.MoveMap, it->second.MoveX, it->second.MoveY);

			return 1;

		}
	}

	return 0;
}