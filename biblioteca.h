// biblioteca.h
// contiene le definizioni delle strutture dati e dei prototipi delle funzioni utilizzate

// Direttive di inclusione condizionale per evitare inclusioni multiple dello stesso file header
// servono a non compilare nuovamente l'header se è già stato compilato una volta
#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#define MAX_STR 100     //costante massima accettata per la lunghezza delle stringhe

// --- SEZIONE 1: TIPI DI DATI E STRUTTURE DINAMICHE ---

// Costrutto Enum per definire lo stato di disponibilità logica del libro.
// Associa identificatori a valori interi costanti (DISPONIBILE = 0, IN_PRESTITO = 1).
typedef enum {
    DISPONIBILE,
    IN_PRESTITO
} StatoLibro;

// Struttura per un nodo di una lista concatenata semplice (Sotto-lista prestiti).
// Gestisce dinamicamente i nomi degli utenti che hanno attualmente in prestito una copia del libro.
typedef struct NodoPrestito {
    char nomeUtente[MAX_STR];       // Stringa contenente il nome e cognome dell'utente beneficiario
    struct NodoPrestito* next;      // Puntatore autoreferenziale al nodo successivo della lista prestiti
} NodoPrestito;

// Struttura principale "Libro" contenente le informazioni che ci servono su ogni testo
// Aggiornata con l'aggiunta di un puntatore alla sotto-lista dinamica dei prestiti attivi.
typedef struct {
    char id[MAX_STR];           // Codice identificativo univoco del libro
    char titolo[MAX_STR];       // Titolo dell'opera
    char autore[MAX_STR];       // Nome dell'autore dell'opera
    int anno;                   // Anno di pubblicazione
    int copieDisponibili;       // Copie fisicamente presenti sugli scaffali
    StatoLibro stato;           // Stato logico corrente del libro (Enum: DISPONIBILE / IN_PRESTITO)
    NodoPrestito* prestitiAttivi; // Puntatore alla testa della sotto-lista dinamica dei prestiti
    // un libro può essere in prestito a molti utenti contemporaneamente
    // il puntatore vale NULL se il libro non è in prestito a nessuno
    // è l'unico modo per tracciare con precisione l'identità di chi ha preso il libro, senza sprecare memoria
} Libro;

// Struttura per il nodo del catalogo principale (Lista concatenata semplice)
// Ogni nodo memorizza le informazioni di un libro e punta al libro successivo nel database.
typedef struct NodoLista {
    Libro info;                     // Campo informativo: contiene l'istanza della struct Libro
    struct NodoLista* next;         // Puntatore autoreferenziale al nodo successivo del catalogo
} NodoLista;

// Struttura per il nodo singolo della Coda di attesa (Queue - Logica FIFO: First In First Out)
// Memorizza in ordine di arrivo gli utenti in attesa di un libro al momento esaurito.
typedef struct NodoCoda {
    char nomeUtente[MAX_STR];       // Nome dell'utente in attesa
    struct NodoCoda* next;          // Puntatore al nodo successivo nella coda
} NodoCoda;

// Struttura descrittore della Coda: mantiene i puntatori alle estremità
typedef struct {
    NodoCoda* head;
    NodoCoda* tail;
} CodaAttesa;

// Struttura per il nodo della Pila cronologica dei resi (Stack - Logica LIFO: Last In First Out)
// Memorizza lo storico inverso degli ultimi libri fisici rientrati in biblioteca.
typedef struct NodoPila {
    char titoloLibro[MAX_STR];  // Copia del titolo del libro restituito
    char nomeUtente[MAX_STR];   // Nome dell'utente che ha effettuato la restituzione
    struct NodoPila* next;      // Puntatore al nodo sottostante nella pila (elemento inserito precedentemente)
} NodoPila;

// --- SEZIONE 2: PROTOTIPI DELLE FUNZIONI INTERFACCIA ---

// --- Sotto-sezione A: Gestione Catalogo Principale e Ricerche ---

