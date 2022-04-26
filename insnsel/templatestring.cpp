#include "templatestring.hpp"

#include <sstream>
#include <iostream>

bool isvarid(char c) {
  return (c >= '0' && c <= '9')  ||
    (c >= 'a' && c <= 'z') ||
    (c >= 'A' && c <= 'Z') ||
    c == '_';
}

template_string::template_instance::template_instance(const template_string &s)
  : m_s(s) {
}

void template_string::template_instance::var(const std::string &nm,
                                             const std::string &val) {
  m_vars.insert_or_assign(nm, val);
}

void template_string::template_instance::outputVar(std::ostream &out, const std::string &var) const {
  auto it(m_vars.find(var));
  if ( it != m_vars.end() )
    out << it->second;
}

void template_string::template_instance::emit_by_lines(const IndentedHeaderBase &b) {
  std::ostringstream out;
  out << *this;

  std::string s(out.str());

  decltype(s)::iterator i(s.begin());
  while ( i != s.end() ) {
    decltype(s)::iterator next(std::find(i, s.end(), '\n'));
    std::string_view line(i, next);

    if ( line.empty() ) b.noindent() << std::endl;
    else b.indent() << line << std::endl;

    if ( next == s.end() ) break;
    else i = next + 1;
  }
}

template_string::part::~part() {
}

template_string::template_string(const std::string &s) {
  push_string(s);
}

template_string::template_string(template_string &&s)
  : m_parts(std::move(s.m_parts)) {
}

template_string::template_string(const template_string &s)
  : m_parts(s.m_parts) {
}

template_string::template_string() {
}

template_string::~template_string() {
}

template_string::var_part template_string::var(const std::string &name) const {
  return var_part(name);
}

template_string::var_part::var_part(const var_part &nm)
  : m_nm(nm.m_nm) {
}

template_string::var_part::var_part(const std::string &nm)
  : m_nm(nm) {
}

template_string::var_part::~var_part() {
}

void template_string::var_part::write(std::ostream &out, const template_instance &i) {
  i.outputVar(out, m_nm);
}

template_string::string_part::string_part(const std::string &s)
  : m_s(s) {
}

template_string::string_part::~string_part() {
}

void template_string::string_part::write(std::ostream &out, const template_instance &i) {
  out << m_s;
}

void template_string::push_string(const std::string &s) {
  // $$ is one dollar signe $$$ is two, etc
  for ( std::string::const_iterator it = s.begin();
       it != s.end();
       ++it ) {
    auto dollarSign(std::find(it, s.end(), '$'));
    if ( dollarSign == s.end() ) {
      *this << std::string(it, dollarSign);
      return;
    } else {
      // Variable
      if ( dollarSign != it )
        *this << std::string(it, dollarSign);
      auto endOfVar(std::find_if_not(dollarSign + 1, s.end(), isvarid));
      if ( endOfVar != (dollarSign + 1) ) {
        std::string varName(dollarSign + 1, endOfVar);
        *this << var(varName);
        it = endOfVar - 1;
      } else {
        auto nextIt(std::find_if_not(dollarSign + 1, s.end(), [](char c) { return c == '$'; }));
        *this << std::string(dollarSign + 1, nextIt);

        it = nextIt - 1;
      }
    }
  }
}

template_string &template_string::operator<<(const std::string &s) {
  m_parts.emplace_back(std::make_shared<string_part>(s));
  return *this;
}

template_string &template_string::operator<<(const var_part &s) {
  m_parts.emplace_back(std::make_shared<var_part>(s));
  return *this;
}

template_string::template_instance template_string::render() const {
  return template_instance(*this);
}

template_string::template_instance template_string::render(const std::map<std::string, std::string> &vars) const {
  template_instance ret(render());
  for ( const auto &e : vars )
    ret.var(e.first, e.second);
  return ret;
}

void template_string::write(std::ostream &out, const template_instance &i) const {
  for ( const auto &p : m_parts ) {
      p->write(out, i);
  }
}

template_string &template_string::operator=(template_string &&o) {
  m_parts = std::move(o.m_parts);
  return *this;
}

template_string &template_string::operator=(const template_string &o) {
  m_parts = o.m_parts;
  return *this;
}

template_string &template_string::operator+=(const template_string &o) {
  std::copy(o.m_parts.begin(), o.m_parts.end(), std::back_insert_iterator(m_parts));
  return *this;
}

std::ostream &operator<<(std::ostream &out, const template_string::template_instance &i) {
  i.base_template().write(out, i);
  return out;
}
