#include "compile/regset.hpp"

#include <iostream>

namespace nnc::compile {
  regset::regset(RegisterFile &regs)
    : m_file(&regs) {
  }

  void regset::insert(const Register &r) {
    insert(r.number());
  }

  void regset::insert(int ix) {
    if ( ix >= m_regs.size() )
      m_regs.resize(ix + 1);

    m_regs[ix] = true;
  }

  void regset::erase(const iterator &i) {
    decltype(m_regs)::iterator u(i.m_underlying);
    *u = false;

    auto last_true(std::find(m_regs.rbegin(), m_regs.rend(), true));
    if ( last_true == m_regs.rend() ) {
      m_regs.clear();
    } else {
      m_regs.erase(last_true.base() + 1, m_regs.end());
    }
  }

  template<typename T, typename It>
  regset::base_iterator<It> regset::find_impl(RegisterFile *file, T regs, int ix) {
    if ( ix >= regs.size() ) return regset::base_iterator<It>(*file, regs.begin(), regs.end(), regs.end());

    auto it(regs.begin() + ix);
    if ( *it ) return regset::base_iterator<It>(*file, regs.begin(), regs.end(), it);
    else return regset::base_iterator<It>(*file, regs.begin(), regs.end(), regs.end());
  }

  template<typename T, typename It>
  regset::base_iterator<It> regset::begin_impl(RegisterFile *file, T regs) {
    auto it(std::find(regs.begin(), regs.end(), true));
    return base_iterator<It>(*file, regs.begin(), regs.end(), it);
  }

  regset::iterator regset::find(int ix) {
    return find_impl<decltype(m_regs) &, decltype(m_regs)::iterator>(m_file, m_regs, ix);
  }

  regset::iterator regset::begin() {
    return begin_impl<decltype(m_regs) &, decltype(m_regs)::iterator>(m_file, m_regs);
  }

  regset::iterator regset::end() {
    return iterator(*m_file, m_regs.begin(), m_regs.end(), m_regs.end());
  }

  regset::const_iterator regset::find(int ix) const {
    return find_impl<const decltype(m_regs) &, decltype(m_regs)::const_iterator>(m_file, m_regs, ix);
  }

  regset::const_iterator regset::begin() const {
    return begin_impl<const decltype(m_regs) &, decltype(m_regs)::const_iterator>(m_file, m_regs);
  }

  regset::const_iterator regset::end() const {
    return const_iterator(*m_file, m_regs.begin(), m_regs.end(), m_regs.end());
  }

  bool regset::empty() const {
    return begin() == end();
  }

  std::size_t regset::size() const {
    return std::count(m_regs.begin(), m_regs.end(), true);
  }

  regset regset::operator~() const {
    regset r(*m_file);
    r.m_regs.resize(m_regs.size());

    std::transform(m_regs.begin(), m_regs.end(), r.m_regs.begin(),
                   [](bool b) { return !b; });

    auto lastTrue(std::find(r.m_regs.rbegin(), r.m_regs.rend(), true));
    if ( lastTrue != r.m_regs.rend() )
      r.m_regs.erase(lastTrue.base() + 1, r.m_regs.end());

    if ( r.m_regs.size() < m_file->registerCount() ) {
      r.m_regs.resize(m_file->registerCount(), true);
    }

    return r;
  }

  regset &regset::operator&=(const regset &o) {
    if ( o.m_regs.size() < m_regs.size() )
      m_regs.resize(o.m_regs.size());

    std::transform(o.m_regs.begin(), o.m_regs.begin() + m_regs.size(),
                   m_regs.begin(), m_regs.begin(),
                   [](bool a, bool b) { return a & b; });

    return *this;
  }

  regset &regset::operator|=(const regset &o) {
    if ( o.m_regs.size() > m_regs.size() )
      m_regs.resize(o.m_regs.size(), false);

    std::transform(o.m_regs.begin(), o.m_regs.end(),
                   m_regs.begin(), m_regs.begin(),
                   [](bool a, bool b) { return a | b; });

    return *this;
  }
}
