#pragma once

#define MAX_CUSTOM_PET 2  // Defina o n�mero m�ximo de CustomPets

// Estrutura para armazenar as informa��es do pet customizado
struct CUSTOM_PET_INFO
{
    int Index;         // �ndice do pet (posi��o no array)
    int ItemIndex;     // ID do item que representa o pet
    int BonusDamage;   // B�nus de dano fornecido pelo pet
    int BonusDefense;  // B�nus de defesa fornecido pelo pet
    // Se desejar, adicione outros atributos (ex.: regenera��o, velocidade, etc.)
};

class CCustomPet
{
public:
    CCustomPet();
    virtual ~CCustomPet();

    void Init();

    // Carrega os dados de um array j� preparado (caso j� exista)
    void Load(CUSTOM_PET_INFO* info);

    // Carrega os dados a partir de um arquivo de script
    void CCustomPet::Load(char* path);

    void SetInfo(CUSTOM_PET_INFO info);
    CUSTOM_PET_INFO* GetInfo(int index);
    CUSTOM_PET_INFO* GetInfoByItem(int ItemIndex);

    // Fun��es para obter os b�nus aplicados pelo pet (ajuste a f�rmula conforme necess�rio)
    int GetCustomPetBonusDamage(int ItemIndex, int PetLevel);
    int GetCustomPetBonusDefense(int ItemIndex, int PetLevel);

public:
    CUSTOM_PET_INFO m_CustomPetInfo[MAX_CUSTOM_PET];
};

extern CCustomPet gCustomPet;
