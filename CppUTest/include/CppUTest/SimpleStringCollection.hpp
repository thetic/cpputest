#ifndef INCLUDED_CPPUTEST_SIMPLESTRINGCOLLECTION_HPP
#define INCLUDED_CPPUTEST_SIMPLESTRINGCOLLECTION_HPP

#include <string>

class SimpleStringCollection {
public:
    SimpleStringCollection();
    SimpleStringCollection(const std::string& orig, const std::string& delimiter);
    ~SimpleStringCollection();

    void allocate(size_t size);

    size_t size() const;
    std::string& operator[](size_t index);
    SimpleStringCollection& operator=(SimpleStringCollection&& rhs);

private:
    std::string* collection_;
    std::string empty_;
    size_t size_;

    void operator=(SimpleStringCollection&);
    SimpleStringCollection(SimpleStringCollection&);
};

#endif // INCLUDED_CPPUTEST_SIMPLESTRINGCOLLECTION_HPP
