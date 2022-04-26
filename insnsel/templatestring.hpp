#ifndef __archdesc_templatestring_HPP__
#define __archdesc_templatestring_HPP__

#include <ostream>
#include <string>
#include <map>
#include <memory>
#include <list>

#include "gen.hpp"

class template_string {
public:
  class template_instance;

private:
  class part {
  public:
    virtual ~part();
    virtual void write(std::ostream &out, const template_instance &i) =0;
  };

  class var_part : public part {
  public:
    virtual ~var_part();

    inline const std::string& name() const { return m_nm; }

    virtual void write(std::ostream &out, const template_instance &i);

    var_part(const std::string &s);
    var_part(const var_part &v);

    std::string m_nm;
  };

public:
  template_string(const std::string &s);
  template_string(template_string &&s);
  template_string(const template_string &s);
  template_string();
  ~template_string();

  var_part var(const std::string &name) const;

  template_string &operator<< (const template_string &s);
  template_string &operator<< (const std::string &s);
  template_string &operator<< (const template_string::var_part &s);

  class template_instance {
  public:
    void var(const std::string &nm, const std::string &value);
    void outputVar(std::ostream &out, const std::string &var) const;

    inline const template_string &base_template() const { return m_s; }

    void emit_by_lines(const IndentedHeaderBase &b);

  private:
    template_instance(const template_string &s);
    std::map<std::string, std::string> m_vars;
    const template_string &m_s;

    friend class ::template_string;
  };

  template_instance render(const std::map<std::string, std::string> &vars) const;
  template_instance render() const;

  void write(std::ostream &out, const template_instance &i) const;
  template_string &operator=(const template_string &o);
  template_string &operator=(template_string &&o);

  template_string &operator+=(const template_string &o);

private:
  class string_part : public virtual part {
  public:
    string_part(const std::string &s);
    virtual ~string_part();

    virtual void write(std::ostream &out, const template_instance &i);
  private:
    std::string m_s;
  };

  void push_string(const std::string &s);

  std::list<std::shared_ptr<part>> m_parts;
};

std::ostream &operator<<(std::ostream &out, const template_string::template_instance &i);

#endif
