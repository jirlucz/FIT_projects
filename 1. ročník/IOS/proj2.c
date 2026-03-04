//***Zacatek souboru proj2.c***//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include "proj2.h"  

FILE *file;

//***********************//
//  Jiri Kotek 
//  xkotek09
//  IOS2 synchronizace
//  30.4.2023
//***********************//

    //Funkce slouzici ke zvoleni nahodne neprazdne fronty
int LinePicker(){

    //sem_wait(sem_zarazeniDoFronty);
    int arr[2];
    int i = 0;

    
    //sem_wait(sem_Fronta1);
    if((*p_Fronta1) != 0){
        arr[i] = 1;
        i++;
    }
   // sem_post(sem_Fronta1);
    //sem_wait(sem_Fronta2);
    if ((*p_Fronta2) != 0){
        arr[i] = 2;
        i++;
    }
    //sem_post(sem_Fronta2);
    //sem_post(sem_Fronta3);
    if ((*p_Fronta3) != 0)
    {
        arr[i] = 3;
        i++;
    }
   // sem_wait(sem_Fronta3);
    

    if (i == 0)
    {
        //Vsechny fronty jsou prazdne
        //sem_post(sem_zarazeniDoFronty);
        return 0;
    }
    else{
        srand(time(NULL) * i* 3);
        int index = (rand() % i);
        //sem_post(sem_zarazeniDoFronty);
        return arr[index];
    }
}

    //Proces Urednik
void Officer (int UrednikID, int TU){

    //Vypis spusteni procesu
    sem_wait(sem_lineNumber);
    fprintf(file, "%d: U %i: started\n",*p_lineNumber, UrednikID);
    (*p_lineNumber)++ ;
    sem_post(sem_lineNumber);

    int zvolenaFronta;


    //Zacatek cyklu pro urednika
    while (1)
    {

    //Vygenerovani fronty, k obsouzeni
            sem_wait(sem_zarazeniDoFronty);
                zvolenaFronta = LinePicker();
    
        //Pokud jsou vsechny fronty prazdne urednik si bere prestavku
        if (zvolenaFronta == 0){
            sem_post(sem_zarazeniDoFronty);

            sem_wait(sem_lineNumber);
                fprintf(file, "%d: U %i: taking break\n",*p_lineNumber ,UrednikID);
                (*p_lineNumber)++;
            sem_post(sem_lineNumber);

            usleep((rand() % TU) * 1000);   //Doba spanku

            sem_wait(sem_lineNumber);
                fprintf(file, "%d: U %i: break finished\n",*p_lineNumber ,UrednikID);
                (*p_lineNumber)++;
            sem_post(sem_lineNumber);
        }
        //Byla nahodne zvolena neprazdna fronta, ktera bude obslouzena
        else{
            switch (zvolenaFronta)
            {
            case 1:
                (*p_Fronta1)--;
            sem_post(sem_zarazeniDoFronty);
                sem_wait(sem_lineNumber); 
                    fprintf(file, "%d: U %i: serving a service of type 1\n",*p_lineNumber ,UrednikID);
                    (*p_lineNumber)++;
                sem_post(sem_lineNumber);
                    //Prizve zakaznika na prepazku 1
                    sem_post(sem_Fronta1);
                break;
            case 2:
                (*p_Fronta2)--;
            sem_post(sem_zarazeniDoFronty);
                sem_wait(sem_lineNumber); 
                    fprintf(file, "%d: U %i: serving a service of type 2\n",*p_lineNumber ,UrednikID);
                    (*p_lineNumber)++;
                sem_post(sem_lineNumber);
                    //Prizve zakaznika na prepazku 2
                    sem_post(sem_Fronta2);
                break;
            case 3:
                (*p_Fronta3)--;
            sem_post(sem_zarazeniDoFronty);
                sem_wait(sem_lineNumber); 
                    fprintf(file, "%d: U %i: serving a service of type 3\n",*p_lineNumber ,UrednikID);
                    (*p_lineNumber)++;
                sem_post(sem_lineNumber);
                    //Prizve zakaznika na prepazku 3
                    sem_post(sem_Fronta3);
                break;
            
            //Pokud se vyskytne chyba, proces chybu ohlasi a radsi se ukonci
            default:
                fprintf(stderr, "Chyba nahodneho zvoleni neprazdne fronty");
                exit(0);
                break;
            }

            //**Pracuje**//
                srand(time(NULL) * getpid());   //Slouzi ke generovani nahodneho cisla
                usleep(rand() % 10 + 1);        //Ceka v intervalu <0,10>
            //**Dokoncil praci**//
                sem_wait(sem_lineNumber); 
                    fprintf(file, "%d: U %i: service finished\n",*p_lineNumber ,UrednikID);
                    (*p_lineNumber)++;
                sem_post(sem_lineNumber);
        }

        //Kontrola zaplneni posty a jeji oteviraci doby
        sem_wait(sem_lineNumber);
            int PostOpenStatus;
            sem_getvalue(sem_postaOpen, &PostOpenStatus);

            //Pokud je zavreno a necekaji zadni dalsi pracovnici, urednik odchazi domu a ukonci svuj proces
            if(PostOpenStatus == 0 && (*p_pocetZakazniku) == 0){
                fprintf(file, "%d: U %i: going home\n",*p_lineNumber ,UrednikID);
                (*p_lineNumber)++;
                sem_post(sem_lineNumber);
                exit (0);
            }
        sem_post(sem_lineNumber);
    //Konec cyklu urednika
    }

}

    //Proces Zakaznik
