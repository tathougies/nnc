#ifndef __archdesc_lexer_HPP__
#define __archdesc_lexer_HPP__

#include "FlexLexer.h"
#include <filesystem>

class ArchDescLexer : public yyFlexLexer {
public:
  ArchDescLexer(const std::filesystem::path &src);

  YY_DECL

  inline const std::filesystem::path &source() const { return m_source; }

private:
  std::filesystem::path m_source;
};

#endif
