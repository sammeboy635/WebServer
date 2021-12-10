#ifndef __LIST_H
#define __LIST_H

#define SIZE_INCREASE 25
typedef struct Block
{
    void *ptr;
};

class List
{
private:
    int len, size;
    Block *list;
    void check();

public:
    List();
    ~List();
    void add(void *ptr);
    void insert();
    void *index(int i);
    void clear();
    int find(void *ptr);
    int length();
    void remove(void *ptr);
    void remove(int i);
};

#endif
