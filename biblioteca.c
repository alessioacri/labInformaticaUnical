//biblioteca.c
//Questo file contiene tutta la logica del programma
//Vengono usati sia i puntatori che l'allocazione dinamica

#include "biblioteca.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// L16a - Funzione Hash semplice (somma i valori ASCII dei caratteri dell'ID)
int funzioneHash(char* id) {
    int sum = 0;
    for(int i = 0; id[i] != '\0'; i++) {
        sum += id[i];
    }
    return sum % HASH_SIZE;
}

// Inserisce il riferimento a un libro nella Tabella Hash
void inserisciHash(NodoHash* tabellaHash[], Libro* l) {
    int indice = funzioneHash(l->id);
    NodoHash* nuovo = (NodoHash*)malloc(sizeof(NodoHash)); // L6b - Allocazione Heap
    nuovo->libroRef = l;
    nuovo->next = tabellaHash[indice];
    tabellaHash[indice] = nuovo;
}
