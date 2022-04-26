#ifndef __nnc_compile_x86_64_types_HPP__
#define __nnc_compile_x86_64_types_HPP__

#include "compile/rtl.hpp"
#include "compile/rtl_ops_base.hpp"

#include <cstdint>
#include <variant>

namespace nnc {
  namespace arch {
    namespace x86_64 {
      namespace types {
        class modrm {
        public:
          modrm(std::shared_ptr<compile::RtlVariable> reg);
          modrm(modrm &&o) =default;
          modrm(const modrm &m) =default;

          static modrm memref(std::shared_ptr<compile::RtlVariable> reg,
                              std::uint32_t disp = 0,
                              std::uint8_t scalePow2 = 0);
          static modrm memref(std::shared_ptr<compile::RtlVariable> reg,
                              std::shared_ptr<compile::RtlVariable> index,
                              std::uint8_t scalePow2 = 0);

          inline std::shared_ptr<compile::RtlVariable> base() const { return m_baseReg; }
          inline bool isMemoryRef() const { return m_scalePow2 >= 0; }
          inline std::shared_ptr<compile::RtlVariable> indexReg() const { return m_indexReg; }
          inline std::uint32_t displacement() const { return m_displacement; }

          void visit(const std::string &baseNm, compile::RtlOperandVisitor &v,
                     bool input = true, bool output = false) const;

          modrm &operator=(const modrm &o) =default;
          modrm &operator=(const std::shared_ptr<compile::RtlVariable> &v);

        private:
          std::shared_ptr<compile::RtlVariable> m_baseReg;
          std::shared_ptr<compile::RtlVariable> m_indexReg;
          std::uint32_t m_displacement;
          std::int8_t m_scalePow2;
        };

        class imm {
        public:
          imm(std::uint64_t i);
          imm(const std::span<std::uint8_t> &i);
          imm(const imm &i) =default;

          inline std::uint64_t value() const { return m_value; }

          void visit(const std::string &baseNm, compile::RtlOperandVisitor &v) const;

          imm &operator=(const imm &o) =default;
        private:
          std::uint64_t m_value;
        };

        class modrmimm {
        public:
          modrmimm(const std::span<std::uint8_t> &imm);
          modrmimm(const modrm &modrm);

          inline bool isImmediate() const { return std::holds_alternative<imm>(m_what); }
          inline bool isRegister() const { return std::holds_alternative<imm>(m_what); }

          inline imm &immediate() { return std::get<imm>(m_what); }
          inline modrm &reg() { return std::get<modrm>(m_what); }

          inline const imm &immediate() const { return std::get<imm>(m_what); }
          inline const modrm &reg() const { return std::get<modrm>(m_what); }

          void visit(const std::string &baseNm, compile::RtlOperandVisitor &v) const;

          modrmimm &operator=(const modrmimm &o) =default;
          modrmimm &operator=(const std::shared_ptr<compile::RtlVariable> &v);
          modrmimm &operator=(const modrm &o);
        private:
          std::variant<imm, modrm> m_what;
        };
      }
    }
  }
}

#endif
