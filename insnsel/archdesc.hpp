#ifndef __archdesc_archdesc_HPP__
#define __archdesc_archdesc_HPP__

#include <memory>
#include <string>
#include <filesystem>

#include "gen.hpp"
#include "ast.hpp"
#include "insnrule.hpp"
#include "insn.hpp"

/* Description */

std::string cName(const std::string &orig, bool full=true);
std::string cInsnName(const std::string &orig);
void declVar(const std::string &name, InsnVariable::Type typ, const IndentedHeaderBase &h);

class GenerateVisitor {
public:
  virtual void generate(const std::filesystem::path &outPath,
                        std::function<void(const std::filesystem::path &)> generator) =0;
};

typedef std::map<std::string, std::unique_ptr<ArchDescLexer>> lexers_type;

class ArchDescBuilder {
public:
  ArchDescBuilder();

  void setArchitecture(const std::string &name);
  void includeFile(const std::string &fromFile, const std::string &filename);
  void includeFile(const std::filesystem::path &path);

  void addCHeader(const std::string &hdr);

  void addRegisterClass(RegClass *regClass);
  void addRegister(Register *reg);
  void addInsnRule(InsnRule *rule);
  void addInsn(Insn *i);
  void addCodeSection(const std::string &sectionName, const std::string &contents);

  void addCType(const std::string &nm, CType *c);
  CType &cType(const std::string &nm);

  const std::unique_ptr<ArchDescLexer> &file(const std::string &filename) const;

  void generate(const std::filesystem::path &outPath);
  void generate(GenerateVisitor &v);

  std::string cArchName() const;
  void includeCHeaders(std::ostream &out) const;

  inline const lexers_type &lexers() const { return m_lexers; }

private:
  void generateRuleImpl(const std::filesystem::path &outPath);
  void generateApply(InsnRule &rule, const IndentedHeaderBase &hdr);
  void generateRuleMatchers(InsnRule &rule, IndentedHeaderBase &hdr);
  void generateInsnsImpl(const std::filesystem::path &outPath);

  void generateScheduleHeader(const std::filesystem::path &outPath);
  void generateInsnsHeader(const std::filesystem::path &outPath);

  void codeSection(std::ostream &out, const std::string &sectionName);

  Insn &lookupInsn(const std::string &nm, const NncErrorContextStack &fromWhere);

  std::optional<std::string> m_name;

  std::list<std::string> m_cheaders;
  std::multimap<std::string, std::string> m_codeSections;

  lexers_type m_lexers;

  std::map<std::string, std::unique_ptr<Register> > m_registers;
  std::map<std::string, std::unique_ptr<RegClass> > m_regClass;
  std::map<std::string, std::unique_ptr<CType> > m_cTypes;

  std::list< std::unique_ptr<InsnRule> > m_rules;
  std::list< std::unique_ptr<Insn> > m_insns;
};

class FilesystemGenerator : public GenerateVisitor {
public:
  FilesystemGenerator(const std::filesystem::path &base);
  ~FilesystemGenerator();

  virtual void generate(const std::filesystem::path &outPath,
                        std::function<void(const std::filesystem::path &)> generator);

private:
  std::filesystem::path m_base;
};

class DepsDumper : public GenerateVisitor {
public:
  DepsDumper(const ArchDescBuilder &b, const std::filesystem::path &relPath, std::ostream &out);
  ~DepsDumper();

  virtual void  generate(const std::filesystem::path &outPath,
                         std::function<void(const std::filesystem::path &)> generator);

private:
  const ArchDescBuilder &m_builder;
  std::ostream &m_out;
  std::filesystem::path m_relative;
};

#endif
