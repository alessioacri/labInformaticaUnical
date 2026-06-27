// biblioteca.c
// Implementazione delle funzioni principali della biblioteca.
// Gestisce la logica delle liste concatenate, pile, code e file I/O.

#include "biblioteca.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funzioni di gestione della sotto-lista dei prestiti
// Registra un prestito nominale allocando un nodo in testa alla sotto-lista del libro.
void aggiungiPrestitoUtente(Libro* l, char* nome) {
    NodoPrestito* nuovo = (NodoPrestito*)malloc(sizeof(NodoPrestito));
    strcpy(nuovo->nomeUtente, nome);
    nuovo->next = l->prestitiAttivi;
    l->prestitiAttivi = nuovo;
}

// Cerca e rimuove un utente dalla sotto-lista dei prestiti (all'atto del reso).
int rimuoviPrestitoUtente(Libro* l, char* nome) {
    NodoPrestito* curr = l->prestitiAttivi;     // Puntatore di tracciamento per scorrere la lista
    NodoPrestito* prev = NULL;      // Puntatore al nodo precedente, fondamentale per riagganciare i nodi in caso di rimozione
    // Ciclo di scorrimento lineare della lista concatenata dei prestiti
    while (curr != NULL) {
        if (strcmp(curr->nomeUtente, nome) == 0) {
            if (prev == NULL) l->prestitiAttivi = curr->next;
            else prev->next = curr->next;       // Il nodo precedente salta il nodo corrente
            free(curr);     // Deallocazione immediata della memoria occupata dal nodo rimosso
            return 1; // Rimozione riuscita
        }
        prev = curr;            // Il corrente diventa il precedente
        curr = curr->next;      // Il corrente avanza al nodo successivo
    }
    return 0; // Utente non trovato nella lista dei prestiti di questo libro
}

// Scorre e stampa a video tutti gli utenti che possiedono una copia del libro.
void stampaUtentiPrestito(Libro l) {            // Istanza della struct Libro
    NodoPrestito* curr = l.prestitiAttivi;
    if (curr == NULL) return;
    printf("   [Attualmente in prestito a: ");
    while (curr != NULL) {
        // Stampa il nome corrente. Usa l'operatore ternario per formattare la virgola separatrice
        printf("%s%s", curr->nomeUtente, curr->next ? ", " : "");
        curr = curr->next;      // Avanzamento lineare al nodo successivo
    }
    printf("]\n");
}

// --- FUNZIONI DI RICERCA E MODIFICA SUL CATALOGO PRINCIPALE ---
// Effettua una ricerca lineare basata sull'ID del libro.
// return Puntatore alla struct Libro se trovato, NULL altrimenti
Libro* cercaPerID(NodoLista* testa, char* id) {
    NodoLista* curr = testa;
    while (curr != NULL) {
        // Se l'ID combacia perfettamente, restituisce l'indirizzo di memoria della struct
        if (strcmp(curr->info.id, id) == 0) return &(curr->info);
        curr = curr->next;
    }
    return NULL;
}

// Inserisce un libro nel catalogo. Se l'ID esiste già, ne incrementa le copie.
NodoLista* inserisciLibro(NodoLista* testa, Libro l) {
    // Verifica preventiva per intercettare tentativi di inserimento di libri già censiti
    Libro* esistente = cercaPerID(testa, l.id);
    if (esistente != NULL) {
        // Aggiorna sul posto il numero di copie sommandole a quelle esistenti
        esistente->copieDisponibili += l.copieDisponibili;
        esistente->stato = DISPONIBILE;     // Ripristina lo stato logico a Disponibile
        printf("[INFO] Libro gia' presente. Copie aggiornate.\n");
        return testa;
    }

    // Se è un libro non ancora presente, alloca dinamicamente un nuovo nodo per la lista principale
    NodoLista* nuovo = (NodoLista*)malloc(sizeof(NodoLista));
    nuovo->info = l;        // Copia in blocco tutti i campi della struttura dati
    nuovo->info.prestitiAttivi = NULL; // Inizializza la sotto-lista dei prestiti come vuota
    // Inserimento in testa al catalogo
    nuovo->next = testa;
    printf("[OK] Libro inserito con successo nel catalogo.\n");
    return nuovo;       // Restituisce il puntatore al nuovo nodo, che diventa la nuova testa
}

