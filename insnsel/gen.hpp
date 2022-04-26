#ifndef __archdesc_gen_HPP__
#define __archdesc_gen_HPP__

class IndentedHeaderBase {
public:
  virtual std::ostream &indent(int n =0) const =0;
  virtual std::ostream &noindent() const =0;
  virtual std::string var(const std::string &base) =0;

private:
};

#endif
