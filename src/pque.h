#ifndef _PQUE_H
#define _PQUE_H
#include <iostream>
/* A priority que, where the smallest element is always returned when popped.
 * The que is implemented as an array with insert, remove and pop operations.
 */

template <class T>
class pque {
    T* m_data;
    size_t m_size;
    size_t m_capacity;
    public:
    pque(size_t n = 5);
    ~pque();
    void insert(const T&);
    T pop() const;
    void remove();
    size_t size() const;
    void print() const;
};

template <class T>
pque<T>::pque(size_t n) : m_size(0), m_capacity(n) {
    m_data = new T[n];
}

template <class T>
pque<T>::~pque() {
    delete[] m_data;
}

template <class T>
void pque<T>::insert(const T& obj) {
    m_size++;
    if (m_size > m_capacity) {
        size_t new_capacity = m_capacity*2;
        T* temp = new T[new_capacity];
        std::copy(m_data,m_data+m_capacity,temp);
        delete[] m_data;
        m_data = temp;
        m_capacity = new_capacity;
    }
    m_data[m_size-1] = obj;
    size_t last = m_size, current = m_size/2;
    while(current > 0) {
        if (m_data[current-1] > m_data[last-1]) {
            std::swap(m_data[current-1],m_data[last-1]);
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
T pque<T>::pop() const {
    if (m_size > 0)
        return m_data[0];
    throw "Index out of bounds";
}

/* Removes the smallest element in the heap and repairs the order.
 * Returns void.
 */
template <class T>
void pque<T>::remove() {
    if (m_size < 1)
        return;
    std::swap(m_data[0],m_data[--m_size]);
    size_t current = 0;
    // Loop while there's leaves left
    while((current+1)*2-1 < m_size) {
        size_t l_index = (current+1)*2-1;
        size_t r_index = (l_index < m_size-1) ? l_index+1 : l_index;
        size_t min_child = (m_data[l_index] < m_data[r_index]) ? l_index : r_index;
        if (m_data[current] < m_data[min_child]) break;

        std::swap(m_data[min_child],m_data[current]);
        current = min_child;
    }
}

/* Prints the current array of data.
 */
template <class T>
void pque<T>::print() const {
    for (size_t i = 0; i < m_size; i++) {
        std::cout << m_data[i] << '\n';
    }
}

/* Returns the current size of the heap.
 */
template <class T>
size_t pque<T>::size() const {
    return m_size;
}
#endif
