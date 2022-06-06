#ifndef __nnc_compile_x86_64_types_HPP__
#define __nnc_compile_x86_64_types_HPP__

#include "compile/rtl.hpp"
#include "compile/rtl_ops_base.hpp"
#include "compile/regalloc.hpp"

#include <cstdint>
#include <variant>

namespace nnc {
  namespace compile { class RtlRegisterMapper; }

  namespace arch {
    namespace x86_64 {
      namespace types {
        class regmem {
        public:
          regmem(std::shared_ptr<compile::RtlVariable> reg,
                 std::shared_ptr<compile::RtlType> ty = nullptr);
          regmem(regmem &&o) =default;
          regmem(const regmem &m) =default;

          static regmem memref(std::shared_ptr<compile::RtlVariable> reg,
                               std::shared_ptr<compile::RtlType> ty = nullptr);

          inline bool isMemoryRef() const { return m_isMemRef; }
          regmem reference(std::shared_ptr<compile::RtlType> ty = nullptr) const;

          inline std::shared_ptr<compile::RtlVariable> base() const { return m_var; }
          inline std::shared_ptr<compile::RtlType> type() const { return m_type; }

          void visit(const std::string &baseNm, compile::RtlOperandVisitor &v,
                     bool input = true, bool output = false) const;
          void regclasses(const std::string &clnm, compile::RegClassDeclarer  &d, compile::RegClass &base) const;

          regmem &operator=(const regmem &r) =default;
          regmem &operator=(const std::shared_ptr<compile::RtlVariable> &v);

          bool operator==(const regmem &o) const;

        private:
          bool m_isMemRef;
          std::shared_ptr<compile::RtlVariable> m_var;
          std::shared_ptr<compile::RtlType> m_type;
        };

        class modrm {
        public:
          modrm(std::shared_ptr<compile::RtlVariable> reg,
                std::shared_ptr<compile::RtlType> ty = nullptr);
          modrm(modrm &&o) =default;
          modrm(const modrm &m) =default;

          modrm asMemRef() const;
          modrm indexedBy(std::shared_ptr<compile::RtlVariable> ix) const;
          modrm withBase(std::shared_ptr<compile::RtlVariable> base) const;

          static modrm memref(std::shared_ptr<compile::RtlVariable> reg,
                              std::shared_ptr<compile::RtlType> ty,
                              std::uint32_t disp = 0,
                              std::uint8_t scalePow2 = 0);
          static modrm memref(std::shared_ptr<compile::RtlVariable> reg,
                              std::shared_ptr<compile::RtlType> ty,
                              std::shared_ptr<compile::RtlVariable> index,
                              std::uint8_t scalePow2 = 0);

          inline std::shared_ptr<compile::RtlVariable> base() const { return m_baseReg; }
          inline bool isMemoryRef() const { return m_scalePow2 >= 0; }
          inline bool isRegister() const { return m_scalePow2 < 0; }
          inline std::shared_ptr<compile::RtlVariable> indexReg() const { return m_indexReg; }
          inline std::uint32_t displacement() const { return m_displacement; }
          inline std::int8_t scalePow2() const { return m_scalePow2; }

          inline std::shared_ptr<compile::RtlType> type() const { return m_type; }

          bool isAReg(const compile::RtlRegisterMapper &a) const;
          bool isBReg(const compile::RtlRegisterMapper &a) const;
          bool isCReg(const compile::RtlRegisterMapper &a) const;
          bool isDReg(const compile::RtlRegisterMapper &a) const;
          bool isReg(const compile::RtlRegisterMapper &a, const compile::Register &r) const;

          void visit(const std::string &baseNm, compile::RtlOperandVisitor &v,
                     bool input = true, bool output = false) const;
          void regclasses(const std::string &clnm, compile::RegClassDeclarer  &d, compile::RegClass &base) const;
          void intersect(compile::RegClassDeclarer &d, const modrm &o, compile::RegClass &rc) const;

          modrm &operator=(const modrm &o) =default;
          modrm &operator=(const std::shared_ptr<compile::RtlVariable> &v);

          bool operator==(const modrm &o) const;

        private:
          std::shared_ptr<compile::RtlVariable> m_baseReg;
          std::shared_ptr<compile::RtlVariable> m_indexReg;
          std::uint32_t m_displacement;
          std::int8_t m_scalePow2;

          std::shared_ptr<compile::RtlType> m_type;
        };

        class imm {
        public:
          imm(std::uint64_t i, std::shared_ptr<compile::RtlType> ty);
          imm(const std::span<std::uint8_t> &i, std::shared_ptr<compile::RtlType> ty);
          imm(const imm &i) =default;

          inline std::uint64_t value() const { return m_value; }
          inline std::shared_ptr<compile::RtlType> type() const { return m_type; }

          void visit(const std::string &baseNm, compile::RtlOperandVisitor &v) const;

          imm &operator=(const imm &o) =default;
          bool operator==(const imm &o) const;

        private:
          std::uint64_t m_value;
          std::shared_ptr<compile::RtlType> m_type;
        };

        class modrmimm {
        public:
          modrmimm(const std::span<std::uint8_t> &imm, std::shared_ptr<compile::RtlType> ty);
          modrmimm(const modrm &modrm);
          modrmimm(std::shared_ptr<compile::RtlVariable> reg, std::shared_ptr<compile::RtlType> ty = nullptr);

          void regclasses(const std::string &clnm, compile::RegClassDeclarer &d, compile::RegClass &base) const;

          inline bool isImmediate() const { return std::holds_alternative<imm>(m_what); }
          inline bool isModRM() const { return std::holds_alternative<modrm>(m_what); }
          inline bool isRegister() const { return isModRM() && reg().isRegister(); }
          inline bool isMemoryRef() const { return isModRM() && reg().isMemoryRef(); }

          inline imm &immediate() { return std::get<imm>(m_what); }
          inline modrm &reg() { return std::get<modrm>(m_what); }

          inline const imm &immediate() const { return std::get<imm>(m_what); }
          inline const modrm &reg() const { return std::get<modrm>(m_what); }

          void visit(const std::string &baseNm, compile::RtlOperandVisitor &v) const;

          modrmimm &operator=(const modrmimm &o) =default;
          modrmimm &operator=(const std::shared_ptr<compile::RtlVariable> &v);
          modrmimm &operator=(const modrm &o);

          bool operator==(const modrmimm &o) const;

          inline std::shared_ptr<compile::RtlType> type() const {
            if ( isImmediate() ) return immediate().type();
            else return reg().type();
          }

        private:
          std::variant<imm, modrm> m_what;
        };
      }
    }
  }
}

#endif
