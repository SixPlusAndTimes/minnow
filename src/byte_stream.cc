#include "byte_stream.hh"
#include <cassert>
#include <cstdint>
#include <string_view>
#include <iostream>
using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : 
  capacity_( capacity ) , 
  error_(false), 
  storage_(),
  eof_(false), 
  bytes_pushed_(0), 
  bytes_popped_(0),
  bytes_buffered_(0),
  prefix_(0)
{

}

bool Writer::is_closed() const
{
  return eof_;
}

void Writer::push( string data )
{
  if (error_ || eof_) {
    return ;
  }

  if (data.size() > available_capacity()) {
    data.resize(available_capacity()); 
  }

  if (0 == data.size()) {
    return ;
  }

  bytes_buffered_ += data.size();
  bytes_pushed_ += data.size();
  storage_.push(std::move(data));
  return;
}

void Writer::close()
{
  eof_ = true;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - bytes_buffered_;
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
  if (storage_.empty()) {
    return {};
  }
  return string_view(storage_.front().begin() + prefix_, storage_.front().end());
}

void Reader::pop( uint64_t len )
{
  while (len > 0) {
    uint64_t wind_len = storage_.front().size() - prefix_;
    if (len >= wind_len) {
      bytes_popped_ += wind_len;
      bytes_buffered_ -= wind_len;
      len -= wind_len;
      storage_.pop();
      prefix_ = 0;
    } else {
      prefix_ += len;
      bytes_popped_ += len;
      bytes_buffered_ -= len;
      break;
    }
  }
}

uint64_t Reader::bytes_buffered() const
{
  return bytes_buffered_;
}