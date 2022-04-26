#ifndef __archdesc_insn_HPP__
#define __archdesc_insn_HPP__

#include "ast.hpp"
#include "templatestring.hpp"
#include "error.hpp"
#include "gen.hpp"

class InsnVarType;

class InsnArgDecl {
public:
  InsnArgDecl(const std::string &nm);
  InsnArgDecl(InsnArgDecl &&d) =default;

  inline const std::string &name() const { return m_name; }

  inline InsnVarType &type() const { return *m_customType; }
  inline void setCustomType(std::unique_ptr<InsnVarType> &&ty) { m_customType = std::move(ty); }

  inline bool optional() const { return m_optional; }
  inline InsnArgDecl &makeOptional() { m_optional = true; return *this; }

  enum Direction { Input, Output, InputOutput };

  inline InsnArgDecl &direction(Direction d) { m_dir = d; return *this; }
  inline Direction direction() const { return m_dir; }

  inline const NncErrorContextStack &errorContext() const { return m_context; }

private:
  NncErrorContextStack m_context;
  std::string m_name;
  bool m_optional;
  Direction m_dir;

  std::unique_ptr<InsnVarType> m_customType;
};

class InsnVarType {
public:
  InsnVarType();
  virtual ~InsnVarType();

  virtual std::ostream &outputCType(std::ostream &out) const =0;

  virtual bool isDefaultable() const =0;
  virtual std::ostream &outputArgDefault(std::ostream &out) const =0;
  virtual void outputArgMakeDefault(const std::string &fnName, const std::string &memName,
                                    const std::string &varNm, IndentedHeaderBase &out) const =0;
  virtual std::ostream &outputVisitOperand(const std::string &visitorNm,
                                           const std::string &argNm,
                                           const std::string &memName,
                                           InsnArgDecl::Direction dir,
                                           std::ostream &out) const;

  inline const NncErrorContextStack &errorContext() const { return m_context; }

protected:
  NncErrorContextStack m_context;
};

// Constructors

class ConstructorDecl : public InsnModifier {
public:
  ConstructorDecl();
  ConstructorDecl(ConstructorDecl &&decl) =default;

  void addType(InsnVarType *ty);
  inline std::size_t arity() const { return m_args.size(); }

  inline const NncErrorContextStack &errorContext() const { return m_context; }
  inline NncErrorContextStack &errorContext() { return m_context; }

  virtual void modify(InsnFactory &f);

private:
  NncErrorContextStack m_context;
  std::list<std::unique_ptr<InsnVarType>> m_args;
};

typedef std::list<InsnArgDecl> args_type;

class InsnArgsIterator {
public:
  inline InsnArgsIterator(args_type &a) : m_args(a) { }

  typedef args_type::iterator iterator;
  inline iterator begin() { return m_args.begin(); }
  inline iterator end() { return m_args.end(); }
  inline std::size_t size() const { return m_args.size(); }

private:
  args_type &m_args;
};

typedef std::list<ConstructorDecl> constructors_type;

class InsnBase {
public:
  InsnBase();

  inline InsnArgsIterator args() { return InsnArgsIterator(m_args); }

  std::ostream &declareConstructor(const std::string &cname, std::ostream &out, bool header=false) const;

  void addConstructor(ConstructorDecl &&decl);

protected:
  InsnBase(InsnBase &&o) =default;

  args_type m_args;
  constructors_type m_constructors;
};

class Insn : public InsnBase {
public:
  Insn(const std::string &nm, InsnBase &&o);

  inline const std::string &name() const { return m_name; }

private:
  std::string m_name;
};

class InsnRtlType {
public:
  InsnRtlType(const InsnRtlType &t) =default;
  InsnRtlType();
  InsnRtlType(const std::string &t);

  InsnRtlType ptr() const;

  void outputTypeBuilder(std::ostream &out) const;

  inline bool isPointer() const { return m_ptrs > 0; }
  inline int width() const { return m_width; }

  enum BaseType {
    UnsignedInteger,
    SignedInteger,
    FloatingPoint,
    Unknown
  };

private:
  int m_ptrs;
  BaseType m_type;
  int m_width;
};

