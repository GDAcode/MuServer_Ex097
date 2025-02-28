#include "stdafx.h"
#include "CustomPet.h"
#include "ReadScript.h"
#include <stdio.h>

CCustomPet gCustomPet;

CCustomPet::CCustomPet()
{
    // Inicializa o array com índices inválidos
    for (int n = 0; n < MAX_CUSTOM_PET; n++)
    {
        this->m_CustomPetInfo[n].Index = -1;
    }
    this->Init();
}

CCustomPet::~CCustomPet()
{
}

void CCustomPet::Init()
{
    // Caso necessário, inicialize valores padrão aqui.
}

// Carrega dados de um array já preparado
void CCustomPet::Load(CUSTOM_PET_INFO* info)
{
    for (int n = 0; n < MAX_CUSTOM_PET; n++)
    {
        this->SetInfo(info[n]);
    }
}

// Carrega os dados a partir de um arquivo de script
void CCustomPet::Load(char* path)
{
    CReadScript* lpReadScript = new CReadScript;
    if (lpReadScript == NULL)
    {
        printf("Falha ao alocar CReadScript para %s\n", path);
        return;
    }
    if (!lpReadScript->Load(path))
    {
        printf("Erro ao carregar o arquivo: %s\n", path);
        delete lpReadScript;
        return;
    }

    int index = 0;
    try
    {
        // Lê cada token até encontrar TOKEN_END ou TOKEN_END_SECTION
        while (true)
        {
            eTokenResult token = lpReadScript->GetToken();
            if (token == TOKEN_END || token == TOKEN_END_SECTION)
                break;

            CUSTOM_PET_INFO info;

            // Exemplo: o script deve ter os dados na seguinte ordem:
            // Pet Index, ItemIndex, BonusDamage, BonusDefense, e possivelmente uma string (que pode ser descartada ou armazenada)
            info.Index = lpReadScript->GetNumber();
            info.ItemIndex = lpReadScript->GetNumber();
            info.BonusDamage = lpReadScript->GetAsNumber();
            info.BonusDefense = lpReadScript->GetAsNumber();
            // Pula a string do model, se houver:
            lpReadScript->GetString();

            this->SetInfo(info);
            index++;
            if (index >= MAX_CUSTOM_PET)
                break;
        }
    }
    catch (...)
    {
        printf(lpReadScript->GetError());
    }

    delete lpReadScript;
}

void CCustomPet::SetInfo(CUSTOM_PET_INFO info)
{
    if (info.Index < 0 || info.Index >= MAX_CUSTOM_PET)
        return;
    this->m_CustomPetInfo[info.Index] = info;
}

CUSTOM_PET_INFO* CCustomPet::GetInfo(int index)
{
    if (index < 0 || index >= MAX_CUSTOM_PET)
        return NULL;
    if (this->m_CustomPetInfo[index].Index != index)
        return NULL;
    return &this->m_CustomPetInfo[index];
}

CUSTOM_PET_INFO* CCustomPet::GetInfoByItem(int ItemIndex)
{
    for (int n = 0; n < MAX_CUSTOM_PET; n++)
    {
        CUSTOM_PET_INFO* lpInfo = this->GetInfo(n);
        if (lpInfo == NULL)
            continue;
        if (lpInfo->ItemIndex == ItemIndex)
            return lpInfo;
    }
    return NULL;
}

int CCustomPet::GetCustomPetBonusDamage(int ItemIndex, int PetLevel)
{
    CUSTOM_PET_INFO* lpInfo = this->GetInfoByItem(ItemIndex);
    if (lpInfo == NULL)
        return 0;
    // Exemplo: bônus de dano proporcional ao nível do pet
    return (lpInfo->BonusDamage * PetLevel);
}

int CCustomPet::GetCustomPetBonusDefense(int ItemIndex, int PetLevel)
{
    CUSTOM_PET_INFO* lpInfo = this->GetInfoByItem(ItemIndex);
    if (lpInfo == NULL)
        return 0;
    return (lpInfo->BonusDefense * PetLevel);
}
