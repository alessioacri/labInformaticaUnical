// biblioteca.h
// contiene le definizioni delle strutture dati e delle funzioni utilizzate

#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#define MAX_STR 100
#define HASH_SIZE 10

// L1b -> Enum per lo stato di disponibilità del libro
typedef enum {
    DISPONIBILE,
    IN_PRESTITO,
    ESAURITO
} StatoLibro;

// L5b -> Struct principale che rappresenta un Libro
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

// questo è un prototipo che dice al programma che esiste questa funzione
void inserisciLibro(Libro nuovoLibro);
