#include "CppUTest/SimpleStringCollection.hpp"

namespace {

const char* StrStr(const char* s1, const char* s2)
{
    if (!*s2)
        return s1;
    std::string::size_type pos = std::string(s1).find(s2);
    if (pos == std::string::npos)
        return nullptr;
    return &s1[pos];
}

size_t count(const SimpleString& str, const SimpleString& substr)
{
    size_t num = 0;
    const char* c_str = str.c_str();
    const char* strpart = nullptr;
    if (*c_str) {
        strpart = StrStr(c_str, substr.c_str());
    }
    while (*c_str && strpart) {
        c_str = strpart;
        c_str++;
        num++;
        strpart = StrStr(c_str, substr.c_str());
    }
    return num;
}

}

SimpleStringCollection::SimpleStringCollection()
{
    collection_ = nullptr;
    size_ = 0;
}

SimpleStringCollection::SimpleStringCollection(
    const SimpleString& orig,
    const SimpleString& delimiter)
{
    size_t num = count(orig, delimiter);
    size_t extraEndToken = (Strings::ends_with(orig, delimiter)) ? 0 : 1U;
    size_ = num + extraEndToken;
    collection_ = new SimpleString[size_];

    const char* str = orig.c_str();
    const char* prev;
    for (size_t i = 0; i < num; ++i) {
        prev = str;
        str = StrStr(str, delimiter.c_str()) + 1;
        collection_[i] = SimpleString(prev).substr(0, size_t(str - prev));
    }
    if (extraEndToken) {
        collection_[num] = str;
    }
}

void SimpleStringCollection::allocate(size_t _size)
{
    delete[] collection_;

    size_ = _size;
    collection_ = new SimpleString[size_];
}

SimpleStringCollection::~SimpleStringCollection()
{
    delete[](collection_);
}

size_t SimpleStringCollection::size() const
{
    return size_;
}

SimpleString& SimpleStringCollection::operator[](size_t index)
{
    if (index >= size_) {
        empty_ = "";
        return empty_;
    }

    return collection_[index];
}

SimpleStringCollection& SimpleStringCollection::operator=(SimpleStringCollection&& rhs)
{
    delete[] collection_;
    collection_ = rhs.collection_;
    rhs.collection_ = nullptr;

    empty_ = rhs.empty_;

    size_ = rhs.size_;
    rhs.size_ = 0;

    return *this;
}
