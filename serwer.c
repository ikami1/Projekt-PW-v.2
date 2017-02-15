#include "funkcje.c"

int main(){
    int idserwera, liczbaUzytkownikow = 0, liczbaGier = 0, i, j;
    char polecenie[MAX_MESSAGE_LENGTH];
    struct command wiadOdebrana;
    struct message wiadWyslana;

    if((idserwera = msgget(IPC_PRIVATE, 0622)) == -1){
       perror("msgget serwer");
       exit(1);
   }
    printf("ID serwera: %d\n", idserwera);

    struct player tabGraczy[MAX_PLAYERS];
    struct gra tabGier[MAX_PLAYERS/2 + 1];

    while(1){
        if(msgrcv(idserwera, &wiadOdebrana, sizeof(wiadOdebrana.data) + sizeof(wiadOdebrana.username), 0, 0) == -1){
            perror("msgrcv serwer");
            exit(1);
        }

        if(wiadOdebrana.mtype == 2){
            if(liczbaUzytkownikow == MAX_PLAYERS){
                wiadWyslana.mtype = 3;
                strcpy(wiadWyslana.to, wiadOdebrana.username);
                strcpy(wiadWyslana.message, "Za duzo uzytkownikow. Sproboj ponownie pozniej\n");
                if(msgsnd(atoi(wiadOdebrana.data), &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd odrzucenie logowania. Za duzo uzytkownikow");
                        exit(1);
                }
                continue;   //przerwanie i odczytanie kolejnej wiadomosci
            }

            if(nickZajety(tabGraczy, wiadOdebrana.username)){
                wiadWyslana.mtype = 3;
                strcpy(wiadWyslana.to, wiadOdebrana.username);
                strcpy(wiadWyslana.message, "Nick zajety. Podaj inny\n");
                if(msgsnd(atoi(wiadOdebrana.data), &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd zajety nick");
                        exit(1);
                }
                continue;   //przerwanie i odczytanie kolejnej wiadomosci
            }

            printf("Zalogowano uzytkownika o nicku %s oraz numerze kolejki %s\n", wiadOdebrana.username, wiadOdebrana.data);
            wiadWyslana.mtype = 2;
            strcpy(wiadWyslana.to, wiadOdebrana.username);
            strcpy(wiadWyslana.message, "Logowanie zakonczone sukcesem\n");
            if(msgsnd(atoi(wiadOdebrana.data), &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                    perror("msgsnd wiadomosc powitalna");
                    exit(1);
            }
            strcpy(tabGraczy[liczbaUzytkownikow].nick, wiadOdebrana.username);
            tabGraczy[liczbaUzytkownikow].kolejka = atoi(wiadOdebrana.data);
            tabGraczy[liczbaUzytkownikow++].czy_gra = 0;
        }

        if(wiadOdebrana.mtype == 1){
            znajdz_polecenie(wiadOdebrana.data, polecenie);
            if(!strcmp(polecenie, "players")){
                wiadWyslana.mtype = 1;
                strcpy(wiadWyslana.to, wiadOdebrana.username);
                for(i=0; i<liczbaUzytkownikow; i++){
                    strcpy(wiadWyslana.message, "");
                    strcpy(wiadWyslana.message, tabGraczy[i].nick);
                    int indeks = znajdzIndeks(tabGraczy, wiadOdebrana.username);
                    if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd players");
                        exit(1);
                    }
                }
            }

            if(!strcmp(polecenie,"logout")){
                wiadWyslana.mtype = 3;
                strcpy(wiadWyslana.to, wiadOdebrana.username);
                strcpy(wiadWyslana.message, "Potwierdzam logout");
                int indeks = znajdzIndeks(tabGraczy, wiadOdebrana.username);
                if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                    perror("msgsnd logout");
                    exit(1);
                }
                int indeksGry = znajdzGre(tabGier, wiadOdebrana.username);
                if(indeksGry != -1){
                    wiadWyslana.mtype = 1;
                    strcpy(wiadWyslana.message, "Wygrales. Przeciwnik sie rozlaczyl");
                    int indeks2;
                    if(!strcmp(tabGier[indeksGry].gracz2.nick, wiadOdebrana.username)){
                        indeks2 = znajdzIndeks(tabGraczy, tabGier[indeksGry].gracz1.nick);
                        if(msgsnd(tabGier[indeksGry].gracz1.kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                            perror("msgsnd logout i przegrana gra");
                            exit(1);
                        }
                    }
                    else{
                        indeks2 = znajdzIndeks(tabGraczy, tabGier[indeksGry].gracz2.nick);
                        if(msgsnd(tabGier[indeksGry].gracz2.kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                            perror("msgsnd logout i przegrana gra");
                            exit(1);
                        }
                    }

                    tabGraczy[indeks2].czy_gra = 0;
                    liczbaGier--;
                    tabGier[indeksGry].gracz1 = tabGier[liczbaGier].gracz1;
                    tabGier[indeksGry].gracz2 = tabGier[liczbaGier].gracz2;
                    for(i=0; i<3; i++)
                        for(j=0; j<3; j++)
                            tabGier[indeksGry].game[i][j] = tabGier[liczbaGier].game[i][j];
                    tabGier[indeksGry].czyjRuch = tabGier[liczbaGier].czyjRuch;
                    tabGier[liczbaGier] = tabGier[MAX_PLAYERS/2];
                }

                strcpy(tabGraczy[indeks].nick,tabGraczy[--liczbaUzytkownikow].nick);
                tabGraczy[indeks].kolejka = tabGraczy[liczbaUzytkownikow].kolejka;
                tabGraczy[indeks].czy_gra = tabGraczy[liczbaUzytkownikow].czy_gra;
                strcpy(tabGraczy[liczbaUzytkownikow].nick, "\0");
            }

            if(!strcmp(polecenie, "help")){
                wiadWyslana.mtype = 1;
                strcpy(wiadWyslana.to, wiadOdebrana.username);
                strcpy(wiadWyslana.message, "KOMENDA                    OPIS\nlogin [id_kolejki]          zalogowanie\nlogout         wylogowanie\nstart [nick_gracza]        nowa gra z graczem\nend [nick_gracza]        koniec gry z graczem");
                int indeks = znajdzIndeks(tabGraczy, wiadOdebrana.username);
                if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd help");
                        exit(1);
                }
                strcpy(wiadWyslana.message,"players                       wyswietl graczy\nhelp                        wyswietl dostepne komendy\n# [x] [y]             ruch na polu o wspolrzednych (x,y) zaczynajac od (1,1)");
                if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd help2");
                        exit(1);
                }
            }

            if(!strcmp(polecenie, "start")){
                znajdz_polecenie(wiadOdebrana.data, polecenie);
                int indeks = znajdzIndeks(tabGraczy, wiadOdebrana.username);
                int indeks2 = znajdzIndeks(tabGraczy, polecenie);
                wiadWyslana.mtype = 1;
                strcpy(wiadWyslana.to, wiadOdebrana.username);
                if(!strcmp(tabGraczy[indeks].nick, tabGraczy[indeks2].nick)){
                    strcpy(wiadWyslana.message, "Nie mozesz grac z samym soba");
                    if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd bledny gracz");
                        exit(1);
                    }
                    continue;
                }
                if(indeks2 == -1){
                    strcpy(wiadWyslana.message, "Nie ma takiego gracza");
                    if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd bledny gracz");
                        exit(1);
                    }
                    continue;
                }
                if(tabGraczy[indeks].czy_gra == 1){
                    strcpy(wiadWyslana.message, "Juz grasz. Mozesz miec tylko 1 gre na raz");
                    if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd wielokrotna gra");
                        exit(1);
                    }
                    continue;
                }
                if(tabGraczy[indeks2].czy_gra == 1){
                    strcpy(wiadWyslana.message, "Przeciwnik gra teraz. Sprobuj pozniej");
                    if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd przeciwnik zajety");
                        exit(1);
                    }
                    continue;
                }

                tabGier[liczbaGier].gracz1 = tabGraczy[indeks];
                tabGier[liczbaGier].gracz2 = tabGraczy[indeks2];
                for(i=0; i<3; i++)
                    for(j=0; j<3; j++)
                        tabGier[liczbaGier].game[i][j] = 0;
                tabGier[liczbaGier].czyjRuch = 1;
                tabGraczy[indeks].czy_gra = 1;
                tabGraczy[indeks2].czy_gra = 1;

                wiadWyslana.mtype = 2;
                for(i=0; i<3; i++)
                    for(j=0; j<3; j++)
                        wiadWyslana.gameMatrix[i][j] = tabGier[liczbaGier].game[i][j];
                strcpy(wiadWyslana.message, "Gra rozpoczeta. Zaczynasz");
                if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                    perror("msgsnd nowa gra");
                    exit(1);
                }
                strcpy(wiadWyslana.to, polecenie);
                strcpy(wiadWyslana.message, "Gra rozpoczeta. Zaczyna przeciwnik");
                if(msgsnd(tabGraczy[indeks2].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                    perror("msgsnd nowa gra2");
                    exit(1);
                }
                liczbaGier++;
            }

            if(!strcmp(polecenie, "end")){
                znajdz_polecenie(wiadOdebrana.data, polecenie);
                int indeksGry = znajdzGre(tabGier, wiadOdebrana.username);
                int indeks = znajdzIndeks(tabGraczy, wiadOdebrana.username);
                int indeks2 = znajdzIndeks(tabGraczy, polecenie);
                int kolejka2 = tabGraczy[indeks2].kolejka;
                wiadWyslana.mtype = 1;
                strcpy(wiadWyslana.to, wiadOdebrana.username);
                if(tabGraczy[indeks].czy_gra == 0){
                    strcpy(wiadWyslana.message, "Nie grasz teraz");
                    if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd end, brak gry");
                        exit(1);
                    }
                }

                tabGraczy[indeks].czy_gra = 0;
                tabGraczy[indeks2].czy_gra = 0;
                for(i=0; i<3; i++)
                    for(j=0; j<3; j++)
                        wiadWyslana.gameMatrix[i][j] = tabGier[indeksGry].game[i][j];
                liczbaGier--;
                tabGier[indeksGry].gracz1 = tabGier[liczbaGier].gracz1;
                tabGier[indeksGry].gracz2 = tabGier[liczbaGier].gracz2;
                for(i=0; i<3; i++)
                    for(j=0; j<3; j++)
                        tabGier[indeksGry].game[i][j] = tabGier[liczbaGier].game[i][j];
                tabGier[indeksGry].czyjRuch = tabGier[liczbaGier].czyjRuch;
                tabGier[liczbaGier] = tabGier[MAX_PLAYERS/2];

                wiadWyslana.mtype = 2;
                strcpy(wiadWyslana.message, "Gra zakonczona. Przegrywasz");
                if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                    perror("msgsnd end");
                    exit(1);
                }
                strcpy(wiadWyslana.to, polecenie);
                strcpy(wiadWyslana.message, "Gra zakonczona przez przeciwnika. Wygrywasz");
                if(msgsnd(kolejka2, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                    perror("msgsnd end2");
                    exit(1);
                }
            }
            if(!strcmp(polecenie, "#")){
                int indeksGry = znajdzGre(tabGier, wiadOdebrana.username);
                int indeks = znajdzIndeks(tabGraczy, wiadOdebrana.username);
                int kolejka2, indeks2;
                if(tabGier[indeksGry].czyjRuch == 1){
                    kolejka2 = tabGier[indeksGry].gracz2.kolejka;
                    indeks2 = znajdzIndeks(tabGraczy, tabGier[indeksGry].gracz2.nick);
                }
                else{
                    kolejka2 = tabGier[indeksGry].gracz1.kolejka;
                    indeks2 = znajdzIndeks(tabGraczy, tabGier[indeksGry].gracz1.nick);
                }

                znajdz_polecenie(wiadOdebrana.data, polecenie);
                int x = atoi(polecenie) - 1;
                znajdz_polecenie(wiadOdebrana.data, polecenie);
                int y = atoi(polecenie) - 1;

                if(tabGraczy[indeks].czy_gra == 0){
                    wiadWyslana.mtype = 1;
                    strcpy(wiadWyslana.message, "Nie prowadzisz zadnej gry");
                    if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd brak gry");
                        exit(1);
                    }
                    continue;
                }

                if((tabGier[indeksGry].czyjRuch == 1 && !strcmp(tabGier[indeksGry].gracz2.nick, tabGraczy[indeks].nick)) || (tabGier[indeksGry].czyjRuch == 2 && !strcmp(tabGier[indeksGry].gracz1.nick, tabGraczy[indeks].nick))){
                    wiadWyslana.mtype = 1;
                    strcpy(wiadWyslana.message, "Nie twoja kolej. Ruch przeciwnika");
                    if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd ruch przeciwnika");
                        exit(1);
                    }
                    continue;
                }

                if( x>2 || x<0 || y<0 || y>2 || tabGier[indeksGry].game[x][y] != 0 ){
                    wiadWyslana.mtype = 1;
                    strcpy(wiadWyslana.message, "Niedozwolony ruch");
                    if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd end");
                        exit(1);
                    }
                    continue;
                }
                tabGier[indeksGry].game[x][y] = tabGier[indeksGry].czyjRuch;
                for(i=0; i<3; i++)
                    for(j=0; j<3; j++)
                        wiadWyslana.gameMatrix[i][j] = tabGier[indeksGry].game[i][j];

                if(czyWygrana(tabGier[indeksGry].game, tabGier[indeksGry].czyjRuch)){
                    printf("wygrana\n");
                    wiadWyslana.mtype = 2;
                    strcpy(wiadWyslana.message, "Wygrales");
                    if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd wygrana");
                        exit(1);
                    }
                    strcpy(wiadWyslana.message, "Przegrales");
                    if(msgsnd(kolejka2, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd przegrana");
                        exit(1);
                    }

                    tabGraczy[indeks].czy_gra = 0;
                    tabGraczy[indeks2].czy_gra = 0;

                    tabGier[indeksGry].czyjRuch = tabGier[indeksGry].czyjRuch + 1;
                    if(tabGier[indeksGry].czyjRuch == 3)
                        tabGier[indeksGry].czyjRuch = 1;

                    liczbaGier--;
                    tabGier[indeksGry].gracz1 = tabGier[liczbaGier].gracz1;
                    tabGier[indeksGry].gracz2 = tabGier[liczbaGier].gracz2;
                    for(i=0; i<3; i++)
                        for(j=0; j<3; j++)
                            tabGier[indeksGry].game[i][j] = tabGier[liczbaGier].game[i][j];
                    tabGier[indeksGry].czyjRuch = tabGier[liczbaGier].czyjRuch;
                    tabGier[liczbaGier] = tabGier[MAX_PLAYERS/2];
                    continue;
                }

                if(czyRemis(tabGier[indeksGry].game)){
                    wiadWyslana.mtype = 2;
                    strcpy(wiadWyslana.message, "Remis");
                    if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd remis");
                        exit(1);
                    }
                    if(msgsnd(kolejka2, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                        perror("msgsnd remis2");
                        exit(1);
                    }

                    tabGraczy[indeks].czy_gra = 0;
                    tabGraczy[indeks2].czy_gra = 0;

                    liczbaGier--;
                    tabGier[indeksGry].gracz1 = tabGier[liczbaGier].gracz1;
                    tabGier[indeksGry].gracz2 = tabGier[liczbaGier].gracz2;
                    for(i=0; i<3; i++)
                        for(j=0; j<3; j++)
                            tabGier[indeksGry].game[i][j] = tabGier[liczbaGier].game[i][j];
                    tabGier[indeksGry].czyjRuch = tabGier[liczbaGier].czyjRuch;
                    tabGier[liczbaGier] = tabGier[MAX_PLAYERS/2];
                    continue;
                }

                tabGier[indeksGry].czyjRuch = tabGier[indeksGry].czyjRuch + 1;
                if(tabGier[indeksGry].czyjRuch == 3)
                    tabGier[indeksGry].czyjRuch = 1;

                wiadWyslana.mtype = 2;
                strcpy(wiadWyslana.message, " ");
                if(msgsnd(kolejka2, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                    perror("msgsnd ruch przeciwnika");
                    exit(1);
                }
                if(msgsnd(tabGraczy[indeks].kolejka, &wiadWyslana, sizeof(wiadWyslana) - sizeof(long), 0) == -1){
                    perror("msgsnd swoj ruch");
                    exit(1);
                }
            }
        }
    }

    return 0;
}
