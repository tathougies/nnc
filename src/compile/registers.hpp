#ifndef __nnc_compile_registers_HPP__
#define __nnc_compile_registers_HPP__

#include <string>
#include <set>
#include <memory>
#include <map>

#include "compile/rtl.hpp"

namespace nnc::compile {
  class RegisterFile;
  class RegClass;
}

namespace nnc::exception {
  class RegisterDoesNotExist : public std::exception {
  public:
    RegisterDoesNotExist(const ::nnc::compile::RegisterFile *file, const std::string &name);
    virtual ~RegisterDoesNotExist();

    inline const std::string &regName() const { return m_regName; }

    virtual const char *what() const noexcept override;

  private:
    std::string m_regName;
  };

  class RegisterIndexOutOfRange : public std::exception {
  public:
    RegisterIndexOutOfRange(int ix, int max);
    virtual ~RegisterIndexOutOfRange();

    inline int registerIndex() const { return m_ix; }
    inline int maxRegisterIndex() const { return m_max; }

    virtual const char *what() const noexcept override;

  private:
    int m_ix, m_max;
  };

  class VariableNotInClass : public std::exception {
  public:
    VariableNotInClass(const compile::RtlVariablePtr &var,
                       const compile::RegClass &cls);
    virtual ~VariableNotInClass();

    virtual const char *what() const noexcept;

    inline const compile::RegClass &regclass() const { return m_cls; }
    inline const compile::RtlVariablePtr &var() const { return m_var; }

  private:
    compile::RtlVariablePtr m_var;
    const compile::RegClass &m_cls;
  };
}

namespace nnc::compile {
  class RegClassMembers;

  class RegClass {
  public:
    virtual int index() const=0;
    virtual const char *name() const =0;
    virtual const char *desc() const =0;
    virtual RegisterFile &registers() const =0;
    virtual void visitRegisters(RegClassMembers &decl) const =0;
  };

  class Register {
  public:
    Register(const RegisterFile *file, int regno);

    inline int number() const { return m_regno; }
    const char *name() const;
    inline const RegisterFile &registers() const { return *m_regs; }

    std::set<int> classes() const;
    bool belongsToClass(int cls) const;
    inline bool belongsToClass(const RegClass &c) const { return belongsToClass(c.index()); }

    inline bool operator==(const Register &o) const { return o.m_regs == m_regs && o.m_regno == m_regno; }
    inline bool operator!=(const Register &o) const { return !(*this == o); }

    inline bool operator<(const Register &o) const {
      if ( m_regs == o.m_regs ) return m_regno < o.m_regno;
      else return m_regs < o.m_regs;
    }

  private:
    const RegisterFile *m_regs;
    int m_regno;
  };

  class RegClassMembers {
  public:
    virtual void reg(Register r) =0;
  };

  class VirtualRegister {
  public:
    VirtualRegister(const Register &reg);
    VirtualRegister(int spillSlot, int size = 1);
    VirtualRegister(const VirtualRegister &r);

    inline bool isSpill() const { return m_size >= 0; }
    inline bool isRegister() const { return !isSpill(); }
    inline int size() const { if ( m_size < 0 ) return 0; else return m_size; }
    inline int spillSlot() const { if ( isSpill() ) return m_spillSlot; else return -1; }
    inline const Register &reg() const { return m_reg; }

    inline bool operator==(const VirtualRegister &o) const {
      if ( isSpill() && o.isSpill() ) {
        return m_size == o.m_size && m_spillSlot == o.m_spillSlot;
      } else {
        return m_reg == o.m_reg;
      }
    }

    inline bool operator<(const VirtualRegister &o) const {
      if ( isSpill() && o.isSpill() ) {
        if ( m_spillSlot == o.m_spillSlot ) return m_size < o.m_size;
        else return m_spillSlot < o.m_spillSlot;
      } else if ( !isSpill() && o.isSpill() ) return true;
      else if ( isSpill() && !o.isSpill() ) return false;
      else return m_reg < o.m_reg;
    }

  private:
    int m_size;
    union {
      Register m_reg;
      int m_spillSlot;
    };
  };

  class RegisterVisitor {
  public:
    virtual void visitRegister(const Register &r) =0;
  };

  class RegisterFile {
  public:
    virtual ~RegisterFile();

    virtual Register lookupRegister(const std::string &name) const =0;

    virtual const char *registerName(int ix) const =0;

    void visitRegisters(RegisterVisitor &v) const;

    virtual int regclassCount() const =0;
    virtual const char *regclassName(int ix) const =0;

    virtual bool registerInClass(int regIx, int regClass) const =0;

    virtual int registerCount() const =0;

  protected:
    virtual const char *registerFileName() const =0;
  };

  class CallingConvention;

  typedef std::multimap<RtlVariablePtr, VirtualRegister> assignments_type;

  struct RegState {
    void assign(const RtlVariablePtr &var, const VirtualRegister &reg);
    void unassign(const RtlVariablePtr &var, const VirtualRegister &reg);
    //    void replace(const RtlVariablePtr &var, const VirtualRegister &reg);

    assignments_type newAssignments, unAssignments;
  };

  class RtlRegisterMapper {
  public:
    virtual std::vector<VirtualRegister> lookupVar(const RtlVariablePtr &v) const =0;
  };

  class RegisterTracker;
  class RegisterTrackerMapper : public RtlRegisterMapper {
  public:
    virtual ~RegisterTrackerMapper();

    virtual std::vector<VirtualRegister> lookupVar(const RtlVariablePtr &v) const override;

  private:
    RegisterTrackerMapper(const RegisterTracker &t, int cycle);

    const RegisterTracker &m_tracker;
    int m_cycle;

    friend class RegisterTracker;
  };

  class RegisterTracker {
  public:
    RegisterTracker(const std::shared_ptr<RtlBasicBlock> &b);

    void assign(int time, const RtlVariablePtr &var, const VirtualRegister &reg);
    void assign(int time, const CallingConvention &conv);

    template<typename It>
    void assign(int time, const RtlVariablePtr &var, It begin, It end) {
      for ( It i(begin); i != end; ++i )
        assign(time, var, *i);
    }

    void unassign(int time, const RtlVariablePtr &var, const VirtualRegister &reg);

    std::vector<VirtualRegister> assignment(int time, const RtlVariablePtr &var) const;
    inline RegisterTrackerMapper mapper(int cycle) const { return RegisterTrackerMapper(*this, cycle); }

    void alias(const RtlVariablePtr &src, const RtlVariablePtr &dest);

    inline const std::vector<RegState> &states() const { return m_states; }

    RtlVariablePtr canonicalize(const RtlVariablePtr &p) const;
    RtlVariablePtr canonicalize(const RtlVariablePtr &p);

  private:
    std::shared_ptr<RtlBasicBlock> m_block;
    std::vector<RegState> m_states;
    std::map<RtlVariablePtr, RtlVariablePtr> m_aliases;
  };

  std::ostream &operator<<(std::ostream &out, const ::nnc::compile::Register &v);
  std::ostream &operator<<(std::ostream &out, const ::nnc::compile::VirtualRegister &v);

  compile::Register chooseRegister(const compile::RtlRegisterMapper &regs,
                                   const compile::RegClass &cls,
                                   const compile::RtlVariablePtr &b);
}

#endif

