#pragma once

#include <mutex>
#include <iostream>

template <typename T>
class Box;

template <typename T>
class txRef
{
private:
    txRef(T *ptr) : p(ptr), count(1){};
    ~txRef()
    {
        if (p != nullptr)
        {
            std::cout << "delete ptr " << p << std::endl;
            delete p;
        }
    };
    friend class Box<T>;
    int ref()
    {
        return ++count;
    };
    int unref()
    {
        return --count;
    };
    int getCount()
    {
        return count;
    };

private:
    int count; //引用计数
    T *p;      //基础对象指针
};

template <typename T>
class Box
{
public:
    Box(T *ptr) : ref(new txRef<T>(ptr)){};
    Box(const Box<T> &nref)
    {
        std::cout << " Copy construct." << std::endl;
        std::lock_guard<std::mutex> lck(_mtx);
        ref = nref.ref;
        if (nref.ref == nullptr)
        {
            return;
        }
        ref->ref();
    };
    virtual ~Box()
    {
        std::lock_guard<std::mutex> lck(_mtx);
        if (ref == nullptr)
        {
            return;
        }

        if (ref->unref() <= 0)
        {
            delete ref;
        }
    };

    Box &operator=(const Box<T> &rhs)
    {
        std::cout << "operator= " << std::endl;
        std::lock_guard<std::mutex> lck(_mtx);
        if (&rhs == this)
        {
            return *this;
        }
        if (ref != nullptr)
        {
            if (ref->unref() <= 0)
            {
                delete ref;
            }
        }

        ref = rhs.ref;
        ref->ref();

        return *this;
    };

    T &operator*()
    {
        return *(ref->p);
    };

    T *operator->()
    {
        return ref->p;
    };

    bool isNullPtr()
    {
        if (ref == nullptr)
        {
            return true;
        }
        return (ref->p == nullptr);
    }

    T *get()
    {
        return ref->p;
    }

    T *deatch()
    {
        T *p = ref->p;
        ref->p = nullptr;
        return p;
    }

    bool operator<(const Box<T> &rhs)
    {
        return ref->p < rhs->ref->p;
    }

    inline int getRefCount() { return ref->getCount(); }

private:
    std::mutex _mtx;
    txRef<T> *ref;
};

template <typename T>
class Abox
{
    Box<T> *_box;
};

// FUNCTION TEMPLATE Box
template <class T, class... _Types>
Box<T> new_box(_Types &&... _Args)
{ // make a unique_ptr
    return Box<T>(new T(std::forward<_Types>(_Args)...));
}
