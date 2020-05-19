#ifndef ARCH_PREFIXTREE_HPP
#define ARCH_PREFIXTREE_HPP

#include <string>
#include <cstring>
#include <cinttypes>
#include <memory>

#include "Code.hpp"

static const int ALPHABET = 256;

class PrefixTree {
 private:
  struct TNode {
    using TNodePtr = std::shared_ptr<TNode>;
    TNode(uint64_t val, TNodePtr l, TNodePtr r) : value(val), left(std::move(l)), right(std::move(r)) {}

    TNode() {}

    uint64_t value;
    TNodePtr left;
    TNodePtr right;
    unsigned char symbol;
  };
  using TNodePtr = std::shared_ptr<TNode>;

 public:
  explicit PrefixTree(uint64_t *values) : values_(new Record[ALPHABET]), root_(nullptr) {
    for (int i = 0; i < ALPHABET; ++i) {
      values_[i].symbol = i;
      values_[i].value = values[i];
      codes_[i] = Code();
    }
  }

  PrefixTree() : values_(nullptr), root_(nullptr) {}

  void BuildTree();

  void ReadTree(FILE *fp);

  void WriteTree(Code &code, unsigned char *symbols_order);

  void FindCodes();

  Code *GetCodes() {
    return codes_;
  }

  void Decode(FILE *in, FILE *out);

  ~PrefixTree() {
    delete[] values_;
  }

 private:

  void FindCodes(TNode *node, Code cur_value);

  void WriteTree(TNode *node, TNode *parent, Code &code, unsigned char *symbols_order, size_t &child_num);


  struct Record {
    uint64_t value;
    unsigned char symbol;

    bool operator<(const PrefixTree::Record &rhs) {
      return value < rhs.value;
    }
  };

  TNodePtr root_;

  // symbol and its frequency
  Record *values_;

  Code codes_[ALPHABET];
};

#endif //ARCH_PREFIXTREE_HPP
