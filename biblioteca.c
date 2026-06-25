#include "biblioteca.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Funzioni di utilità per gestire i nomi di chi ha preso il libro (Lista Concatenata secondaria)
void aggiungiPrestitoUtente(Libro* l, char* nome) {
    NodoPrestito* nuovo = (NodoPrestito*)malloc(sizeof(NodoPrestito));
    strcpy(nuovo->nomeUtente, nome);
    nuovo->next = l->prestitiAttivi;
    l->prestitiAttivi = nuovo;
}

int rimuoviPrestitoUtente(Libro* l, char* nome) {
    NodoPrestito* curr = l->prestitiAttivi;
    NodoPrestito* prev = NULL;
    while (curr != NULL) {
        if (strcmp(curr->nomeUtente, nome) == 0) {
            if (prev == NULL) l->prestitiAttivi = curr->next;
            else prev->next = curr->next;
            free(curr);
            return 1; // Rimozione riuscita
        }
        prev = curr;
        curr = curr->next;
    }
    return 0; // Utente non trovato nella lista dei prestiti di questo libro
}

void stampaUtentiPrestito(Libro l) {
    NodoPrestito* curr = l.prestitiAttivi;
    if (curr == NULL) return;
    printf("   [Attualmente in prestito a: ");
    while (curr != NULL) {
        printf("%s%s", curr->nomeUtente, curr->next ? ", " : "");
        curr = curr->next;
    }
    printf("]\n");
}

Libro* cercaPerID(NodoLista* testa, char* id) {
    NodoLista* curr = testa;
    while (curr != NULL) {
        if (strcmp(curr->info.id, id) == 0) return &(curr->info);
        curr = curr->next;
    }
    return NULL;
}

NodoLista* inserisciLibro(NodoLista* testa, Libro l) {
    Libro* esistente = cercaPerID(testa, l.id);
    if (esistente != NULL) {
        esistente->copieDisponibili += l.copieDisponibili;
        esistente->stato = DISPONIBILE;
        printf("[INFO] Libro gia' presente. Copie aggiornate.\n");
        return testa;
    }

    NodoLista* nuovo = (NodoLista*)malloc(sizeof(NodoLista));
    nuovo->info = l;
    nuovo->info.prestitiAttivi = NULL; // All'inizio nessuno ha preso il libro
    nuovo->next = testa;
    printf("[OK] Libro inserito con successo nel catalogo.\n");
    return nuovo;
}

void cercaLibro(NodoLista* testa, char* query) {
    NodoLista* curr = testa;
    int trovati = 0;
    while (curr != NULL) {
        if (strstr(curr->info.titolo, query) != NULL || strstr(curr->info.autore, query) != NULL) {
            printf("- [%s] %s di %s (Copie disponibili a scaffale: %d | Stato: %s)\n",
                   curr->info.id, curr->info.titolo, curr->info.autore, curr->info.copieDisponibili,
                   curr->info.stato == DISPONIBILE ? "DISPONIBILE" : "IN PRESTITO (TUTTE LE COPIE)");
            stampaUtentiPrestito(curr->info);
            trovati++;
        }
        curr = curr->next;
    }
    if (trovati == 0) printf("[X] Nessun libro corrisponde ai criteri di ricerca.\n");
}

void liberaPrestitiLibro(NodoPrestito* testa) {
    while (testa != NULL) {
        NodoPrestito* tmp = testa;
        testa = testa->next;
        free(tmp);
    }
}

NodoLista* eliminaLibro(NodoLista* testa, char* id) {
    NodoLista* curr = testa;
    NodoLista* prev = NULL;
    while (curr != NULL) {
        if (strcmp(curr->info.id, id) == 0) {
            if (prev == NULL) testa = curr->next;
            else prev->next = curr->next;

            liberaPrestitiLibro(curr->info.prestitiAttivi);
            free(curr);
            printf("[OK] Libro interamente rimosso dal database.\n");
            return testa;
        }
        prev = curr;
        curr = curr->next;
    }
    printf("[X] Errore: Libro non trovato.\n");
    return testa;
}

NodoLista* riduciCopiaLibro(NodoLista* testa, char* id) {
    Libro* l = cercaPerID(testa, id);
    if (l == NULL) {
        printf("[X] Errore: Libro non trovato.\n");
        return testa;
    }
    if (l->copieDisponibili > 1) {
        l->copieDisponibili--;
        printf("[OK] Una copia fisica scartata. Copie rimaste a scaffale: %d\n", l->copieDisponibili);
        return testa;
    } else if (l->copieDisponibili == 1 && l->prestitiAttivi == NULL) {
        printf("[INFO] Questa era l'ultima copia e non ci sono prestiti attivi. Rimozione totale...\n");
        return eliminaLibro(testa, id);
    } else {
        printf("[X] Impossibile scartare l'ultima copia fisica: ci sono libri ancora in prestito agli utenti!\n");
        return testa;
    }
}