void Customer(int ZakaznikID, int TZ){
    
    //Vypis spusteni procesu
    sem_wait(sem_lineNumber); 
        fprintf(file, "%d: Z %i: started\n",*p_lineNumber ,ZakaznikID);
        (*p_lineNumber)++;
    sem_post(sem_lineNumber);

    //pocatecni wait pred postou
    srand(time(NULL) * getpid());       //Slouzi ke generovani nahodneho cisla
    int wait = rand() % (TZ + 1);
    usleep(wait);


    sem_wait(sem_lineNumber);
    //Kontrola otevreni posty
    int sem_postaOpen_value;
    
    sem_getvalue(sem_postaOpen, &sem_postaOpen_value);

    //Pokud je posta uzavrena, zakaznik od odchazi a ukonci svuj proces
    if ( sem_postaOpen_value == 0){ 
        //sem_wait(sem_lineNumber);
            fprintf(file, "%d: Z %i: going home\n",*p_lineNumber ,ZakaznikID);
            (*p_lineNumber)++;
        sem_post(sem_lineNumber);
        exit(0);
    }
    //Jinak si zvoli nahodne jednu ze tri sluzeb a zaradi se do fronty
    else{
        //sem_wait(sem_lineNumber);
            srand(time(NULL) * getpid());   //Slouzi ke generovani nahodneho cisla
            int service = rand() % 3 + 1;
            fprintf(file, "%d: Z %i: entering office for a service %i\n",*p_lineNumber ,ZakaznikID, service);
            (*p_lineNumber)++;
            sem_post(sem_lineNumber);

            sem_wait(sem_zarazeniDoFronty);
                (*p_pocetZakazniku)++;
                
                switch (service)
                {
                case 1:
                    (*p_Fronta1)++;
                    break;
                case 2:
                    (*p_Fronta2)++;
                    break;
                case 3:
                    (*p_Fronta3)++;
                    break;
                
                //Osetreni chyboveho stavu, proces chybu ohlasi a ukonci se
                default:
                    fprintf(stderr, "Chyba prirazeni zakaznika do fronty");
                    exit(0);
                    break;
                }
            sem_post(sem_zarazeniDoFronty);
            
        //sem_post(sem_lineNumber);

        //Podle vyberu sluzby vyckava
        switch (service)
        {
        case 1:
            //Ceka na zavolani urednikem
            sem_wait(sem_Fronta1);
                    //Zakaznik prichazi k prepazce
                    sem_wait(sem_lineNumber);
                        fprintf(file, "%d: Z %i: called by office worker\n",*p_lineNumber ,ZakaznikID);
                    (*p_lineNumber)++;
                    sem_post(sem_lineNumber);

                //Vyrizuje
                srand(time(NULL) * getpid());   //Slouzi ke generovani nahodneho cisla
                usleep(rand() % 10 + 1);        //Ceka v intervalu <0,10>

                //Po vyrizeni odchazi od prepazky
                sem_wait(sem_zarazeniDoFronty);
                    (*p_pocetZakazniku)--;       
                sem_post(sem_zarazeniDoFronty);

                    //Odchazi domu
                    sem_wait(sem_lineNumber);
                        fprintf(file, "%d: Z %i: going home\n",*p_lineNumber ,ZakaznikID);
                        (*p_lineNumber)++;
                    sem_post(sem_lineNumber);
                
            exit (0);
            
            break;
        case 2:
            //Ceka na zavolani urednikem
            sem_wait(sem_Fronta2);
                    //Zakaznik prichazi k prepazce
                    sem_wait(sem_lineNumber);
                        fprintf(file, "%d: Z %i: called by office worker\n",*p_lineNumber ,ZakaznikID);
                    (*p_lineNumber)++;
                    sem_post(sem_lineNumber);

                //Vyrizuje
                srand(time(NULL) * getpid());   //Slouzi ke generovani nahodneho cisla
                usleep(rand() % 10 + 1);        //Ceka v intervalu <0,10>

                //Po vyrizeni odchazi od prepazky
                sem_wait(sem_zarazeniDoFronty);
                    (*p_pocetZakazniku)--;
                sem_post(sem_zarazeniDoFronty);

                    //Odchazi domu
                    sem_wait(sem_lineNumber);
                    fprintf(file, "%d: Z %i: going home\n",*p_lineNumber ,ZakaznikID);
                    (*p_lineNumber)++;
                    sem_post(sem_lineNumber);
                
            exit (0);
            break;
        case 3:
            //Ceka na zavolani urednikem
            sem_wait(sem_Fronta3);
                    //Zakaznik prichazi k prepazce
                    sem_wait(sem_lineNumber);
                        fprintf(file, "%d: Z %i: called by office worker\n",*p_lineNumber ,ZakaznikID);
                    (*p_lineNumber)++;
                    sem_post(sem_lineNumber);
                
                //Vyrizuje
                srand(time(NULL) * getpid());   //Slouzi ke generovani nahodneho cisla
                usleep(rand() % 10 + 1);        //Ceka v intervalu <0,10>

                //Po vyrizeni odchazi od prepazky
                sem_wait(sem_zarazeniDoFronty);
                    (*p_pocetZakazniku)--;
                sem_post(sem_zarazeniDoFronty);

                    //Odchazi domu
                    sem_wait(sem_lineNumber);
                    fprintf(file, "%d: Z %i: going home\n",*p_lineNumber ,ZakaznikID);
                    (*p_lineNumber)++;
                    sem_post(sem_lineNumber);
                
            exit (0);
            break;

        //Osetreni chyboveho stavu, proces chybu ohlasi a ukonci se
        default:
            fprintf(stderr, "Chyba vyberu fronty");
            exit(0);
            break;
        }
    }
    //Konec procesu zakaznika
}

    //Funkce k vytvoreni semaforu
