#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "list.h"

/**
 * @brief Constructs an dynamic array;
 *
 */
List::List()
{
    // Init the variables
    len = 0;
    size = SIZE_INCREASE;

    // malloc the space;
    list = (Block *)malloc(sizeof(Block *) * size);
}

List::~List()
{
    free(list);
}
/**
 * @brief Add a new object to the list
 *
 * @param ptr Takes the void * of a variable
 */
void List::add(void *ptr)
{
    check();
    list[len].ptr = ptr;
    len++;
}
/**
 * @brief Private function to check the space of the array to see if more is needed
 *
 */
void List::check()
{
    if (len < size)
        return;

    if (size + SIZE_INCREASE < __INT_MAX__)
        if ((list = (Block *)realloc(list, sizeof(Block *) * (size + SIZE_INCREASE))) == nullptr)
            perror("List: Could not allocate more: ");
}
/**
 * @brief Clears list
 *
 */
void List::clear()
{
    if ((list = (Block *)realloc(list, sizeof(Block *) * (size + SIZE_INCREASE))) == nullptr)
        perror("List: couldn't realloc");

    // Reset init;
    len = 0;
    size = SIZE_INCREASE;
}
/**
 * @brief Find the object at the current index;
 *
 * @param i Takes the array index
 * @return void*
 */
void *List::index(int i)
{
    // makes sure index isn't outside the bounds;
    if (i >= len | i < 0)
        return nullptr;

    return list[i].ptr;
}
int List::find(void *ptr)
{
    if (len == 0)
        printf("List: Length of array is 0\n");
    for (int i = 0; i < len; i++)
    {
        if (list[i].ptr == ptr)
            return i;
    }
    return -1;
}
/**
 * @brief Returns the length of the array;
 *
 * @return Return int of size;
 */
int List::length()
{
    return len;
}
void List::remove(void *ptr)
{

    int index = find(ptr);

    remove(index);
    return;
}
void List::remove(int i)
{
    void *ptr;
    if ((ptr = index(i)) == nullptr)
        return;

    if (i < 0) // error
    {
        printf("List: Remove recieced -1 from find\n");
        return;
    }
    else if (i == len)
    {
        list[i].ptr = nullptr;
    }
    else
    {
        list[i].ptr == nullptr;
        list[i].ptr = list[len - 1].ptr;
    }

    len--;
    return;
}