// Esegue una ricerca parziale di testo all'interno dei campi Titolo ed Autore.
void cercaLibro(NodoLista* testa, char* query) {
    NodoLista* curr = testa;
    int trovati = 0;        // Contatore dei match positivi
    while (curr != NULL) {
        if (strstr(curr->info.titolo, query) != NULL || strstr(curr->info.autore, query) != NULL) {
            printf("- [%s] %s di %s (Copie disponibili a scaffale: %d | Stato: %s)\n",
                   curr->info.id, curr->info.titolo, curr->info.autore, curr->info.copieDisponibili,
                   curr->info.stato == DISPONIBILE ? "DISPONIBILE" : "IN PRESTITO (TUTTE LE COPIE)");
                    // operatore ternario per valutare se è disponibile e restituire la stringa adatta.

            // Invoca la sotto-funzione per mostrare gli eventuali possessori correnti
            stampaUtentiPrestito(curr->info);
            trovati++;
        }
        curr = curr->next;
    }
    if (trovati == 0) printf("[X] Nessun libro corrisponde ai criteri di ricerca.\n");
}

// Funzione ausiliaria per svuotare e deallocare la sotto-lista dei prestiti di un libro.
void liberaPrestitiLibro(NodoPrestito* testa) {
    while (testa != NULL) {
        NodoPrestito* tmp = testa;      // Memorizza temporaneamente il nodo corrente
        testa = testa->next;            // Sposta la testa sul nodo successivo
        free(tmp);                      // Libera la memoria del vecchio nodo
    }
}

// Rimuove interamente un nodo (libro) dal catalogo principale.
NodoLista* eliminaLibro(NodoLista* testa, char* id) {
    NodoLista* curr = testa;
    NodoLista* prev = NULL;
    // Algoritmo di rimozione standard da una lista concatenata singola
    while (curr != NULL) {
        if (strcmp(curr->info.id, id) == 0) {
            // Sgancio del nodo dalla catena principale
            if (prev == NULL) testa = curr->next;   // Il libro si trovava in prima posizione
            else prev->next = curr->next;           // Il libro era in mezzo o in fondo

            // !!! Prima di liberare il libro, svuota la sua sotto-lista per evitare memory leak
            liberaPrestitiLibro(curr->info.prestitiAttivi);
            free(curr);
            printf("[OK] Libro interamente rimosso dal database.\n");
            return testa;       // Ritorna il catalogo aggiornato
        }
        prev = curr;
        curr = curr->next;
    }
    printf("[X] Errore: Libro non trovato.\n");
    return testa;
}

// Diminuisce di 1 le copie di un libro (Scarto copia rovinata). Elimina il nodo se arriva a 0.
NodoLista* riduciCopiaLibro(NodoLista* testa, char* id) {
    // Recupera l'indirizzo della struct del libro interessato
    Libro* l = cercaPerID(testa, id);
    if (l == NULL) {
        printf("[X] Errore: Libro non trovato.\n");
        return testa;
    }

    // Se ci sono più copie disponibili, decrementa semplicemente il contatore
    if (l->copieDisponibili > 1) {
        l->copieDisponibili--;
        printf("[OK] Una copia fisica scartata. Copie rimaste a scaffale: %d\n", l->copieDisponibili);
        return testa;
    }
    // Se c'è una sola copia disponibile, eliminiamo il libro dalla biblioteca
    else if (l->copieDisponibili == 1 && l->prestitiAttivi == NULL) {
        printf("[INFO] Questa era l'ultima copia e non ci sono prestiti attivi. Rimozione totale...\n");
        return eliminaLibro(testa, id);
    }
    // Se ci sono copie disponibili, ma sono in prestito agli utenti, non possiamo eliminare il libro
    else {
        printf("[X] Impossibile scartare l'ultima copia fisica: ci sono libri ancora in prestito agli utenti!\n");
        return testa;
    }
}

// --- GESTIONE DELLA CODA D'ATTESA (STRUTTURA DATI LINEARE AD ASTRATTA - FIFO) ---

// Inizializza i descrittori di testa e coda impostandoli a NULL
void initCoda(CodaAttesa* q) { q->head = NULL; q->tail = NULL; }

// Inserisce un utente in fondo alla coda d'attesa (Operazione Enqueue).
void enqueue(CodaAttesa* q, char* nome) {
    // Alloca un nodo per la coda
    NodoCoda* nuovo = (NodoCoda*)malloc(sizeof(NodoCoda));
    strcpy(nuovo->nomeUtente, nome);
    nuovo->next = NULL;     // Essendo l'ultimo nodo, il suo next è nativamente NULL

    // Se la coda è vuota, il nuovo nodo fa sia da head che da tail
    if (q->tail == NULL) { q->head = nuovo; q->tail = nuovo; }
    // Se contiene già elementi, si aggancia all'attuale coda (tail)
    else { q->tail->next = nuovo; q->tail = nuovo; }
}

