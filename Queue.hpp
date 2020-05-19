#ifndef ARCH_QUEUE_HPP
#define ARCH_QUEUE_HPP

#include <cstring>
#include <utility>
#include <memory>


template<class T>
class Queue {
 private:
  struct ListNode {
    ListNode() : next(nullptr) {}

    ListNode(T val) : value(std::move(val)), next(nullptr) {}

    T value;
    std::shared_ptr<ListNode> next;
  };
  using ListNodePtr = std::shared_ptr<ListNode>;

 public:
  Queue() : size_(0), root_(std::make_shared<ListNode>()) {
    root_->next = nullptr;
    last_ = root_;
  }

  void Push(T value) {
    auto new_node(std::make_shared<ListNode>(std::move(value)));
    last_->next = new_node;
    last_ = last_->next;
    ++size_;
  }

  void Pop() {
    root_->next = root_->next->next;
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

 private:

  size_t size_;
  ListNodePtr root_;
  ListNodePtr last_;
};

#endif //ARCH_QUEUE_HPP