// Coda (FIFO)
void initCoda(CodaAttesa* q) { q->head = NULL; q->tail = NULL; }
void enqueue(CodaAttesa* q, char* nome) {
    NodoCoda* nuovo = (NodoCoda*)malloc(sizeof(NodoCoda));
    strcpy(nuovo->nomeUtente, nome);
    nuovo->next = NULL;
    if (q->tail == NULL) { q->head = nuovo; q->tail = nuovo; }
    else { q->tail->next = nuovo; q->tail = nuovo; }
}
char* dequeue(CodaAttesa* q) {
    if (q->head == NULL) return NULL;
    NodoCoda* temp = q->head;
    char* nome = (char*)malloc(MAX_STR * sizeof(char));
    strcpy(nome, temp->nomeUtente);
    q->head = q->head->next;
    if (q->head == NULL) q->tail = NULL;
    free(temp);
    return nome;
}
int codaVuota(CodaAttesa* q) { return q->head == NULL; }

// Pila (LIFO)
NodoPila* pushReso(NodoPila* testa, char* titolo, char* utente) {
    NodoPila* nuovo = (NodoPila*)malloc(sizeof(NodoPila));
    strcpy(nuovo->titoloLibro, titolo);
    strcpy(nuovo->nomeUtente, utente);
    nuovo->next = testa;
    return nuovo;
}
void mostraCronologiaResi(NodoPila* testa) {
    if (testa == NULL) { printf("[INFO] Cronologia resi vuota.\n"); return; }
    printf("--- CRONOLOGIA RECENTI RESTITUZIONI (Pila) ---\n");
    NodoPila* curr = testa;
    int c = 1;
    while (curr != NULL && c <= 5) {
        printf("%d. '%s' restituito da %s\n", c, curr->titoloLibro, curr->nomeUtente);
        curr = curr->next; c++;
    }
}

// Ordinamento (Bubble Sort)
void bubbleSort(NodoLista* testa) {
    int scambiato;
    NodoLista* ptr1;
    NodoLista* lptr = NULL;
    if (testa == NULL || testa->next == NULL) return;
    do {
        scambiato = 0; ptr1 = testa;
        while (ptr1->next != lptr) {
            if (strcmp(ptr1->info.titolo, ptr1->next->info.titolo) > 0) {
                // Scambiamo le informazioni ma preserviamo i puntatori alle sotto-liste prestiti!
                Libro temp = ptr1->info;
                ptr1->info = ptr1->next->info;
                ptr1->next->info = temp;
                scambiato = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (scambiato);
}

int contaCopieTotaliRicorsivo(NodoLista* testa) {
    if (testa == NULL) return 0;
    // Conta sia le copie a scaffale che quelle attualmente in mano agli utenti
    int inPrestito = 0;
    NodoPrestito* p = testa->info.prestitiAttivi;
    while(p) { inPrestito++; p = p->next; }
    return testa->info.copieDisponibili + inPrestito + contaCopieTotaliRicorsivo(testa->next);
}

void generaRicevutaTesto(char* utente, Libro l, char* tipoOperazione) {
    FILE* f = fopen("ricevuta_operazione.txt", "w");
    if (!f) return;
    fprintf(f, "===================================\n");
    fprintf(f, "       RICEVUTA AZIONE BIBLIOTECA  \n");
    fprintf(f, "===================================\n");
    fprintf(f, "Operazione: %s\n", tipoOperazione);
    fprintf(f, "Utente coinvolto: %s\n", utente);
    fprintf(f, "Libro: %s (ID: %s)\n", l.titolo, l.id);
    fprintf(f, "===================================\n");
    fclose(f);
}

// Salvataggio avanzato per gestire le sotto-liste dinamiche
void salvaSuFileBinario(NodoLista* testa, const char* filename) {
    FILE* f = fopen(filename, "wb");
    if (!f) return;
    NodoLista* curr = testa;
    while (curr != NULL) {
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

NodoLista* caricaDaFileBinario(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;
    NodoLista* testa = NULL;
    Libro l;
    while (fread(&l, sizeof(Libro), 1, f) == 1) {
        NodoLista* nuovo = (NodoLista*)malloc(sizeof(NodoLista));
        nuovo->info = l;
        nuovo->info.prestitiAttivi = NULL; // Ricostruiamo la sotto-lista pulita

        int quantiPrestiti;
        fread(&quantiPrestiti, sizeof(int), 1, f);
        for(int i=0; i<quantiPrestiti; i++) {
            char nome[MAX_STR];
            fread(nome, sizeof(char), MAX_STR, f);
            aggiungiPrestitoUtente(&(nuovo->info), nome);
        }

        nuovo->next = testa;
        testa = nuovo;
    }
    fclose(f);
    return testa;
}

void liberaTutto(NodoLista* lista, NodoPila* pila) {
    while (lista != NULL) {
        NodoLista* tmp = lista;
        lista = lista->next;
        liberaPrestitiLibro(tmp->info.prestitiAttivi);
        free(tmp);
    }
    while (pila != NULL) {
        NodoPila* temp = pila;
        pila = pila->next;
        free(temp);
    }
}