// Estrae e rimuove l'utente in cima alla coda (Operazione Dequeue).
char* dequeue(CodaAttesa* q) {
    if (q->head == NULL) return NULL;   // Controllo Underflow (coda vuota)
    NodoCoda* temp = q->head;       // Salva il puntatore al nodo da estrarre
    // Alloca dinamicamente una stringa per restituire il nome al main senza perderlo
    char* nome = (char*)malloc(MAX_STR * sizeof(char));
    strcpy(nome, temp->nomeUtente);
    q->head = q->head->next;        // Avanza il puntatore head al nodo successivo
    // Se dopo l'estrazione la coda è diventata vuota, resetta anche il puntatore tail
    if (q->head == NULL) q->tail = NULL;
    free(temp);         // Libera la memoria del nodo rimosso dalla struttura della coda
    return nome;        // Restituisce la stringa contenente il nome estratto
}

// Verifica lo stato di vuoto della coda. Ritorna 1 se vuota, 0 se popolata.
int codaVuota(CodaAttesa* q) { return q->head == NULL; }

// --- GESTIONE DELLA PILA CRONOLOGIA RESI (STRUTTURA DATI ASTRATTA - LIFO) ---
// Inserisce un record di avvenuto reso in cima alla pila (Operazione Push).
NodoPila* pushReso(NodoPila* testa, char* titolo, char* utente) {
    // Allocazione del record storico sulla pila
    NodoPila* nuovo = (NodoPila*)malloc(sizeof(NodoPila));
    strcpy(nuovo->titoloLibro, titolo);
    strcpy(nuovo->nomeUtente, utente);
    // Il nuovo nodo si posiziona sopra l'attuale cima (testa) diventando il nuovo picco della pila
    nuovo->next = testa;
    return nuovo;       // Restituisce il puntatore alla nuova cima della pila
}

// Mostra i dati memorizzati nella pila partendo dalla cima senza alterare la struttura (Operazione Peek/View).
void mostraCronologiaResi(NodoPila* testa) {
    if (testa == NULL) { printf("[INFO] Cronologia resi vuota.\n"); return; }
    printf("--- CRONOLOGIA RECENTI RESTITUZIONI (Pila) ---\n");
    NodoPila* curr = testa;
    int c = 1;      // Contatore per forzare l'arresto della visualizzazione ai primi 5 record

    // Scorrimento della pila dalla cima (LIFO - l'ultimo inserito è il primo visualizzato)
    while (curr != NULL && c <= 5) {
        printf("%d. '%s' restituito da %s\n", c, curr->titoloLibro, curr->nomeUtente);
        curr = curr->next; c++;
    }
}

// --- ALGORITMO DI ORDINAMENTO (BUBBLE SORT SU LISTE CONCATENATE) ---
// Ordina il catalogo principale alfabeticamente per titolo.
void bubbleSort(NodoLista* testa) {
    int scambiato;              // Flag booleano (0/1) per tracciare se sono avvenuti scambi nell'iterazione corrente
    NodoLista* ptr1;            // Puntatore a NodoLista di scansione che scorre i nodi affiancati
    NodoLista* lptr = NULL;     // Delimitatore di fine passata (ottimizzazione: l'ultimo elemento è già stabile)

    // Condizione di arresto immediato: lista vuota o mononodo (già ordinata)
    if (testa == NULL || testa->next == NULL) return;
    do {
        scambiato = 0;      // Presume che la lista sia ordinata all'inizio di ogni passata
        ptr1 = testa;       // Riparte dall'inizio del catalogo
        // Scorre i nodi finché il successivo non interseca la zona già stabilizzata (lptr)
        while (ptr1->next != lptr) {
            // Confronto lessicografico dei titoli dei due nodi adiacenti (ptr1 e ptr1->next)
            if (strcmp(ptr1->info.titolo, ptr1->next->info.titolo) > 0) {
                // Scambiamo le informazioni ma preserviamo i puntatori alle sotto-liste prestiti!
                Libro temp = ptr1->info;
                ptr1->info = ptr1->next->info;
                ptr1->next->info = temp;
                scambiato = 1;      // Accende il flag: la lista necessita di un ulteriore controllo
            }
            ptr1 = ptr1->next;      // Sposta in avanti il puntatore di scansione
        }
        lptr = ptr1;            // Memorizza il punto massimo raggiunto: ptr1 è ora fisso e corretto nella sua posizione finale
    } while (scambiato);        // Continua finché viene registrato almeno uno scambio
}

// --- ALGORITMO RICORSIVO ---
// Calcola matematicamente il patrimonio librario totale unendo copie stoccate e libri prestati.
int contaCopieTotaliRicorsivo(NodoLista* testa) {
    // CASO BASE: Se il puntatore è NULL, la lista è terminata. Ritorna 0 ponendo fine alla ricorsione.
    if (testa == NULL) return 0;
    // PASSO RICORSIVO: Conta quanti utenti hanno attualmente in prestito il libro corrente
    // Conta sia le copie a scaffale che quelle attualmente in mano agli utenti
    int inPrestito = 0;
    NodoPrestito* p = testa->info.prestitiAttivi;
    while(p) { inPrestito++; p = p->next; }

    // Calcola il totale parziale del nodo e lancia la chiamata ricorsiva sul nodo successivo ('testa->next')
    // Sfrutta lo stack dei record di attivazione della CPU per accumulare i valori al ritorno delle chiamate.
    return testa->info.copieDisponibili + inPrestito + contaCopieTotaliRicorsivo(testa->next);
}

