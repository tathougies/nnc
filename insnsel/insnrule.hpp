#ifndef __archdesc_insnrule_HPP__
#define __archdesc_insnrule_HPP__

#include <string>

#include "ast.hpp"
#include "error.hpp"
#include "templatestring.hpp"
#include "location.hh"

class InsnArgPatterns;

class MnemonicInterestDeclarer {
public:
  virtual void declareInterest(const std::string &mn) =0;
};

class InsnVariableVisitor {
public:
  virtual void visit(const std::string &name, bool isType) =0;
};

class InsnPattern;
class InsnPatternVisitor {
public:
  virtual void newBranch();
  virtual void endBranch();
  virtual void newAlternative();

  virtual void visit(const InsnPattern &p) =0;
};

class InsnPatternBranch {
public:
  inline InsnPatternBranch(InsnPatternVisitor &d) : m_visitor(d) {
    m_visitor.newBranch();
  }
  inline ~InsnPatternBranch() { m_visitor.endBranch(); }

  inline void newAlternative() const { m_visitor.newAlternative(); }

private:
  InsnPatternVisitor &m_visitor;
};

class InsnGen : public InsnRuleModifier {
public:
  void visitVars(InsnVariableVisitor &v) const;
  virtual void visitPatterns(InsnPatternVisitor &d) const =0;
  virtual void declMnemonicInterest(MnemonicInterestDeclarer &d) const =0;

  virtual void modify(InsnRuleFactory &f);

  virtual bool shouldDeleteInsnRuleModifierAfterModify() const;

  int opCount() const;
};

class InsnRuleOptionalPattern : public InsnGen {
public:
  virtual ~InsnRuleOptionalPattern();

  virtual void visitPatterns(InsnPatternVisitor &d) const;
  virtual void declMnemonicInterest(MnemonicInterestDeclarer &d) const;

  static InsnGen *make(InsnGen *a);

private:
  InsnRuleOptionalPattern(std::unique_ptr<InsnGen> &&o);

  std::unique_ptr<InsnGen> m_optional;
};

class InsnRuleCompoundPattern : public InsnGen {
public:
  virtual ~InsnRuleCompoundPattern();

  virtual void visitPatterns(InsnPatternVisitor &d) const;
  virtual void declMnemonicInterest(MnemonicInterestDeclarer &d) const;

protected:
  std::list<std::unique_ptr<InsnGen>> m_gens;
};

class InsnRuleConjunction : public InsnRuleCompoundPattern {
public:
  virtual ~InsnRuleConjunction();

  static InsnGen *make(InsnGen *a, InsnGen *b);

private:
  InsnRuleConjunction();
};

class InsnRuleDisjunction : public InsnRuleCompoundPattern {
public:
  virtual ~InsnRuleDisjunction();

  static InsnGen *make(InsnGen *a, InsnGen *b);

  virtual void visitPatterns(InsnPatternVisitor &d) const;
private:
  InsnRuleDisjunction();
};

class InsnPattern : public InsnGen {
public:
  InsnPattern(const std::string &s, InsnArgPatterns *pats);
  InsnPattern(InsnPattern &&p);
  virtual ~InsnPattern();

  inline const std::string &mnemonic() const { return m_mnemonic; }
  inline InsnArgPatterns &args() const { return *m_pats; }

  virtual void visitPatterns(InsnPatternVisitor &d) const;
  virtual void declMnemonicInterest(MnemonicInterestDeclarer &d) const;

  virtual InsnRuleModifier *modifier();

private:
  std::string m_mnemonic;
  InsnArgPatterns *m_pats;
};

class InsnVariable {
public:
  enum Type {
    RegisterType,
    RegTypeType,
    ConstantType
  };
  InsnVariable(const nnc::location &where,
               const std::string &name,
               Type ty);

  inline const std::string &name() const { return m_name; }
  inline const nnc::location &where() const { return m_where; }
  inline InsnVariable::Type type() const { return m_type; }

protected:
  nnc::location m_where;
  std::string m_name;
  InsnVariable::Type m_type;
};

typedef std::map<std::string, InsnVariable> variables_type;
class InsnRuleVarIterator {
public:
  typedef variables_type::iterator iterator;
  inline InsnRuleVarIterator(variables_type &v) : m_vars(v) { };

  inline iterator begin() { return m_vars.begin(); }
  inline iterator end() { return m_vars.end(); }

private:
  variables_type &m_vars;
};

typedef std::list<template_string> conditions_type;
class InsnRuleCondIterator {
public:
  typedef conditions_type::iterator iterator;
  inline InsnRuleCondIterator(conditions_type &c) : m_conds(c) { };

  inline iterator begin() { return m_conds.begin(); }
  inline iterator end() { return m_conds.end(); }

private:
  conditions_type &m_conds;
};

class AsmInsnCall {
public:
  AsmInsnCall(const std::string &mn);
  AsmInsnCall();

  inline void mnemonic(const std::string &mn) { m_mnemonic = mn; }
  inline const std::string &mnemonic() const { return m_mnemonic; }
  inline const std::list<template_string> &args() const { return m_args; }
  inline const std::map<std::string, template_string> &namedArgs() const { return m_namedArgs; }

  void swap(AsmInsnCall &o);

  void addArg(const template_string &t);
  void addArg(const std::string &name, const template_string &t);

  inline NncErrorContextStack &errorContext() { return m_context; }
  inline const NncErrorContextStack &errorContext() const { return m_context; }

private:
  NncErrorContextStack m_context;
  std::string m_mnemonic;
  std::list<template_string> m_args;
  std::map<std::string, template_string> m_namedArgs;
};

