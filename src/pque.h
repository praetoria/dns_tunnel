#ifndef _PQUE_H
#define _PQUE_H

template <class T>
class pque {
    T* data;
    int size;
    int capacity;
    public:
    pque(int n = 5);
    ~pque();
    void insert(const T&);
    T pop();
    void remove();
};

template <class T>
pque<T>::pque(int n) : size(0), capacity(n) {
    if (n < 0) {
        capacity = n = 0;
    }
    data = new T[n];
}

template <class T>
pque<T>::~pque() {
    delete[] data;
}
template <class T>
void pque<T>::insert(const T& obj) {
    size++;
    if (size > capacity) {
        int new_capacity = capacity*2;
        T* temp = new T[new_capacity];
        std::copy(data,data+capacity,temp);
        delete[] data;
        data = temp;
        capacity = new_capacity;
    }
    data[size-1] = obj;
    int last = size, current = size/2;
    while(current > 0) {
        if (data[current-1] > data[last-1]) {
            std::swap(data[current-1],data[last-1]);
        }
        last = current;
        current = current/2;
    }
}
template <class T>
T pque<T>::pop() {
    return data[0];
}
template <class T>
void pque<T>::remove() {
}
#endif