void SemInit(){
    if((sem_zarazeniDoFronty = mmap (NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,  MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED){
        fprintf(stderr, "Chyba pri vytvareni semaforu1\n");
        exit(EXIT_FAILURE);
    }
    if((sem_lineNumber = mmap (NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,  MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED){
        fprintf(stderr, "Chyba pri vytvareni semaforu\n");
        exit(EXIT_FAILURE);
    }
    if((sem_postaOpen = mmap (NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,  MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED){
        fprintf(stderr, "Chyba pri vytvareni semaforu\n");
        exit(EXIT_FAILURE);
    }
    if((sem_Fronta1 = mmap (NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,  MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED){
        fprintf(stderr, "Chyba pri vytvareni semaforu\n");
        exit(EXIT_FAILURE);
    }
    if((sem_Fronta2 = mmap (NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,  MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED){
        fprintf(stderr, "Chyba pri vytvareni semaforu\n");
        exit(EXIT_FAILURE);
    }
    if((sem_Fronta3 = mmap (NULL, sizeof(sem_t), PROT_READ | PROT_WRITE,  MAP_ANONYMOUS | MAP_SHARED, -1, 0)) == MAP_FAILED){
        fprintf(stderr, "Chyba pri vytvareni semaforu\n");
        exit(EXIT_FAILURE);
    }
    if((sem_init (sem_zarazeniDoFronty, 1,1)) == -1){
        fprintf(stderr, "Chyba pri inicializaci semaforu\n");
        exit(EXIT_FAILURE);
    }
    if((sem_init (sem_lineNumber, 1,1)) == -1){
        fprintf(stderr, "Chyba pri inicializaci semaforu\n");
        exit(EXIT_FAILURE);
    }
    if((sem_init (sem_postaOpen, 1,1)) == -1){
        fprintf(stderr, "Chyba pri inicializaci semaforu\n");
        exit(EXIT_FAILURE);
    }
    if((sem_init (sem_Fronta1, 1,0)) == -1){
        fprintf(stderr, "Chyba pri inicializaci semaforu\n");
        exit(EXIT_FAILURE);
    }
    if((sem_init (sem_Fronta2, 1,0)) == -1){
        fprintf(stderr, "Chyba pri inicializaci semaforu\n");
        exit(EXIT_FAILURE);
    }
    if((sem_init (sem_Fronta3, 1,0)) == -1){
        fprintf(stderr, "Chyba pri inicializaci semaforu\n");
        exit(EXIT_FAILURE);
    }
}

    //Fuknce ke smazani semaforu a souboru semaforu
void Semclear(){ 
    if ((sem_destroy(sem_zarazeniDoFronty)) == -1){
        fprintf(stderr, "Chyba pri mazani semaforu sem_zarazeniDoFronty\n");
        exit(EXIT_FAILURE);
    }
    if((sem_destroy(sem_lineNumber)) == -1){
        fprintf(stderr, "Chyba pri mazani semaforu sem_lineNumber\n");
        exit(EXIT_FAILURE);
    }
    if((sem_destroy(sem_postaOpen)) == -1){
        fprintf(stderr, "Chyba pri mazani semaforu sem_postaOpen\n");
        exit(EXIT_FAILURE);
    }
    if((sem_destroy(sem_Fronta1)) == -1){
        fprintf(stderr, "Chyba pri mazani semaforu sem_Fronta1\n");
        exit(EXIT_FAILURE);
    }
    if((sem_destroy(sem_Fronta2)) == -1){
        fprintf(stderr, "Chyba pri mazani semaforu sem_Fronta2\n");
        exit(EXIT_FAILURE);
    }
    if((sem_destroy(sem_Fronta3)) == -1){
        fprintf(stderr, "Chyba pri mazani semaforu sem_Fronta3\n");
        exit(EXIT_FAILURE);
    }
    if(munmap(sem_zarazeniDoFronty, sizeof(sem_t)) == -1){
        fprintf(stderr, "Chyba pri mazani sem_zarazeniDoFronty\n");
        exit(EXIT_FAILURE);
    }
    if(munmap(sem_lineNumber, sizeof(sem_t)) == -1){
        fprintf(stderr, "Chyba pri mazani sem_lineNumber\n");
        exit(EXIT_FAILURE);
    }
    if(munmap(sem_postaOpen, sizeof(sem_t)) == -1){
        fprintf(stderr, "Chyba pri mazani sem_postaOpen\n");
        exit(EXIT_FAILURE);
    }
    if(munmap(sem_Fronta1, sizeof(sem_t)) == -1){
        fprintf(stderr, "Chyba pri mazani sem_Fronta1\n");
        exit(EXIT_FAILURE);
    }
    if(munmap(sem_Fronta2, sizeof(sem_t)) == -1){
        fprintf(stderr, "Chyba pri mazani sem_Fronta2\n");
        exit(EXIT_FAILURE);
    }
    if(munmap(sem_Fronta3, sizeof(sem_t)) == -1){
        fprintf(stderr, "Chyba pri mazani sem_Fronta3\n");
        exit(EXIT_FAILURE);
    }
}

    //Inicializace sdilene pameti
void SharedMemInit(){
     if((lineNumber = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        fprintf(stderr, "Chyba pri vytvareni sdilene pameti\n");
        exit(EXIT_FAILURE);
    }

    if((p_lineNumber = (int *)shmat(lineNumber, NULL, 0)) == (void *) -1)
    {
        fprintf(stderr, "Chyba pri vytvareni sdilene pameti\n");
        exit(EXIT_FAILURE);
            }
    *p_lineNumber=1;
    //

    if((pocetZakazniku = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        fprintf(stderr, "Chyba pri vytvareni sdilene pameti\n");
        exit(EXIT_FAILURE);
    }

    if((p_pocetZakazniku = (int *)shmat(pocetZakazniku, NULL, 0)) == (void *) -1)
    {
        fprintf(stderr, "Chyba pri vytvareni sdilene pameti\n");
        exit(EXIT_FAILURE);
            }
    *p_pocetZakazniku=0;
    //

    if((Fronta1 = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        fprintf(stderr, "Chyba pri vytvareni sdilene pameti");
        exit(EXIT_FAILURE);
    }

    if((p_Fronta1 = (int *)shmat(Fronta1, NULL, 0)) == (void *) -1)
    {
        fprintf(stderr, "Chyba pri vytvareni sdilene pameti");
        exit(EXIT_FAILURE);
            }
    *p_Fronta1=0;
    //

    if((Fronta2 = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        fprintf(stderr, "Chyba pri vytvareni sdilene pameti");
        exit(EXIT_FAILURE);
    }

    if((p_Fronta2 = (int *)shmat(Fronta2, NULL, 0)) == (void *) -1)
    {
        fprintf(stderr, "Chyba pri vytvareni sdilene pameti");
        exit(EXIT_FAILURE);
            }
    *p_Fronta2=0;
    //

    if((Fronta3 = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1)
    {
        fprintf(stderr, "Chyba pri vytvareni sdilene pameti");
        exit(EXIT_FAILURE);
    }

    if((p_Fronta3 = (int *)shmat(Fronta3, NULL, 0)) == (void *) -1)
    {
        fprintf(stderr, "Chyba pri vytvareni sdilene pameti");
        exit(EXIT_FAILURE);
            }
    *p_Fronta3=0;
    
}

    //Fuknce ke smazani souboru sdilene pameti
void SharedMemDest(){
    shmdt (p_lineNumber);
    if((shmctl (lineNumber, IPC_RMID, NULL)) == -1){
        fprintf (stderr, "Chyba pri odstranovani sdilene pameti p_lineNumber\n");
        exit(EXIT_FAILURE);
    }

    shmdt (p_pocetZakazniku);
    if((shmctl (pocetZakazniku, IPC_RMID, NULL)) == -1){
        fprintf (stderr, "Chyba pri odstranovani sdilene pameti p_pocetZakazniku\n");
        exit(EXIT_FAILURE);
    }

    shmdt (p_Fronta1);
    if((shmctl (Fronta1, IPC_RMID, NULL)) == -1){
        fprintf (stderr, "Chyba pri odstranovani sdilene pameti p_Fronta1\n");
        exit(EXIT_FAILURE);
    }

    shmdt (p_Fronta2);
    if((shmctl (Fronta2, IPC_RMID, NULL)) == -1){
        fprintf (stderr, "Chyba pri odstranovani sdilene pameti p_Fronta2\n");
        exit(EXIT_FAILURE);
    }

    shmdt (p_Fronta3);
    if((shmctl (Fronta3, IPC_RMID, NULL)) == -1){
        fprintf (stderr, "Chyba pri odstranovani sdilene pameti p_Fronta3\n");
        exit(EXIT_FAILURE);
    }
}

 

int main (int argc,char **argv){

    //Kontrola poctu argumentu
        if (argc != 6){
            fprintf (stderr,"Chyba, neplatny argument\n");
            exit(1);
        }

        char* ptr = NULL;
    //Ziskani promennych ze stdin

        //NZ -> Pocet Zakazniku
            int NZ = strtol(argv[1],&ptr,10);

            if (NZ < 0 || *ptr != '\0'){
                fprintf (stderr,"Chyba, neplatny argument\n");
                exit(1);
            }
            ptr = NULL;
        //NU -> Pocet Uredniku
            int NU = strtol(argv[2],&ptr,10) ;

            if ( NU < 0 || *ptr != '\0'){
                fprintf (stderr,"Chyba, neplatny argument\n");
                exit(1);
            }
            ptr = NULL;
        //TZ -> Maximalni cas po ktery zakaznik po prichodu 
            int TZ = strtol(argv[3],&ptr,10) ;

            if ( TZ < 0 || TZ > 10000|| *ptr != '\0'){
                fprintf (stderr,"Chyba, neplatny argument\n");
                exit(1);
            }
            ptr = NULL;
        //TU -> Maximalni delka prestavky urednika
            int TU = strtol(argv[4],&ptr,10) ;

            if ( TU < 0 || TU > 100 || *ptr != '\0'){
                fprintf (stderr,"Chyba, neplatny argument\n");
                exit(1);
            }
            ptr = NULL;
        //F -> Maximalni cas v ms, po kterem je posta uzavrena pro nove prichozi
            int F = strtol(argv[5],&ptr,10) ;

            if ( F < 0 || F> 10000 || *ptr != '\0'){
                fprintf (stderr,"Chyba, neplatny argument\n");
                exit(1);
            }

    //Pokud jsou parametry v poradku, otevreme soubor
        file = fopen ("proj2.out", "w");
            if(file == NULL){
                fprintf (stderr, "Chyba otevreni souboru\n");
                exit(1);
            } 
        
    //Provedeme inicializaci semaforu a sdilene pameti
       SemInit();
       SharedMemInit();

    //Zakaze se bufferovani souboru
       setbuf(file, NULL);  


    //Vytvoreni procesu uredniku
       for (int u = 1; u <= NU; u++)
       {
            pid_t id = fork();
            if (id == 0){
                Officer(u, TU);
            }
            else if (id == -1) {
                // Vyskytla se chyba při vytváření nového procesu
                fprintf (stderr, "Chyba pri vytvareni noveho procesu urednika\n");
                fclose (file);
                Semclear();
                SharedMemDest();
                exit(1);
        }
       }

    //Vytvoreni zakazniku
       for (int z = 1; z <= NZ; z++)
       {
            pid_t id = fork();
            if (id == 0){
                Customer(z, TZ);
            }
            else if (id == -1) {
                // Vyskytla se chyba při vytváření nového procesu
                fprintf (stderr, "Chyba pri vytvareni noveho procesu zakaznika\n");
                fclose (file);
                Semclear();
                SharedMemDest();
                exit(1);
        }
       }

    //Vypocte oteviraci dobu posty
        usleep((rand() % ((F/2)+1) + (F/2)) * 1000);

        sem_wait(sem_lineNumber);
            sem_wait(sem_postaOpen);
            fprintf (file, "%d: closing\n", *p_lineNumber);
            (*p_lineNumber)++;
        sem_post(sem_lineNumber);

    //Vyckame na ukonceni vsech procesu
        while(wait(NULL) > 0);

    //Uzavreme soubor 
        fclose (file);

    //Provede smazani souboru semaforu a sdilene pameti
        Semclear();
        SharedMemDest();

    return 0;
}


//***Konec souboru proj2.c ***//
 
