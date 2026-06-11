//main.c
//Questo file contiene l'interfaccia utente e coordina i vari moduli del programma.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "biblioteca.h"

int main() {
    printf("Benvenuto nella gestione biblioteca!\n");
    printf("Ecco le opzioni disponibili:\n");
    NodoLista* catalogo = NULL;
    NodoHash* tabellaHash[HASH_SIZE] = { NULL };
    CodaAttesa codaPrestiti;
    initCoda(&codaPrestiti);
    NodoPila* pilaResi = NULL;
