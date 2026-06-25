// biblioteca.h
// contiene le definizioni delle strutture dati e delle funzioni utilizzate

// Direttive di inclusione condizionale per evitare inclusioni multiple dello stesso file header
// servono a non compilare nuovamente l'header se è già stato compilato una volta
#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#define MAX_STR 100

// L1b - Enum semplificata: rimosso ESAURITO
typedef enum {
    DISPONIBILE,
    IN_PRESTITO
} StatoLibro;

// L8b - Nodo per la lista dei prestiti attivi di un SINGOLO libro
typedef struct NodoPrestito {
    char nomeUtente[MAX_STR];
    struct NodoPrestito* next;
} NodoPrestito;

// L5b - Struct principale del Libro (aggiornata con la sotto-lista dei prestiti)
typedef struct {
    char id[MAX_STR];
    char titolo[MAX_STR];
    char autore[MAX_STR];
    int anno;
    int copieDisponibili;     // Copie fisicamente presenti sugli scaffali
    StatoLibro stato;
    NodoPrestito* prestitiAttivi; // L6b/L8b - Puntatore alla testa della lista di chi ha preso il libro
} Libro;

// L8b - Nodo del Catalogo principale
typedef struct NodoLista {
    Libro info;
    struct NodoLista* next;
} NodoLista;

// L13 - Struttura per la Coda (Queue) di attesa se il libro ha copieDisponibili == 0
typedef struct NodoCoda {
    char nomeUtente[MAX_STR];
    struct NodoCoda* next;
} NodoCoda;

typedef struct {
    NodoCoda* head;
    NodoCoda* tail;
} CodaAttesa;

// L13 - Struttura per la Pila (Stack) della cronologia dei resi globali
typedef struct NodoPila {
    char titoloLibro[MAX_STR]; // Salviamo solo il titolo per la cronologia
    char nomeUtente[MAX_STR];
    struct NodoPila* next;
} NodoPila;

// --- PROTOTIPI DELLE FUNZIONI ---
NodoLista* inserisciLibro(NodoLista* testa, Libro l);
void cercaLibro(NodoLista* testa, char* query);
Libro* cercaPerID(NodoLista* testa, char* id);
NodoLista* eliminaLibro(NodoLista* testa, char* id);
NodoLista* riduciCopiaLibro(NodoLista* testa, char* id);

// Sotto-funzioni per la gestione dei prestiti nominali
void aggiungiPrestitoUtente(Libro* l, char* nome);
int rimuoviPrestitoUtente(Libro* l, char* nome);
void stampaUtentiPrestito(Libro l);

// L13 - Coda e Pila
void initCoda(CodaAttesa* q);
void enqueue(CodaAttesa* q, char* nome);
char* dequeue(CodaAttesa* q);
int codaVuota(CodaAttesa* q);

NodoPila* pushReso(NodoPila* testa, char* titolo, char* utente);
void mostraCronologiaResi(NodoPila* testa);

// L14b - Ordinamento
void bubbleSort(NodoLista* testa);

// L16c - Ricorsione
int contaCopieTotaliRicorsivo(NodoLista* testa);

// File I/O e Pulizia
void salvaSuFileBinario(NodoLista* testa, const char* filename);
NodoLista* caricaDaFileBinario(const char* filename);
void generaRicevutaTesto(char* utente, Libro l, char* tipoOperazione);
void liberaTutto(NodoLista* lista, NodoPila* pila);

#endif
