#include "database.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static clock_t clk;
static size_t col, cmps, swaps;
static unsigned seed;

static double estimated[3];

static bool is_sorted(struct Node** array, size_t n)
{
    for (size_t i = 1; i < n; ++i)
        if (compare_entries(array[i - 1]->entry, array[i]->entry, col) > 0)
            return false;

    return true;
}

static bool is_stable(struct Node** array, size_t n)
{
    for (size_t i = 1; i < n; ++i)
        if (compare_entries(array[i - 1]->entry, array[i]->entry, col) == 0 && array[i - 1]->entry->id > array[i]->entry->id)
            return false;

    return true;
}

static void show_cmps()
{
    double elapsed = ((double)clock() - clk) / CLOCKS_PER_SEC * 1000;
    
    printf("Cazurile complexitatii temporale -- "
        "Bun: %.4f; Mediu: %.4f; Rau: %.4f\n"
        "Nr. de comparari: %zu\n"
        "Nr. de interschimbari: %zu\n"
        "Timpul de executie: %.2f ms\n",
        estimated[0], estimated[1], estimated[2],
        cmps, swaps, elapsed);

    clk = 0;
    cmps = swaps = 0;
}

static void swap_ptrs(void** lhs, void** rhs)
{
    void* tmp = *lhs;
    *lhs = *rhs;
    *rhs = tmp;
}

static void swap_ptrs_l(void** lhs, void** rhs)
{
    ++swaps;
    swap_ptrs(lhs, rhs);
}

static int compare_nodes_l(struct Node* lhs, struct Node* rhs)
{
    if (cmps == 0)
        clk = clock();
    
    ++cmps;
    return compare_entries(lhs->entry, rhs->entry, col);
}

static struct Node** create_array(struct Node* head, size_t n)
{
    struct Node** arr = malloc(n * sizeof(struct Node*));

    if (arr == NULL)
        return NULL;

    size_t i = 0;

    for (struct Node* it = head; i < n; it = it->next)
        arr[i++] = it;

    return arr;
}

static void shuffle_array(struct Node*** array, size_t n)
{
    srand(seed);
    struct Node** newArray = malloc(n * sizeof(struct Node*));

    if (newArray == NULL)
        return;

    for (size_t i = 0; i < n; ++i)
    {
        size_t idx = rand() % (n - i);
        newArray[i] = (*array)[idx];
        swap_ptrs(*array + idx, *array + (n - i - 1));
    }

    free(*array);
    *array = newArray;
}

static void write_array(struct Node** array, size_t n)
{
    char tmpname[L_tmpnam + 16] = "start ";
    tmpnam_s(tmpname + 6, L_tmpnam);
    strcat_s(tmpname, sizeof tmpname, ".csv");
    FILE* fout;

    if (fopen_s(&fout, tmpname + 6, "w") == 0)
    {
        for (size_t i = 0; i < n; ++i)
            write_entry(fout, array[i]->entry);

        fclose(fout);

        if (!ferror(fout))
        {
            printf("succes");
            strcat_s(tmpname, sizeof tmpname, " > nul");
            system(tmpname);
        }
    }
}

void BubbleSort(struct Node** array, size_t n)
{
    for (size_t j, i = 0; i < n - 1; ++i)
    {
        bool sorted = true;
        
        for (j = 1; j < n - i; ++j)
            if (compare_nodes_l(array[j - 1], array[j]) > 0)
            {
                swap_ptrs_l(array + j - 1, array + j);
                sorted = false;
            }

        if (sorted)
            return;
    }
}

void InsertionSort(struct Node** array, size_t n)
{
    for (size_t j, i = 1; i < n; ++i)
        for (j = i; j != 0 && compare_nodes_l(array[j - 1], array[j]) > 0; --j)
            swap_ptrs_l(array + j - 1, array + j);
}

void SelectionSort(struct Node** array, size_t n)
{
    for (size_t j, i = 0; i < n - 1; ++i)
    {
        struct Node** mnNode = array + i;

        for (j = i + 1; j < n; ++j)
            if (compare_nodes_l(*mnNode, array[j]) > 0)
                mnNode = array + j;
        
        swap_ptrs_l(mnNode, array + i);
    }
}

