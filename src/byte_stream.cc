#include "byte_stream.hh"
#include <cassert>
#include <cstdint>
#include <string_view>
#include <iostream>
using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : 
  capacity_( capacity + 1) , 
  error_(false), 
  storage_(capacity_ , ' '),
  readptr_(0), 
  writeptr_(0), 
  eof_(false), 
  bytes_pushed_(0), 
  bytes_popped_(0)
{

}

bool Writer::is_closed() const
{
  return eof_;
}

void Writer::push( string data )
{
  for (uint64_t i= 0; i < data.size(); i++) {
    if ((writeptr_ + 1) % capacity_ == readptr_) {
      break;
    }
    printf("i = %lu, writeptr_ = %lu, readptr_ = %lu, capacity_ = %lu\n",i, writeptr_, readptr_, capacity_);
    
    assert(writeptr_ <= storage_.size());
    
    storage_[writeptr_] = data[i];
    writeptr_ = (writeptr_ + 1) % capacity_;
    ++bytes_pushed_;
  }
  cout << "bytes_pushed = " << bytes_pushed_ << endl;
  return;
}

void Writer::close()
{
  eof_ = true;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - ((writeptr_ - readptr_ + capacity_) % capacity_) - 1;
}

uint64_t Writer::bytes_pushed() const
{

  return bytes_pushed_;
}

bool Reader::is_finished() const
{
  return eof_ && (0 == bytes_buffered());
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_;
}

string_view Reader::peek() const
{
  string_view res{};
  if (writeptr_ > readptr_) {
    res = string_view(storage_.begin() + readptr_, storage_.begin() + writeptr_);
  }else if (writeptr_ < readptr_) {

    res = string_view(storage_.begin() + writeptr_, storage_.end());
  }
  cout << "peek: " << res << endl;
  return res;
}

void Reader::pop( uint64_t len )
{
  if (len <= 0) {
    return ;
  }

  if (len < bytes_buffered()) {
    bytes_popped_ += len;
    readptr_ = (readptr_ + len) % capacity_;
  }else {
    bytes_popped_ += bytes_buffered();
    readptr_ = writeptr_;
  }
}

uint64_t Reader::bytes_buffered() const
{
  return (writeptr_ - readptr_ + capacity_) % capacity_;
}