// Inserisce un nuovo libro in testa al catalogo o incrementa le copie se già esistente. Ritorna la nuova testa.
NodoLista* inserisciLibro(NodoLista* testa, Libro l);
// Esegue una ricerca parziale (tramite strstr) per titolo o autore e stampa a video i risultati corrispondenti.
void cercaLibro(NodoLista* testa, char* query);
// Effettua una ricerca puntuale basata sull'ID univoco. Ritorna il puntatore diretto alla struct Libro se trovato.
Libro* cercaPerID(NodoLista* testa, char* id);
// Rimuove permanentemente un nodo/libro dal catalogo deallocando la sua memoria e le relative sotto-liste.
NodoLista* eliminaLibro(NodoLista* testa, char* id);
// Decrementa di 1 unità le copie a scaffale di un libro (scarto). Se le copie giungono a 0, invoca eliminaLibro.
NodoLista* riduciCopiaLibro(NodoLista* testa, char* id);

// --- Sotto-sezione B: Gestione Prestiti Nominali ---

// Alloca dinamicamente un nodo nella sotto-lista 'prestitiAttivi' del libro per registrare l'utente.
void aggiungiPrestitoUtente(Libro* l, char* nome);
// Cerca e rimuove un utente dalla sotto-lista dei prestiti del libro all'atto della restituzione. Ritorna 1 se rimosso, 0 altrimenti.
int rimuoviPrestitoUtente(Libro* l, char* nome);
// Scorre in modo lineare la sotto-lista dei prestiti stampando a schermo tutti i possessori correnti del libro.
void stampaUtentiPrestito(Libro l);

// --- Sotto-sezione C: Gestione delle Strutture Dati (Coda e Pila) ---

// Inizializza i puntatori head e tail della struttura descrittore della Coda di attesa a NULL.
void initCoda(CodaAttesa* q);
// Inserisce un nuovo utente in fondo alla coda d'attesa (Operazione di inserimento FIFO).
void enqueue(CodaAttesa* q, char* nome);
// Estrae e rimuove l'utente in testa alla coda restituendone il nome allocato dinamicamente (Operazione di estrazione FIFO).
char* dequeue(CodaAttesa* q);
// Funzione di logica booleana: ritorna 1 (vero) se la coda è vuota, 0 (falso) se contiene elementi.
int codaVuota(CodaAttesa* q);

// Inserisce un record di restituzione in cima alla pila della cronologia (Operazione Push - LIFO).
NodoPila* pushReso(NodoPila* testa, char* titolo, char* utente);
// Scorre la pila partendo dalla cima mostrando a schermo fino a un massimo di 5 record storici recenti.
void mostraCronologiaResi(NodoPila* testa);

// --- Sotto-sezione D: Algoritmi Avanzati (Ordinamento e Ricorsione) ---

// Ordina il catalogo principale alfabeticamente per titolo modificando i collegamenti tramite Bubble Sort.
void bubbleSort(NodoLista* testa);

// Calcola in modo ricorsivo la somma totale delle copie a scaffale e dei prestiti attivi nel catalogo
int contaCopieTotaliRicorsivo(NodoLista* testa);

// --- Sotto-sezione E: Persistenza Dati (File I/O) e Memory Management ---

// Conserva i dati del catalogo e delle sotto-liste salvandoli in formato binario su disco.
void salvaSuFileBinario(NodoLista* testa, const char* filename);
// Legge il file binario all'avvio, ricostruisce il catalogo nell'heap e alloca le rispettive sotto-liste.
NodoLista* caricaDaFileBinario(const char* filename);
// Genera un file di testo ("ricevuta_operazione.txt") per documentare formalmente l'ultimo prestito o reso.
void generaRicevutaTesto(char* utente, Libro l, char* tipoOperazione);
// Scorre l'intera memoria dinamica occupata da catalogo, sotto-liste e pila eseguendo la deallocazione (free).
void liberaTutto(NodoLista* lista, NodoPila* pila);

#endif      // Fine della direttiva condizionale BIBLIOTECA_H