void QuickSort(struct Node** array, size_t n)
{
    for (; ; )
    {
        if (n <= 1)
            return;

        struct Node** pivot = array, **antiPivot = array + n - 1;

        for (bool parity = true; pivot != antiPivot; parity ? --antiPivot : ++pivot)
            if (compare_nodes_l(*pivot, *antiPivot) > 0)
            {
                swap_ptrs_l(pivot, antiPivot);
                parity ^= true;
            }

        QuickSort(array, pivot - array);

        n = array + n - pivot - 1;
        array = pivot + 1;
    }
}

void ShellSort(struct Node** array, size_t n)
{
    // Secventa lui Marcin Ciura
    static const size_t steps[] = { 701, 301, 132, 57, 23, 10, 4, 1 }, nrIt = sizeof steps / sizeof(size_t);

    for (size_t j, i, it = 0; it < nrIt; ++it)
    {
        const size_t step = steps[it];

        for (i = step; i < n; i += step)
            for (j = i; j != 0 && compare_nodes_l(array[j - step], array[j]) > 0; j -= step)
                swap_ptrs_l(array + j - step, array + j);
    }
}

void Heapify(struct Node** array, size_t parent, size_t n)
{
    for (; ; )
    {
        size_t mx = parent, first = (parent << 1) + 1, second = first + 1;

        if (first < n)
        {
            if (compare_nodes_l(array[mx], array[first]) < 0)
                mx = first;

            if (second < n && compare_nodes_l(array[mx], array[second]) < 0)
                mx = second;

            if (mx != parent)
            {
                swap_ptrs_l(array + parent, array + mx);
                parent = mx;
                continue;
            }
        }

        return;
    }
}

void HeapSort(struct Node** array, size_t n)
{
    if (n <= 1)
        return;
    
    size_t i;

    for (i = n >> 1; i--; )
        Heapify(array, i, n);

    for (i = n; --i; )
    {
        swap_ptrs_l(array, array + i);
        Heapify(array, 0, i);
    }
}

void Merge(struct Node** array, size_t half, size_t n)
{
    if (half >= n)
        return;
    
    struct Node** temp = malloc(n * sizeof(struct Node*));

    size_t i = 0, j = half, k = 0;

    while (i < half && j < n)
        temp[k++] = array[compare_nodes_l(array[i], array[j]) > 0 ? j++ : i++];

    while (i < half)
        temp[k++] = array[i++];

    while (j < n)
        temp[k++] = array[j++];

    swaps += n;
    memcpy_s(array, n * sizeof(struct Node*), temp, n * sizeof(struct Node*));
    free(temp);
}

void MergeSort(struct Node** array, size_t n)
{
    if (n <= 1)
        return;

    if (n == 2)
    {
        if (compare_nodes_l(array[0], array[1]) > 0)
            swap_ptrs_l(array, array + 1);

        return;
    }

    size_t half = n >> 1;

    MergeSort(array, half);
    MergeSort(array + half, half + (n & 1));
    Merge(array, half, n);
}

void IntroSort(struct Node** array, size_t n, size_t depth)
{
    for (; ; )
    {
        if (n < 16)
            InsertionSort(array, n);
        else if (depth == 0)
            HeapSort(array, n);
        else
        {
            struct Node** pivot = array, ** antiPivot = array + n - 1;

            for (bool parity = true; pivot != antiPivot; parity ? --antiPivot : ++pivot)
                if (compare_nodes_l(*pivot, *antiPivot) > 0)
                {
                    swap_ptrs_l(pivot, antiPivot);
                    parity ^= true;
                }

            IntroSort(array, pivot - array, --depth);

            n = array + n - pivot - 1;
            array = pivot + 1;
            continue;
        }

        return;
    }
}

void TimSort(struct Node** array, size_t n)
{
    static const size_t RUN = 32;

    for (size_t i = 0; i < n; i += RUN)
        InsertionSort(array + i, min(n - i, RUN));

    for (size_t j, i = RUN; i < n; i <<= 1)
        for (j = 0; j < n; j += i << 1)
            Merge(array + j, i, min(n - j, (i << 1)));
}

