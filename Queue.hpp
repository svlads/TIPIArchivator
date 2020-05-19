#ifndef ARCH_QUEUE_HPP
#define ARCH_QUEUE_HPP

#include <cstring>
#include <utility>


template<class T>
class Queue {
 public:
  Queue() : size_(0) {
    root_ = new ListNode();
    root_->next = nullptr;
    last_ = root_;
  };

  void Push(T value) {
    auto *new_node = new ListNode(std::move(value));
    last_->next = new_node;
    last_ = last_->next;
    ++size_;
  }

  void Pop() {
    auto *new_head = root_->next->next;
    delete root_->next;
    root_->next = new_head;
    --size_;
    if (size_ == 0) {
      last_ = root_;
    }
  }

  T Front() {
    return root_->next->value;
  }

  T Second() {
    return root_->next->next->value;
  }

  size_t GetSize() {
    return size_;
  }

  ~Queue() {
    delete root_;
  }

 private:

  struct ListNode {
    ListNode() : next(nullptr) {}

    ListNode(T val) : value(std::move(val)), next(nullptr) {}

    T value;
    ListNode *next;
  };

  size_t size_;
  ListNode *root_;
  ListNode *last_;
};

#endif //ARCH_QUEUE_HPP
