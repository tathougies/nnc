/* -*- major-mode: bison -*- */

%option c++
%option yyclass="ArchDescLexer"

%{
#undef YY_DECL
#define YY_DECL nnc::parser::symbol_type ArchDescLexerImpl::lex_impl()

#include "parser.hpp"
#include <sstream>
#include <map>

class ArchDescLexerImpl : public yyFlexLexer, public virtual ArchDescLexer {
public:
  ArchDescLexerImpl(const std::string &filename, std::istream &in);
  virtual ~ArchDescLexerImpl();

  typedef std::uint64_t fileoff;

  virtual nnc::parser::symbol_type lex();
  virtual int LexerInput(char *buf, int max_size); // Useful for location tracking
  virtual nnc::parser::token_type token_type(std::uint64_t offs);
  virtual fileoff translate_position(const nnc::position &p);
  virtual const std::string &filename() const;

protected:
  void saveToken(nnc::parser::token::yytokentype tok);
  nnc::parser::symbol_type lex_impl();

  void endString();
  void startString();
  std::uint32_t parseNumber();
  void hexEscapeCode();
  void escapeCode(bool multiLineString);
  inline void pushStringChunk() { m_stringVal << YYText(); pushCurLocation(); }

  void pushCurLocation();
  void pushStartLocation();
  inline void pushChar(char c) { m_stringVal << c; }

  inline nnc::location loc() { return nnc::location(m_startLoc, m_curLoc); }
  std::string stringVal() const;

private:
  std::string m_filename;

  std::stringstream m_stringVal;
  nnc::position m_startLoc, m_curLoc;
  fileoff m_startPos, m_curPos;

  std::map<fileoff, nnc::parser::token_type> m_lexResult;
  std::map<int, fileoff> m_lines;
};
%}

%x SLCOMMENT
%x MLCOMMENT
%x MLSTRING
%x QSTRING
%x QID

/*                       Pattern aliases */

ID [a-zA-Z][a-zA-Z0-9_\.]*
HEXDIGIT [0-9a-fA-F]
UI [0-9]+

%%

c_header     { return nnc::parser::make_CHEADER(loc()); }
include      { return nnc::parser::make_INCLUDE(loc()); }
regclass     { return nnc::parser::make_REGCLASS(loc()); }
regtype      { return nnc::parser::make_REGTYPE(loc()); }
constant     { return nnc::parser::make_CONSTANT(loc()); }
register     { return nnc::parser::make_REGISTER(loc()); }
alias        { return nnc::parser::make_ALIAS(loc()); }
type         { return nnc::parser::make_TYPE(loc()); }
clobbers     { return nnc::parser::make_CLOBBERS(loc()); }
insnrule     { return nnc::parser::make_INSNRULE(loc()); }
insn         { return nnc::parser::make_INSN(loc()); }
var          { return nnc::parser::make_VAR(loc()); }
pattern      { return nnc::parser::make_PATTERN(loc()); }
if           { return nnc::parser::make_IF(loc()); }
check        { return nnc::parser::make_CHECK(loc()); }
asm          { return nnc::parser::make_ASM(loc()); }
code         { return nnc::parser::make_CODE(loc()); }
ctype        { return nnc::parser::make_CTYPE(loc()); }
constructor  { return nnc::parser::make_CONSTRUCTOR(loc()); }
intersect    { return nnc::parser::make_INTERSECT(loc()); }

\-           { return nnc::parser::make_DASH(loc()); }
\{           { return nnc::parser::make_OBRACE(loc()); }
\}           { return nnc::parser::make_CBRACE(loc()); }
\(           { return nnc::parser::make_OPAREN(loc()); }
\)           { return nnc::parser::make_CPAREN(loc()); }
\[           { return nnc::parser::make_OBRACKET(loc()); }
\]           { return nnc::parser::make_CBRACKET(loc()); }
;            { return nnc::parser::make_SEMICOLON(loc()); }
\|           { return nnc::parser::make_BAR(loc()); }
\?           { return nnc::parser::make_QUESTIONMARK(loc()); }
:            { return nnc::parser::make_COLON(loc()); }
,            { return nnc::parser::make_COMMA(loc()); }
=            { return nnc::parser::make_EQUAL(loc()); }
\->          { return nnc::parser::make_OUTPUT(loc()); }
\<\->         { return nnc::parser::make_INOUT(loc()); }

\[\[         { BEGIN(MLSTRING); startString(); }

\${ID}       { std::string s(YYText());
               std::string varName(s.begin() + 1, s.end());
               return nnc::parser::make_VARNAME(varName, loc()); }

<MLSTRING>\]\] { BEGIN(0); endString(); return nnc::parser::make_STRING(stringVal(), loc()); }
<MLSTRING,QSTRING,QID>\\[xX]{HEXDIGIT}{1,8} { hexEscapeCode(); }
<MLSTRING>\\. { escapeCode(true); }
<MLSTRING>\][^\]] { pushStringChunk(); }
<MLSTRING>[^\]\\]+ { pushStringChunk(); }

