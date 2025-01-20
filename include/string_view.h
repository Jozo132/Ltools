#pragma once

#include <stdexcept>
// #include <string>
#include <iostream>
#include <stdlib.h>
#include <string.h>


char* sstrstr(char* haystack, char* needle, size_t length) {
    size_t needle_length = strlen(needle);
    size_t i;
    for (i = 0; i < length; i++) {
        if (i + needle_length > length) {
            return NULL;
        }
        if (strncmp(&haystack[i], needle, needle_length) == 0) {
            return &haystack[i];
        }
    }
    return NULL;
}

char* sstrstr(const char* haystack, const char* needle, size_t length) {
    return sstrstr((char*) haystack, (char*) needle, length);
}

class StringView {
private:
    const char* str_;
    size_t offset_;
    size_t length_;

public:

    // Constructor
    StringView(const char* str, size_t length)
        : str_(str), offset_(0), length_(length) {}

    // Overload constructor for C++ strings
    StringView(const std::string& str)
        : str_(str.c_str()), offset_(0), length_(str.length()) {}

    StringView() {
        str_ = nullptr;
        offset_ = 0;
        length_ = 0;
    }


    void deepCopy(const char* str, int length = -1) {
        if (length == -1) length = strlen(str);
        char* copy = (char*) malloc(length + 1);
        memcpy(copy, str, length);
        copy[length] = '\0';
        str_ = copy;
        offset_ = 0;
        length_ = length;
    }

    // Returns the current length
    size_t size() const { return length_; }
    size_t length() const { return length_; }

    void setLength(size_t length) {
        if (length > length_) throw std::out_of_range("Length exceeds string length");
        length_ = length;
    }

    size_t offset() const { return offset_; }

    // Returns true if empty
    bool empty() const { return length_ == 0; }

    // Access a character at a given index
    char operator[](size_t index) const {
        if (index >= length_) throw std::out_of_range("Index out of bounds");
        return str_[offset_ + index];
    }

    // StringView new_string = old_string;
    StringView& operator=(const StringView& other) {
        str_ = other.str_;
        offset_ = other.offset_;
        length_ = other.length_;
        return *this;
    }
    // string_view_1 = const_char_pointer;
    StringView& operator=(const char* str) {
        str_ = str;
        offset_ = 0;
        length_ = strlen(str);
        return *this;
    }


    // Extract a substring as another StringView
    StringView substr(size_t pos, size_t len) const {
        if (pos > length_) throw std::out_of_range("Position out of bounds");
        len = std::min(len, length_ - pos);
        return StringView(str_ + offset_ + pos, len);
    }
    StringView substring(size_t pos, size_t len) const { return substr(pos, len); }

    // Shift characters by a given amount
    StringView shift(size_t amount) {
        if (amount > length_) throw std::out_of_range("Shift exceeds string length");
        StringView result = substr(0, amount);
        offset_ += amount;
        length_ -= amount;
        return result;
    }

    char shift() {
        if (length_ == 0) throw std::out_of_range("Shift exceeds string length");
        char result = str_[offset_];
        offset_++;
        length_--;
        return result;
    }

    // Find the first occurrence of a character
    size_t find(char c) const {
        for (size_t i = 0; i < length_; i++) {
            if (str_[offset_ + i] == c) return i;
        }
        return length_;
    }

    int indexOf(char c) const {
        for (size_t i = 0; i < length_; i++) {
            if (str_[offset_ + i] == c) return i;
        }
        return -1;
    }

    // Match the start of the string with a given prefix
    bool startsWith(const char* prefix) const {
        size_t len = strlen(prefix);
        if (len > length_) return false;
        return strncmp(str_ + offset_, prefix, len) == 0;
    }
    bool startsWith(const std::string& prefix) const { return startsWith(prefix.c_str()); }

    // Match the end of the string with a given suffix
    bool endsWith(const char* suffix) const {
        size_t len = strlen(suffix);
        if (len > length_) return false;
        return strncmp(str_ + offset_ + length_ - len, suffix, len) == 0;
    }
    bool endsWith(const std::string& suffix) const { return endsWith(suffix.c_str()); }

    // Includes a given substring
    bool includes(const char* substr) const {
        size_t len = strlen(substr);
        if (len > length_) return false;
        return sstrstr(str_ + offset_, substr, length_) != NULL;
    }
    bool includes(const std::string& substr) const { return includes(substr.c_str()); }

    // Consumes characters
    int parseInt() {
        int result = 0;
        size_t i = 0;
        while (i < length_ && isdigit(str_[offset_ + i])) {
            result = result * 10 + (str_[offset_ + i] - '0');
            i++;
        }
        if (i == 0) throw std::invalid_argument("No digits found");
        shift(i);
        return result;
    }

    StringView cutAt(char c) {
        size_t i = find(c);
        StringView result = substr(0, i);
        shift(i + 1);
        return result;
    }

    StringView cutUntil(char c) {
        size_t i = find(c);
        StringView result = substr(0, i);
        shift(i);
        return result;
    }
    
    StringView& subtract(StringView& other) {
        int new_length = other.offset_ - offset_;
        if (new_length < 0) throw std::out_of_range("Subtraction exceeds string length");
        if (new_length > length_) throw std::out_of_range("Subtraction exceeds string length");
        length_ = new_length;
        return *this;
    }

    int to_string(char* buffer, size_t buffer_size) const {
        size_t len = std::min(length_, buffer_size - 1);
        memcpy(buffer, str_ + offset_, len);
        buffer[len] = '\0';
        return len;
    }

    char* c_str() const {
        char* result = (char*) malloc(length_ + 1);
        to_string(result, length_ + 1);
        return result;
    }

    // Get the current view as a std::string
    std::string to_string() const {
        return std::string(str_ + offset_, length_);
    }

    // Print for debugging purposes
    void debug_print() const {
        std::cout << "StringView(\"" << to_string() << "\", offset=" << offset_
            << ", length=" << length_ << ")" << std::endl;
    }
};