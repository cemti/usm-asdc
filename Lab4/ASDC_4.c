#include "database.h"
#include <time.h>
#include <string.h>

struct Pair
{
    int a, b;
};

static struct MetaVector
{
    int offset;
    size_t sizes[];
} *vd;

static void* iliffe;

static size_t cursor, arraySize, rank, (*getCursor)(void);
static struct Entry** array;
static struct Pair* pairs;
static int* inputIdx;

static size_t CreateIliffe(size_t depth, size_t index, void** ptr, bool inverse)
{
    size_t sz = pairs[depth].b - pairs[depth].a + 1, lDepth = inverse ? 0 : rank - 1;

    if (depth + (inverse ? -1 : 1) == lDepth)
    {
        size_t *tmp = cem_calloc(sz, sizeof(size_t)), lSz = pairs[lDepth].b - pairs[lDepth].a + 1;

        for (size_t i = 0; i < sz; ++i)
        {
            tmp[i] = index - pairs[lDepth].a;
            index += lSz;
        }

        *ptr = tmp - pairs[depth].a;
    }
    else
    {
        void **tmp = cem_calloc(sz, sizeof(void*));

        for (size_t i = 0; i < sz; ++i)
            index = CreateIliffe(depth + (inverse ? -1 : 1), index, tmp + i, inverse);

        *ptr = tmp - pairs[depth].a;
    }

    return index;
}

static size_t DirectRM(void)
{
    size_t idx = inputIdx[rank - 1] - pairs[rank - 1].a, sz = 1;

    for (size_t i = rank - 2; i != SIZE_MAX; --i)
    {
        sz *= pairs[i + 1].b - pairs[i + 1].a + 1;
        size_t rel = inputIdx[i] - pairs[i].a;
        idx += rel * sz;
    }

    return idx;
}

static size_t DirectCM(void)
{
    size_t idx = inputIdx[0] - pairs[0].a, sz = 1;

    for (size_t i = 1; i != rank; ++i)
    {
        sz *= pairs[i - 1].b - pairs[i - 1].a + 1;
        size_t rel = inputIdx[i] - pairs[i].a;
        idx += rel * sz;
    }

    return idx;
}

static size_t Cached(void)
{
    size_t idx = 0;

    for (size_t i = 0; i < rank; ++i)
        idx += vd->sizes[i] * inputIdx[i];

    return idx - vd->offset;
}

static size_t IliffeRM(void)
{
    if (rank == 1)
        return inputIdx[rank - 1];
    
    void** ptr = iliffe;
    
    for (size_t i = 0; i < rank - 2; ++i)
        ptr = ptr[inputIdx[i]];

    return ((size_t*)ptr)[inputIdx[rank - 2]] + inputIdx[rank - 1];
}

static size_t IliffeCM(void)
{
    if (rank == 1)
        return inputIdx[0];
    
    void** ptr = iliffe;

    for (size_t i = rank - 1; i > 1; --i)
        ptr = ptr[inputIdx[i]];

    return ((size_t*)ptr)[inputIdx[1]] + inputIdx[0];
}

static void print_help(void)
{
    puts("Metode:\n"
        "\tselect  \tSelectarea unui element;\n"
        "\tupdate  \tActualizarea valorii elementului selectat;\n"
        "\tget     \tAfisarea valorii elementului selectat;\n"
        "\tbench   \tMasurarea performantei de selectare aleatoare;\n"
        "\tlinbench   \tMasurarea performantei de selectare secventiale.\n"
        "Setari:\n"
        "\thelp    \tAfisarea acestui mesaj;\n"
        "\trange   \tAfisarea diapazoanelor matricei;\n"
        "\tmem     \tAfisarea amprentei de memorie a tipului de date curent;\n"
        "\trewrap  \tResetarea metodei de acces;\n"
        "\treset   \tResetarea matricei;\n"
        "\texit    \tIesire.");
}

static void reset_array(void)
{
    free(array);
    free(pairs);
    free(inputIdx);

    cursor = SIZE_MAX;

    do
    {
        printf("Dimensiuni = ");
        scanf_s("%zu", &rank);
    } while (rank == 0);

    pairs = malloc(rank * sizeof(struct Pair));
    inputIdx = malloc(rank * sizeof(int));

    printf("Dimensiunea #1: ");
    scanf_s("%d%d", &pairs[0].a, &pairs[0].b);

    arraySize = pairs[0].b - pairs[0].a + 1;

    for (size_t i = 1; i < rank; )
    {
        printf("Dimensiunea #%zu: ", i + 1);
        scanf_s("%d%d", &pairs[i].a, &pairs[i].b);

        if (pairs[i].a >= pairs[i].b)
        {
            puts("Intervalul invalid.");
            continue;
        }

        arraySize *= pairs[i].b - pairs[i].a + 1;
        ++i;
    }

    array = calloc(arraySize, sizeof(struct Entry*));

    if (array == NULL)
    {
        printf("Eroare la alocarea memoriei.");
        exit(EXIT_FAILURE);
    }
}

