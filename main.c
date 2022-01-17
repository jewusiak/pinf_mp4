#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct m {
    int m, n;
    int *rib; //tasma: 0, 1, 2 to symbol B - blank
    int size;
    int state_index; //0, 1...
    int state; //q0, q1...
} MT_t;

MT_t mt;

void add_to_rib(int arg) {
    mt.rib = realloc(mt.rib, ++mt.size * sizeof *mt.rib);
    mt.rib[mt.size - 1] = arg;
}

char *get_rib_string() {
    char *sl = malloc((mt.size + 1) * sizeof *sl);
    int i;
    for (i = 0; i < mt.size; i++)
        switch (mt.rib[i]) {
            case 0:
                sl[i] = '0';
                break;
            case 1:
                sl[i] = '1';
                break;
            case 2:
                sl[i] = 'B';
                break;
        }
    sl[i] = '\0';
    return sl;
}

char *get_rib_stat() {
    char *sl = malloc((mt.size + 1) * sizeof *sl);
    int i, ps;
    for (i = 0, ps = 0; i < mt.size; i++, ps++) {
        if (i == mt.state_index) {
            sl = realloc(sl, (mt.size + 1) * sizeof *sl);
            sl[ps++] = 'q';
            sl[ps++] = '(';
            sl[ps++] = 48 + mt.state;
            sl[ps++] = ')';
        }
        switch (mt.rib[i]) {
            case 0:
                sl[ps] = '0';
                break;
            case 1:
                sl[ps] = '1';
                break;
            case 2:
                sl[ps] = 'B';
                break;
        }
    }
    if (i == mt.state_index) {
        sl = realloc(sl, (mt.size + 1) * sizeof *sl);
        sl[ps++] = 'q';
        sl[ps++] = '(';
        sl[ps++] = 48 + mt.state;
        sl[ps++] = ')';
    }
    sl[ps] = '\0';
    return sl;
}

void mt_init(char *a, char *b) {
    mt.m = atoi(a);
    mt.n = atoi(b);
    mt.size = 0;
    mt.rib = malloc(sizeof *mt.rib);
    for (int i = 0; i < mt.m; i++)
        add_to_rib(0);
    add_to_rib(1);
    for (int i = 0; i < mt.n; i++)
        add_to_rib(0);
    mt.state_index = 0;
    mt.state = 0;
}

typedef struct n {
    int next_state; //(q)0-n
    int insert; // 0,1,2 - B
    int action;//-1/1 - L/P
} move_t;

move_t NA = {-1, -1, 0}; //brak przejścia

//[stan][symbol]
move_t *moves[] = {(move_t[]) {{1,  2,  1},
                               {5,  2,  1},
                               {-1, -1, 0}},
                   (move_t[]) {{1,  0,  1},
                               {2,  1,  1},
                               {-1, -1, 0}},
                   (move_t[]) {{3, 1, -1},
                               {2, 1, 1},
                               {4, 2, -1}},
                   (move_t[]) {{3, 0, -1},
                               {3, 1, -1},
                               {0, 2, 1}},
                   (move_t[]) {{4, 0, -1},
                               {4, 2, -1},
                               {6, 0, 1}},
                   (move_t[]) {{5, 2, 1},
                               {5, 2, 1},
                               {6, 2, 1}},
                   (move_t[]) {{-1, -1, 0},
                               {-1, -1, 0},
                               {-1, -1, 0}}};


move_t *get_next_move(int this_state, int next_sym) {
    if (this_state > 6 || next_sym > 2) {
        fprintf(stderr, "Maszyna napotkala blad w danych!\n");
        exit(2);
    }
    if (moves[this_state][next_sym].next_state != -1)
        return &moves[this_state][next_sym];
    return NULL;
}

void trim_B() {
    for (int i = mt.size - 1; i >= 0 && i >= mt.state_index; i--)
        if (mt.rib[i] == 2)
            mt.rib = realloc(mt.rib, --mt.size * sizeof *mt.rib);
        else
            break;

}

int count_zeros_in_rib() {
    int res = 0;
    for (int i = 0; i < mt.size; i++)
        if (mt.rib[i] == 0)
            res++;
    return res;
}

int main(int argc, char **argv) {
    //setbuf(stdout, NULL);
    printf("turing: Informacja o maszynie Turinga:\n"
           "Obilcza m-n\n"
           "Wywolanie: turing <m> <n>\n"
           "Argumenty obowiazkowe:\n"
           "- m to liczba calkowita\n"
           "- n to liczba calowita\n"
           "m i n powinny byc m>n aby uzyskac wynik inny niz 0\n\n");

    if (argc < 3) {
        fprintf(stderr, "Brak argumentow!\n"
                        "turing <m> <n>");
        return 1;
    }

    mt_init(argv[1], argv[2]);

    printf("Tasma wejsciowa dla (m = %d, n = %d) - %s\n\n", mt.m, mt.n, get_rib_string());

    move_t *nmv = get_next_move(mt.state, mt.state_index == mt.size ? 2 : mt.rib[mt.state_index]);
    int it = 0;
    while (1 == 1) {

        printf("%s -> ", get_rib_stat());
        if (++it % 3 == 0)
            printf("\n");

        if (mt.state_index >= mt.size)
            add_to_rib(2);

        mt.rib[mt.state_index] = nmv->insert;
        mt.state_index += nmv->action;
        //jeżeli jesteśmy na state_index > mt.size to należy rozszerzyć ribbon o B


        mt.state = nmv->next_state;

        if ((nmv = get_next_move(mt.state, mt.state_index == mt.size ? 2 : mt.rib[mt.state_index])) == NULL)
            break;


    }
    printf("%s\n\n", get_rib_stat());

    printf("Koniec dzialania maszyny!\n");

    trim_B();

    printf("Tasma wynikowa: %s\n\n", get_rib_stat());

    int result = count_zeros_in_rib();
printf("Wniosek: ");
    if (result == mt.m - mt.n)
        printf("Wynik prawidlowy: %d - %d = %d", mt.m, mt.n, result);
    else if (result == 0 && result != mt.m - mt.n)
        printf("Wpisano liczby m < n (%d < %d). Maszyna Turinga nie obsluguje takiego dzialania.\n", mt.m, mt.n);
    else
        printf("Wynik jest nieprawidlowy: %d - %d != %d\n", mt.m, mt.n, result);

    return 0;
}
