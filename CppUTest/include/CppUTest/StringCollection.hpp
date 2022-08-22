#ifndef INCLUDED_STRINGCOLLECTION_HPP
#define INCLUDED_STRINGCOLLECTION_HPP

#include <string>

namespace cpputest {

class StringCollection {
public:
    StringCollection() = default;
    StringCollection(const std::string& string, char delimiter);
    ~StringCollection();

    StringCollection& operator=(StringCollection&&);

    void allocate(size_t size);

    size_t size() const;
    std::string& operator[](size_t index);

private:
    std::string* collection_ = nullptr;
    std::string empty_;
    size_t size_ = 0;

    void operator=(StringCollection&);
    StringCollection(StringCollection&);
};

}

#endif // INCLUDED_STRINGCOLLECTION_HPP
