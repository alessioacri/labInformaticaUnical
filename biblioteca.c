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

// 2. Cerca un libro per titolo o autore (Scansione lineare con stringhe)
void cercaLibro(NodoLista* testa, char* query) {
    NodoLista* curr = testa;
    int trovati = 0;
    while(curr != NULL) {
        // L1 - Uso di strstr per verificare se la query è contenuta nel titolo o autore
        if(strstr(curr->info.titolo, query) != NULL || strstr(curr->info.autore, query) != NULL) {
            printf("- [%s] %s di %s (Copie disponibili: %d)\n",
                   curr->info.id, curr->info.titolo, curr->info.autore, curr->info.copie);
            trovati++;
        }
        curr = curr->next;
    }
    if(trovati == 0) printf("[X] Nessun libro corrisponde ai criteri di ricerca.\n");
}

// 8. Elimina un libro inservibile
NodoLista* eliminaLibro(NodoLista* testa, char* id, NodoHash* tabellaHash[]) {
    NodoLista* curr = testa;
    NodoLista* prev = NULL;

    while(curr != NULL) {
        if(strcmp(curr->info.id, id) == 0) {
            rimuoviDallHash(tabellaHash, id);
            if(prev == NULL) testa = curr->next;
            else prev->next = curr->next;

            free(curr); // L6b - Deallocazione della memoria liberata
            printf("[OK] Libro eliminato dal sistema.\n");
            return testa;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("[X] Errore: Libro non trovato.\n");
    return testa;
}

// L13 - Funzioni della Coda (Queue)
void initCoda(CodaAttesa* q) {
    q->head = NULL;
    q->tail = NULL;
}

void enqueue(CodaAttesa* q, char* nome) {
    NodoCoda* nuovo = (NodoCoda*)malloc(sizeof(NodoCoda));
    strcpy(nuovo->nomeUtente, nome);
    nuovo->next = NULL;
    if(q->tail == NULL) {
        q->head = nuovo;
        q->tail = nuovo;
    } else {
        q->tail->next = nuovo;
        q->tail = nuovo;
    }
}

char* dequeue(CodaAttesa* q) {
    if(q->head == NULL) return NULL;
    NodoCoda* temp = q->head;
    char* nome = (char*)malloc(MAX_STR * sizeof(char));
    strcpy(nome, temp->nomeUtente);
    q->head = q->head->next;
    if(q->head == NULL) q->tail = NULL;
    free(temp);
    return nome;
}

int codaVuota(CodaAttesa* q) {
    return q->head == NULL;
}
