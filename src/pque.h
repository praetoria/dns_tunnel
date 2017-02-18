#ifndef _PQUE_H
#define _PQUE_H
#include <iostream>
/* A priority que, where the smallest element is always returned when popped.
 * The que is implemented as an array with insert, remove and pop operations.
 */

template <class T>
class pque {
    T* data;
    int m_size;
    int capacity;
    public:
    pque(int n = 5);
    ~pque();
    void insert(const T&);
    T pop();
    void remove();
    int size();
    void print();
};

template <class T>
pque<T>::pque(int n) : m_size(0), capacity(n) {
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
    m_size++;
    if (m_size > capacity) {
        int new_capacity = capacity*2;
        T* temp = new T[new_capacity];
        std::copy(data,data+capacity,temp);
        delete[] data;
        data = temp;
        capacity = new_capacity;
    }
    data[m_size-1] = obj;
    int last = m_size, current = m_size/2;
    while(current > 0) {
        if (data[current-1] > data[last-1]) {
            std::swap(data[current-1],data[last-1]);
            last = current;
            current = current/2;
        } else {
            break;
        }
    }
}

/* Returns the element at the root of the heap (smallest).
 * Does not remove the element, for that you have to call remove.
 */
template <class T>
T pque<T>::pop() {
    if (m_size > 0)
        return data[0];
    throw "Index out of bounds";
}

/* Removes the smallest element in the heap and repairs the order.
 * Returns void.
 */
template <class T>
void pque<T>::remove() {
    if (m_size < 1)
        return;
    std::swap(data[0],data[--m_size]);
    int current = 0;
    // Loop while there's leaves left
    while((current+1)*2-1 < m_size) {
        int l_index = (current+1)*2-1;
        int r_index = (l_index < m_size-1) ? l_index+1 : l_index;
        int min_child = (data[l_index] < data[r_index]) ? l_index : r_index;
        if (data[current] < data[min_child]) break;

        std::swap(data[min_child],data[current]);
        current = min_child;
    }
}

/* Prints the current array of data.
 */
template <class T>
void pque<T>::print() {
    for (int i = 0; i < m_size; i++) {
        std::cout << data[i] << '\n';
    }
}

/* Returns the current size of the heap.
 */
template <class T>
int pque<T>::size() {
    return m_size;
}
#endif
