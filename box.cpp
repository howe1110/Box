#include "box.h"
#include <stdio.h>
#include <iostream>
#include <memory>
#include <algorithm>
#include <cstring>

class A
{
public:
    A(const char *name)
    {
        sprintf(_name, "%s", name);
        _name[strlen(name)] = 0;
    };
    virtual ~A(){};

private:
    char _name[128];

public:
    void show() { printf("My name is %s\n", _name); };
};

template <typename T>
Box<T> testfn(Box<T> rh)
{
    rh->show();
    return rh;
}

int main()
{
    Box<A> s = new_box<A>("zhoululu");
    printf("111\n");
    testfn(s);
    printf("222\n");
    Box<A> t = new_box<A>("zhoumumu");;
    t = s;
    printf("hello world.\n");
}
