/** Functions for reading the MNIST data format
 */
#ifndef __nnc_mnist_HPP__
#define __nnc_mnist_HPP__

#include "memory_tensor.hpp"

#include <exception>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <arpa/inet.h>

namespace nnc {
  namespace exception {
    class InvalidMnistException : public std::exception {
    public:
      InvalidMnistException (const char* msg);
      virtual ~InvalidMnistException();

      virtual const char *what() const noexcept;

    private:
      const char *m_msg;
    };
  }

  namespace io {
    template<typename T>
    struct MnistTraits {};

    template<>
    struct MnistTraits<std::uint8_t> {
      static constexpr std::uint8_t typeCode = 0x08;
      static inline std::uint8_t fromStored(std::uint8_t x) { return x; }
    };

    template<typename T, int n, typename Allocator = std::allocator<T> >
    class MnistReader : public InMemoryTensor<T, n, Allocator> {
    public:
      MnistReader(const std::string &filepath) {
        readFile(filepath);
      }

      MnistReader(std::istream &input) {
        readStream(input);
      }

      virtual ~MnistReader () {
      }

    private:
      void readFile(const std::string &filepath) {
        std::fstream input(filepath.c_str(), std::istream::in | std::istream::binary);
        readStream(input);
      }

      void readStream(std::istream &input) {
        std::uint16_t header;
        std::uint8_t type_code, dims, i;
        int total_size, ix;
        std::array<int, n> sizes;

        input.read((char *) &header, 2);
        if ( input.eof() || header != 0 )
          throw exception::InvalidMnistException("bad header");

        input.read((char *) &type_code, 1);
        if ( input.eof() || type_code != MnistTraits<T>::typeCode )
          throw exception::InvalidMnistException("type mismatch");

        input.read((char *) &dims, 1);
        if ( input.eof() || dims != n )
          throw exception::InvalidMnistException("dimension mismatch");

        for ( i = 0; i < n; ++i ) {
          std::uint32_t dim_size;
          input.read((char *) &dim_size, 4);
          if ( input.eof() )
            throw exception::InvalidMnistException("dimension mismatch");

          sizes[n - i - 1] = htonl(dim_size);
        }

        total_size = this->calcSize(sizes);
        this->resize(sizes);

        for ( ix = 0; ix < total_size; ++ix ) {
          T d;
          input.read((char *) &d, sizeof(d));
          if ( input.eof() )
            throw exception::InvalidMnistException("data was cut short");

          this->m_data[ix] = MnistTraits<T>::fromStored(d);
        }
      }
    };
  }
}

#endif
