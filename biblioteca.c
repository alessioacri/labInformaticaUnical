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

// L16b - Ricerca Istantanea tramite Hash
Libro* cercaHash(NodoHash* tabellaHash[], char* id) {
    int indice = funzioneHash(id);
    NodoHash* curr = tabellaHash[indice];
    while(curr != NULL) {
        if(strcmp(curr->libroRef->id, id) == 0) {
            return curr->libroRef; // Trovato!
        }
        curr = curr->next;
    }
    return NULL; // Non trovato
}

void rimuoviDallHash(NodoHash* tabellaHash[], char* id) {
    int indice = funzioneHash(id);
    NodoHash* curr = tabellaHash[indice];
    NodoHash* prev = NULL;
    while(curr != NULL) {
        if(strcmp(curr->libroRef->id, id) == 0) {
            if(prev == NULL) tabellaHash[indice] = curr->next;
            else prev->next = curr->next;
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

// 1. Inserisci un nuovo libro (Lista concatenata + Hash)
NodoLista* inserisciLibro(NodoLista* testa, Libro l, NodoHash* tabellaHash[]) {
    // Controlliamo se esiste già tramite Hash
    Libro* esistente = cercaHash(tabellaHash, l.id);
    if(esistente != NULL) {
        esistente->copie += l.copie;
        if(esistente->stato == ESAURITO) esistente->stato = DISPONIBILE;
        printf("[INFO] Libro gia' presente. Copie aggiornate.\n");
        return testa;
    }

    // Se è un nuovo libro, allochiamo dinamicamente un nodo
    NodoLista* nuovo = (NodoLista*)malloc(sizeof(NodoLista));
    nuovo->info = l;
    nuovo->next = testa; // Inserimento in testa (più veloce, $O(1)$)
    testa = nuovo;

    // Inseriamo il puntatore anche nell'Hash per le ricerche future
    inserisciHash(tabellaHash, &(testa->info));
    printf("[OK] Libro inserito con successo nel catalogo.\n");
    return testa;
}
