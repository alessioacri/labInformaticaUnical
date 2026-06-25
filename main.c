#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "biblioteca.h"

int main() {
    NodoLista* catalogo = NULL;
    CodaAttesa codaPrestiti;
    initCoda(&codaPrestiti);
    NodoPila* pilaResi = NULL;

    catalogo = caricaDaFileBinario("biblioteca.dat");
    printf("[SISTEMA] Caricati i dati dal file binario 'biblioteca.dat'.\n");

    int scelta;
    char bufferID[MAX_STR], bufferStringa[MAX_STR], bufferUtente[MAX_STR];

    do {
        printf("\n=== GESTIONALE BIBLIOTECA ===\n");
        printf("1. Inserisci un nuovo libro\n");
        printf("2. Cerca un libro (per titolo/autore)\n");
        printf("3. Cerca libro istantaneo (per ID)\n");
        printf("4. Gestione Prestiti e Restituzioni (Resi Nominali)\n");
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
                printf("Inserisci Anno: ");
                scanf("%d", &nuovoLibro.anno);
                printf("Inserisci Numero di copie iniziali: ");
                scanf("%d", &nuovoLibro.copieDisponibili);
                nuovoLibro.stato = DISPONIBILE;
                nuovoLibro.prestitiAttivi = NULL;

                catalogo = inserisciLibro(catalogo, nuovoLibro);
                break;
            }
            case 2:
                printf("Inserisci titolo/autore da cercare: ");
                fgets(bufferStringa, MAX_STR, stdin); bufferStringa[strcspn(bufferStringa, "\n")] = 0;
                cercaLibro(catalogo, bufferStringa);
                break;

            case 3: {
                printf("Inserisci ID: ");
                fgets(bufferID, MAX_STR, stdin); bufferID[strcspn(bufferID, "\n")] = 0;
                Libro* trovato = cercaPerID(catalogo, bufferID);
                if (trovato != NULL) {
                    printf("[TROVATO] %s - %s di %s (Copie disponibili a scaffale: %d)\n",
                           trovato->id, trovato->titolo, trovato->autore, trovato->copieDisponibili);
                    stampaUtentiPrestito(*trovato);
                } else {
                    printf("[X] Libro non trovato.\n");
                }
                break;
            }
            case 4: {
                int sottoScelta;
                printf("1. Prendi in prestito un libro\n2. Restituisci un libro\nScegli: ");
                scanf("%d", &sottoScelta);
                getchar();

                printf("Inserisci ID del libro: ");
                fgets(bufferID, MAX_STR, stdin); bufferID[strcspn(bufferID, "\n")] = 0;
                Libro* libroSelezionato = cercaPerID(catalogo, bufferID);

                if (libroSelezionato == NULL) {
                    printf("[X] Libro inesistente nel database.\n");
                    break;
                }

                if (sottoScelta == 1) { // PRESTITO
                    printf("Inserisci il tuo NOME e COGNOME: ");
                    fgets(bufferUtente, MAX_STR, stdin); bufferUtente[strcspn(bufferUtente, "\n")] = 0;

                    if (libroSelezionato->copieDisponibili > 0) {
                        libroSelezionato->copieDisponibili--;
                        aggiungiPrestitoUtente(libroSelezionato, bufferUtente);

                        if (libroSelezionato->copieDisponibili == 0) {
                            libroSelezionato->stato = IN_PRESTITO;
                        }
                        printf("[OK] Prestito registrato a nome di: %s\n", bufferUtente);
                        generaRicevutaTesto(bufferUtente, *libroSelezionato, "PRESTITO CONCESSO");
                    } else {
                        printf("[NON DISPONIBILE] Copie esaurite a scaffale. Ti aggiungiamo alla CODA D'ATTESA: \n");
                        enqueue(&codaPrestiti, bufferUtente);
                    }
                } else if (sottoScelta == 2) { // RESO NOMINALE
                    printf("Inserisci il NOME registrato al momento del prestito: ");
                    fgets(bufferUtente, MAX_STR, stdin); bufferUtente[strcspn(bufferUtente, "\n")] = 0;

                    // Controlla se questo utente ha effettivamente quel libro
                    if (rimuoviPrestitoUtente(libroSelezionato, bufferUtente)) {
                        libroSelezionato->copieDisponibili++;
                        libroSelezionato->stato = DISPONIBILE;

                        pilaResi = pushReso(pilaResi, libroSelezionato->titolo, bufferUtente);
                        printf("[OK] Restituzione accettata da %s.\n", bufferUtente);
                        generaRicevutaTesto(bufferUtente, *libroSelezionato, "RESTITUZIONE LIBRO");

                        // Se c'è una coda d'attesa, assegna subito la copia disponibile appena rientrata
                        if (!codaVuota(&codaPrestiti)) {
                            char* prossimoUtente = dequeue(&codaPrestiti);
                            printf("[CODA D'ATTESA] Copia riassegnata immediatamente a: %s\n", prossimoUtente);
                            libroSelezionato->copieDisponibili--;
                            aggiungiPrestitoUtente(libroSelezionato, prossimoUtente);
                            if(libroSelezionato->copieDisponibili == 0) libroSelezionato->stato = IN_PRESTITO;
                            generaRicevutaTesto(prossimoUtente, *libroSelezionato, "PRESTITO DA CODA");
                            free(prossimoUtente);
                        }
                    } else {
                        printf("[X] Errore: Non risulta nessun prestito di questo libro a nome di '%s'.\n", bufferUtente);
                    }
                }
                break;
            }
            case 5:
                mostraCronologiaResi(pilaResi);
                break;

            case 6:
                bubbleSort(catalogo);
                printf("[ORDINATO - BUBBLE SORT] Catalogo riordinato alfabeticamente:\n");
                cercaLibro(catalogo, "");
                break;

            case 7: {
                int totali = contaCopieTotaliRicorsivo(catalogo);
                printf("=== STATISTICHE BIBLIOTECA ===\n");
                printf("Volume totale del patrimonio librario (Scaffali + Utenti): %d v.\n", totali);
                break;
            }
            case 8:
                printf("Inserisci l'ID del libro da eliminare COMPLETAMENTE dal database: ");
                fgets(bufferID, MAX_STR, stdin); bufferID[strcspn(bufferID, "\n")] = 0;
                catalogo = eliminaLibro(catalogo, bufferID);
                break;

            case 9:
                printf("Inserisci l'ID del libro per rimuovere una copia fisica rovinata: ");
                fgets(bufferID, MAX_STR, stdin); bufferID[strcspn(bufferID, "\n")] = 0;
                catalogo = riduciCopiaLibro(catalogo, bufferID);
                break;

            case 10:
                salvaSuFileBinario(catalogo, "biblioteca.dat");
                liberaTutto(catalogo, pilaResi);
                printf("[SISTEMA] Database sincronizzato su 'biblioteca.dat'. Memoria liberata.\n");
                break;

            default:
                printf("[X] Opzione non valida.\n");
        }
    } while (scelta != 10);

    return 0;
}