void OddEvenSort(struct Node** array, size_t n)
{
    for (; ; )
    {
        bool sorted = true;

        for (size_t j = 2; j < n; j += 2)
            if (compare_nodes_l(array[j - 1], array[j]) > 0)
            {
                swap_ptrs_l(array + j - 1, array + j);
                sorted = false;
            }

        for (size_t j = 1; j < n; j += 2)
            if (compare_nodes_l(array[j - 1], array[j]) > 0)
            {
                swap_ptrs_l(array + j - 1, array + j);
                sorted = false;
            }

        if (sorted)
            return;
    }
}

void RadixSort(struct Node** array, size_t n, size_t buckets);

void PigeonholeSort(struct Node** array, size_t n);

static void print_help()
{
    printf(
        "Insertie:\n"
        "\tinsertion \tMetoda insertiei;\n"
        "\tshell     \tMetoda lui Shell.\n"
        "Selectie:\n"
        "\tselection \tMetoda selectiei;\n"
        "\theap      \tMetoda Heap.\n"
        "Partitionare:\n"
        "\tquick     \tMetoda rapida;\n"
        "\tintro     \tMetoda introspectiva.\n"
        "Interclasare:\n"
        "\tmerge     \tMetoda prin interclasare;\n"
        "\ttim       \tMetoda lui Tim.\n"
        "Zero comparatii:\n"
        "\tradix     \tMetoda bazata pe bazele de numeratie (Radix LSD);\n"
        "\tpigeonhole\tMetoda sertarelor.\n"
        "Interschimbare:\n"
        "\tbubble    \tMetoda bulelor;\n"
        "\toddeven   \tMetoda prin paritate.\n"
        "Setari:\n"
        "\thelp      \tDe afisat acest mesaj;\n"
        "\tmix       \tDe amestecat pozitiile randurilor din tabel;\n"
        "\tselect    \tSelecteaza toate randurile;\n"
        "\ttest      \tVerifica consecutivitatea crescatoare a randurilor;\n"
        "\treset     \tReseteaza consecutivitatea elementelor;\n"
        "\texit      \tIesire.");
}

int main(void)
{
    struct Node *head, **array;
    size_t n;

    {
        FILE* f;

        if (fopen_s(&f, "database.csv", "r") != 0)
            return 1;

        head = read_db(f, &n);
        array = create_array(head, n);
        fclose(f);
    }

    printf("Lucrarea de laborator nr. 2 la ASDC (c) Cemirtan Cristian 2023\n"
    "Nr. total de tupluri: %zu\n\n", n);

    print_help();

    for (; ; )
    {
        char input[21] = { '\0' };
        printf("\n>");
        scanf_s("%s", input, 21);

        if (!strcmp(input, "help"))
        {
            print_help();
        }
        else if (!strcmp(input, "mix"))
        {
            printf("Seed = ");
            scanf_s("%ud", &seed);
            shuffle_array(&array, n);
        }
        else if (!strcmp(input, "select"))
        {
            write_array(array, n);
        }
        else if (!strcmp(input, "test"))
        {
            printf("Sortat: %s; Stabil: %s", is_sorted(array, n) ? "da" : "nu", is_stable(array, n) ? "da" : "nu");
        }
        else if (!strcmp(input, "reset"))
        {
            free(array);
            array = create_array(head, n);
        }
        else if (!strcmp(input, "exit"))
        {
            free_db(head);
            free(array);
            return 0;
        }
        else
        {
            if (!strcmp(input, "radix"))
            {
                size_t buckets;
                printf("Baza numeratiei = ");
                scanf_s("%zu", &buckets);
                RadixSort(array, n, buckets);
                estimated[0] = estimated[1] = estimated[2] = (double)n * buckets;
            }
            else if (!strcmp(input, "pigeonhole"))
            {
                PigeonholeSort(array, n);
                estimated[0] = estimated[1] = estimated[2] = (double)n;
            }
            else
            {
                printf("ORDER BY ");
                scanf_s("%zu", &col);

                if (!strcmp(input, "bubble"))
                {
                    BubbleSort(array, n);
                    estimated[0] = (double)n;
                    estimated[1] = estimated[2] = (double)n * n;
                }
                else if (!strcmp(input, "insertion"))
                {
                    InsertionSort(array, n);
                    estimated[0] = (double)n;
                    estimated[1] = estimated[2] = (double)n * n;
                }
                else if (!strcmp(input, "selection"))
                {
                    SelectionSort(array, n);
                    estimated[0] = estimated[1] = estimated[2] = (double)n * n;
                }
                else if (!strcmp(input, "quick"))
                {
                    QuickSort(array, n);
                    estimated[0] = estimated[1] = n * log2((double)n);
                    estimated[2] = (double)n * n;
                }
                else if (!strcmp(input, "shell"))
                {
                    ShellSort(array, n);
                    estimated[0] = (double)n;
                    estimated[1] = estimated[2] = n * pow(log2((double)n), 2);
                }
                else if (!strcmp(input, "heap"))
                {
                    HeapSort(array, n);
                    estimated[0] = estimated[1] = estimated[2] = n * log2((double)n);
                }
                else if (!strcmp(input, "merge"))
                {
                    MergeSort(array, n);
                    estimated[0] = estimated[1] = estimated[2] = n * log2((double)n);
                }
                else if (!strcmp(input, "intro"))
                {
                    IntroSort(array, n, (size_t)(log2((double)n) * 2));
                    estimated[0] = estimated[1] = estimated[2] = n * log2((double)n);
                }
                else if (!strcmp(input, "tim"))
                {
                    TimSort(array, n);
                    estimated[0] = (double)n;
                    estimated[1] = estimated[2] = n * log2((double)n);
                }
                else if (!strcmp(input, "oddeven"))
                {
                    OddEvenSort(array, n);
                    estimated[0] = (double)n;
                    estimated[1] = estimated[2] = (double)n * n;
                }
                else
                    continue;
            }
 
            show_cmps();
        }
    }
}

