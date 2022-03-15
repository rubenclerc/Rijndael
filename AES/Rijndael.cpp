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
    this->addRoundKey(this->round);
    this->round++;

    // Rounds principaux
    for (this->round; this->round < 10; this->round++) {
        this->subBytes();
        this->shiftRows();
        this->mixColumns();
        this->addRoundKey(this->round);
    }

    // Round final
    this->subBytes();
    this->shiftRows();
    this->addRoundKey(this->round);

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
void Rijndael::addRoundKey(int round) {

    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            this->state[row][col] = this->roundKey.at(round)[row][col];
        }
    }
}

// Key schedule

/// <summary>
/// Créé les 10 rounds key
/// </summary>
void Rijndael::expandKey() {

    // Initialisation
    unsigned char currentKey[4][4], previousKey[4][4], tmp;
    int row, col;

    // Remplissage de la clé précédente
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            previousKey[i][j] = this->key[i][j];
        }
    }

    // 1 boucle = 1 round key créée
    for (int i = 1; i < 11; i++) {

        // Colonne 1

        // Remplissage de la première colonne
        for (int line = 0; line < 4; line++) {
            currentKey[line][0] = previousKey[line][3];
        }

        // Rot word
        tmp = currentKey[0][0];
        currentKey[0][0] = currentKey[1][0];
        currentKey[1][0] = currentKey[2][0];
        currentKey[2][0] = currentKey[3][0];
        currentKey[3][0] = tmp;

        // SubBytes
        for (int j = 0; j < 4; j++) {
            col = currentKey[i][0] & 0x0f;
            row = (currentKey[i][0] >> 4) & 0xff;
            currentKey[i][j] = this->sBox[row][col];
        }

        // Xor avec la première colonne de la previous key et le Rcon
        for (int line = 0; line < 4; line++) {
            currentKey[line][0] = previousKey[line][0] ^ currentKey[line][0] ^ this->rCon[line][i-1];
        }

        // Colonnes 2, 3 & 4
        for (int column = 1; column < 4; column++) {
            for (int line = 0; line < 4; line++) {
                currentKey[line][column] = previousKey[line][column] ^ currentKey[line][column - 1];
            }
        }

        // Ajout de la round key à la liste de rounds keys
        this->roundKey.push_back(currentKey);
        
        // Passe à la prochaine clé
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                previousKey[r][c] = currentKey[r][c];
            }
        }
    }

}
