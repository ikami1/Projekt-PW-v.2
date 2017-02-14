#include "funkcje.c"

int main(){
    int idserwer, idklient;
    char bufor[2*MAX_MESSAGE_LENGTH];
    char polecenie[MAX_MESSAGE_LENGTH];

    struct command wiadDoSerwera;
    struct message wiadOdebrana;
    wiadDoSerwera.mtype = 2;

    while(1){
        while(strcmp(polecenie, "login")){
            printf("login [id_kolejki] do zalogowania\n");
            read(1, bufor, sizeof(bufor));
            znajdz_polecenie(bufor, polecenie);
       }

       printf("Podaj swoj nick\n");
       scanf("%s", wiadDoSerwera.username);

       znajdz_polecenie(bufor, polecenie);
       idserwer = atoi(polecenie);

       if((idklient = msgget(IPC_PRIVATE, 0622)) == -1){
          perror("msgget klient");
          exit(1);
       }

       sprintf(wiadDoSerwera.data, "%d", idklient);
       if(msgsnd(idserwer, &wiadDoSerwera, sizeof(wiadDoSerwera.data) + sizeof(wiadDoSerwera.username), 0) == -1){
          perror("msgsnd przesylanie numeru kolejki");
          exit(1);
       }
       if(msgrcv(idklient, &wiadOdebrana, sizeof(wiadOdebrana) - sizeof(long), 3, 0) == -1){
            perror("msgrcv wiadomosc powitalna");
            exit(1);
        }
        printf("%s", wiadOdebrana.message);

        if(wiadOdebrana.mtype == 2)
            break;
        else
            if(msgctl(idklient, IPC_RMID, 0)){
                perror("msgctl logowanie");
                exit(1);
            }
    }

    wiadDoSerwera.mtype = 1;
    if(fork() == 0)
        while(1){
            memset(wiadDoSerwera.data, '\0', 2*MAX_MESSAGE_LENGTH);
            read(1, wiadDoSerwera.data, sizeof(wiadDoSerwera.data));

            if(msgsnd(idserwer, &wiadDoSerwera, sizeof(wiadDoSerwera.data) + sizeof(wiadDoSerwera.username), 0) == -1){
              perror("msgsnd klient do serwera");
              exit(1);
           }
           if(!strcmp(wiadDoSerwera.data,"logout\n"))
              exit(1);
      }
    else
   	  while(1){
          if(msgrcv(idklient, &wiadOdebrana, sizeof(wiadOdebrana) - sizeof(long), -3, 0) == -1){
            perror("msgrcv od serwera");
            exit(1);
          }

          if(wiadOdebrana.mtype == 1)
             printf("%s\n", wiadOdebrana.message);


          if(wiadOdebrana.mtype == 2){
             wyswietlStan(wiadOdebrana.gameMatrix);
          }


          if(wiadOdebrana.mtype == 3){
            if(msgctl(idklienta, IPC_RMID, 0)){
                perror("msgctl klient logout");
                exit(1);
            }
            exit(1);
          }
      }

    return 0;
}