struct RadixNode
{
    struct Node* node;
    struct RadixNode* next;
};

void RadixSort(struct Node** array, size_t n, size_t buckets)
{
    clk = clock();

    if (buckets == 0)
        return;

    struct RadixNode** nodes = calloc(buckets, sizeof(struct RadixNode*));

    size_t acc = 1;

    for (; ; )
    {
        bool stop = true;

        for (size_t i = n; i-- != 0; )
        {
            size_t digit = array[i]->entry->id / acc % buckets;
            stop &= array[i]->entry->id / acc / buckets % buckets == 0;

            struct RadixNode* temp = calloc(1, sizeof(struct RadixNode));
            temp->node = array[i];

            if (nodes[digit])
                temp->next = nodes[digit];

            nodes[digit] = temp;
        }

        size_t k = 0;

        for (size_t i = 0; i < buckets; ++i)
        {
            for (struct RadixNode* head = nodes[i]; head; )
            {
                ++swaps;
                array[k++] = head->node;
                void* temp = head->next;
                free(head);
                head = temp;
            }

            nodes[i] = NULL;
        }

        if (stop)
        {
            free(nodes);
            return;
        }

        acc *= buckets;
    }
}

void PigeonholeSort(struct Node** array, size_t n)
{
    clk = clock();

    if (n <= 1)
        return;

    size_t mx = array[0]->entry->id, mn = mx;

    for (size_t i = 1; i < n; ++i)
    {
        size_t temp = array[i]->entry->id;

        if (++cmps, temp > mx)
            mx = temp;
        else if (++cmps, temp < mn)
            mn = temp;
    }

    size_t buckets = mx - mn + 1;
    struct RadixNode** nodes = calloc(buckets, sizeof(struct RadixNode*));

    for (size_t i = n; i-- != 0; )
    {
        size_t digit = array[i]->entry->id - mn;

        struct RadixNode* temp = calloc(1, sizeof(struct RadixNode));
        temp->node = array[i];

        if (nodes[digit])
            temp->next = nodes[digit];

        nodes[digit] = temp;
    }

    size_t k = 0;

    for (size_t i = 0; i < buckets; ++i)
        for (struct RadixNode* head = nodes[i]; head; )
        {
            ++swaps;
            array[k++] = head->node;
            void* temp = head->next;
            free(head);
            head = temp;
        }

    free(nodes);
}