#ifndef ARCH_PREFIXTREE_HPP
#define ARCH_PREFIXTREE_HPP

#include <string>
#include <cstring>
#include <cinttypes>

#include "Code.hpp"

static const int ALPHABET = 256;

class PrefixTree {
 public:
  explicit PrefixTree(uint64_t *values) : values_(new Record[ALPHABET]), root_(nullptr) {
    for (int i = 0; i < ALPHABET; ++i) {
      values_[i].symbol = i;
      values_[i].value = values[i];
      codes_[i] = Code();
    }
  }

  PrefixTree() = default;

  void BuildTree();

  void ReadTree(FILE *fp);

  void WriteTree(Code &code, unsigned char *symbols_order);

  void FindCodes();

  Code *GetCodes() {
    return codes_;
  }

  void Decode(FILE *in, FILE *out);

  // TODO: delete nodes
  ~PrefixTree() {
    delete[] values_;
    delete root_;
  }

 private:
  struct TNode {
    TNode(uint64_t val, TNode *l, TNode *r) : value(val), left(l), right(r) {}

    TNode() {}

    ~TNode() {
      delete left;
      delete right;
    }

    uint64_t value;
    TNode *left;
    TNode *right;
    unsigned char symbol;
  };

  void FindCodes(TNode *node, Code cur_value);

  void WriteTree(TNode *node, TNode *parent, Code &code, unsigned char *symbols_order, size_t &child_num);


  struct Record {
    uint64_t value;
    unsigned char symbol;

    bool operator<(const PrefixTree::Record &rhs) {
      return value < rhs.value;
    }
  };

  TNode *root_;

  // symbol and its frequency
  Record *values_;

  Code codes_[ALPHABET];
};

#endif //ARCH_PREFIXTREE_HPP
