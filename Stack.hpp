#ifndef ARCH_STACK_HPP
#define ARCH_STACK_HPP

#include <cstring>
#include <utility>
#include <memory>

template<class T>
class Stack {
 private:
  struct ListNode {
    ListNode() {}

    ListNode(T val) : value(std::move(val)), next(nullptr) {}

    T value;
    std::shared_ptr<ListNode> next;
  };

  using ListNodePtr = std::shared_ptr<ListNode>;

 public:
  Stack() : size_(0) {
    root_ = nullptr;
  };

  void Push(T value) {
    // auto *new_node = new ListNode(std::move(value));
    auto new_node = std::make_shared<ListNode>(std::move(value));
    new_node->next = root_;
    root_ = std::move(new_node);
    ++size_;
  }

  void Pop() {
    root_ = root_->next;
    --size_;
  }

  T Front() {
    return root_->value;
  }

  size_t GetSize() {
    return size_;
  }

 private:
  size_t size_;
  ListNodePtr root_;
};


#endif //ARCH_STACK_HPP
