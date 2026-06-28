// main.c
// Punto di ingresso del programma. Implementa l'interfaccia utente CLI
// Utilizza un menu interattivo
// Coordina tute le funzioni definite nel file "biblioteca.c"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "biblioteca.h"     // Inclusione dell'interfaccia del modulo biblioteca

int main() {
    // --- INIZIALIZZAZIONE DELLE STRUTTURE DATI COMPONENTI IL SISTEMA ---
    // Inizializza il catalogo principale (Lista Concatenata) come vuoto
    NodoLista* catalogo = NULL;
    // Dichiarazione e contestuale inizializzazione della Coda d'Attesa dei prestiti (FIFO)
    CodaAttesa codaPrestiti;
    initCoda(&codaPrestiti);    // Setup dei puntatori head e tail a NULL
    // Inizializza la Pila della cronologia delle restituzioni (LIFO) come vuota
    NodoPila* pilaResi = NULL;

    // --- FASE DI CARICAMENTO DATI ---
    // All'avvio, ricostruisce lo stato del database ripristinando i dati salvati su disco
    catalogo = caricaDaFileBinario("biblioteca.dat");
    printf("[SISTEMA] Caricati i dati dal file binario 'biblioteca.dat'.\n");

    // --- VARIABILI DI SUPPORTO PER IL FLUSSO E I BUFFER DI INPUT ---
    int scelta=0;       // Memorizza il codice dell'opzione inserita nel menu
    // Buffer temporanei per l'acquisizione sicura di stringhe da tastiera
    char bufferID[MAX_STR], bufferStringa[MAX_STR], bufferUtente[MAX_STR];

    // --- CICLO PRINCIPALE DELL'INTERFACCIA UTENTE (MENU INTERATTIVO) ---
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

        // Acquisizione dell'opzione numerica scelta dall'utente
        scanf("%d", &scelta);
        getchar();      // Svuota il carattere 'newline' (\n) rimasto intrappolato nel buffer dello standard input

        // Costrutto switch-case per conntrollare il flusso verso l'azione richiesta
        switch(scelta) {
            case 1: {       // --- INSERIMENTO O AGGIORNAMENTO DI UN LIBRO ---
                Libro nuovoLibro;       // Allocazione record temporaneo sullo Stack
                printf("Inserisci ID/ISBN univoco: ");
                fgets(nuovoLibro.id, MAX_STR, stdin);       //usiamo fgets su stdin per non incorrere negli errori di scanf
                // Rimozione dello \n finale tramite strcspn per ripulire la stringa
                nuovoLibro.id[strcspn(nuovoLibro.id, "\n")] = 0;

                printf("Inserisci Titolo: ");
                fgets(nuovoLibro.titolo, MAX_STR, stdin);
                nuovoLibro.titolo[strcspn(nuovoLibro.titolo, "\n")] = 0;

                printf("Inserisci Autore: ");
                fgets(nuovoLibro.autore, MAX_STR, stdin);
                nuovoLibro.autore[strcspn(nuovoLibro.autore, "\n")] = 0;

                printf("Inserisci Anno: ");
                scanf("%d", &nuovoLibro.anno);

                printf("Inserisci Numero di copie iniziali: ");
                scanf("%d", &nuovoLibro.copieDisponibili);

                // Inizializzazione dei campi di stato e del puntatore alle sotto-liste nominali
                nuovoLibro.stato = DISPONIBILE;
                nuovoLibro.prestitiAttivi = NULL;       // Nuovo libro = nessun utente associato

                // Invoca la funzione di inserimento aggiornando la testa del catalogo
                catalogo = inserisciLibro(catalogo, nuovoLibro);
                break;
            }
            case 2: {       // --- RICERCA TESTUALE PARZIALE (TITOLO/AUTORE) ---
                printf("Inserisci titolo/autore da cercare: ");
                fgets(bufferStringa, MAX_STR, stdin);
                bufferStringa[strcspn(bufferStringa, "\n")] = 0;
                // Scorre il catalogo stampando i record corrispondenti alla sotto-stringa
                cercaLibro(catalogo, bufferStringa);
                break;
            }

            case 3: {       // --- RICERCA TRAMITE ID UNIVOCO ---
                printf("Inserisci ID: ");
                fgets(bufferID, MAX_STR, stdin);
                // Rimozione dello \n finale tramite strcspn per ripulire la stringa
                bufferID[strcspn(bufferID, "\n")] = 0;
                // Cerca il libro e restituisce il suo indirizzo di memoria (Puntatore)
                Libro* trovato = cercaPerID(catalogo, bufferID);
                if (trovato != NULL) {
                    printf("[TROVATO] %s - %s di %s (Copie disponibili a scaffale: %d)\n",
                           trovato->id, trovato->titolo, trovato->autore, trovato->copieDisponibili);
                    // Passa il libro per valore per ispezionare la sotto-lista dei prestiti
                    stampaUtentiPrestito(*trovato);
                } else {
                    printf("[X] Libro non trovato.\n");
                }
                break;
            }
            case 4: {       // --- SOTTO-SISTEMA GESTIONE FLUSSO PRESTITI E RESTITUZIONI ---
                int sottoScelta;
                printf("1. Prendi in prestito un libro\n2. Restituisci un libro\nScegli: ");
                scanf("%d", &sottoScelta);
                getchar();  // Pulisce il newline residuo

                printf("Inserisci ID del libro: ");
                fgets(bufferID, MAX_STR, stdin);
                bufferID[strcspn(bufferID, "\n")] = 0;
                // Ricerca preventiva del volume nel catalogo
                Libro* libroSelezionato = cercaPerID(catalogo, bufferID);

                if (libroSelezionato == NULL) {
                    printf("[X] Libro inesistente nel database.\n");
                    break;  // Interrompe il case corrente tornando al menu
                }

                if (sottoScelta == 1) { // --- SOTTOCASO A: EROGAZIONE PRESTITO ---
                    printf("Inserisci il tuo NOME e COGNOME: ");
                    fgets(bufferUtente, MAX_STR, stdin);
                    bufferUtente[strcspn(bufferUtente, "\n")] = 0;

                    // Verifica della disponibilità fisica del volume a scaffale
                    if (libroSelezionato->copieDisponibili > 0) {
                        libroSelezionato->copieDisponibili--;
                        // Alloca dinamicamente l'utente nella sotto-lista del libro
                        aggiungiPrestitoUtente(libroSelezionato, bufferUtente);

                        // Se le copie si azzerano, aggiorna lo stato logico del volume
                        if (libroSelezionato->copieDisponibili == 0) {
                            libroSelezionato->stato = IN_PRESTITO;
                        }
                        printf("[OK] Prestito registrato a nome di: %s\n", bufferUtente);
                        // Produce la ricevuta fisica su file di testo
                        generaRicevutaTesto(bufferUtente, *libroSelezionato, "PRESTITO CONCESSO");
                    } else {    // --- GESTIONE CODA FIFO IN CASO DI COPIE ESAURITE ---
                        printf("[NON DISPONIBILE] Copie esaurite a scaffale. Ti aggiungiamo alla CODA D'ATTESA: \n");
                        enqueue(&codaPrestiti, bufferUtente);   // Inserimento ordinato nella coda
                    }
                } else if (sottoScelta == 2) { // --- SOTTOCASO B: RESTITUZIONE E VERIFICA NOMINALE ---
                    printf("Inserisci il NOME registrato al momento del prestito: ");
                    fgets(bufferUtente, MAX_STR, stdin);
                    bufferUtente[strcspn(bufferUtente, "\n")] = 0;

                    // Controlla se questo utente ha effettivamente quel libro
                    // Tenta di rimuovere l'utente dalla sotto-lista. Restituisce 1 (Vero) in caso di riscontro positivo
                    if (rimuoviPrestitoUtente(libroSelezionato, bufferUtente)) {
                        libroSelezionato->copieDisponibili++;       // La copia fisica rientra a scaffale
                        libroSelezionato->stato = DISPONIBILE;      // Il libro torna disponibile per tutti

                        // Storicizza l'evento inserendolo in cima alla Pila dei resi (Logica LIFO)
                        pilaResi = pushReso(pilaResi, libroSelezionato->titolo, bufferUtente);
                        printf("[OK] Restituzione accettata da %s.\n", bufferUtente);
                        generaRicevutaTesto(bufferUtente, *libroSelezionato, "RESTITUZIONE LIBRO");

                        // --- GESTIONE AUTOMATICA DELLA CODA FIFO ALL'ATTO DEL RESO ---
                        // Se c'è una coda d'attesa, assegna subito la copia disponibile appena rientrata
                        if (!codaVuota(&codaPrestiti)) {
                            // Estrae l'utente rimasto in attesa da più tempo (Logica First In, First Out)
                            char* prossimoUtente = dequeue(&codaPrestiti);
                            printf("[CODA D'ATTESA] Copia riassegnata immediatamente a: %s\n", prossimoUtente);
                            libroSelezionato->copieDisponibili--;       // La copia viene subito prelevata dal nuovo utente
                            aggiungiPrestitoUtente(libroSelezionato, prossimoUtente);       // Registrazione nominale

                            if(libroSelezionato->copieDisponibili == 0) libroSelezionato->stato = IN_PRESTITO;

                            generaRicevutaTesto(prossimoUtente, *libroSelezionato, "PRESTITO DA CODA");
                            free(prossimoUtente);       // Libera la stringa temporanea generata dalla funzione dequeue
                        }
                    } else {
                        // Protezione da errori di inserimento: il nome non coincideva con i prestiti registrati
                        printf("[X] Errore: Non risulta nessun prestito di questo libro a nome di '%s'.\n", bufferUtente);
                    }
                }
                break;
            }
            case 5:{        // --- VISUALIZZAZIONE CRONOLOGIA STORICA (PEEK DELLA PILA) ---
                // Mostra gli ultimi 5 resi memorizzati seguendo l'ordine inverso di inserimento
                mostraCronologiaResi(pilaResi);
                break;
            }

            case 6: {       // --- ORDINAMENTO ALFABETICO ---
                // Applica il Bubble Sort alterando i dati interni della lista catalogo
                bubbleSort(catalogo);
                printf("[ORDINATO CON BUBBLE SORT] Catalogo riordinato alfabeticamente:\n");
                cercaLibro(catalogo, "");   // Passando una stringa vuota "", mostra tutti i libri indistintamente
                break;
            }

            case 7: {       // --- METRICHE E ANALISI RICORSIVA ---
                // Richiama l'algoritmo ricorsivo per sommare l'intero patrimonio librario
                int totali = contaCopieTotaliRicorsivo(catalogo);
                printf("=== STATISTICHE BIBLIOTECA ===\n");
                printf("Volume totale del patrimonio librario (Scaffali + Utenti): %d v.\n", totali);
                break;
            }
            case 8: {       // --- CANCELLAZIONE RADICALE DI UN RECORD ---
                printf("Inserisci l'ID del libro da eliminare COMPLETAMENTE dal database: ");
                fgets(bufferID, MAX_STR, stdin); bufferID[strcspn(bufferID, "\n")] = 0;
                // Sgancia il libro, svuota la sua sotto-lista prestiti e aggiorna la testa
                catalogo = eliminaLibro(catalogo, bufferID);
                break;
            }

            case 9: {       // --- SCARTO INVENTARIO ---
                printf("Inserisci l'ID del libro per rimuovere una copia fisica rovinata: ");
                fgets(bufferID, MAX_STR, stdin);
                bufferID[strcspn(bufferID, "\n")] = 0;
                // Riduce di uno le copie disponibili, controllando la sicurezza dei prestiti
                catalogo = riduciCopiaLibro(catalogo, bufferID);
                break;
            }

            case 10: {      // --- FASE DI USCITA E SINCRONIZZAZIONE DATI ---
                // Salva lo stato corrente delle strutture dati all'interno del file binario
                salvaSuFileBinario(catalogo, "biblioteca.dat");
                // Scorre tutte le liste e le pile superstiti nello heap deallocandole tramite free
                liberaTutto(catalogo, pilaResi);
                printf("[SISTEMA] Database sincronizzato su 'biblioteca.dat'. Memoria liberata.\n");
                break;
            }

            default:
                printf("[X] Opzione non valida.\n");
        }
    } while (scelta != 10);     // Il ciclo termina solo quando l'utente immette esplicitamente il codice di uscita 10

    return 0;       // Termina il programma restituendo il codice di stato 0 (nessun errore riscontrato)
}
