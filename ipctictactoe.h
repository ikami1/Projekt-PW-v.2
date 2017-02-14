#ifndef IPCTicTacToe_IPCTicTacToe_H
#define IPCTicTacToe_IPCTicTacToe_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/sem.h>

// maksymalna liczba u¿ytkowników
#define MAX_PLAYERS 32

// maksymalna d³ugoœæ nazwy u¿ytkownika
#define MAX_NAME_LENGTH 32

// maksymalna d³ugoœæ wiadomoœci
#define MAX_MESSAGE_LENGTH 1024


/*
# Wymiana danych
Serwer tworzy kolejkê i wypisuje jej ID, ¿eby klient wiedzia³, gdzie siê pod³¹czyæ.
Klient te¿ tworzy swoj¹ kolejkê i zaraz po otwarciu kolejki serwera wysy³a tam komendê login ze swoim queue_id.
Po wylogowaniu klient nie wy³¹cza siê sam z siebie, ale czeka na wiadomoœæ po¿egnaln¹ (typu 3).

Komunikacja klient -> serwer przez kolejkê serwera, struktura command
Komunikacja serwer -> klient przez kolejkê klienta, struktura message


# Lista komend

KOMENDA                 MTYPE   OPIS                        UWAGI
login [id_kolejki]      2       zalogowanie                 to powinna byæ pierwsza komenda wysy³ana po pod³¹czeniu klienta
logout                  1       wylogowanie                 trzeba odpowiedzieæ wiadomoœci¹ o mtype = 3, ¿eby wy³¹czyæ klienta
start [gracz]           1       zacznij grê z graczem
end [gracz]             1       zakoñcz grê z graczem       jeœli gracz chce siê poddaæ
players                 1       wyœwietl graczy
help                    1       wyœwietl dostêpne komendy
# [x] [y]               1       ruch w grze danego gracza   ruch na pole x y    liczmy od 1. czyli kolumy 1,2,3. Warto je ponumerowaæ przy wyœwietlaniu
*/


// struktura komend wysy³anych przez klienta na serwer
// (wys³anie wiadomoœci te¿ rozumiemy jako komendê)
struct command {
    long mtype; // zale¿nie od typu komendy, opisane wy¿ej
    char data[MAX_MESSAGE_LENGTH]; // treœæ komendy
    char username[MAX_NAME_LENGTH]; // nazwa u¿ytkownika wysy³aj¹cego komendê
};

// struktura wiadomoœci wysy³anych od serwera do klienta
struct message {
    long mtype;                         //3-wylogowanie,2-ruch/plansza,1-wiadomoœæ(help, players itp.)
    int gameMatrix[3][3];               //macierz stanu gry wysy³ana przez server
    char to[MAX_NAME_LENGTH];           // nazwa u¿ytkownika/kana³u albo puste, jeœli to_symbol = *
    char message[MAX_MESSAGE_LENGTH];   // treœæ wiadomoœci
};

#endif //IPCCHAT_IPCCHAT_H
