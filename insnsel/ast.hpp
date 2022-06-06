#ifndef __archdesc_ast_HPP__
#define __archdesc_ast_HPP__

#include <optional>
#include <string>
#include <functional>
#include <set>
#include <list>
#include <memory>
#include <filesystem>
#include <map>

#include "templatestring.hpp"

class ArchDescLexer;
class ArchDescBuilder;

class RegType {
public:
  enum RegFormat {
    RegFormatUnknown = 0,
    RegFormatInt,
    RegFormatIEEE754
  };

  RegType();
  RegType(RegFormat fmt, int width);
  RegType(const RegType &t) =default;

  inline int width() const { return m_width; }
  inline RegFormat format() const { return m_format; }

  static RegType fromString(const std::string &s);

  bool operator==(const RegType &t) const;
  bool operator<(const RegType &t) const;

private:
  RegFormat m_format;
  int m_width;
};

class LiteralVisitor {
public:
  virtual ~LiteralVisitor();

  virtual void visit(const std::string &s);
  virtual void visit(std::uint32_t u);
};

class SetLiteral {
public:
  SetLiteral(std::function<void()> onMissing);
  ~SetLiteral();

protected:
  void seen();

private:
  bool m_created;
  std::function<void()> m_onMissing;
};

class WarnMissing {
public:
  WarnMissing(const std::string &name,
              const std::string &type);

  void operator() ();

private:
  std::string m_name, m_type;
};

class SetStringLiteral : public SetLiteral, public virtual LiteralVisitor {
public:
  SetStringLiteral(std::string &s, std::function<void()> onMissing);
  virtual ~SetStringLiteral();

  virtual void visit(const std::string &s);

private:
  std::string &m_target;
};

class SetTemplateStringLiteral : public SetStringLiteral {
public:
  SetTemplateStringLiteral(template_string &s, std::function<void()> onMissing);
  virtual ~SetTemplateStringLiteral();

  virtual void visit(const std::string &s);

private:
  std::string m_internal;
  template_string &m_s;
};

class SetNumberLiteral : public SetLiteral, public virtual LiteralVisitor {
public:
  SetNumberLiteral(std::uint32_t &x, std::function<void()> onMissing);
  virtual ~SetNumberLiteral();

  virtual void visit(std::uint32_t ui);

private:
  std::uint32_t &m_target;
};

class Literal {
public:
  virtual void visit(LiteralVisitor &v) const =0;
};

class StringLiteral : public virtual Literal {
public:
  StringLiteral(const std::string &value);
  virtual ~StringLiteral();

  inline const std::string &value() const { return m_value; }
  virtual void visit(LiteralVisitor &v) const;

private:
  std::string m_value;
};

class NumberLiteral : public virtual Literal {
public:
  NumberLiteral(std::uint32_t v);
  virtual ~NumberLiteral();

  inline std::uint32_t value() const { return m_value; }
  virtual void visit(LiteralVisitor &v) const;

private:
  std::uint32_t m_value;
};

/* Register */
class RegisterBase {
public:
  RegisterBase();

protected:
  RegisterBase(const RegisterBase &b);

  std::set<std::string> m_clobbers;
};

class RegisterFactory : public RegisterBase {
public:
  RegisterFactory();

  void param(const std::string &doc, const Literal &l);

  void addClobber(const std::string &regName);
};

class RegisterModifier {
public:
  virtual void modify(RegisterFactory &f) const =0;
};

class CompositeRegisterModifier : public virtual RegisterModifier {
public:
  virtual void modify(RegisterFactory &f) const;

  void addModifier(RegisterModifier *m);
private:
  std::list<RegisterModifier *> m_mods;
};

class ClobberListDecl : public virtual RegisterModifier {
public:
  virtual ~ClobberListDecl();

  void addRegister(const std::string &nm);

  virtual void modify(RegisterFactory &f) const;

private:
  std::set<std::string> m_registers;
};

/* Regclass */
class RegClassBase {
protected:
  std::set<std::string> m_regNames;

public:
  RegClassBase();