class CType : public InsnVarType {
public:
  CType();
  CType(const CType &c) =default;
  virtual ~CType();

  void param(const std::string &name, const Literal &l);

  virtual std::ostream &outputCType(std::ostream &out) const;

  virtual bool isDefaultable() const;
  virtual std::ostream &outputArgDefault(std::ostream &out) const;
  virtual void outputArgMakeDefault(const std::string &fnName, const std::string &memName,
                                    const std::string &varNm, IndentedHeaderBase &out) const;
  virtual std::ostream &outputVisitOperand(const std::string &visitorNm,
                                           const std::string &argName,
                                           const std::string &memName,
                                           InsnArgDecl::Direction dir,
                                           std::ostream &out) const;

  void setRtlType(const InsnRtlType &ty);

  inline bool acceptsRtlType() const { return m_acceptsRtlType; }

protected:
  template_string m_name, m_makeDefault, m_visitOperand;
  std::optional<template_string> m_default;
  std::optional<InsnRtlType> m_rtlType;
  bool m_acceptsRtlType;
};

class InsnRegisterVarType : public InsnVarType {
public:
  InsnRegisterVarType();
  virtual ~InsnRegisterVarType();

  inline void setRtlType(const InsnRtlType &ty) { m_rtlType = ty; }

  virtual std::ostream &outputCType(std::ostream &out) const;
  virtual bool isDefaultable() const;
  virtual std::ostream &outputArgDefault(std::ostream &out) const;
  virtual void outputArgMakeDefault(const std::string &fnName, const std::string &memName,
                                    const std::string &varNm, IndentedHeaderBase &out) const;

 private:
  std::optional<InsnRtlType> m_rtlType;
};

class InsnConstantVarType : public InsnVarType {
public:
  InsnConstantVarType();
  virtual ~InsnConstantVarType();

  inline void setRtlType(const InsnRtlType &ty) { m_rtlType = ty; }

  virtual std::ostream &outputCType(std::ostream &out) const;
  virtual bool isDefaultable() const;
  virtual std::ostream &outputArgDefault(std::ostream &out) const;
  virtual void outputArgMakeDefault(const std::string &fnName, const std::string &memName,
                                    const std::string &varNm, IndentedHeaderBase &out) const;

 private:
  std::optional<InsnRtlType> m_rtlType;
};

class InsnArgsModifier;
class ConstructorDecl;

class InsnFactory : public InsnBase {
public:
  InsnFactory();

  void param(const std::string &name, const Literal &l);

  InsnArgDecl &addArgument(const std::string &nm);
  InsnArgDecl &addArgument(InsnArgDecl &&decl);
  bool hasArgument(const std::string &nm) const;
  InsnArgDecl &argument(const std::string &nm);

  void addConstructor(ConstructorDecl &&con);

  Insn *build(const std::string &name);

protected:
  inline args_type &arguments() { return m_args; }
  friend class InsnArgsModifier;
};

class InsnArgsModifier : public InsnModifier {
public:
  InsnArgsModifier();
  virtual ~InsnArgsModifier();

  InsnArgDecl &addArgument(const std::string &varNm);

  virtual void modify(InsnFactory &f);

private:
  args_type m_args;
};

class InsnVarDeclBase {
public:
  InsnVarDeclBase();

  inline const NncErrorContextStack &errorContext() const { return m_context; }

protected:
  NncErrorContextStack m_context;

  InsnVarDeclBase(InsnVarDeclBase &&o);
};

class InsnVarDecl : public InsnVarDeclBase, public InsnModifier {
public:
  InsnVarDecl(const std::string &varNm,
              InsnVarType *ty,
              InsnVarDeclBase &&o);

  inline InsnVarType &type() const { return *m_type; }
  inline const std::string &name() const { return m_name; }

  virtual void modify(InsnFactory &f);

private:
  std::string m_name;
  std::unique_ptr<InsnVarType> m_type;
};

class InsnVarDeclFactory : public InsnVarDeclBase {
public:
  InsnVarDeclFactory();

  void param(const std::string &name, const Literal &l);

  InsnVarDecl *build(const std::string &varNm, InsnVarType *ty);
};

#endif
