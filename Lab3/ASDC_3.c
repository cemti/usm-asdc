#include "BST.h"
#include "DoublyLinkedList.h"
#include "ListStack.h"
#include "ListQueue.h"
#include "BufferStack.h"
#include "BufferQueue.h"
#include <time.h>
#include <string.h>

static struct AbstractType type;
static void* data;

static void print_help(void)
{
    puts("Metode:\n"
        "\tselect  \tParcurgere si afisare;\n"
        "\tinsert  \tInserarea unui element;\n"
        "\tfind    \tCautarea unui element;\n"
        "\terase   \tStergerea unui element;\n"
        "\tinspect \tObtinerea denumirii a tipului de date curent.\n"
        "Setari:\n"
        "\thelp    \tAfiseaza acest mesaj;\n"
        "\tmem     \tAfiseaza amprenta de memorie a tipului de date curent;\n"
        "\treset   \tResetarea tipului de date abstracte;\n"
        "\texit    \tIesire.");
}

static void reset_type(void)
{
    puts("Alegeti tipul de date:\n"
        "1. Lista simplu inlantuita;\n"
        "2. Lista dublu inlantuita;\n"
        "3. Lista simplu inlantuita (circulara);\n"
        "4. Lista dublu inlantuita (circulara);\n"
        "5. Stiva (lista);\n"
        "6. Stiva (buffer);\n"
        "7. Coada (lista);\n"
        "8. Coada (buffer);\n"
        "9. Arbore binar de cautare.");
    
    for (; ; )
    {
        putchar('>');

        size_t n;
        if (scanf_s("%zu", &n) == 0)
            while (getchar() != '\n') {}

        switch (n)
        {
        case 1:
            data = InitLinkedList(&type);
            return;

        case 2:
            data = InitDoublyLinkedList(&type);
            return;

        case 3:
            data = InitCircularLinkedList(&type);
            return;

        case 4:
            data = InitCircularDoublyLinkedList(&type);
            return;

        case 5:
            data = InitListStack(&type);
            return;

        case 6:
            data = InitBufferStack(&type);
            return;

        case 7:
            data = InitListQueue(&type);
            return;

        case 8:
            data = InitBufferQueue(&type);
            return;

        case 9:
            data = InitBST(&type);
            return;

        default:
            puts("Optiune invalida.");
        }
    }
}

int main(void)
{
    get_db();

    printf("Lucrarea de laborator nr. 3 la ASDC (c) Cemirtan Cristian 2023\n"
        "Tupluri disponibile: %zu\n", db_count());

    print_help();
    putchar('\n');
    reset_type();

    for (; ; )
    {
        char input[21] = { '\0' };
        printf("\n>");
        scanf_s(" %s", input, 21);

        if (!strcmp(input, "select"))
        {
            type.print(data, stdout);
        }
        else if (!strcmp(input, "insert"))
        {
            size_t n = 0;
            printf("Nr. de elemente de inserat: ");
            scanf_s("%zu", &n);

            while (n--)
            {
                printf("INSERT id = ");
                size_t id = 0;
                scanf_s("%zu", &id);

                if (--id < db_count())
                {
                    type.insert(data, get_db() + id);
                    puts("succes\n");
                }
                else
                {
                    puts("ID-ul este in afara intervalului tabelei.");
                    break;
                }
            }
        }
        if (!strcmp(input, "find"))
        {
            void* entry = type.find(data);

            if (entry)
                write_entry(stdout, entry);
        }
        if (!strcmp(input, "erase"))
        {
            type.erase(data);
        }
        if (!strcmp(input, "inspect"))
        {
            puts(type.name);
        }
        if (!strcmp(input, "help"))
        {
            print_help();
        }
        else if (!strcmp(input, "mem"))
        {
            printf("Amprenta de memorie: %zu alocari (%zu bytes)\n", cem_allocations(), cem_memusage());
        }
        else if (!strcmp(input, "reset"))
        {
            cem_collect();
            reset_type();
        }
        else if (!strcmp(input, "exit"))
        {
            cem_collect();
            return 0;
        }
    }
}