  inline const std::string &doc() const { return m_doc; }
  inline std::uint32_t regCount() const { return m_count; }

  typedef decltype(m_regNames)::iterator iterator;
  inline iterator begin() { return m_regNames.begin(); }
  inline iterator end() { return m_regNames.end(); }

protected:
  RegClassBase(const RegClassBase &bs);

  std::string m_doc;
  std::uint32_t m_count;

  std::set<RegType> m_regTypes;

  friend class RegClassFactory;
};

class RegClass : public RegClassBase {
public:
  RegClass(const std::string &clsNm, const RegClassBase &base);

  inline const std::string &name() const { return m_name; }

  void ensureRegisters(ArchDescBuilder &b) const;

protected:
  std::string m_name;
};

class RegClassFactory : public RegClassBase {
public:
  RegClassFactory();

  void param(const std::string &doc, const Literal &l);

  void addRegister(const std::string &regName);
  void addType(const RegType &regType);

  RegClass *build(const std::string &clsNm) const;
};

class RegClassModifier {
public:
  virtual ~RegClassModifier();
  virtual void modify(RegClassFactory &f) const =0;
};

class RegMemberDecl : public virtual RegClassModifier {
public:
  RegMemberDecl();
  virtual ~RegMemberDecl();

  virtual void modify(RegClassFactory &f) const;

  void addRegister(const std::string &nm);

private:
  std::list<std::string> m_names;
};

class RegMemberDeclarer {
public:
  virtual ~RegMemberDeclarer();
  virtual void modify(RegMemberDecl &d) const =0;
};

class RangedRegMemberDeclarer : public RegMemberDeclarer {
public:
  RangedRegMemberDeclarer();
  virtual ~RangedRegMemberDeclarer();

  void setName(const std::string &name);
  void addRange(int start, int end);

  virtual void modify(RegMemberDecl &d) const override;

private:
  std::string m_name;
  std::list< std::pair<int, int> > m_ranges;
};

class SingleRegMemberDeclarer : public RegMemberDeclarer {
public:
  SingleRegMemberDeclarer(const std::string &nm);
  virtual ~SingleRegMemberDeclarer();

  virtual void modify(RegMemberDecl &d) const override;

private:
  std::string m_name;
};

class RegClassTypeDecl : public virtual RegClassModifier {
public:
  RegClassTypeDecl();
  virtual ~RegClassTypeDecl();

  virtual void modify(RegClassFactory &f) const;

  void addType(const RegType &t);

private:
  std::list<RegType> m_types;
};

/* Params */

class InsnRuleFactory;

class InsnRuleModifier {
public:
  virtual ~InsnRuleModifier();
  virtual void modify(InsnRuleFactory &f) =0;
  virtual bool shouldDeleteInsnRuleModifierAfterModify() const;
};

class InsnFactory;

class InsnModifier {
public:
  virtual ~InsnModifier();
  virtual void modify(InsnFactory &f) =0;
};

class InsnVarDeclFactory;

class InsnVarDeclModifier {
public:
  virtual ~InsnVarDeclModifier();
  virtual void modify(InsnVarDeclFactory &f) =0;
};

class CType;

class CTypeModifier {
public:
  virtual ~CTypeModifier();
  virtual void modify(CType &f) =0;
};

class ParamDecl : public virtual RegClassModifier, public virtual RegisterModifier,
                  public virtual InsnRuleModifier, public virtual InsnModifier, public virtual InsnVarDeclModifier,
                  public virtual CTypeModifier {
public:
  ParamDecl(const std::string &name, Literal *value);
  virtual ~ParamDecl();

  Literal &value() const;
  const std::string &name() const;

  virtual void modify(RegClassFactory &f) const;
  virtual void modify(RegisterFactory &f) const;
  virtual void modify(InsnRuleFactory &f);
  virtual void modify(InsnFactory &f);
  virtual void modify(InsnVarDeclFactory &f);
  virtual void modify(CType &c);

private:
  std::string m_name;
  Literal *m_value;
};


#endif
