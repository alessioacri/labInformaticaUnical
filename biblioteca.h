// biblioteca.h
// contiene le definizioni delle strutture dati e delle funzioni utilizzate

#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#define MAX_STR 100
#define HASH_SIZE 10

// L1b - Enum per lo stato di disponibilità del libro
typedef enum {
    DISPONIBILE,
    IN_PRESTITO,
    ESAURITO
} StatoLibro;

// L5b - Struct principale che rappresenta un Libro
typedef struct {
    char id[MAX_STR];        // Stringa univoca (es. ISBN o codice)
    char titolo[MAX_STR];
    char autore[MAX_STR];
    int anno;
    int copie;
    StatoLibro stato;
} Libro;

// L8b - Nodo della Lista Concatenata principale (Il catalogo)
typedef struct NodoLista {
    Libro info;
    struct NodoLista* next;
} NodoLista;

// L13 - Struttura per la Coda (Queue) di attesa prestiti
typedef struct NodoCoda {
    char nomeUtente[MAX_STR];
    struct NodoCoda* next;
} NodoCoda;

typedef struct {
    NodoCoda* head;
    NodoCoda* tail;
} CodaAttesa;

// L13 - Struttura per la Pila (Stack) degli ultimi libri restituiti
typedef struct NodoPila {
    Libro info;
    struct NodoPila* next;
} NodoPila;

// L16a - Tabella Hash (Array di liste concatenate per gestire le collisioni)
typedef struct NodoHash {
    Libro* libroRef; // Puntatore al libro nella lista principale
    struct NodoHash* next;
} NodoHash;

// --- PROTOTIPI DELLE FUNZIONI ---

// Gestione Lista Principale
NodoLista* inserisciLibro(NodoLista* testa, Libro l, NodoHash* tabellaHash[]);
void cercaLibro(NodoLista* testa, char* query);
NodoLista* eliminaLibro(NodoLista* testa, char* id, NodoHash* tabellaHash[]);
NodoLista* riduciCopiaLibro(NodoLista* testa, char* id, NodoHash* tabellaHash[]); // NUOVA

// L16a - Gestione Tabella Hash (Ricerca istantanea)
int funzioneHash(char* id);
void inserisciHash(NodoHash* tabellaHash[], Libro* l);
Libro* cercaHash(NodoHash* tabellaHash[], char* id);
void rimuoviDallHash(NodoHash* tabellaHash[], char* id);

// L13 - Gestione Coda (Queue)
void initCoda(CodaAttesa* q);
void enqueue(CodaAttesa* q, char* nome);
char* dequeue(CodaAttesa* q);
int codaVuota(CodaAttesa* q);

// L13 - Gestione Pila (Stack)
NodoPila* push(NodoPila* testa, Libro l);
NodoPila* pop(NodoPila* testa, Libro* l_out);
void mostraCronologiaResi(NodoPila* testa); // NUOVA

// L14b & L16b - Ordinamento e Ricerca
NodoLista* mergeSort(NodoLista* testa);

// L16c - Ricorsione
int contaCopieTotaliRicorsivo(NodoLista* testa);

// L3 & L4 - File I/O e Pulizia Memoria
void salvaSuFileBinario(NodoLista* testa, const char* filename);
NodoLista* caricaDaFileBinario(const char* filename, NodoHash* tabellaHash[]);
void generaRicevutaTesto(char* utente, Libro l);
void liberaTutto(NodoLista* lista, NodoPila* pila, NodoHash* tabellaHash[]);

#endif
