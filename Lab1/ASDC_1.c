#include "database.h"
#include <math.h>

static size_t cmps;
static double estimated;

static void show_cmps()
{
    printf("Nr. de comparari: %zu", cmps);

    if (estimated)
        printf(" (estimat: %f)", estimated);

    putchar('\n');
    cmps = 0;
    estimated = 0;
}

struct Node* SequentialSearch(struct Node* head, size_t id)
{
    for (struct Node* it = head; it != NULL; it = it->next)
    {
        ++cmps;
        if (it->entry->id == id)
            return it;
    }

    return NULL;
}

struct BSTNode
{
    struct Node* node;
    struct BSTNode* prev, * next;
};

struct BSTNode* CreateBST(struct Node* head)
{
    struct BSTNode* root = cem_calloc(1, sizeof(struct BSTNode));
    root->node = head;
    
    for (; ; )
    {
        head = head->next;

        if (head == NULL)
            return root;

        for (struct BSTNode* temp = root; ; )
        {
            ++cmps;

            if (temp->node->entry->id < head->entry->id)
            {
                if (temp->next)
                {
                    temp = temp->next;
                    continue;
                }
                else
                    temp = temp->next = cem_calloc(1, sizeof(struct BSTNode));
            }
            else
            {
                if (temp->prev)
                {
                    temp = temp->prev;
                    continue;
                }
                else
                    temp = temp->prev = cem_calloc(1, sizeof(struct BSTNode));
            }

            temp->node = head;
            break;
        }
    }
}

struct Node* BST_Search(struct BSTNode* root, size_t id)
{
    while (++cmps, root && root->node->entry->id != id)
        root = root->node->entry->id < id ? root->next : root->prev;

    return root ? root->node : NULL;
}

struct Node** CreateArray(struct Node* head, size_t n)
{
    struct Node** arr = cem_calloc(n, sizeof(struct Node*));

    if (arr == NULL)
        return NULL;

    size_t i = 0;

    for (struct Node* it = head; i < n && it != NULL; it = it->next)
        arr[i++] = it;

    return arr;
}

struct Node* BinarySearch(struct Node** list, size_t n, size_t id)
{
    size_t lhs = 0, rhs = n - 1;

    for (; ; )
    {
        size_t mhs = (lhs + rhs) >> 1;

        ++cmps;
        
        if (list[mhs]->entry->id == id)
            return list[mhs];

        if (lhs >= rhs)
            return NULL;

        if (id > list[mhs]->entry->id)
            lhs = mhs + 1;
        else
            rhs = mhs - 1;
    }
}

struct Node* InterpolationSearch(struct Node** list, size_t n, size_t id)
{
    size_t lhs = 0, rhs = n - 1;

    while (++cmps, lhs <= rhs)
    {
        size_t pos = lhs + (id - list[lhs]->entry->id) * (rhs - lhs) / (list[rhs]->entry->id - list[lhs]->entry->id);

        if (pos >= n)
            return NULL;

        if (list[pos]->entry->id == id)
            return list[pos];

        if (list[pos]->entry->id < id)
            lhs = pos + 1;
        else
            rhs = pos - 1;
    }

    return NULL;
}

struct Node* FibonacciSearch(struct Node** list, size_t n, size_t id)
{
    size_t lhs = 0, mhs = 1, rhs = 1;

    for (; ; )
    {
        size_t temp = mhs + rhs;

        if (temp > n)
            break;
        
        lhs = mhs;
        mhs = rhs;
        rhs = lhs + mhs;
    }

    for (; ; )
    {
        ++cmps;

        if (list[rhs - 1]->entry->id == id)
            return list[rhs - 1];

        if (id > list[rhs - 1]->entry->id)
        {
            if (mhs == 1)
                return NULL;
            
            rhs += lhs;
            mhs -= lhs;
            lhs -= mhs;

            if (rhs > n)
                rhs = n;
        }
        else
        {
            if (lhs == 0)
                return NULL;
            
            size_t temp = mhs;
            rhs -= lhs;
            mhs = lhs;
            lhs = temp - lhs;
        }
    }
}

struct HashTableNode
{
    struct Node* data;
    struct HashTableNode* next;
};

