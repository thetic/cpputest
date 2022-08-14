#ifndef INCLUDED_CPPUTEST_SIMPLESTRINGCOLLECTION_HPP
#define INCLUDED_CPPUTEST_SIMPLESTRINGCOLLECTION_HPP

#include "CppUTest/SimpleString.hpp"

class SimpleStringCollection {
public:
    SimpleStringCollection();
    SimpleStringCollection(const SimpleString& orig, const SimpleString& delimiter);
    ~SimpleStringCollection();

    void allocate(size_t size);

    size_t size() const;
    SimpleString& operator[](size_t index);
    SimpleStringCollection& operator=(SimpleStringCollection&& rhs);

private:
    SimpleString* collection_;
    SimpleString empty_;
    size_t size_;

    void operator=(SimpleStringCollection&);
    SimpleStringCollection(SimpleStringCollection&);
};

#endif // INCLUDED_CPPUTEST_SIMPLESTRINGCOLLECTION_HPP
