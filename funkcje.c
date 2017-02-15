#include "ipctictactoe.h"

struct player{
    char nick[MAX_NAME_LENGTH];
    int kolejka;
    int czy_gra;
};

struct gra{
    struct player gracz1;
    struct player gracz2;
    int game[3][3];
    int czyjRuch;
};

void znajdz_polecenie(char linia[], char polecenie[]){
    int i=0, j=0, n=0;

    while(linia[i] == ' ' || linia[i] == '\n')
        i+=1;

    while(linia[i] != ' ' && linia[i] != '\n' && linia[i] != '\0' && linia[i] != '\t'){
        polecenie[j] = linia[i];
        i+=1;
        j += 1;
    }

    polecenie[j] = '\0';                //znak '\0' na koniec

    while(linia[i-1] != '\0'){          //i-1 zeby skopiowa³o '\0'
        linia[n] = linia[i];
        n+=1;
        i+=1;
    }
}

int nickZajety(struct player tablica[MAX_PLAYERS], char nickname[]){
    int i=0;
    for(i=0; i<MAX_PLAYERS; i++)
        if(!strcmp(tablica[i].nick,nickname))
            return 1;
    return 0;
}

int znajdzIndeks(struct player tablica[MAX_PLAYERS], char nickname[]){
    int i=0,n = -1;
    for(i=0; i<MAX_PLAYERS; i++)
        if(!strcmp(tablica[i].nick,nickname)){
            n = i;
            break;
        }
    return n;
}

int znajdzGre(struct gra tablica[MAX_PLAYERS/2], char nickname[]){
    int i=0,n = -1;
    for(i=0; i<MAX_PLAYERS/2; i++){
        if(!strcmp(tablica[i].gracz1.nick,nickname)){
            n = i;
            break;
        }
        if(!strcmp(tablica[i].gracz2.nick,nickname)){
            n = i;
            break;
        }
    }
    return n;
}

void wyswietlStan(int gameMatrix[3][3]){
    int i,j;
    char gameMatrix2[3][3];
    for(i=0; i<3; i++)
        for(j=0; j<3; j++){
            if(gameMatrix[i][j] == 1)
                gameMatrix2[i][j] = 'O';
            if(gameMatrix[i][j] == 2)
                gameMatrix2[i][j] = 'X';
            if(gameMatrix[i][j] == 0)
                gameMatrix2[i][j] = ' ';
        }

    for(i=0; i<3; i++)
            printf("%c|%c|%c\n", gameMatrix2[i][0], gameMatrix2[i][1], gameMatrix2[i][2]);
}

int czyWygrana(int gameMatrix[3][3], int czyjaTura){
    int i,j;

    for(i=0; i<3; i++)
        for(j=0; j<3; j++){
            if(gameMatrix[i][j] != czyjaTura)
                break;
            if(j == 2)
                return 1;
        }

    for(i=0; i<3; i++)
        for(j=0; j<3; j++){
            if(gameMatrix[j][i] != czyjaTura)
                break;
            if(j == 2)
                return 1;
        }

    for(i=0, j=0; i<3; i++, j++){
        if(gameMatrix[i][j] != czyjaTura)
            break;
        if(i == 2)
            return 1;
    }

    for(i=2, j=2; i>=0; i--, j--){
        if(gameMatrix[i][j] != czyjaTura)
            break;
        if(i == 0)
            return 1;
    }
    return 0;
}

int czyRemis(int gameMatrix[3][3]){
    int i, j;

    for(i=0; i<3; i++){
        for(j=0; j<3; j++){
            if(gameMatrix[i][j] == 0)
                return 0;
        }
    }
    return 1;
}

