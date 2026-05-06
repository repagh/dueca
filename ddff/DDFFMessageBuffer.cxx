/* ------------------------------------------------------------------   */
/*      item            : DDFFMessageBuffer.cxx
        made by         : Rene' van Paassen
        date            : 161203
        category        : body file
        description     :
        changes         : 161203 first version
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 René van Paassen
        license         : EUPL-1.2
        language        : C++
*/

#define DDFFMessageBuffer_cxx
#include "DDFFMessageBuffer.hxx"
#include "debug.h"
#include <dassert.h>
#include <exception>
#include <limits>

DUECA_NS_START;

#define DEBPRINTLEVEL -1
#include "debprint.h"

struct bufferboundaryexception: public std::exception
{
  const char * what() const noexcept
  { return "attempt to write past buffer size"; }
};

unsigned DDFFMessageBuffer::creation_count = 0;

DDFFMessageBuffer::DDFFMessageBuffer(size_t size, size_t offset) :
  capacity(size),
  fill(0U),
  object_offset(0U),
  stream_id(std::numeric_limits<uint32_t>::max()),
  start_offset(std::numeric_limits<uint32_t>::max()),
  creation_id(creation_count++),
  buffer(new char[capacity])
{
  DEB("DDFFMessageBuffer, creating id=" << creation_id << " size " << size);
}

DDFFMessageBuffer& DDFFMessageBuffer::operator=(const DDFFMessageBuffer& o)
{
  if (this != &o) {
    if (this->capacity < o.fill) {
      throw(bufferboundaryexception());
    }
    this->fill = o.fill;
    this->object_offset = o.object_offset;
    this->stream_id = o.stream_id;
    this->start_offset = o.start_offset;
    const_cast<DDFFMessageBuffer*>(&o)->start_offset = std::numeric_limits<uint32_t>::max();
    std::copy(o.buffer, o.buffer + o.fill, this->buffer);
  }

  return *this;
}

void  DDFFMessageBuffer::reset()
{
  // capacity does not change
  this->fill = 0U;
  this->object_offset = 0U;
  this->stream_id = std::numeric_limits<uint32_t>::max();
  this->start_offset = std::numeric_limits<uint32_t>::max();
}

DDFFMessageBuffer::~DDFFMessageBuffer()
{
  delete [] buffer;
  DEB("DDFFMessageBuffer, deleting id=" << creation_id);
}

void DDFFMessageBuffer::write(const char* data, std::size_t dsize)
{
  if (fill + dsize <= capacity) {
    memcpy(&buffer[fill], data, dsize);
    fill += dsize;
  }
  else {
    throw bufferboundaryexception();
  }
}

void DDFFMessageBuffer::zeroUnused()
{
  if (fill < capacity) {
    memset(&buffer[fill], 0, capacity - fill);
  }
}

DUECA_NS_END;
