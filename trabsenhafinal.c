#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

int main() {
    register short int j, i = 0, k;
    unsigned short int sen[4][4], digCert = 0, digDes = 0;
    char gsen[4];

    printf("\n\t\t\t  BEM VINDO AO JOGO DA SENHA!\n\n");
    printf("Para iniciar, o jogador 1 deve digitar uma senha:\n");
    printf("- A senha deve possuir 4 algarismos de 1 a 6 -\n");
    printf("- Para apagar um algarismo, digite 'e' -\n\n");
    printf("Senha correta: ");
    while (i < 4) {
        gsen[i] = getch();
        if (gsen[i] < '1' || gsen[i] > '6') {
            if (gsen[i] == 'e' && i > 0) {
                printf("\b \b");
                i--;
                continue;
            }
            continue;
        }
        printf("*");
        sen[0][i] = gsen[i] - '0';
        i++;
    }

    printf("\n");

    for (j = 1; j <= 10; j++) {
        printf("\nTentativa %d: ", j);

        i = 0;
        while (i < 4) {
            gsen[i] = getch();
            if (gsen[i] < '1' || gsen[i] > '6') {
                if (gsen[i] == 'e' && i > 0) {
                    printf("\b \b");
                    i--;
                    continue;
                }
                continue;
            }
            printf("%c", gsen[i]);
            sen[1][i] = gsen[i] - '0';
            sen[2][i] = 0;
            sen[3][i] = 0;
            i++;
        }

        for (i = 0; i < 4; i++) {
            if (sen[0][i] == sen[1][i]) {
                digCert++;
                sen[2][i] = 1;
                sen[3][i] = 1;
            }
        }

        for (i = 0; i < 4; i++) {
            for (k = 0; k < 4; k++) {
                if (sen[1][i] == sen[0][k] && sen[2][k] == 0 && sen[3][i] == 0) {
                    digDes++;
                    sen[2][k] = 1;
                    sen[3][i] = 1;
                }
            }
        }

        printf("\nDigitos certos: %hu", digCert);
        printf("\nDigitos deslocados: %hu\n", digDes);

        if (digCert == 4) {
            printf("\n- Jogador desafiado acertou!! -\n\n");

			system("pause");

			return 0;
        }

        digCert = 0;
        digDes = 0;
    }

    printf("\n- Que pena, voce perdeu... -\n\n");

    system("pause");

    return 0;
}
