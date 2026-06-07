// biblioteca.h
typedef struct Libro {
    char titolo[100];
    char autore[100];
    int copie;
    int anno;
} Libro;

// questo è un prototipo che dice al programma che esiste questa funzione
void inserisciLibro(Libro nuovoLibro);
