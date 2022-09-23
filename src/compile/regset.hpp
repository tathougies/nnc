#ifndef __nnc_compile_regset_HPP__
#define __nnc_compile_regset_HPP__

#include "compile/registers.hpp"

namespace nnc::compile {
  class regset {
  private:
    RegisterFile *m_file;
    std::vector<bool> m_regs;

  public:
    typedef Register value_type;

    regset(RegisterFile &regs);
    regset(const regset &r) =default;
    regset(regset &&r) =default;

    regset &operator&=(const regset &o);
    regset &operator|=(const regset &o);

    regset operator~() const;
    inline static regset full(RegisterFile &s) { regset r(s); return ~r; }

    std::size_t size() const;

    inline regset operator&(const regset &o) {
      regset l(*this);
      l &= o;
      return l;
    }

    inline regset operator|(const regset &o) {
      regset l(*this);
      l |= o;
      return l;
    }

    regset &operator=(const regset &r) =default;
    regset &operator=(regset &&r) =default;

    void insert(const Register &r);
    void insert(int ix);

    bool empty() const;
    void clear();

    template<typename It>
    class base_iterator {
    public:
      Register operator*() {
        return Register(m_file, std::distance(m_start, m_underlying));
      }

      base_iterator<It> &operator++() {
        if ( m_underlying == m_end ) return *this;

        m_underlying = std::find(m_underlying + 1, m_end, true);
        return *this;
      }

      base_iterator<It> operator++(int i) {
        base_iterator<It> ret(*this);
        ++(*this);
        return ret;
      }

      bool operator==(const base_iterator<It> &i) const {
        return m_start == i.m_start && m_underlying == i.m_underlying;
      }
      inline bool operator!=(const base_iterator<It> &i) { return !(*this == i); }

      base_iterator(const base_iterator<It> &o) =default;
      base_iterator(base_iterator<It> &&o) =default;

      base_iterator<It> &operator=(const base_iterator<It> &b) =default;
      base_iterator<It> &operator=(base_iterator<It> &&b) =default;
    private:
      base_iterator(RegisterFile &file,
                    const It &begin, const It &end,
                    const It &i)
        : m_file(&file), m_start(begin), m_end(end), m_underlying(i) {
      }

      RegisterFile *m_file;
      It m_start, m_end, m_underlying;

      friend class regset;
    };

    typedef base_iterator<decltype(m_regs)::iterator> iterator;
    typedef base_iterator<decltype(m_regs)::const_iterator> const_iterator;

    void erase(const iterator &i);

    iterator find(int ix);
    inline iterator find(const Register &r) { return find(r.number()); }
    iterator begin();
    iterator end();

    const_iterator find(int ix) const;
    inline const_iterator find(const Register &r) const { return find(r.number()); }
    const_iterator begin() const;
    const_iterator end() const;

  private:
    template<typename T, typename It>
    static base_iterator<It> find_impl(RegisterFile *file, T regs, int ix);
    template<typename T, typename It>
    static base_iterator<It> begin_impl(RegisterFile *file, T regs);
  };
}

#endif
