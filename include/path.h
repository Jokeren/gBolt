#ifndef INCLUDE_PATH_H_
#define INCLUDE_PATH_H_

namespace gbolt {

template <typename T>
class Path {
 public:
  Path(size_t capacity = 1) : capacity_(capacity), index_(0) {
    entries_ = new T[capacity_]();
  }

  void push_back(T &value) {
    entries_[index_++] = value;
    if (index_ == capacity_) {
      reserve(capacity_ * 2);
    }
  }

  void push_back(T value) {
    entries_[index_++] = value;
    if (index_ == capacity_) {
      reserve(capacity_ * 2);
    }
  }

  T pop_back() {
    return entries_[--index_];
  }

  T operator[](size_t index) {
    return entries_[index];
  }

  T operator[](size_t index) const {
    return entries_[index];
  }

  void reset() {
    this->index_ = 0;
  }

  size_t size() const {
    return this->index_;
  }

  size_t capacity() const {
    return this->capacity_;
  }

  bool empty() const {
    return this->index_ == 0;
  }

  void reserve(size_t new_capacity) {
    T *new_entries = new T[new_capacity]();
    std::copy(entries_, entries_ + capacity_, new_entries);
    delete [] entries_;
    entries_ = new_entries;
    capacity_ = new_capacity;
  }

  ~Path() {
    delete [] entries_;
  }

 private:
  T *entries_;
  size_t index_;
  size_t capacity_;
};

}  // namespace gbolt

#endif  // INCLUDE_PATH_H_