static void reset_type(void)
{
    puts("Alegeti metoda de acces:\n"
        "Directa:\n"
        "  1. Pe linii;\n"
        "  2. Pe coloane.\n"
        "Vector definitoriu:\n"
        "  3. Pe linii;\n"
        "  4. Pe coloane.\n"
        "Vector Iliffe:\n"
        "  5. Pe linii;\n"
        "  6. Pe coloane.");

    for (; ; )
    {
        putchar('>');

        size_t n;
        scanf_s("%zu", &n);

        switch (n)
        {
        case 1:
            getCursor = DirectRM;
            return;

        case 2:
            getCursor = DirectCM;
            return;

        case 3:
        {
            vd = cem_calloc(1, sizeof(struct MetaVector) + rank * sizeof(size_t));
            
            vd->sizes[rank - 1] = 1;
            vd->offset = pairs[rank - 1].a;
            size_t sz = 1;

            for (size_t i = rank - 2; i != SIZE_MAX; --i)
            {
                sz *= pairs[i + 1].b - pairs[i + 1].a + 1;
                vd->sizes[i] = sz;
                vd->offset += pairs[i].a * sz;
            }
            
            getCursor = Cached;
            return;
        }

        case 4:
        {
            vd = cem_calloc(1, sizeof(struct MetaVector) + rank * sizeof(size_t));

            vd->sizes[0] = 1;
            vd->offset = pairs[0].a;
            size_t sz = 1;

            for (size_t i = 1; i != rank; ++i)
            {
                sz *= pairs[i - 1].b - pairs[i - 1].a + 1;
                vd->sizes[i] = sz;
                vd->offset += pairs[i].a * sz;
            }
            
            getCursor = Cached;
            return;
        }

        case 5:
        {
            if (rank > 1)
                CreateIliffe(0, 0, &iliffe, false);

            getCursor = IliffeRM;
            return;
        }

        case 6:
        {
            if (rank > 1)
                CreateIliffe(rank - 1, 0, &iliffe, true);

            getCursor = IliffeCM;
            return;
        }

        default:
            puts("Optiune invalida.");
        }
    }
}

static bool increment_inputs(size_t depth)
{
    if (depth != rank - 1)
    {
        if (increment_inputs(depth + 1))
            return true;

        inputIdx[depth + 1] = pairs[depth + 1].a;
    }

    return ++inputIdx[depth] <= pairs[depth].b;
}

int main(void)
{
    get_db();

    printf("Lucrarea de laborator nr. 4 la ASDC (c) Cemirtan Cristian 2023\n"
        "Tupluri disponibile: %zu\n", db_count());

    print_help();
    putchar('\n');

    reset_array();
    reset_type();

    for (; ; )
    {
        char input[21] = { '\0' };
        printf("\n>");
        scanf_s(" %s", input, 21);

        if (!strcmp(input, "select"))
        {
            for (size_t i = 0; ; )
            {
                printf("Indicele #%zu: ", i + 1);
                scanf_s("%d", inputIdx + i);

                if (inputIdx[i] < pairs[i].a || inputIdx[i] > pairs[i].b)
                {
                    puts("In afara diapazonului.");
                    break;
                }

                if (++i == rank)
                {
                    cursor = getCursor();
                    struct Entry* entry = array[cursor];
                    printf("Relativ: %zu\n", cursor);

                    if (entry == NULL)
                        printf("null");
                    else
                        write_entry(stdout, entry);

                    break;
                }
            }
        }
        else if (!strcmp(input, "update"))
        {
            if (cursor == SIZE_MAX)
                printf("Nu a fost selectata nici o inregistrare");
            else
            {
                printf("SET ROWID = ");
                size_t id;
                scanf_s("%zu", &id);

                array[cursor] = id < db_count() ? get_db() + id : NULL;
            }
        }
        else if (!strcmp(input, "get"))
        {
            if (cursor == SIZE_MAX)
                printf("Nu a fost selectata nici o inregistrare");
            else
            {
                struct Entry* entry = array[cursor];
                
                if (entry == NULL)
                    printf("null");
                else
                    write_entry(stdout, entry);
            }
        }
        else if (!strcmp(input, "bench"))
        {
            clock_t clk = 0;
            srand(time(NULL));
            
            for (size_t i = 0; i < 10000; ++i)
            {
                for (size_t i = 0; i < arraySize; ++i)
                {
                    for (size_t i = 0; i < rank; ++i)
                        inputIdx[i] = rand() % (pairs[i].b - pairs[i].a + 1) + pairs[i].a;

                    clock_t tmp = clock();
                    getCursor();
                    clk += clock() - tmp;
                }
            }

            printf("%.0fms", (double)clk / CLOCKS_PER_SEC * 1000);
        }
        else if (!strcmp(input, "linbench"))
        {
            clock_t clk = 0;

            for (size_t i = 0; i < 10000; ++i)
            {
                for (size_t i = 0; i < rank; ++i)
                    inputIdx[i] = pairs[i].a;
                
                do
                {
                    clock_t tmp = clock();
                    getCursor();
                    clk += clock() - tmp;
                } while (increment_inputs(0));
            }

            printf("%.0fms", (double)clk / CLOCKS_PER_SEC * 1000);
        }
        else if (!strcmp(input, "help"))
        {
            print_help();
        }
        else if (!strcmp(input, "range"))
        {
            for (size_t i = 0; i < rank; ++i)
                printf("[%d..%d]", pairs[i].a, pairs[i].b);
        }
        else if (!strcmp(input, "mem"))
        {
            printf("Amprenta de memorie: %zu alocari (%zu bytes)\nMarimea tabloului: %zu elemente", cem_allocations(), cem_memusage(), arraySize);
        }
        else if (!strcmp(input, "rewrap"))
        {
            cem_collect();
            reset_type();
        }
        else if (!strcmp(input, "reset"))
        {
            cem_collect();
            reset_array();
            reset_type();
        }
        else if (!strcmp(input, "exit"))
        {
            cem_collect();
            return 0;
        }
    }
}