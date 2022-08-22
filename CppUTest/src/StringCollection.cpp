#include "CppUTest/StringCollection.hpp"

#include <algorithm>
#include <cstring>

namespace cpputest {

namespace {
    const char* StrStr(const char* s1, const char* s2)
    {
        if (!*s2)
            return s1;
        for (; *s1; s1++)
            if (std::strncmp(s1, s2, std::strlen(s2)) == 0)
                return s1;
        return nullptr;
    }
}

StringCollection::StringCollection(const std::string& string, char delimiter)
{
    size_t num = std::count(string.begin(), string.end(), delimiter);
    size_t extraEndToken = (string[string.length() - 1] == delimiter) ? 0 : 1U;
    allocate(num + extraEndToken);

    const char* str = string.c_str();
    const char* prev;
    for (size_t i = 0; i < num; ++i) {
        prev = str;
        str = StrStr(str, std::string(1, delimiter).c_str()) + 1;
        collection_[i] = std::string(prev).substr(0, size_t(str - prev));
    }
    if (extraEndToken) {
        collection_[num] = str;
    }
}

StringCollection& StringCollection::operator=(StringCollection&& rhs)
{
    delete[] collection_;
    collection_ = rhs.collection_;
    rhs.collection_ = nullptr;

    size_ = rhs.size_;
    rhs.size_ = 0;

    return *this;
}

void StringCollection::allocate(size_t _size)
{
    delete[] collection_;

    size_ = _size;
    collection_ = new std::string[size_];
}

StringCollection::~StringCollection()
{
    delete[](collection_);
}

size_t StringCollection::size() const
{
    return size_;
}

std::string& StringCollection::operator[](size_t index)
{
    if (index >= size_) {
        empty_ = "";
        return empty_;
    }

    return collection_[index];
}

}
