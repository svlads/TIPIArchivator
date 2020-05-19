#include "PrefixTree.hpp"
#include "Queue.hpp"
#include "Stack.hpp"
#include <iostream>
#include <cassert>
#include <algorithm>

void PrefixTree::BuildTree() {
  std::sort(values_, values_ + ALPHABET);
  Queue<TNodePtr> sums;

  int start = 0;
  while (!values_[start].value) {
    ++start;
  }
  // number of merges left
  int iteration_count = ALPHABET - start - 1;

  while (iteration_count--) {
    uint64_t sum_from_values = UINT64_MAX;
    uint64_t sum_from_nodes = UINT64_MAX;
    uint64_t sum_from_both = UINT64_MAX;
    if (start <= ALPHABET - 2) {
      sum_from_values = values_[start].value + values_[start + 1].value;
    }
    if (start <= ALPHABET - 1 && sums.GetSize() > 0) {
      sum_from_both = values_[start].value + sums.Front()->value;
    }
    if (sums.GetSize() > 1) {
      sum_from_nodes = sums.Front()->value + sums.Second()->value;
    }

    uint64_t minsum = std::min(std::min(sum_from_both, sum_from_values), sum_from_nodes);

    TNodePtr left;
    TNodePtr right;
    TNodePtr next_node;
    if (minsum == sum_from_nodes) {
      left = sums.Front();
      right = sums.Second();
      sums.Pop();
      sums.Pop();
    } else if (minsum == sum_from_values) {
      left = std::make_shared<TNode>(values_[start].value, nullptr, nullptr);
      left->symbol = values_[start].symbol;
      right = std::make_shared<TNode>(values_[start + 1].value, nullptr, nullptr);
      right->symbol = values_[start + 1].symbol;
      start += 2;
    } else {
      left = std::make_shared<TNode>(values_[start].value, nullptr, nullptr);
      left->symbol = values_[start].symbol;
      right = sums.Front();
      sums.Pop();
      start += 1;
    }
    next_node = std::make_shared<TNode>(left->value + right->value, left, right);
    sums.Push(next_node);
  }
  root_ = sums.Front();
}

void PrefixTree::FindCodes() {
  FindCodes(root_.get(), Code());
}

void PrefixTree::FindCodes(TNode* node, Code cur_value) {
  if (node->left == nullptr && node->right == nullptr) {
    codes_[node->symbol] = cur_value;
  } else {
    auto cur_first = cur_value;
    cur_value.Append(0);
    cur_first.Append(1);
    FindCodes(node->left.get(), cur_value);
    FindCodes(node->right.get(), cur_first);
  }
}

void PrefixTree::ReadTree(FILE *fp) {
  uint32_t code_size = 0;
  size_t got = fread(&code_size, 4, 1, fp);
  unsigned char leafs[256];
  fread(leafs, 1, code_size / 2 + 1, fp);
  size_t cur_leaf = 0;

  root_ = std::make_shared<TNode>();
  Stack<TNode*> stack;
  auto cur_node = root_.get();
  while (code_size > 0) {
    unsigned char current;
    fread(&current, 1, 1, fp);
    for (int i = 7; i >= std::max(0, 8 - (int) code_size); --i) {
      if (current & (1 << i)) {
        auto new_node = std::make_shared<TNode>();
        cur_node->left = new_node;
        stack.Push(cur_node);
        cur_node = new_node.get();
      } else {
        cur_node->symbol = leafs[cur_leaf++];
        cur_node->right = nullptr;
        cur_node->left = nullptr;
        auto node = stack.Front();
        stack.Pop();
        while (node->right.get() == cur_node) {
          cur_node = node;
          node = stack.Front();
          stack.Pop();
        }
        stack.Push(node);
        auto new_node = std::make_shared<TNode>();
        cur_node = new_node.get();
        node->right = new_node;
      }
    }
    if (code_size > 8) code_size -= 8;
    else code_size = 0;
  }
  cur_node->symbol = leafs[cur_leaf++];
  cur_node->left = nullptr;
  cur_node->right = nullptr;
}

void PrefixTree::WriteTree(Code &code, unsigned char *symbols_order) {
  // Code code(2 * 256);
  size_t child = 0;
  WriteTree(root_.get(), root_.get(), code, symbols_order, child);
}

void PrefixTree::WriteTree(PrefixTree::TNode *node, PrefixTree::TNode *parent, Code &code, unsigned char *symbols_order,
                           size_t &child) {
  if (node->left != nullptr && node->right != nullptr) {
    code.Append(1);
    WriteTree(node->left.get(), node, code, symbols_order, child);
    code.Append(0);
    WriteTree(node->right.get(), node, code, symbols_order, child);
  } else {
    symbols_order[child++] = node->symbol;
  }
}

void PrefixTree::Decode(FILE *in, FILE *out) {
  uint32_t buffer[4096];
  size_t file_size;
  fread(&file_size, 4, 1, in);
  size_t got = fread(buffer, 4, 4096, in);
  unsigned char result[4096 * 8 * 4];
  size_t index_result = 0;
  TNode *cur = root_.get();

  while (file_size > 0) {

    for (size_t i = 0; i < got; ++i) {
      for (int j = 31; j >= 0; --j) {
        if (buffer[i] & (1u << j)) {
          cur = cur->right.get();
        } else {
          cur = cur->left.get();
        }
        if (cur->left == nullptr) {
          assert(cur->right == nullptr);
          result[index_result++] = cur->symbol;
          --file_size;
          if (!file_size) {
            break;
          }
          cur = root_.get();
        }
      }
      if (!file_size) {
        break;
      }
    }
    fwrite(result, 1, index_result, out);
    index_result = 0;
    got = fread(buffer, 4, 4096, in);
  }
}
