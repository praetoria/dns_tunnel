#ifndef __VEC_H
#define __VEC_H
#include <iostream>

template <class T>
class vec {
    T* m_data;
    size_t m_size;
    size_t m_capacity;
    public:
    typedef const T* const_iterator;
    typedef T* iterator;
    iterator begin() { return m_data; }
    iterator end() { return m_data+m_size; };
    const_iterator begin() const { return m_data; }
    const_iterator end() const { return m_data+m_size; };
    const T& operator[](size_t);
    vec<T>& operator=(const vec<T>&);
    vec<T>& operator=(vec<T>&);

    size_t size() const;
    void push_back(T el);
    void clear();
    vec(size_t n = 10);
    vec(const vec& obj);
    ~vec();
};

template <class T>
vec<T>::vec(const vec<T>& obj) : m_data(nullptr) {
    m_size = m_capacity = obj.m_size;
    m_data = new T[m_capacity];
    std::copy(obj.m_data,obj.m_data+obj.m_size,m_data);
}

template <class T>
vec<T>& vec<T>::operator=(vec<T>& obj) {
    if (m_data != nullptr)
        delete [] m_data;
    m_size = obj.m_size;
    m_capacity = obj.m_capacity;
    m_data = obj.m_data;
    obj.m_data = nullptr;
    obj.m_size = obj.m_capacity = 0;
    return *this;
}

template <class T>
vec<T>& vec<T>::operator=(const vec<T>& obj) {
    if (m_data != nullptr)
        delete [] m_data;
    m_size = m_capacity = obj.m_size;
    m_data = new T[m_capacity];
    std::copy(obj.m_data,obj.m_data+obj.m_size,m_data);
    return *this;
}

template <class T>
const T& vec<T>::operator[](size_t i) {
    if (i < m_size) {
        return m_data[i];
    }
    return m_data[0];
}

template <class T>
void vec<T>::clear() {
    m_size = 0;
}

template <class T>
vec<T>::vec(size_t n) : m_size(0), m_capacity(n), m_data(nullptr) {
    if (n == 0) {
        m_capacity = 1;
    }
    m_data = new T[m_capacity];
}

template <class T>
vec<T>::~vec() {
    if (m_data != nullptr)
        delete[] m_data;
}

template <class T>
void vec<T>::push_back(T el) {
    if (++m_size > m_capacity) {
        if (m_capacity == 0)
            m_capacity = 1;
        size_t new_capacity = m_capacity*2;
        T* temp = new T[new_capacity];
        if (m_data) {
            std::copy(m_data,m_data+m_size-1,temp);
            delete[] m_data;
        }
        m_data = temp;
        m_capacity = new_capacity;
    }
    m_data[m_size-1] = el;
}

template <class T>
size_t vec<T>::size() const {
    return m_size;
}
#endif
