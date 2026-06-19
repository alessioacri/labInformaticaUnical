//main.c
//Questo file contiene l'interfaccia utente e coordina i vari moduli del programma.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "biblioteca.h"

int main() {
    printf("Benvenuto nella gestione biblioteca!\n");
    printf("Ecco le opzioni disponibili:\n");
    NodoLista* catalogo = NULL;
    NodoHash* tabellaHash[HASH_SIZE] = { NULL };
    CodaAttesa codaPrestiti;
    initCoda(&codaPrestiti);
    NodoPila* pilaResi = NULL;

    catalogo = caricaDaFileBinario("biblioteca.dat", tabellaHash);
    printf("[SISTEMA] Caricati i dati dal file binario 'biblioteca.dat'.\n");

    int scelta;
    char bufferID[MAX_STR], bufferStringa[MAX_STR], bufferUtente[MAX_STR];

    //inserisco uno switch-case dentro ad un do-while per gestire il menu
    do {
        printf("\n=== GESTIONALE BIBLIOTECA ===\n");
        printf("1. Inserisci un nuovo libro\n");
        printf("2. Cerca un libro (per titolo/autore)\n");
        printf("3. Cerca libro istantaneo (per ID)\n");
        printf("4. Gestione Prestiti e Restituzioni\n");
        printf("5. Mostra cronologia ultimi libri restituiti (pila)\n");
        printf("6. Stampa catalogo ordinato alfabeticamente\n");
        printf("7. Mostra statistiche biblioteca (ricorsione)\n");
        printf("8. Rimuovi interamente un libro dal catalogo\n");
        printf("9. Riduci di 1 il numero di copie (Scarto copia rovinata)\n");
        printf("10. Salva ed Esci\n");
        printf("Scegli un'opzione: ");

        scanf("%d", &scelta);
        getchar();

        switch(scelta) {
            case 1: {
                Libro nuovoLibro;
                printf("Inserisci ID/ISBN univoco: ");
                fgets(nuovoLibro.id, MAX_STR, stdin); nuovoLibro.id[strcspn(nuovoLibro.id, "\n")] = 0;
                printf("Inserisci Titolo: ");
                fgets(nuovoLibro.titolo, MAX_STR, stdin); nuovoLibro.titolo[strcspn(nuovoLibro.titolo, "\n")] = 0;
                printf("Inserisci Autore: ");
                fgets(nuovoLibro.autore, MAX_STR, stdin); nuovoLibro.autore[strcspn(nuovoLibro.autore, "\n")] = 0;
                printf("Inserisci Anno di pubblicazione: ");
                scanf("%d", &nuovoLibro.anno);
                printf("Inserisci Numero di copie: ");
                scanf("%d", &nuovoLibro.copie);
                nuovoLibro.stato = DISPONIBILE;

                catalogo = inserisciLibro(catalogo, nuovoLibro, tabellaHash);
                break;
            }
            case 2:
                printf("Inserisci stringa da cercare (Titolo o Autore): ");
                fgets(bufferStringa, MAX_STR, stdin); bufferStringa[strcspn(bufferStringa, "\n")] = 0;
                cercaLibro(catalogo, bufferStringa);
                break;

            case 3: {
                printf("Inserisci ID esatto da cercare: ");
                fgets(bufferID, MAX_STR, stdin); bufferID[strcspn(bufferID, "\n")] = 0;
                Libro* trovato = cercaHash(tabellaHash, bufferID);
                if(trovato != NULL) {
                    printf("[TROVATO O(1)] %s - %s di %s (Copie: %d)\n", trovato->id, trovato->titolo, trovato->autore, trovato->copie);
                } else {
                    printf("[X] Libro non trovato tramite tabella hash.\n");
                }
                break;
            }
            default:
                printf("[X] Opzione non valida.\n");
        }
    } while(scelta != 10);

    return 0;
}
