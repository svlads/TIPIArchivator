#ifndef ARCH_STACK_HPP
#define ARCH_STACK_HPP

#include <cstring>
#include <utility>

template<class T>
class Stack {
 public:
  Stack() : size_(0) {
    root_ = NULL;
  };

  void Push(T value) {
    auto *new_node = new ListNode(std::move(value));
    new_node->next = root_;
    root_ = new_node;
    ++size_;
  }

  void Pop() {
    auto *new_root = root_->next;
    delete root_;
    root_ = new_root;
    --size_;
  }

  T Front() {
    return root_->value;
  }

  size_t GetSize() {
    return size_;
  }

 private:

  struct ListNode {
    ListNode() {}

    ListNode(T val) : value(std::move(val)), next(NULL) {}

    T value;
    ListNode *next;
  };

  size_t size_;
  ListNode *root_;
};


#endif //ARCH_STACK_HPP