struct HashTable
{
    struct HashTableNode* nodes;
    size_t buckets;
};

struct HashTable BuildHashTable(struct Node* head, size_t buckets)
{
    struct HashTable table = { cem_calloc(buckets, sizeof(struct HashTableNode)), buckets };

    if (buckets == 0)
        return table;
    
    for (struct Node* it = head; it != NULL; it = it->next)
    {
        size_t hash = it->entry->id % buckets;

        if (table.nodes[hash].data == NULL)
            table.nodes[hash].data = it;
        else
        {
            struct HashTableNode* htIt = &table.nodes[hash];

            for (; ++cmps, htIt->next != NULL; htIt = htIt->next) {}

            htIt->next = cem_calloc(1, sizeof(struct HashTableNode));
            htIt->next->data = it;
        }
    }

    return table;
}

struct Node* HashTableSearch(struct HashTable table, size_t id)
{
    if (table.buckets == 0)
        return NULL;
    
    size_t hash = id % table.buckets;

    for (struct HashTableNode* it = &table.nodes[hash]; it; it = it->next)
    {
        ++cmps;
        if (it->data && it->data->entry->id == id)
            return it->data;
    }

    return NULL;
}

void print_help()
{
    printf("0. Stergerea tuplului recent selectat;\n"
        "1. Metoda secventiala de cautare;\n"
        "2. Metoda de cautare in tabelele neordonate structurate arborescent;\n"
        "3. Metoda binara de cautare;\n"
        "4. Metoda de cautare prin tabela de repartizare cu inlantuirea externa + adresare deschisa;\n"
        "5. Metoda de cautare prin interpolare;\n"
        "6. Metoda Fibonacci de cautare;\n"
        "7. De amestecat pozitiile elementelor din tabel;\n"
        "8. Selecteaza toate tuplele;\n"
        "9. Iesire;\n"
        "10. De afisat acest mesaj.");
}

int main(void)
{
    struct Node* head;
    bool shuffled = false;
    size_t n;
    
    {
        FILE* f;

        if (fopen_s(&f, "database.csv", "r") != 0)
            return 1;

        head = read_db(f, &n);
        fclose(f);
    }

    puts("Lucrarea de laborator nr. 1 la ASDC (c) Cemirtan Cristian 2023\n");

    print_help();

    struct Node* node = NULL;

    for (; ; )
    {
        printf("\n>");
        size_t d;
        scanf_s("%zu", &d);

        switch (d)
        {
        case 0:
            if (node)
            {
                printf("WHERE id = %zu\n", node->entry->id);
                remove_node(node);
                node = NULL;
                --n;
            }

            continue;

        case 7:
            shuffle_db(head);
            shuffled ^= true;
            puts(shuffled ? "activat" : "dezactivat");
            continue;

        case 8:
            write_db(stdout, head);
            continue;

        case 9:
            free_db(head);
            return 0;

        case 10:
            print_help();
            continue;
        }

        size_t id;
        printf("WHERE id = ");
        scanf_s("%zu", &id);

        node = NULL;

        switch (d)
        {
        case 1:
            estimated = (n + 1) / 2.;
            node = SequentialSearch(head, id);
            break;

        case 2:
        {
            void* bst = CreateBST(head);
            show_cmps();
            estimated = log2((double)n) + 2;
            node = BST_Search(bst, id);
            break;
        }

        case 3:
            estimated = log2((double)n) + 2;
            node = BinarySearch(CreateArray(head, n), n, id);
            break;

        case 4:
        {
            size_t buckets;
            printf("Marimea tabelei hash = ");
            scanf_s("%zu", &buckets);

            struct HashTable table = BuildHashTable(head, buckets);
            show_cmps();

            estimated = 1 + (n - 1.) / (2 * buckets);
            node = HashTableSearch(table, id);
            break;
        }

        case 5:
            estimated = log10(log10((double)n));
            node = InterpolationSearch(CreateArray(head, n), n, id);
            break;

        case 6:
            estimated = log2((double)n) + 2;
            node = FibonacciSearch(CreateArray(head, n), n, id);
            break;
        }

        if (node)
            write_entry(stdout, node->entry);

        show_cmps();
        cem_collect();
    }
}