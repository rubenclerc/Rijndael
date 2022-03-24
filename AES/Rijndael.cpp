//
// Created by travail on 12/03/2022.
//

#include "Rijndael.h"

// Constructeur

/// Constructeur: initialise les paramètres de base
/// \param message
/// \param key
Rijndael::Rijndael(unsigned char key[4][4]) {

    // Remplissage de la clé
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            this->key[row][col] = key[row][col];
        }
    }

    this->round = 0;
}


// Getters/Setters
string Rijndael::getMessage() {
    return this->message;
}

// Chiffrement

/// <summary>
/// Chiffrer un morceau du texte et le renvoie
/// </summary>
/// <param name="morceau">morceau à chiffrer</param>
/// <returns>morceau chiffré</returns>
void Rijndael::chiffrerMorceau(unsigned char morceau[4][4]) {
    
    // Génération des clés
    this->expandKey();

    // Remplissage du state
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            this->state[row][col] = morceau[row][col];
        }
    }

    // Premier round
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            this->state[row][col] ^= this->key[row][col];
        }
    }

    // Rounds principaux
    for (this->round; this->round < 9; this->round++) {
        this->subBytes();
        this->shiftRows();
        this->mixColumns();
        this->addRoundKey(this->round);
    }

    // Round final
    this->subBytes();
    this->shiftRows();
    this->addRoundKey(this->round);

    /*
    // Affichage (pour les tests)
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            printf("%x", this->state[row][col]);
        }
        printf("%s", "\n");
    }
    */
}

/// Chiffre le message à travers les 10 rounds de Rijndael
/// \return message chiffré
string Rijndael::chiffrer(string message) {




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
            this->state[row][col] ^= this->roundKey[round][row][col];
        }
    }
}

// Key schedule

/// <summary>
/// Créé les 10 rounds key
/// </summary>
void Rijndael::expandKey() {

    // Transformation
    unsigned char currentKey[4][4], tmp, previousKey[4][4];
    int col, row;

    // Remplissage clé précédente
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            previousKey[i][j] = key[i][j];
        }
    }

    for (int cle = 0; cle < 10; cle++) {

        // Colonne 1
        for (int i = 0; i < 4; i++) {
            currentKey[i][0] = previousKey[i][3];
        }

        // Rot word
        tmp = currentKey[0][0];
        currentKey[0][0] = currentKey[1][0];
        currentKey[1][0] = currentKey[2][0];
        currentKey[2][0] = currentKey[3][0];
        currentKey[3][0] = tmp;

        // SubBytes
        for (int i = 0; i < 4; i++) {
            col = currentKey[i][0] & 0x0f;
            row = (currentKey[i][0] >> 4) & 0x0f;
            currentKey[i][0] = sBox[row][col];
        }

        // Xor avec la première colonne de la previous key et le Rcon
        for (int line = 0; line < 4; line++) {
            currentKey[line][0] = previousKey[line][0] ^ currentKey[line][0] ^ this->rCon[line][cle];
        }


        // Colonnes 2, 3 & 4
        for (int column = 1; column < 4; column++) {
            for (int line = 0; line < 4; line++) {
                currentKey[line][column] = currentKey[line][column - 1] ^ previousKey[line][column];
            }
        }

        // Passe a la prochaine clé
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                previousKey[i][j] = currentKey[i][j];
            }
        }

        // Ajout de la round key à la liste de rounds keys
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                this->roundKey[cle][r][c] = currentKey[r][c];
            }
        }
    }

}
