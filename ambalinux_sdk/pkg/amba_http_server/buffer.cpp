#include "public.h"
#include "buffer.h"

Buffer::Buffer(BufferPool* owner)
  : owner_(owner)
  , data_(new uint8_t[owner_->buffer_capacity()]) {
}

Buffer::~Buffer() {
  delete[]data_;
}

asio::mutable_buffers_1 Buffer::asio_read_buffer() {
  return asio::buffer(data_ + length_, owner_->buffer_capacity() - length_);
}

asio::mutable_buffers_1 Buffer::asio_write_buffer() {
  return asio::buffer(data_, length_);
}

bool Buffer::is_full() const {
  return length_ == owner_->buffer_capacity();
}

void Buffer::reset() {
  length_ = 0;
}

uint32_t Buffer::left_capacity() const {
  return owner_->buffer_capacity() - length_;
}

bool Buffer::append(char const* data) {
  return append(data, strlen(data));
}

bool Buffer::append(char const* data, size_t len) {
  if (left_capacity() < len) {
    return false;
  }
  memcpy(data_ + length_, data, len);
  length_ += len;
  return true;
}

void BufferDeleter::operator()(Buffer* buffer) const {
  buffer->owner_->free(buffer);
}

BufferPool::BufferPool(uint32_t buffer_capacity, uint32_t buffer_count)
  : buffer_capacity_(buffer_capacity)
  , buffer_count_(buffer_count) {
  left_buffers_.reserve(buffer_count);
  buffers_ = new Buffer*[buffer_count];
  for (uint32_t i = 0; i < buffer_count; ++i) {
    buffers_[i] = new Buffer(this);
    left_buffers_.push_back(buffers_[i]);
  }
}

BufferPool::~BufferPool() {
  assert(left_buffers_.size() == buffer_count_);
  for (uint32_t i = 0; i < buffer_count_; ++i) {
    delete buffers_[i];
  }
  delete[]buffers_;
}

BufferUPtr BufferPool::allocate() {
  return BufferUPtr(pop());
}

void BufferPool::free(Buffer* buffer) {
  buffer->owner_->push(buffer);
}

Buffer* BufferPool::pop() {
  if (left_buffers_.empty()) {
    return nullptr;
  } else {
    Buffer* buffer = left_buffers_.back();
    left_buffers_.pop_back();
    buffer->reset();
    return buffer;
  }
}

void BufferPool::push(Buffer* buffer) {
  left_buffers_.push_back(buffer);
}