//
// Created by travail on 12/03/2022.
//

#include "Rijndael.h"

// Constructeur

/// Constructeur: initialise les paramètres de base
/// \param message
/// \param key
Rijndael::Rijndael() {}


// Getters/Setters
string Rijndael::getMessage() {
    return this->message;
}

// Chiffrement

/// Chiffre le message à travers les 10 rounds de Rijndael
/// \return message chiffré
string Rijndael::encrypt(string message) {

    this->message = message;

    // Premier round
    this->addRoundKey();
    this->round++;

    // Rounds principaux
    for (this->round; this->round < 10; this->round++) {
        this->subBytes();
        this->shiftRows();
        this->mixColumns();
        this->addRoundKey();
    }

    // Round final
    this->subBytes();
    this->shiftRows();
    this->addRoundKey();

    return this->getMessage();
}

/// <summary>
/// Remplace chaque byte dans le state par celui qui lui correspond dans la sBox
/// </summary>
void Rijndael::subBytes() {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int column = this->state[i][j] & 0x0f;
            int line = (this->state[i][j] >> 4) & 0xff;
            this->state[i][j] = this->sBox[line][column];
        }
    }
}

/// <summary>
/// Remplace les dernier n derniers bytes de chaque ligne par les n premiers bytes de cette ligne (n = numéro de ligne)
/// </summary>
void Rijndael::shiftRows() {

    // Ligne 1
    unsigned char tmp = this->state[1][0];

    this->state[1][0] = this->state[1][1];
    this->state[1][1] = this->state[1][2];
    this->state[1][2] = this->state[1][3];
    this->state[1][3] = tmp;

    // Ligne 2
    tmp = this->state[2][0];
    this->state[2][0] = this->state[2][2];
    this->state[2][2] = tmp;
    tmp = this->state[2][1];
    this->state[2][1] = this->state[2][3];
    this->state[2][3] = tmp;

    // Ligne 3
    tmp = this->state[3][0];
    this->state[3][0] = this->state[3][3];
    this->state[3][3] = this->state[3][2];
    this->state[3][2] = this->state[3][1];
    this->state[3][1] = tmp;
}

/// <summary>
/// Chaque colonne est multipliée dans le groupe de Galois par une matrice donnée
/// </summary>
void Rijndael::mixColumns() {

    unsigned char a[4];
    unsigned char b[4];
    unsigned h;
    int col = 0;

    // Mix chaque colonne
    while (col < 4) {
        for (int c = 0; c < 4; c++) {
            a[c] = this->state[c][col];
            h = this->state[c][col] & 0x80;
            b[c] = this->state[c][col] << 1;
            if (h == 0x80) {
                b[c] ^= 0x1B;
            }
        }

        this->state[0][col] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1];
        this->state[1][col] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2];
        this->state[2][col] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3];
        this->state[3][col] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0];

        col++;
    }
}

/// <summary>
/// XOR chaque colonne avec la round key correspondante
/// </summary>
void Rijndael::addRoundKey() {

    // Récupère la clé du round
    unsigned char key = this->roundKey.at(this->round);

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            this->state[row][col] ^= key;
        }
    }

}
