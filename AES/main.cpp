#include <iostream>

int main() {

	unsigned char state[4][4] = {
		0xf2, 0xdb, 0x01, 0xc6,
		0x0a, 0x13, 0x01, 0xc6,
		0x22, 0x53, 0x01, 0xc6,
		0x5c, 0x45, 0x01, 0xc6
	};

	
	for (int i = 0; i < 4; i++) {

		printf("%s", "\n");

		for (int j = 0; j < 4; j++) {
			printf("%x", state[i][j]);
		}
	}
	

	unsigned char a[4];
	unsigned char b[4];
	unsigned h;
	int col = 0;

	while (col < 4) {
		for (int c = 0; c < 4; c++) {
			a[c] = state[c][col];
			h = state[c][col] & 0x80;
			b[c] = state[c][col] << 1;
			if (h == 0x80) {
				b[c] ^= 0x1B;
			}
		}

		state[0][col] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1];
		state[1][col] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2];
		state[2][col] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3];
		state[3][col] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0];

		col++;
	}
	



	
	for (int i = 0; i < 4; i++) {

		printf("%s", "\n");

		for (int j = 0; j < 4; j++) {
			printf("%x", state[i][j]);
		}
	}
    
    return 0;
}
