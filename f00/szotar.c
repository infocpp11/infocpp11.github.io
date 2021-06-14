#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct Szo {
    char magyar[50+1];
    char angol[50+1];
} Szo;

typedef struct Szotar {
    Szo szavak[200];
    int db;
} Szotar;


/**
 * Felvesz egy új szót a szótárba.
 * Ha már benne van, nem csinál semmit.
 * @return True, ha bekerült a szó, false, ha megtelt a szótár.
 */
bool felvesz(Szotar *szt, Szo uj) {
    /* benne van? */
    for (int i = 0; i < szt->db; ++i)
        if (strcmp(szt->szavak[i].magyar, uj.magyar) == 0
            && strcmp(szt->szavak[i].angol, uj.angol) == 0)
            return true;
    /* megtelt? */
    if (szt->db == 200)
        return false;
    /* betesszük */
    strcpy(szt->szavak[szt->db].magyar, uj.magyar);
    strcpy(szt->szavak[szt->db].angol, uj.angol);
    szt->db += 1;
    return true;
}

/**
 * Megkeres egy szót. A megadott szó akár angol,
 * akár magyar nyelvű lehet.
 * @return A találatra mutató pointer, vagy NULL.
 */
Szo * keres(Szotar *szt, char *szo) {
    for (int i = 0; i < szt->db; ++i)
        if (strcmp(szt->szavak[i].magyar, szo) == 0
            || strcmp(szt->szavak[i].angol, szo) == 0)
            return &szt->szavak[i];
    return NULL;
}

/**
 * Fájlba írja a szótárat.
 * @return true, ha sikeres volt a művelet.
 * Ha nem, hibaüzenetet is ad a kimeneten.
 */
bool ment(Szotar *szt, char *fajlnev) {
    FILE *f = fopen(fajlnev, "wt");
    if (f == NULL) {
        perror("Nem sikerült menteni a szótárat");
        return false;
    }
    for (int i = 0; i < szt->db; ++i)
        fprintf(f, "%s\t%s\n", szt->szavak[i].magyar,
                               szt->szavak[i].angol);
    fclose(f);
    return true;
}


/**
 * Betölti a szótárat. Teljesen felülírja az
 * addigi tartalmat a fájlból beolvasott adatokkal.
 * @return true, ha sikeres volt.
 */
bool betolt(Szotar *szt, char *fajlnev) {
    FILE *f = fopen(fajlnev, "rt");
    if (f == NULL) {
        perror("Nem sikerült betölteni a szótárat");
        return false;
    }
    szt->db = 0;
    Szo uj;
    while (fscanf(f, " %[^\t] %[^\n]", uj.magyar, uj.angol) == 2) {
        szt->szavak[szt->db] = uj;
        szt->db += 1;
    }
    fclose(f);
    return true;
}


/**
 * Kilistázza a szótár teljes tartalmát a képernyőre.
 */
void listaz(Szotar *szt) {
    for (int i = 0; i < szt->db; ++i)
        printf("%s = %s\n", szt->szavak[i].magyar, szt->szavak[i].angol);
}


int main(void) {
    Szotar sz;
    sz.db = 0;  /* inicializálás; a többit nem kell */ 
    
    felvesz(&sz, (Szo) { "alma", "apple" });
    felvesz(&sz, (Szo) { "körte", "pear" });
    felvesz(&sz, (Szo) { "barack", "peach" });
    felvesz(&sz, (Szo) { "tábla csoki", "bar of chocolate" });
    felvesz(&sz, (Szo) { "doboz gyufa", "box of matches" });
    felvesz(&sz, (Szo) { "idegroncs", "nervous wreck" });
    
    printf("Keresés =====\n");
    Szo * talalat = keres(&sz, "alma");
    if (talalat == NULL)
        printf("Nincs meg");
    else
        printf("Találat: %s = %s\n", talalat->magyar, talalat->angol);
    printf("\n");
    
    printf("Fájlkezelés =====\n");
    ment(&sz, "szotar.txt");
    sz.db = 0;
    betolt(&sz, "szotar.txt");
    printf("%d szó beolvasdva.\n", sz.db);
    printf("\n");

    printf("A szótár tartalma =====\n");
    listaz(&sz);
    printf("\n");
}
