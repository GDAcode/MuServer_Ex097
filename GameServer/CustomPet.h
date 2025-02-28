#pragma once

#define MAX_CUSTOM_PET 2  // Defina o número máximo de CustomPets

// Estrutura para armazenar as informações do pet customizado
struct CUSTOM_PET_INFO
{
    int Index;         // Índice do pet (posição no array)
    int ItemIndex;     // ID do item que representa o pet
    int BonusDamage;   // Bônus de dano fornecido pelo pet
    int BonusDefense;  // Bônus de defesa fornecido pelo pet
    // Se desejar, adicione outros atributos (ex.: regeneração, velocidade, etc.)
};

class CCustomPet
{
public:
    CCustomPet();
    virtual ~CCustomPet();

    void Init();

    // Carrega os dados de um array já preparado (caso já exista)
    void Load(CUSTOM_PET_INFO* info);

    // Carrega os dados a partir de um arquivo de script
    void CCustomPet::Load(char* path);

    void SetInfo(CUSTOM_PET_INFO info);
    CUSTOM_PET_INFO* GetInfo(int index);
    CUSTOM_PET_INFO* GetInfoByItem(int ItemIndex);

    // Funções para obter os bônus aplicados pelo pet (ajuste a fórmula conforme necessário)
    int GetCustomPetBonusDamage(int ItemIndex, int PetLevel);
    int GetCustomPetBonusDefense(int ItemIndex, int PetLevel);

public:
    CUSTOM_PET_INFO m_CustomPetInfo[MAX_CUSTOM_PET];
};

extern CCustomPet gCustomPet;
