
    

    #include <unistd.h> /* pentru open(), exit() */
#include <fcntl.h> /* O_RDWR */
#include <stdio.h> /* perror() */
#include <errno.h>
#include <stdlib.h>

void fatal(char * mesaj_eroare)
{
    perror(mesaj_eroare);
    exit(0);
}

int main(void)
{
    int miner_sursa, miner_destinatie;
    int copiat;
    char buf[1024];

    /* Open primeste ca argumente path-ul catre un fisier
       si un flag, in cazul acesta flag-ul speicica ca urmeaza
       doar sa citim din fisier.
    */
    miner_sursa = open("sursa", O_RDONLY);
    /* O_CREAT este un flag special care specifica faptul ca daca
       fisierul nu exista, vom creea unul noi cu permisiunile 0644
    */
    miner_destinatie = open("destinatie", O_WRONLY | O_CREAT, 0644);
    
    /* Daca open returneaza o valoare mai mica de 0, atunci inseamna
       ca avem o eroare */
    if (miner_sursa < 0 || miner_destinatie < 0)
        fatal("Nu pot deschide un fisier");
        
    /* Fisierele sunt acuma identificate prin cele 2 int-uri,
       miner_sursa si miner_destinatie 
    */
    
    /* Cu ajutorul functie read citim din fisier, in cazul de fata
       citim din fisierul identificat prin miner_sursa date
       si le punem in buf. Citim maxim sizeof(buf), adica 1024 */
    while ((copiat = read(miner_sursa, buf, sizeof(buf)))) {
    
        /* read returneaza numarul de bytes cititi */

        if (copiat < 0)
            fatal("Eroare la citire");
            
         /* read muta si ceea ce numim cursorul, daca de exemplu
           fisierul are 2048 de bytes, primul apel de read va muta
           cursorul pe pozitia 1024, o citire ulterioara va returna
           date de la pozitia 1024 in sus. lseek este o functie speciala
           pentru a interactiona cu cursorul
         */
           
        /* write este similar cu read */
        copiat = write(miner_destinatie, buf, copiat);
        if (copiat < 0)
            fatal("Eroare la scriere");
    }
    
    /* Este good practice sa inchidem fisierele */
    close(miner_sursa);
    close(miner_destinatie);
    return 0;
}