\"           { BEGIN(QSTRING); startString(); }
<QSTRING>\"   { BEGIN(0); endString(); return nnc::parser::make_STRING(stringVal(), loc()); }
<QSTRING>\\. { escapeCode(false); }
<QSTRING>[^\"\\]+ { pushStringChunk(); }

{ID}           { startString(); pushStringChunk(); endString();
               return nnc::parser::make_ID(stringVal(), loc()); }

\<           { BEGIN(QID); startString(); }
<QID>\>      { BEGIN(0); endString(); return nnc::parser::make_ID(stringVal(), loc()); }
<QID>[^\>]+  { pushStringChunk(); }

{UI}           { return nnc::parser::make_NUMBER(parseNumber(), loc()); }

[ \t\n]+     { pushStartLocation(); } /* eat up whitespace */
\/\/         { BEGIN(SLCOMMENT); pushStartLocation(); }
<SLCOMMENT>\n { BEGIN(0); pushStartLocation(); }
<SLCOMMENT>[^\n] { pushStartLocation(); }
\/\*          { BEGIN(MLCOMMENT); pushStartLocation(); }
<MLCOMMENT>[^\*]+ { pushStartLocation(); }
<MLCOMMENT>\*[^\/] { pushStartLocation(); }
<MLCOMMENT>\*\/ { BEGIN(0); pushStartLocation(); }
<<EOF>>      { return nnc::parser::make_EOF(loc()); }

%%

std::unique_ptr<ArchDescLexer> makeLexer(const std::string &filename, std::istream &in) {
    ArchDescLexerImpl *i(new ArchDescLexerImpl(filename, in));
    return std::unique_ptr<ArchDescLexer>(static_cast<ArchDescLexer *>(i));
}

ArchDescLexerImpl::ArchDescLexerImpl(const std::string &filename, std::istream &in)
  : yyFlexLexer(&in),
    m_filename(filename) {
  m_curLoc.filename = &m_filename;
}

void ArchDescLexerImpl::escapeCode(bool multiLineString) {
    auto txt(YYText());
    switch ( txt[1] ) {
    case 'a': pushChar('\a'); return;
    case 'v': pushChar('\v'); return;
    case 'f': pushChar('\f'); return;
    case 't': pushChar('\t'); return;
    case 'r': pushChar('\r'); return;
    case 'n': pushChar('\n'); return;
    default:  pushChar(txt[1]); return;
    }
}

void ArchDescLexerImpl::hexEscapeCode() {
    std::string hexCode(YYText() + 1);
    std::stringstream hexStream(hexCode);
    std::uint32_t charCode;
    hexStream >> std::hex >> charCode;

    pushChar(charCode);
}

void ArchDescLexerImpl::startString() {
    m_stringVal.str("");
}

void ArchDescLexerImpl::endString() {
}

std::string ArchDescLexerImpl::stringVal() const {
    return m_stringVal.str();
}

void ArchDescLexerImpl::pushCurLocation() {
    for ( const char *lexed = YYText(); *lexed; lexed++ ) {
        m_curPos++;
        if ( *lexed == '\n' ) {
            m_curLoc.lines();
            m_lines.insert(std::make_pair(m_curLoc.line, m_curPos));
        } else {
            m_curLoc.columns();
        }
    }
}

void ArchDescLexerImpl::pushStartLocation() {
    pushCurLocation();
    m_startLoc = m_curLoc;
    m_startPos = m_curPos;
}

int ArchDescLexerImpl::LexerInput(char *buf, int max_size) {
  yyin.read(buf, max_size);
  return yyin.gcount();
}

std::uint32_t ArchDescLexerImpl::parseNumber() {
  std::stringstream s(YYText());
  std::uint32_t r;
  s >> r;

  return r;
}

nnc::parser::symbol_type ArchDescLexerImpl::lex() {
    auto symbol(lex_impl());

    pushCurLocation();

    saveToken((nnc::parser::token_type) symbol.kind() );
    return symbol;
}

void ArchDescLexerImpl::saveToken(nnc::parser::token::yytokentype tok) {
    auto existing(m_lexResult.find(m_startPos));
    if ( existing != m_lexResult.end() )
        m_lexResult.erase(existing);

    m_lexResult.insert(std::make_pair(m_startPos, tok));
    m_lexResult.insert(std::make_pair(m_curPos, nnc::parser::token::TOK_WHITESPACE));
}

nnc::parser::token_type ArchDescLexerImpl::token_type(std::uint64_t offs) {
    auto next_token(m_lexResult.upper_bound(offs));
    if ( next_token == m_lexResult.end() ||
         next_token == m_lexResult.begin() )
        return nnc::parser::token::TOK_WHITESPACE;

    next_token--;
    return next_token->second;
}

ArchDescLexerImpl::fileoff ArchDescLexerImpl::translate_position(const nnc::position &p) {
  fileoff lineStart(0);
  if ( p.line > 1 ) {
      auto pos(m_lines.lower_bound(p.line));
      if ( pos != m_lines.end() &&
           pos != m_lines.begin() ) {
          pos--;
          lineStart = pos->second;
      }
  }
  return lineStart + (p.column - 1);
}

const std::string &ArchDescLexerImpl::filename() const { return m_filename; }

ArchDescLexerImpl::~ArchDescLexerImpl() {
}

int yyFlexLexer::yywrap() {
  return 1;
}
