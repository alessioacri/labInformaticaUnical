// biblioteca.h
// contiene le definizioni delle strutture dati e delle funzioni utilizzate

#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#define MAX_STR 100
#define HASH_SIZE 10

typedef struct Libro {
    char titolo[100];
    char autore[100];
    int copie;
    int anno;
} Libro;

// questo è un prototipo che dice al programma che esiste questa funzione
void inserisciLibro(Libro nuovoLibro);