// --- PERSISTENZA DEI DATI (FILE I/O) E GESTIONE MEMORIA ---
// Genera una ricevuta formale in modalità scrittura sovrascrittura ("w").
void generaRicevutaTesto(char* utente, Libro l, char* tipoOperazione) {
    FILE* f = fopen("ricevuta_operazione.txt", "w");
    if (!f) return;     // Gestione difensiva contro errori di apertura del file

    // Scrittura formattata tramite fprintf (analoga a printf ma indirizzata allo stream del file)
    fprintf(f, "===================================\n");
    fprintf(f, "       RICEVUTA AZIONE BIBLIOTECA  \n");
    fprintf(f, "===================================\n");
    fprintf(f, "Operazione: %s\n", tipoOperazione);
    fprintf(f, "Utente coinvolto: %s\n", utente);
    fprintf(f, "Libro: %s (ID: %s)\n", l.titolo, l.id);
    fprintf(f, "===================================\n");
    fclose(f);      // Rilascia lo stream e svuota i buffer di scrittura
}

// Salvataggio avanzato per gestire le sotto-liste dinamiche
// Salva l'intero database in formato binario sul disco ("wb")
void salvaSuFileBinario(NodoLista* testa, const char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) return;
    NodoLista* curr = testa;
    while (curr != NULL) {
        // Scrive in formato binario l'intera struct del libro
        // (escluso il puntatore prestiti che sarebbe un indirizzo inutile al riavvio)
        // Scriviamo i dati base del libro
        fwrite(&(curr->info), sizeof(Libro), 1, f);
        // Contiamo quanti prestiti attivi ci sono
        int quantiPrestiti = 0;
        NodoPrestito* p = curr->info.prestitiAttivi;
        while(p) { quantiPrestiti++; p = p->next; }
        // Scriviamo il numero di prestiti a seguire
        fwrite(&quantiPrestiti, sizeof(int), 1, f);
        // Scriviamo i singoli nomi degli utenti
        p = curr->info.prestitiAttivi;
        while(p) {
            fwrite(p->nomeUtente, sizeof(char), MAX_STR, f);
            p = p->next;
        }
        curr = curr->next;
    }
    fclose(f);
}

// Carica ed alloca in memoria i dati precedentemente serializzati sul file binario ("rb").
NodoLista* caricaDaFileBinario(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;        // Ritorna NULL se il file non esiste ancora (primo avvio)

    NodoLista* testa = NULL;
    Libro l;

    // Ciclo di lettura binaria continua: fread restituisce il numero di blocchi letti (si ferma a EOF)
    while (fread(&l, sizeof(Libro), 1, f) == 1) {
        NodoLista* nuovo = (NodoLista*)malloc(sizeof(NodoLista));
        nuovo->info = l;
        nuovo->info.prestitiAttivi = NULL; // Pulisce il puntatore ereditato dal file

        // Legge quanti prestiti nominali erano associati a questo specifico volume
        int quantiPrestiti;
        fread(&quantiPrestiti, sizeof(int), 1, f);

        // Ciclo for per riallocare nell'heap e ricostruire la sotto-lista di stringhe dei prestiti
        for(int i=0; i<quantiPrestiti; i++) {
            char nome[MAX_STR];
            fread(nome, sizeof(char), MAX_STR, f);
            aggiungiPrestitoUtente(&(nuovo->info), nome);   // Ricostruisce il nodo nell'heap
        }

        // Inserimento del blocco libro all'interno del nuovo catalogo ricostruito
        nuovo->next = testa;
        testa = nuovo;
    }
    fclose(f);
    return testa;       // Restituisce la testa del catalogo ripopolata
}


// Sgombera l'intera memoria Heap allocata prima della chiusura del programma.
void liberaTutto(NodoLista* lista, NodoPila* pila) {
    // Svuota e libera il catalogo principale e tutte le relative sotto-liste prestiti nidificate
    while (lista != NULL) {
        NodoLista* tmp = lista;
        lista = lista->next;
        liberaPrestitiLibro(tmp->info.prestitiAttivi);
        free(tmp);
    }
    // Svuota e dealloca completamente tutti i record storici rimasti agganciati alla Pila dei resi
    while (pila != NULL) {
        NodoPila* temp = pila;
        pila = pila->next;
        free(temp);
    }
}
