#pragma once

class BufferPool;

struct Buffer {
public:  
  friend class BufferPool;
  BufferPool* const owner_;
  gsl::owner<uint8_t*> const data_;
  uint32_t length_ = 0;
  asio::mutable_buffers_1 asio_read_buffer();
  asio::mutable_buffers_1 asio_write_buffer();
  bool is_full() const;
  void reset();
  bool append(char const* data);
  bool append(char const* data, size_t len);
  uint32_t left_capacity() const;
private:
  Buffer(BufferPool* owner);  
  ~Buffer();
  Buffer(Buffer const&) = delete;
  Buffer& operator=(Buffer const&) = delete;
  Buffer(Buffer &&) = delete;
  Buffer& operator=(Buffer &&) = delete;
};

struct BufferDeleter {
  void operator()(Buffer* buffer) const;
};

typedef std::unique_ptr<Buffer, BufferDeleter> BufferUPtr;

class BufferPool {
public:
  friend struct BufferDeleter;
  BufferPool(uint32_t buffer_capacity, uint32_t buffer_count);
  ~BufferPool();
  uint32_t buffer_capacity() {
    return buffer_capacity_;
  }    

  BufferUPtr allocate();
private:
  void free(Buffer* buffer);
  Buffer* pop();
  void push(Buffer* buffer);
private:
  const uint32_t buffer_capacity_;
  const uint32_t buffer_count_;
  std::vector<Buffer*> left_buffers_;
  gsl::owner<gsl::owner<Buffer*>*> buffers_;
};

typedef std::unique_ptr<BufferPool> BufferPoolUPtr;