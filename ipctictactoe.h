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

// maksymalna liczba u�ytkownik�w
#define MAX_PLAYERS 32

// maksymalna d�ugo�� nazwy u�ytkownika
#define MAX_NAME_LENGTH 32

// maksymalna d�ugo�� wiadomo�ci
#define MAX_MESSAGE_LENGTH 1024


/*
# Wymiana danych
Serwer tworzy kolejk� i wypisuje jej ID, �eby klient wiedzia�, gdzie si� pod��czy�.
Klient te� tworzy swoj� kolejk� i zaraz po otwarciu kolejki serwera wysy�a tam komend� login ze swoim queue_id.
Po wylogowaniu klient nie wy��cza si�sam z siebie, ale czeka na wiadomo�� po�egnaln� (typu 3).

Komunikacja klient -> serwer przez kolejk� serwera, struktura command
Komunikacja serwer -> klient przez kolejk� klienta, struktura message


# Lista komend

KOMENDA                 MTYPE   OPIS                        UWAGI
login [id_kolejki]      2       zalogowanie                 to powinna by� pierwsza komenda wysy�ana po pod��czeniu klienta
logout                  1       wylogowanie                 trzeba odpowiedzie� wiadomo�ci� o mtype = 3, �eby wy��czy� klienta
start [gracz]           1       zacznij gr� z graczem
end [gracz]             1       zako�cz gr� z graczem       je�li gracz chce si� podda�
players                 1       wy�wietl graczy
help                    1       wy�wietl dost�pne komendy
# [x] [y]               1       ruch w grze danego gracza   ruch na pole x y    liczmy od 1. czyli kolumy 1,2,3. Warto je ponumerowa� przy wy�wietlaniu
*/


// struktura komend wysy�anych przez klienta na serwer
// (wys�anie wiadomo�ci te� rozumiemy jako komend�)
struct command {
    long mtype; // zale�nie od typu komendy, opisane wy�ej
    char data[MAX_MESSAGE_LENGTH]; // tre�� komendy
    char username[MAX_NAME_LENGTH]; // nazwa u�ytkownika wysy�aj�cego komend�
};

// struktura wiadomo�ci wysy�anych od serwera do klienta
struct message {
    long mtype;                         //3-wylogowanie,2-ruch/plansza,1-wiadomo��(help, players itp.)
    int gameMatrix[3][3];               //macierz stanu gry wysy�ana przez server
    char to[MAX_NAME_LENGTH];           // nazwa u�ytkownika/kana�u albo puste, je�li to_symbol = *
    char message[MAX_MESSAGE_LENGTH];   // tre�� wiadomo�ci
};

#endif //IPCCHAT_IPCCHAT_H