class AsmInsnArg {
public:
  virtual ~AsmInsnArg();
  virtual void addToCall(AsmInsnCall &c) =0;
};

class AsmInsnPositionalArg : public AsmInsnArg {
public:
  AsmInsnPositionalArg(template_string &&s);
  virtual ~AsmInsnPositionalArg();

  virtual void addToCall(AsmInsnCall &c);

protected:
  template_string m_value;
};

class AsmInsnNamedArg : public AsmInsnPositionalArg {
public:
  AsmInsnNamedArg(const std::string &nm, template_string &&s);
  virtual ~AsmInsnNamedArg();

  virtual void addToCall(AsmInsnCall &c);

protected:
  std::string m_name;
};

typedef std::list<AsmInsnCall> asm_type;
class InsnRuleAsmIterator {
public:
  typedef asm_type::iterator iterator;
  inline InsnRuleAsmIterator(asm_type &c) : m_asm(c) { };

  inline iterator begin() { return m_asm.begin(); }
  inline iterator end() { return m_asm.end(); }

private:
  asm_type &m_asm;
};

class InsnRuleBase {
public:
  typedef InsnRuleVarIterator variables;
  typedef InsnRuleAsmIterator generated_asm;

  InsnRuleBase();

  inline InsnRuleVarIterator vars() { return InsnRuleVarIterator(m_variables); }

  inline InsnRuleCondIterator conds() { return InsnRuleCondIterator(m_conds); }
  inline InsnRuleAsmIterator generated() { return InsnRuleAsmIterator(m_generate); }

  inline const template_string &extraPrivate() const { return m_extraPrivate; }

  inline InsnGen &pats() const { return *m_pats; }

  InsnVariable &var(const std::string &name);
  bool has_var(const std::string &name) const;

protected:
  InsnRuleBase(InsnRuleBase &&o) =default;

  variables_type m_variables;
  conditions_type m_conds;
  template_string m_extraPrivate;

  std::unique_ptr<InsnGen> m_pats;

  asm_type m_generate;
};

class InsnRule : public InsnRuleBase {
public:
  InsnRule(const std::string &name,
            InsnRuleBase &&tmpl);

  inline const std::string &name() const { return m_name; }
  inline const NncErrorContextStack &errorContext() const { return m_context; }

private:
  NncErrorContextStack m_context;
  std::string m_name;
};

class InsnRuleFactory : public InsnRuleBase {
public:
  InsnRuleFactory();

  void param(const std::string &doc, const Literal &l);

  void declareVariable(const InsnVariable &v);
  void addCondition(const template_string &s);
  void setPattern(std::unique_ptr<InsnGen> &&gen);
  void addGeneratedInsn(AsmInsnCall &&c);

  InsnRule *build(const std::string &nm);
};

class InsnRuleAsmDecl : public virtual InsnRuleModifier {
public:
  InsnRuleAsmDecl();
  virtual ~InsnRuleAsmDecl();

  void generateInsn(AsmInsnCall &call);

  virtual void modify(InsnRuleFactory &f);

private:
  std::list<AsmInsnCall> m_calls;
};

class InsnRuleVarDecl : public InsnVariable, public virtual InsnRuleModifier {
public:
  InsnRuleVarDecl(const nnc::location &where,
                  const std::string &nm, InsnVariable::Type ty);
  virtual ~InsnRuleVarDecl();

  virtual void modify(InsnRuleFactory &f);
};

class InsnCondDecl : public virtual InsnRuleModifier {
public:
  InsnCondDecl(const std::string &cond);
  virtual ~InsnCondDecl();

  virtual void modify(InsnRuleFactory &f);

private:
  template_string m_cond;
};

class InsnCheckDecl : public virtual InsnRuleModifier {
public:
  InsnCheckDecl(const nnc::location &l, const std::string &cond);
  virtual ~InsnCheckDecl();
  virtual void modify(InsnRuleFactory &f);

private:
  nnc::location m_loc;
  std::string m_check;
};

class InsnArgPattern {
public:
  enum Directionality {
    Input, Output, InputOutput
  };

  InsnArgPattern();

  InsnArgPattern(const nnc::location &l,
                 Directionality dir,
                 const std::string &tyVar,
                 const std::string &regVar);
  InsnArgPattern(const nnc::location &l,
                 Directionality dir,
                 const std::string &regVar);

  inline const nnc::location &where() const { return m_loc; }
  void visitVariables(InsnVariableVisitor &v) const;

  inline const std::optional<std::string> &tyVar() const { return m_tyVar; }
  inline const std::string &var() const { return m_regVar; }

private:
  nnc::location m_loc;
  Directionality m_dir;
  std::optional<std::string> m_tyVar;
  std::string m_regVar;
};

class InsnArgPatterns {
public:
  InsnArgPatterns();

  void addArgument(const std::string &arg,
                   const InsnArgPattern &p);

  void visitVariables(InsnVariableVisitor &v) const;

  typedef std::map<std::string, InsnArgPattern> args_type;
  typedef args_type::iterator iterator;
  typedef args_type::const_iterator const_iterator;

  inline iterator begin() { return m_args.begin(); }
  inline const_iterator begin() const { return m_args.begin(); }

  inline iterator end() { return m_args.end(); }
  inline const_iterator end() const { return m_args.end(); }

private:
  args_type m_args;
};


#endif
