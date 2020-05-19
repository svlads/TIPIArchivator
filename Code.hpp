#ifndef ARCH_CODE_HPP
#define ARCH_CODE_HPP

#include <algorithm>
#include <cinttypes>
#include <iostream>
#include <cstring>

// Code for char in Huffman algorithm
// Maximum length - 256 bits
// Contains data in blocks 32 bits each
class Code {
 public:
  Code(size_t maxlen = 256) : size_(0), maxlen_(maxlen) {
    code_ = new unsigned char[maxlen / 8];
    memset(code_, 0, maxlen / 8);
  }

  Code &operator=(const Code &other) {
    maxlen_ = other.maxlen_;
    delete[] code_;
    code_ = new unsigned char[other.maxlen_ / 8];
    memcpy(code_, other.code_, maxlen_ / 8);
    size_ = other.size_;
    return *this;
  }

  Code(const Code &other) {
    maxlen_ = other.maxlen_;
    code_ = new unsigned char[other.maxlen_ / 8];
    memcpy(code_, other.code_, maxlen_ / 8);
    size_ = other.size_;
  }

  Code &operator=(Code &&other) {
    delete[] code_;
    code_ = other.code_;
    other.code_ = nullptr;
    size_ = other.size_;
    maxlen_ = other.maxlen_;
    return *this;
  }

  Code(Code &&other) {
    code_ = other.code_;
    other.code_ = nullptr;
    size_ = other.size_;
    maxlen_ = other.maxlen_;
  }

  void Append(int bit) {
    if (size_ < maxlen_) {
      size_t block = size_ >> 3u;
      size_t position = (size_ & 7u);
      if (bit) {
        code_[block] |= (1u << (7 - position));
      } else {
        code_[block] &= ~(1u << (7 - position));
      }
      size_ += 1;
    }
  }

  void Print() const {
    for (int i = 0; i < maxlen_ / 8; ++i) {
      printf("%u ", code_[i]);
    }
    printf("size: %zu\n", size_);
    fflush(stdout);
  }

  unsigned char GetBlock(int index) const {
    return code_[index];
  }

  unsigned char *GetBlockPtr(int index) {
    return code_ + index;
  }

  // Returns size of code in bits;
  size_t GetSize() const {
    return size_;
  }

  ~Code() {
    delete[] code_;
  }

 private:
  unsigned char *code_;
  size_t size_;
  size_t maxlen_;
};


#endif //ARCH_CODE_HPP
