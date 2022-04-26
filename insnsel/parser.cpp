// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "parser.hpp"


// Unqualified %code blocks.
#line 161 "/home/tathougies/Projects/nnc/./insnsel/parser.y"

  #define yylex(x) lexer.lex()

#line 50 "./insnsel/parser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 10 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
namespace nnc {
#line 143 "./insnsel/parser.cpp"

  /// Build a parser object.
  parser::parser (ArchDescLexer &lexer_yyarg, ArchDescBuilder &builder_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      yy_lac_established_ (false),
      lexer (lexer_yyarg),
      builder (builder_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_asmInsnArg: // asmInsnArg
        value.YY_MOVE_OR_COPY< AsmInsnArg * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_asmInsn: // asmInsn
      case symbol_kind::S_asmInsnArgs: // asmInsnArgs
      case symbol_kind::S_moreAsmInsnArgs: // moreAsmInsnArgs
        value.YY_MOVE_OR_COPY< AsmInsnCall * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ctypeBody: // ctypeBody
        value.YY_MOVE_OR_COPY< CType * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ctypeBodyDecl: // ctypeBodyDecl
        value.YY_MOVE_OR_COPY< CTypeModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_clobberRegisterList: // clobberRegisterList
        value.YY_MOVE_OR_COPY< ClobberListDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_constructorDecl: // constructorDecl
      case symbol_kind::S_constructorArgList: // constructorArgList
      case symbol_kind::S_moreConstructorArgs: // moreConstructorArgs
        value.YY_MOVE_OR_COPY< ConstructorDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnDecl: // insnDecl
        value.YY_MOVE_OR_COPY< Insn * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgDeclDirection: // insnArgDeclDirection
        value.YY_MOVE_OR_COPY< InsnArgDecl::Direction > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgPattern: // insnArgPattern
        value.YY_MOVE_OR_COPY< InsnArgPattern > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgDirectionality: // insnArgDirectionality
        value.YY_MOVE_OR_COPY< InsnArgPattern::Directionality > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgs: // insnArgs
      case symbol_kind::S_moreInsnArgs: // moreInsnArgs
        value.YY_MOVE_OR_COPY< InsnArgPatterns * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgsDecl: // insnArgsDecl
      case symbol_kind::S_insnArgsDeclList: // insnArgsDeclList
      case symbol_kind::S_moreInsnArgsDeclList: // moreInsnArgsDeclList
        value.YY_MOVE_OR_COPY< InsnArgsModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnBody: // insnBody
      case symbol_kind::S_insnBodyDecls: // insnBodyDecls
        value.YY_MOVE_OR_COPY< InsnFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_patternsDisjunction: // patternsDisjunction
      case symbol_kind::S_morePatternsDisjunction: // morePatternsDisjunction
      case symbol_kind::S_patternsConjunction: // patternsConjunction
      case symbol_kind::S_morePatternsConjunction: // morePatternsConjunction
      case symbol_kind::S_baseInsnPattern: // baseInsnPattern
        value.YY_MOVE_OR_COPY< InsnGen * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnBodyDecl: // insnBodyDecl
        value.YY_MOVE_OR_COPY< InsnModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_simpleInsnPattern: // simpleInsnPattern
        value.YY_MOVE_OR_COPY< InsnPattern * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_rtlType: // rtlType
        value.YY_MOVE_OR_COPY< InsnRtlType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnRuleDecl: // insnRuleDecl
        value.YY_MOVE_OR_COPY< InsnRule * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_asmDecl: // asmDecl
        value.YY_MOVE_OR_COPY< InsnRuleAsmDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnRuleBody: // insnRuleBody
        value.YY_MOVE_OR_COPY< InsnRuleFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnRuleBodyDecl: // insnRuleBodyDecl
      case symbol_kind::S_patternsDecl: // patternsDecl
        value.YY_MOVE_OR_COPY< InsnRuleModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnVarDecl: // insnVarDecl
        value.YY_MOVE_OR_COPY< InsnVarDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnVarDeclBody: // insnVarDeclBody
      case symbol_kind::S_insnVarDeclBodyDecls: // insnVarDeclBodyDecls
        value.YY_MOVE_OR_COPY< InsnVarDeclFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnVarDeclBodyDecl: // insnVarDeclBodyDecl
        value.YY_MOVE_OR_COPY< InsnVarDeclModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_constructorArgTy: // constructorArgTy
      case symbol_kind::S_insnVarTyDecl: // insnVarTyDecl
        value.YY_MOVE_OR_COPY< InsnVarType * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnRuleVarType: // insnRuleVarType
        value.YY_MOVE_OR_COPY< InsnVariable::Type > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_literal: // literal
        value.YY_MOVE_OR_COPY< Literal * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        value.YY_MOVE_OR_COPY< ParamDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassBody: // regclassBody
        value.YY_MOVE_OR_COPY< RegClassFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassBodyDecl: // regclassBodyDecl
        value.YY_MOVE_OR_COPY< RegClassModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassTypeDecl: // regclassTypeDecl
        value.YY_MOVE_OR_COPY< RegClassTypeDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassRegDecl: // regclassRegDecl
        value.YY_MOVE_OR_COPY< RegMemberDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regtype: // regtype
        value.YY_MOVE_OR_COPY< RegType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_registerBody: // registerBody
        value.YY_MOVE_OR_COPY< RegisterFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        value.YY_MOVE_OR_COPY< RegisterModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_optionalPattern: // optionalPattern
        value.YY_MOVE_OR_COPY< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_insnArgTypePattern: // insnArgTypePattern
      case symbol_kind::S_insnArgExprPattern: // insnArgExprPattern
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMBER: // NUMBER
        value.YY_MOVE_OR_COPY< std::uint32_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_asmInsnExpr: // asmInsnExpr
        value.YY_MOVE_OR_COPY< template_string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_asmInsnArg: // asmInsnArg
        value.move< AsmInsnArg * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_asmInsn: // asmInsn
      case symbol_kind::S_asmInsnArgs: // asmInsnArgs
      case symbol_kind::S_moreAsmInsnArgs: // moreAsmInsnArgs
        value.move< AsmInsnCall * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ctypeBody: // ctypeBody
        value.move< CType * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ctypeBodyDecl: // ctypeBodyDecl
        value.move< CTypeModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_clobberRegisterList: // clobberRegisterList
        value.move< ClobberListDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_constructorDecl: // constructorDecl
      case symbol_kind::S_constructorArgList: // constructorArgList
      case symbol_kind::S_moreConstructorArgs: // moreConstructorArgs
        value.move< ConstructorDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnDecl: // insnDecl
        value.move< Insn * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgDeclDirection: // insnArgDeclDirection
        value.move< InsnArgDecl::Direction > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgPattern: // insnArgPattern
        value.move< InsnArgPattern > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgDirectionality: // insnArgDirectionality
        value.move< InsnArgPattern::Directionality > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgs: // insnArgs
      case symbol_kind::S_moreInsnArgs: // moreInsnArgs
        value.move< InsnArgPatterns * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgsDecl: // insnArgsDecl
      case symbol_kind::S_insnArgsDeclList: // insnArgsDeclList
      case symbol_kind::S_moreInsnArgsDeclList: // moreInsnArgsDeclList
        value.move< InsnArgsModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnBody: // insnBody
      case symbol_kind::S_insnBodyDecls: // insnBodyDecls
        value.move< InsnFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_patternsDisjunction: // patternsDisjunction
      case symbol_kind::S_morePatternsDisjunction: // morePatternsDisjunction
      case symbol_kind::S_patternsConjunction: // patternsConjunction
      case symbol_kind::S_morePatternsConjunction: // morePatternsConjunction
      case symbol_kind::S_baseInsnPattern: // baseInsnPattern
        value.move< InsnGen * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnBodyDecl: // insnBodyDecl
        value.move< InsnModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_simpleInsnPattern: // simpleInsnPattern
        value.move< InsnPattern * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_rtlType: // rtlType
        value.move< InsnRtlType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnRuleDecl: // insnRuleDecl
        value.move< InsnRule * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_asmDecl: // asmDecl
        value.move< InsnRuleAsmDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnRuleBody: // insnRuleBody
        value.move< InsnRuleFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnRuleBodyDecl: // insnRuleBodyDecl
      case symbol_kind::S_patternsDecl: // patternsDecl
        value.move< InsnRuleModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnVarDecl: // insnVarDecl
        value.move< InsnVarDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnVarDeclBody: // insnVarDeclBody
      case symbol_kind::S_insnVarDeclBodyDecls: // insnVarDeclBodyDecls
        value.move< InsnVarDeclFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnVarDeclBodyDecl: // insnVarDeclBodyDecl
        value.move< InsnVarDeclModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_constructorArgTy: // constructorArgTy
      case symbol_kind::S_insnVarTyDecl: // insnVarTyDecl
        value.move< InsnVarType * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnRuleVarType: // insnRuleVarType
        value.move< InsnVariable::Type > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_literal: // literal
        value.move< Literal * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        value.move< ParamDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassBody: // regclassBody
        value.move< RegClassFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassBodyDecl: // regclassBodyDecl
        value.move< RegClassModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassTypeDecl: // regclassTypeDecl
        value.move< RegClassTypeDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassRegDecl: // regclassRegDecl
        value.move< RegMemberDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regtype: // regtype
        value.move< RegType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_registerBody: // registerBody
        value.move< RegisterFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        value.move< RegisterModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_optionalPattern: // optionalPattern
        value.move< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_insnArgTypePattern: // insnArgTypePattern
      case symbol_kind::S_insnArgExprPattern: // insnArgExprPattern
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMBER: // NUMBER
        value.move< std::uint32_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_asmInsnExpr: // asmInsnExpr
        value.move< template_string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_asmInsnArg: // asmInsnArg
        value.copy< AsmInsnArg * > (that.value);
        break;

      case symbol_kind::S_asmInsn: // asmInsn
      case symbol_kind::S_asmInsnArgs: // asmInsnArgs
      case symbol_kind::S_moreAsmInsnArgs: // moreAsmInsnArgs
        value.copy< AsmInsnCall * > (that.value);
        break;

      case symbol_kind::S_ctypeBody: // ctypeBody
        value.copy< CType * > (that.value);
        break;

      case symbol_kind::S_ctypeBodyDecl: // ctypeBodyDecl
        value.copy< CTypeModifier * > (that.value);
        break;

      case symbol_kind::S_clobberRegisterList: // clobberRegisterList
        value.copy< ClobberListDecl * > (that.value);
        break;

      case symbol_kind::S_constructorDecl: // constructorDecl
      case symbol_kind::S_constructorArgList: // constructorArgList
      case symbol_kind::S_moreConstructorArgs: // moreConstructorArgs
        value.copy< ConstructorDecl * > (that.value);
        break;

      case symbol_kind::S_insnDecl: // insnDecl
        value.copy< Insn * > (that.value);
        break;

      case symbol_kind::S_insnArgDeclDirection: // insnArgDeclDirection
        value.copy< InsnArgDecl::Direction > (that.value);
        break;

      case symbol_kind::S_insnArgPattern: // insnArgPattern
        value.copy< InsnArgPattern > (that.value);
        break;

      case symbol_kind::S_insnArgDirectionality: // insnArgDirectionality
        value.copy< InsnArgPattern::Directionality > (that.value);
        break;

      case symbol_kind::S_insnArgs: // insnArgs
      case symbol_kind::S_moreInsnArgs: // moreInsnArgs
        value.copy< InsnArgPatterns * > (that.value);
        break;

      case symbol_kind::S_insnArgsDecl: // insnArgsDecl
      case symbol_kind::S_insnArgsDeclList: // insnArgsDeclList
      case symbol_kind::S_moreInsnArgsDeclList: // moreInsnArgsDeclList
        value.copy< InsnArgsModifier * > (that.value);
        break;

      case symbol_kind::S_insnBody: // insnBody
      case symbol_kind::S_insnBodyDecls: // insnBodyDecls
        value.copy< InsnFactory * > (that.value);
        break;

      case symbol_kind::S_patternsDisjunction: // patternsDisjunction
      case symbol_kind::S_morePatternsDisjunction: // morePatternsDisjunction
      case symbol_kind::S_patternsConjunction: // patternsConjunction
      case symbol_kind::S_morePatternsConjunction: // morePatternsConjunction
      case symbol_kind::S_baseInsnPattern: // baseInsnPattern
        value.copy< InsnGen * > (that.value);
        break;

      case symbol_kind::S_insnBodyDecl: // insnBodyDecl
        value.copy< InsnModifier * > (that.value);
        break;

      case symbol_kind::S_simpleInsnPattern: // simpleInsnPattern
        value.copy< InsnPattern * > (that.value);
        break;

      case symbol_kind::S_rtlType: // rtlType
        value.copy< InsnRtlType > (that.value);
        break;

      case symbol_kind::S_insnRuleDecl: // insnRuleDecl
        value.copy< InsnRule * > (that.value);
        break;

      case symbol_kind::S_asmDecl: // asmDecl
        value.copy< InsnRuleAsmDecl * > (that.value);
        break;

      case symbol_kind::S_insnRuleBody: // insnRuleBody
        value.copy< InsnRuleFactory * > (that.value);
        break;

      case symbol_kind::S_insnRuleBodyDecl: // insnRuleBodyDecl
      case symbol_kind::S_patternsDecl: // patternsDecl
        value.copy< InsnRuleModifier * > (that.value);
        break;

      case symbol_kind::S_insnVarDecl: // insnVarDecl
        value.copy< InsnVarDecl * > (that.value);
        break;

      case symbol_kind::S_insnVarDeclBody: // insnVarDeclBody
      case symbol_kind::S_insnVarDeclBodyDecls: // insnVarDeclBodyDecls
        value.copy< InsnVarDeclFactory * > (that.value);
        break;

      case symbol_kind::S_insnVarDeclBodyDecl: // insnVarDeclBodyDecl
        value.copy< InsnVarDeclModifier * > (that.value);
        break;

      case symbol_kind::S_constructorArgTy: // constructorArgTy
      case symbol_kind::S_insnVarTyDecl: // insnVarTyDecl
        value.copy< InsnVarType * > (that.value);
        break;

      case symbol_kind::S_insnRuleVarType: // insnRuleVarType
        value.copy< InsnVariable::Type > (that.value);
        break;

      case symbol_kind::S_literal: // literal
        value.copy< Literal * > (that.value);
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        value.copy< ParamDecl * > (that.value);
        break;

      case symbol_kind::S_regclassBody: // regclassBody
        value.copy< RegClassFactory * > (that.value);
        break;

      case symbol_kind::S_regclassBodyDecl: // regclassBodyDecl
        value.copy< RegClassModifier * > (that.value);
        break;

      case symbol_kind::S_regclassTypeDecl: // regclassTypeDecl
        value.copy< RegClassTypeDecl * > (that.value);
        break;

      case symbol_kind::S_regclassRegDecl: // regclassRegDecl
        value.copy< RegMemberDecl * > (that.value);
        break;

      case symbol_kind::S_regtype: // regtype
        value.copy< RegType > (that.value);
        break;

      case symbol_kind::S_registerBody: // registerBody
        value.copy< RegisterFactory * > (that.value);
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        value.copy< RegisterModifier * > (that.value);
        break;

      case symbol_kind::S_optionalPattern: // optionalPattern
        value.copy< bool > (that.value);
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_insnArgTypePattern: // insnArgTypePattern
      case symbol_kind::S_insnArgExprPattern: // insnArgExprPattern
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_NUMBER: // NUMBER
        value.copy< std::uint32_t > (that.value);
        break;

      case symbol_kind::S_asmInsnExpr: // asmInsnExpr
        value.copy< template_string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_asmInsnArg: // asmInsnArg
        value.move< AsmInsnArg * > (that.value);
        break;

      case symbol_kind::S_asmInsn: // asmInsn
      case symbol_kind::S_asmInsnArgs: // asmInsnArgs
      case symbol_kind::S_moreAsmInsnArgs: // moreAsmInsnArgs
        value.move< AsmInsnCall * > (that.value);
        break;

      case symbol_kind::S_ctypeBody: // ctypeBody
        value.move< CType * > (that.value);
        break;

      case symbol_kind::S_ctypeBodyDecl: // ctypeBodyDecl
        value.move< CTypeModifier * > (that.value);
        break;

      case symbol_kind::S_clobberRegisterList: // clobberRegisterList
        value.move< ClobberListDecl * > (that.value);
        break;

      case symbol_kind::S_constructorDecl: // constructorDecl
      case symbol_kind::S_constructorArgList: // constructorArgList
      case symbol_kind::S_moreConstructorArgs: // moreConstructorArgs
        value.move< ConstructorDecl * > (that.value);
        break;

      case symbol_kind::S_insnDecl: // insnDecl
        value.move< Insn * > (that.value);
        break;

      case symbol_kind::S_insnArgDeclDirection: // insnArgDeclDirection
        value.move< InsnArgDecl::Direction > (that.value);
        break;

      case symbol_kind::S_insnArgPattern: // insnArgPattern
        value.move< InsnArgPattern > (that.value);
        break;

      case symbol_kind::S_insnArgDirectionality: // insnArgDirectionality
        value.move< InsnArgPattern::Directionality > (that.value);
        break;

      case symbol_kind::S_insnArgs: // insnArgs
      case symbol_kind::S_moreInsnArgs: // moreInsnArgs
        value.move< InsnArgPatterns * > (that.value);
        break;

      case symbol_kind::S_insnArgsDecl: // insnArgsDecl
      case symbol_kind::S_insnArgsDeclList: // insnArgsDeclList
      case symbol_kind::S_moreInsnArgsDeclList: // moreInsnArgsDeclList
        value.move< InsnArgsModifier * > (that.value);
        break;

      case symbol_kind::S_insnBody: // insnBody
      case symbol_kind::S_insnBodyDecls: // insnBodyDecls
        value.move< InsnFactory * > (that.value);
        break;

      case symbol_kind::S_patternsDisjunction: // patternsDisjunction
      case symbol_kind::S_morePatternsDisjunction: // morePatternsDisjunction
      case symbol_kind::S_patternsConjunction: // patternsConjunction
      case symbol_kind::S_morePatternsConjunction: // morePatternsConjunction
      case symbol_kind::S_baseInsnPattern: // baseInsnPattern
        value.move< InsnGen * > (that.value);
        break;

      case symbol_kind::S_insnBodyDecl: // insnBodyDecl
        value.move< InsnModifier * > (that.value);
        break;

      case symbol_kind::S_simpleInsnPattern: // simpleInsnPattern
        value.move< InsnPattern * > (that.value);
        break;

      case symbol_kind::S_rtlType: // rtlType
        value.move< InsnRtlType > (that.value);
        break;

      case symbol_kind::S_insnRuleDecl: // insnRuleDecl
        value.move< InsnRule * > (that.value);
        break;

      case symbol_kind::S_asmDecl: // asmDecl
        value.move< InsnRuleAsmDecl * > (that.value);
        break;

      case symbol_kind::S_insnRuleBody: // insnRuleBody
        value.move< InsnRuleFactory * > (that.value);
        break;

      case symbol_kind::S_insnRuleBodyDecl: // insnRuleBodyDecl
      case symbol_kind::S_patternsDecl: // patternsDecl
        value.move< InsnRuleModifier * > (that.value);
        break;

      case symbol_kind::S_insnVarDecl: // insnVarDecl
        value.move< InsnVarDecl * > (that.value);
        break;

      case symbol_kind::S_insnVarDeclBody: // insnVarDeclBody
      case symbol_kind::S_insnVarDeclBodyDecls: // insnVarDeclBodyDecls
        value.move< InsnVarDeclFactory * > (that.value);
        break;

      case symbol_kind::S_insnVarDeclBodyDecl: // insnVarDeclBodyDecl
        value.move< InsnVarDeclModifier * > (that.value);
        break;

      case symbol_kind::S_constructorArgTy: // constructorArgTy
      case symbol_kind::S_insnVarTyDecl: // insnVarTyDecl
        value.move< InsnVarType * > (that.value);
        break;

      case symbol_kind::S_insnRuleVarType: // insnRuleVarType
        value.move< InsnVariable::Type > (that.value);
        break;

      case symbol_kind::S_literal: // literal
        value.move< Literal * > (that.value);
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        value.move< ParamDecl * > (that.value);
        break;

      case symbol_kind::S_regclassBody: // regclassBody
        value.move< RegClassFactory * > (that.value);
        break;

      case symbol_kind::S_regclassBodyDecl: // regclassBodyDecl
        value.move< RegClassModifier * > (that.value);
        break;

      case symbol_kind::S_regclassTypeDecl: // regclassTypeDecl
        value.move< RegClassTypeDecl * > (that.value);
        break;

      case symbol_kind::S_regclassRegDecl: // regclassRegDecl
        value.move< RegMemberDecl * > (that.value);
        break;

      case symbol_kind::S_regtype: // regtype
        value.move< RegType > (that.value);
        break;

      case symbol_kind::S_registerBody: // registerBody
        value.move< RegisterFactory * > (that.value);
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        value.move< RegisterModifier * > (that.value);
        break;

      case symbol_kind::S_optionalPattern: // optionalPattern
        value.move< bool > (that.value);
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_insnArgTypePattern: // insnArgTypePattern
      case symbol_kind::S_insnArgExprPattern: // insnArgExprPattern
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_NUMBER: // NUMBER
        value.move< std::uint32_t > (that.value);
        break;

      case symbol_kind::S_asmInsnExpr: // asmInsnExpr
        value.move< template_string > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // Discard the LAC context in case there still is one left from a
    // previous invocation.
    yy_lac_discard_ ("init");

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex ());
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        if (!yy_lac_establish_ (yyla.kind ()))
          goto yyerrlab;
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        if (!yy_lac_establish_ (yyla.kind ()))
          goto yyerrlab;

        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    yy_lac_discard_ ("shift");
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_asmInsnArg: // asmInsnArg
        yylhs.value.emplace< AsmInsnArg * > ();
        break;

      case symbol_kind::S_asmInsn: // asmInsn
      case symbol_kind::S_asmInsnArgs: // asmInsnArgs
      case symbol_kind::S_moreAsmInsnArgs: // moreAsmInsnArgs
        yylhs.value.emplace< AsmInsnCall * > ();
        break;

      case symbol_kind::S_ctypeBody: // ctypeBody
        yylhs.value.emplace< CType * > ();
        break;

      case symbol_kind::S_ctypeBodyDecl: // ctypeBodyDecl
        yylhs.value.emplace< CTypeModifier * > ();
        break;

      case symbol_kind::S_clobberRegisterList: // clobberRegisterList
        yylhs.value.emplace< ClobberListDecl * > ();
        break;

      case symbol_kind::S_constructorDecl: // constructorDecl
      case symbol_kind::S_constructorArgList: // constructorArgList
      case symbol_kind::S_moreConstructorArgs: // moreConstructorArgs
        yylhs.value.emplace< ConstructorDecl * > ();
        break;

      case symbol_kind::S_insnDecl: // insnDecl
        yylhs.value.emplace< Insn * > ();
        break;

      case symbol_kind::S_insnArgDeclDirection: // insnArgDeclDirection
        yylhs.value.emplace< InsnArgDecl::Direction > ();
        break;

      case symbol_kind::S_insnArgPattern: // insnArgPattern
        yylhs.value.emplace< InsnArgPattern > ();
        break;

      case symbol_kind::S_insnArgDirectionality: // insnArgDirectionality
        yylhs.value.emplace< InsnArgPattern::Directionality > ();
        break;

      case symbol_kind::S_insnArgs: // insnArgs
      case symbol_kind::S_moreInsnArgs: // moreInsnArgs
        yylhs.value.emplace< InsnArgPatterns * > ();
        break;

      case symbol_kind::S_insnArgsDecl: // insnArgsDecl
      case symbol_kind::S_insnArgsDeclList: // insnArgsDeclList
      case symbol_kind::S_moreInsnArgsDeclList: // moreInsnArgsDeclList
        yylhs.value.emplace< InsnArgsModifier * > ();
        break;

      case symbol_kind::S_insnBody: // insnBody
      case symbol_kind::S_insnBodyDecls: // insnBodyDecls
        yylhs.value.emplace< InsnFactory * > ();
        break;

      case symbol_kind::S_patternsDisjunction: // patternsDisjunction
      case symbol_kind::S_morePatternsDisjunction: // morePatternsDisjunction
      case symbol_kind::S_patternsConjunction: // patternsConjunction
      case symbol_kind::S_morePatternsConjunction: // morePatternsConjunction
      case symbol_kind::S_baseInsnPattern: // baseInsnPattern
        yylhs.value.emplace< InsnGen * > ();
        break;

      case symbol_kind::S_insnBodyDecl: // insnBodyDecl
        yylhs.value.emplace< InsnModifier * > ();
        break;

      case symbol_kind::S_simpleInsnPattern: // simpleInsnPattern
        yylhs.value.emplace< InsnPattern * > ();
        break;

      case symbol_kind::S_rtlType: // rtlType
        yylhs.value.emplace< InsnRtlType > ();
        break;

      case symbol_kind::S_insnRuleDecl: // insnRuleDecl
        yylhs.value.emplace< InsnRule * > ();
        break;

      case symbol_kind::S_asmDecl: // asmDecl
        yylhs.value.emplace< InsnRuleAsmDecl * > ();
        break;

      case symbol_kind::S_insnRuleBody: // insnRuleBody
        yylhs.value.emplace< InsnRuleFactory * > ();
        break;

      case symbol_kind::S_insnRuleBodyDecl: // insnRuleBodyDecl
      case symbol_kind::S_patternsDecl: // patternsDecl
        yylhs.value.emplace< InsnRuleModifier * > ();
        break;

      case symbol_kind::S_insnVarDecl: // insnVarDecl
        yylhs.value.emplace< InsnVarDecl * > ();
        break;

      case symbol_kind::S_insnVarDeclBody: // insnVarDeclBody
      case symbol_kind::S_insnVarDeclBodyDecls: // insnVarDeclBodyDecls
        yylhs.value.emplace< InsnVarDeclFactory * > ();
        break;

      case symbol_kind::S_insnVarDeclBodyDecl: // insnVarDeclBodyDecl
        yylhs.value.emplace< InsnVarDeclModifier * > ();
        break;

      case symbol_kind::S_constructorArgTy: // constructorArgTy
      case symbol_kind::S_insnVarTyDecl: // insnVarTyDecl
        yylhs.value.emplace< InsnVarType * > ();
        break;

      case symbol_kind::S_insnRuleVarType: // insnRuleVarType
        yylhs.value.emplace< InsnVariable::Type > ();
        break;

      case symbol_kind::S_literal: // literal
        yylhs.value.emplace< Literal * > ();
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        yylhs.value.emplace< ParamDecl * > ();
        break;

      case symbol_kind::S_regclassBody: // regclassBody
        yylhs.value.emplace< RegClassFactory * > ();
        break;

      case symbol_kind::S_regclassBodyDecl: // regclassBodyDecl
        yylhs.value.emplace< RegClassModifier * > ();
        break;

      case symbol_kind::S_regclassTypeDecl: // regclassTypeDecl
        yylhs.value.emplace< RegClassTypeDecl * > ();
        break;

      case symbol_kind::S_regclassRegDecl: // regclassRegDecl
        yylhs.value.emplace< RegMemberDecl * > ();
        break;

      case symbol_kind::S_regtype: // regtype
        yylhs.value.emplace< RegType > ();
        break;

      case symbol_kind::S_registerBody: // registerBody
        yylhs.value.emplace< RegisterFactory * > ();
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        yylhs.value.emplace< RegisterModifier * > ();
        break;

      case symbol_kind::S_optionalPattern: // optionalPattern
        yylhs.value.emplace< bool > ();
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_insnArgTypePattern: // insnArgTypePattern
      case symbol_kind::S_insnArgExprPattern: // insnArgExprPattern
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_NUMBER: // NUMBER
        yylhs.value.emplace< std::uint32_t > ();
        break;

      case symbol_kind::S_asmInsnExpr: // asmInsnExpr
        yylhs.value.emplace< template_string > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 14: // ctypeDecl: CTYPE ID OBRACE ctypeBody
#line 183 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                      {
                    builder.addCType(yystack_[2].value.as < std::string > (), yystack_[0].value.as < CType * > ());
                }
#line 1426 "./insnsel/parser.cpp"
    break;

  case 15: // ctypeBody: ctypeBodyDecl ctypeBody
#line 188 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                    {
                    yystack_[1].value.as < CTypeModifier * > ()->modify(*yystack_[0].value.as < CType * > ());
                    delete yystack_[1].value.as < CTypeModifier * > ();
                    yylhs.value.as < CType * > () = yystack_[0].value.as < CType * > ();
                }
#line 1436 "./insnsel/parser.cpp"
    break;

  case 16: // ctypeBody: cbraceSemi
#line 193 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < CType * > () = new CType(); }
#line 1442 "./insnsel/parser.cpp"
    break;

  case 17: // ctypeBodyDecl: paramDecl
#line 196 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                 { yylhs.value.as < CTypeModifier * > () = static_cast<CTypeModifier *>(yystack_[0].value.as < ParamDecl * > ()); }
#line 1448 "./insnsel/parser.cpp"
    break;

  case 18: // codeDecl: CODE STRING STRING SEMICOLON
#line 200 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                  builder.addCodeSection(yystack_[2].value.as < std::string > (), yystack_[1].value.as < std::string > ());
                }
#line 1456 "./insnsel/parser.cpp"
    break;

  case 21: // cheaderDecl: CHEADER STRING SEMICOLON
#line 209 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                         {
                    builder.addCHeader(yystack_[1].value.as < std::string > ());
                }
#line 1464 "./insnsel/parser.cpp"
    break;

  case 22: // architectureDecl: ARCHITECTURE STRING SEMICOLON
#line 215 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                              {
                    builder.setArchitecture(yystack_[1].value.as < std::string > ());
                }
#line 1472 "./insnsel/parser.cpp"
    break;

  case 23: // includeDecl: INCLUDE STRING SEMICOLON
#line 220 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                         {
                    NncErrorLocation l(yystack_[1].location, "Included from here");
                    builder.includeFile(lexer.filename(), yystack_[1].value.as < std::string > ());
                }
#line 1481 "./insnsel/parser.cpp"
    break;

  case 24: // regclassDecl: REGCLASS ID OBRACE regclassBody
#line 227 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                {
                    auto newRegClass(yystack_[0].value.as < RegClassFactory * > ()->build(yystack_[2].value.as < std::string > ()));
                    delete yystack_[0].value.as < RegClassFactory * > ();
                    builder.addRegisterClass(newRegClass);
                }
#line 1491 "./insnsel/parser.cpp"
    break;

  case 25: // registerDecl: REGISTER ID OBRACE registerBody
#line 234 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                 {
                     auto newRegister(yystack_[0].value.as < RegisterFactory * > ()->build(yystack_[2].value.as < std::string > ()));
                     delete yystack_[0].value.as < RegisterFactory * > ();

                     builder.addRegister(newRegister);
                 }
#line 1502 "./insnsel/parser.cpp"
    break;

  case 26: // registerBody: cbraceSemi
#line 242 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < RegisterFactory * > () = new RegisterFactory(); }
#line 1508 "./insnsel/parser.cpp"
    break;

  case 27: // registerBody: regBodyDecl registerBody
#line 243 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                         {
                  yystack_[1].value.as < RegisterModifier * > ()->modify(*yystack_[0].value.as < RegisterFactory * > ());
                  delete yystack_[1].value.as < RegisterModifier * > ();
                  yylhs.value.as < RegisterFactory * > () = yystack_[0].value.as < RegisterFactory * > ();
                }
#line 1518 "./insnsel/parser.cpp"
    break;

  case 28: // regBodyDecl: CLOBBERS clobberRegisterList SEMICOLON
#line 250 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                        { yylhs.value.as < RegisterModifier * > () = static_cast<RegisterModifier *>(yystack_[1].value.as < ClobberListDecl * > ()); }
#line 1524 "./insnsel/parser.cpp"
    break;

  case 29: // clobberRegisterList: ID COMMA clobberRegisterList
#line 255 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                  yystack_[0].value.as < ClobberListDecl * > ()->addRegister(yystack_[2].value.as < std::string > ());
                  yylhs.value.as < ClobberListDecl * > () = yystack_[0].value.as < ClobberListDecl * > ();
                }
#line 1533 "./insnsel/parser.cpp"
    break;

  case 30: // clobberRegisterList: ID
#line 259 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                   {
                    yylhs.value.as < ClobberListDecl * > () = new ClobberListDecl();
                    yylhs.value.as < ClobberListDecl * > ()->addRegister(yystack_[0].value.as < std::string > ());
                }
#line 1542 "./insnsel/parser.cpp"
    break;

  case 31: // regclassBody: cbraceSemi
#line 265 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < RegClassFactory * > () = new RegClassFactory(); }
#line 1548 "./insnsel/parser.cpp"
    break;

  case 32: // regclassBody: regclassBodyDecl regclassBody
#line 266 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                              {
                  yystack_[1].value.as < RegClassModifier * > ()->modify(*(yystack_[0].value.as < RegClassFactory * > ()));
                  delete yystack_[1].value.as < RegClassModifier * > ();
                  yylhs.value.as < RegClassFactory * > () = yystack_[0].value.as < RegClassFactory * > ();
                }
#line 1558 "./insnsel/parser.cpp"
    break;

  case 33: // regclassBodyDecl: paramDecl
#line 273 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                              { yylhs.value.as < RegClassModifier * > () = static_cast<RegClassModifier *>(yystack_[0].value.as < ParamDecl * > ()); }
#line 1564 "./insnsel/parser.cpp"
    break;

  case 34: // regclassBodyDecl: REGISTER regclassRegDecl SEMICOLON
#line 274 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                       { yylhs.value.as < RegClassModifier * > () = static_cast<RegClassModifier *>(yystack_[1].value.as < RegMemberDecl * > ()); }
#line 1570 "./insnsel/parser.cpp"
    break;

  case 35: // regclassBodyDecl: TYPE regclassTypeDecl SEMICOLON
#line 275 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                    { yylhs.value.as < RegClassModifier * > () =static_cast<RegClassModifier *>(yystack_[1].value.as < RegClassTypeDecl * > ()); }
#line 1576 "./insnsel/parser.cpp"
    break;

  case 36: // regclassRegDecl: ID COMMA regclassRegDecl
#line 280 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                    yystack_[0].value.as < RegMemberDecl * > ()->addRegister(yystack_[2].value.as < std::string > ());
                    yylhs.value.as < RegMemberDecl * > () = yystack_[0].value.as < RegMemberDecl * > ();
                }
#line 1585 "./insnsel/parser.cpp"
    break;

  case 37: // regclassRegDecl: ID
#line 285 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                    yylhs.value.as < RegMemberDecl * > () = new RegMemberDecl();
                    yylhs.value.as < RegMemberDecl * > ()->addRegister(yystack_[0].value.as < std::string > ());
                }
#line 1594 "./insnsel/parser.cpp"
    break;

  case 38: // regclassTypeDecl: regtype COMMA regclassTypeDecl
#line 293 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                    yystack_[0].value.as < RegClassTypeDecl * > ()->addType(yystack_[2].value.as < RegType > ());
                    yylhs.value.as < RegClassTypeDecl * > () = yystack_[0].value.as < RegClassTypeDecl * > ();
                }
#line 1603 "./insnsel/parser.cpp"
    break;

  case 39: // regclassTypeDecl: regtype
#line 298 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                    yylhs.value.as < RegClassTypeDecl * > () = new RegClassTypeDecl();
                    yylhs.value.as < RegClassTypeDecl * > ()->addType(yystack_[0].value.as < RegType > ());
                }
#line 1612 "./insnsel/parser.cpp"
    break;

  case 40: // regtype: ID
#line 304 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
               { NncErrorLocation l(yystack_[0].location, "While parsing the register type");
                 yylhs.value.as < RegType > () = RegType::fromString(yystack_[0].value.as < std::string > ()); }
#line 1619 "./insnsel/parser.cpp"
    break;

  case 41: // insnDecl: INSN ID insnArgsDecl insnBody
#line 308 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                    {
                    yystack_[1].value.as < InsnArgsModifier * > ()->modify(*yystack_[0].value.as < InsnFactory * > ());
                    delete yystack_[1].value.as < InsnArgsModifier * > ();

                    NncErrorLocation l(yystack_[3].location + yystack_[0].location, "In the instruction declaration for " + yystack_[2].value.as < std::string > ());
                    builder.addInsn(yystack_[0].value.as < InsnFactory * > ()->build(yystack_[2].value.as < std::string > ()));
                }
#line 1631 "./insnsel/parser.cpp"
    break;

  case 42: // insnArgsDecl: OPAREN insnArgsDeclList
#line 317 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                        { yylhs.value.as < InsnArgsModifier * > () = yystack_[0].value.as < InsnArgsModifier * > (); }
#line 1637 "./insnsel/parser.cpp"
    break;

  case 43: // insnArgsDecl: %empty
#line 318 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgsModifier * > () = new InsnArgsModifier(); }
#line 1643 "./insnsel/parser.cpp"
    break;

  case 44: // insnArgDeclDirection: %empty
#line 322 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgDecl::Direction > () = InsnArgDecl::Input; }
#line 1649 "./insnsel/parser.cpp"
    break;

  case 45: // insnArgDeclDirection: OUTPUT
#line 323 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgDecl::Direction > () = InsnArgDecl::Output; }
#line 1655 "./insnsel/parser.cpp"
    break;

  case 46: // insnArgDeclDirection: INOUT
#line 324 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgDecl::Direction > () = InsnArgDecl::InputOutput; }
#line 1661 "./insnsel/parser.cpp"
    break;

  case 47: // insnArgsDeclList: insnArgDeclDirection VARNAME moreInsnArgsDeclList
#line 328 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                  {
                  NncErrorLocation l(yystack_[2].location + yystack_[1].location, "In the insn arg declaration");
                  yystack_[0].value.as < InsnArgsModifier * > ()->addArgument(yystack_[1].value.as < std::string > ()).direction(yystack_[2].value.as < InsnArgDecl::Direction > ());
                  yylhs.value.as < InsnArgsModifier * > () = yystack_[0].value.as < InsnArgsModifier * > ();
                }
#line 1671 "./insnsel/parser.cpp"
    break;

  case 48: // insnArgsDeclList: insnArgDeclDirection OBRACKET VARNAME CBRACKET moreInsnArgsDeclList
#line 333 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                                              {
                  NncErrorLocation l(yystack_[4].location + yystack_[1].location, "In the insn arg declaration");
                  yystack_[0].value.as < InsnArgsModifier * > ()->addArgument(yystack_[2].value.as < std::string > ()).direction(yystack_[4].value.as < InsnArgDecl::Direction > ()).makeOptional();
                  yylhs.value.as < InsnArgsModifier * > () = yystack_[0].value.as < InsnArgsModifier * > ();
                }
#line 1681 "./insnsel/parser.cpp"
    break;

  case 49: // moreInsnArgsDeclList: COMMA insnArgsDeclList
#line 341 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                       { yylhs.value.as < InsnArgsModifier * > () = yystack_[0].value.as < InsnArgsModifier * > (); }
#line 1687 "./insnsel/parser.cpp"
    break;

  case 50: // moreInsnArgsDeclList: CPAREN
#line 342 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgsModifier * > () = new InsnArgsModifier(); }
#line 1693 "./insnsel/parser.cpp"
    break;

  case 51: // insnBody: SEMICOLON
#line 345 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { yylhs.value.as < InsnFactory * > () = new InsnFactory(); }
#line 1699 "./insnsel/parser.cpp"
    break;

  case 52: // insnBody: OBRACE insnBodyDecls
#line 346 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                     { yylhs.value.as < InsnFactory * > () = yystack_[0].value.as < InsnFactory * > (); }
#line 1705 "./insnsel/parser.cpp"
    break;

  case 53: // insnBodyDecls: insnBodyDecl insnBodyDecls
#line 349 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                           {
                    yystack_[1].value.as < InsnModifier * > ()->modify(*yystack_[0].value.as < InsnFactory * > ());
                    delete yystack_[1].value.as < InsnModifier * > ();
                    yylhs.value.as < InsnFactory * > () = yystack_[0].value.as < InsnFactory * > ();
                }
#line 1715 "./insnsel/parser.cpp"
    break;

  case 54: // insnBodyDecls: CBRACE
#line 354 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnFactory * > () = new InsnFactory(); }
#line 1721 "./insnsel/parser.cpp"
    break;

  case 55: // insnBodyDecl: paramDecl
#line 357 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { yylhs.value.as < InsnModifier * > () = static_cast<InsnModifier *>(yystack_[0].value.as < ParamDecl * > ()); }
#line 1727 "./insnsel/parser.cpp"
    break;

  case 56: // insnBodyDecl: insnVarDecl
#line 358 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                            { yylhs.value.as < InsnModifier * > () = static_cast<InsnModifier *>(yystack_[0].value.as < InsnVarDecl * > ()); }
#line 1733 "./insnsel/parser.cpp"
    break;

  case 57: // insnBodyDecl: constructorDecl
#line 359 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                { yylhs.value.as < InsnModifier * > () = static_cast<InsnModifier *>(yystack_[0].value.as < ConstructorDecl * > ()); }
#line 1739 "./insnsel/parser.cpp"
    break;

  case 58: // constructorDecl: CONSTRUCTOR OPAREN constructorArgList SEMICOLON
#line 364 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                    NncErrorLocation l(yystack_[3].location + yystack_[1].location, "In the constructor declaration");
                    yystack_[1].value.as < ConstructorDecl * > ()->errorContext() = NncErrorContextStack::current();
                    yylhs.value.as < ConstructorDecl * > () = yystack_[1].value.as < ConstructorDecl * > ();
                }
#line 1749 "./insnsel/parser.cpp"
    break;

  case 59: // constructorArgList: constructorArgTy moreConstructorArgs
#line 372 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                {
                  yystack_[0].value.as < ConstructorDecl * > ()->addType(yystack_[1].value.as < InsnVarType * > ());
                  yylhs.value.as < ConstructorDecl * > () = yystack_[0].value.as < ConstructorDecl * > ();
                }
#line 1758 "./insnsel/parser.cpp"
    break;

  case 60: // constructorArgList: CPAREN
#line 376 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < ConstructorDecl * > () = new ConstructorDecl(); }
#line 1764 "./insnsel/parser.cpp"
    break;

  case 61: // moreConstructorArgs: COMMA constructorArgList
#line 380 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                               { yylhs.value.as < ConstructorDecl * > () = yystack_[0].value.as < ConstructorDecl * > (); }
#line 1770 "./insnsel/parser.cpp"
    break;

  case 62: // moreConstructorArgs: CPAREN
#line 381 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < ConstructorDecl * > () = new ConstructorDecl(); }
#line 1776 "./insnsel/parser.cpp"
    break;

  case 63: // constructorArgTy: REGISTER
#line 385 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { yylhs.value.as < InsnVarType * > () = new InsnRegisterVarType(); }
#line 1782 "./insnsel/parser.cpp"
    break;

  case 64: // constructorArgTy: CONSTANT
#line 386 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { yylhs.value.as < InsnVarType * > () = new InsnConstantVarType(); }
#line 1788 "./insnsel/parser.cpp"
    break;

  case 65: // constructorArgTy: ID
#line 387 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                               {
                   NncErrorLocation l(yystack_[0].location, "In constructor arg type declaration");
                   CType &ty(builder.cType(yystack_[0].value.as < std::string > ()));
                   yylhs.value.as < InsnVarType * > () = static_cast<InsnVarType *>(new CType(ty));
                 }
#line 1798 "./insnsel/parser.cpp"
    break;

  case 66: // insnVarDecl: VAR VARNAME COLON insnVarTyDecl insnVarDeclBody SEMICOLON
#line 395 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                  NncErrorLocation l(yystack_[5].location + yystack_[0].location, "In the var decl");
                  yylhs.value.as < InsnVarDecl * > () = yystack_[1].value.as < InsnVarDeclFactory * > ()->build(yystack_[4].value.as < std::string > (), yystack_[2].value.as < InsnVarType * > ());
                  delete yystack_[1].value.as < InsnVarDeclFactory * > ();
                }
#line 1808 "./insnsel/parser.cpp"
    break;

  case 67: // insnVarTyDecl: REGISTER
#line 402 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                         { yylhs.value.as < InsnVarType * > () = new InsnRegisterVarType(); }
#line 1814 "./insnsel/parser.cpp"
    break;

  case 68: // insnVarTyDecl: REGISTER OPAREN rtlType CPAREN
#line 403 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                               {
                  auto newTy(new InsnRegisterVarType());
                  newTy->setRtlType(yystack_[1].value.as < InsnRtlType > ());
                  yylhs.value.as < InsnVarType * > () = static_cast<InsnVarType *>(newTy);
                }
#line 1824 "./insnsel/parser.cpp"
    break;

  case 69: // insnVarTyDecl: CONSTANT
#line 408 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                         { yylhs.value.as < InsnVarType * > () = new InsnConstantVarType(); }
#line 1830 "./insnsel/parser.cpp"
    break;

  case 70: // insnVarTyDecl: CONSTANT OPAREN rtlType CPAREN
#line 409 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                               {
                  auto newTy(new InsnConstantVarType());
                  newTy->setRtlType(yystack_[1].value.as < InsnRtlType > ());
                  yylhs.value.as < InsnVarType * > () = static_cast<InsnVarType *>(newTy);
                }
#line 1840 "./insnsel/parser.cpp"
    break;

  case 71: // insnVarTyDecl: ID
#line 414 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                              {
                  NncErrorLocation l(yystack_[0].location, "In the var type declaration");
                  CType &ty(builder.cType(yystack_[0].value.as < std::string > ()));
                  if ( ty.acceptsRtlType() )
                    throw NncParseError(yystack_[0].location, "Type " + yystack_[0].value.as < std::string > () + " requires an RTL type parameter");
                  yylhs.value.as < InsnVarType * > () = static_cast<InsnVarType *>(new CType(ty));
                }
#line 1852 "./insnsel/parser.cpp"
    break;

  case 72: // insnVarTyDecl: ID OPAREN rtlType CPAREN
#line 421 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                    {
                  NncErrorLocation l(yystack_[3].location, "In the var type declaration");
                  CType &ty(builder.cType(yystack_[3].value.as < std::string > ()));
                  if ( !ty.acceptsRtlType() )
                    throw NncParseError(yystack_[3].location, "Type " + yystack_[3].value.as < std::string > () + " does not accept RTL type parameters");
                  auto newType(new CType(ty));
                  newType->setRtlType(yystack_[1].value.as < InsnRtlType > ());
                  yylhs.value.as < InsnVarType * > () = static_cast<InsnVarType *>(newType);
                }
#line 1866 "./insnsel/parser.cpp"
    break;

  case 73: // rtlType: ID rtlType
#line 432 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           {
                    NncErrorLocation l(yystack_[1].location + yystack_[0].location, "In the RTL type");
                    if ( yystack_[1].value.as < std::string > () != "ptr" )
                      throw NncParseError("Expected ptr");
                    yylhs.value.as < InsnRtlType > () = yystack_[0].value.as < InsnRtlType > ().ptr();
                }
#line 1877 "./insnsel/parser.cpp"
    break;

  case 74: // rtlType: ID
#line 438 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                   {
                   NncErrorLocation l(yystack_[0].location, "In the primitive RTL type");
                   yylhs.value.as < InsnRtlType > () = InsnRtlType(yystack_[0].value.as < std::string > ());
                }
#line 1886 "./insnsel/parser.cpp"
    break;

  case 75: // insnVarDeclBody: %empty
#line 444 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < InsnVarDeclFactory * > () = new InsnVarDeclFactory(); }
#line 1892 "./insnsel/parser.cpp"
    break;

  case 76: // insnVarDeclBody: OBRACE insnVarDeclBodyDecls
#line 445 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                { yylhs.value.as < InsnVarDeclFactory * > () = yystack_[0].value.as < InsnVarDeclFactory * > (); }
#line 1898 "./insnsel/parser.cpp"
    break;

  case 77: // insnVarDeclBodyDecls: CBRACE
#line 448 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                             { yylhs.value.as < InsnVarDeclFactory * > () = new InsnVarDeclFactory(); }
#line 1904 "./insnsel/parser.cpp"
    break;

  case 78: // insnVarDeclBodyDecls: insnVarDeclBodyDecl insnVarDeclBodyDecls
#line 450 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                      {
                        yystack_[1].value.as < InsnVarDeclModifier * > ()->modify(*yystack_[0].value.as < InsnVarDeclFactory * > ());
                        delete yystack_[1].value.as < InsnVarDeclModifier * > ();
                        yylhs.value.as < InsnVarDeclFactory * > () = yystack_[0].value.as < InsnVarDeclFactory * > ();
                      }
#line 1914 "./insnsel/parser.cpp"
    break;

  case 79: // insnVarDeclBodyDecl: paramDecl
#line 458 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { yylhs.value.as < InsnVarDeclModifier * > () = static_cast<InsnVarDeclModifier *>(yystack_[0].value.as < ParamDecl * > ()); }
#line 1920 "./insnsel/parser.cpp"
    break;

  case 80: // insnRuleDecl: INSNRULE ID OBRACE insnRuleBody
#line 461 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                {
                    NncErrorLocation l(yystack_[3].location + yystack_[0].location, "In the instruction rule");
                    builder.addInsnRule(yystack_[0].value.as < InsnRuleFactory * > ()->build(yystack_[2].value.as < std::string > ()));
                    delete yystack_[0].value.as < InsnRuleFactory * > ();
                }
#line 1930 "./insnsel/parser.cpp"
    break;

  case 81: // insnRuleBody: cbraceSemi
#line 468 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < InsnRuleFactory * > () = new InsnRuleFactory(); }
#line 1936 "./insnsel/parser.cpp"
    break;

  case 82: // insnRuleBody: insnRuleBodyDecl insnRuleBody
#line 469 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                              {
                  if ( yystack_[1].value.as < InsnRuleModifier * > () ) {
                    yystack_[1].value.as < InsnRuleModifier * > ()->modify(*yystack_[0].value.as < InsnRuleFactory * > ());
                    if ( yystack_[1].value.as < InsnRuleModifier * > ()->shouldDeleteInsnRuleModifierAfterModify() ) delete yystack_[1].value.as < InsnRuleModifier * > ();
                  }
                  yylhs.value.as < InsnRuleFactory * > () = yystack_[0].value.as < InsnRuleFactory * > ();
                }
#line 1948 "./insnsel/parser.cpp"
    break;

  case 83: // insnRuleVarType: REGTYPE
#line 479 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                         { yylhs.value.as < InsnVariable::Type > () = InsnVariable::RegTypeType; }
#line 1954 "./insnsel/parser.cpp"
    break;

  case 84: // insnRuleVarType: REGISTER
#line 480 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                         { yylhs.value.as < InsnVariable::Type > () = InsnVariable::RegisterType; }
#line 1960 "./insnsel/parser.cpp"
    break;

  case 85: // insnRuleVarType: CONSTANT
#line 481 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                         { yylhs.value.as < InsnVariable::Type > () = InsnVariable::ConstantType; }
#line 1966 "./insnsel/parser.cpp"
    break;

  case 86: // insnRuleBodyDecl: VAR VARNAME COLON insnRuleVarType SEMICOLON
#line 486 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                   auto decl(new InsnRuleVarDecl(yystack_[3].location, yystack_[3].value.as < std::string > (), yystack_[1].value.as < InsnVariable::Type > ()));
                   yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(decl);
                }
#line 1975 "./insnsel/parser.cpp"
    break;

  case 87: // insnRuleBodyDecl: PATTERN OBRACE patternsDecl cbraceSemi
#line 490 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                        { yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(yystack_[1].value.as < InsnRuleModifier * > ()); }
#line 1981 "./insnsel/parser.cpp"
    break;

  case 88: // insnRuleBodyDecl: IF STRING SEMICOLON
#line 491 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                     { yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(new InsnCondDecl(yystack_[1].value.as < std::string > ())); }
#line 1987 "./insnsel/parser.cpp"
    break;

  case 89: // insnRuleBodyDecl: CHECK STRING SEMICOLON
#line 492 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                        { yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(new InsnCheckDecl(yystack_[2].location, yystack_[1].value.as < std::string > ())); }
#line 1993 "./insnsel/parser.cpp"
    break;

  case 90: // insnRuleBodyDecl: paramDecl
#line 493 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(yystack_[0].value.as < ParamDecl * > ()); }
#line 1999 "./insnsel/parser.cpp"
    break;

  case 91: // insnRuleBodyDecl: ASM OBRACE asmDecl
#line 494 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                    { yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(yystack_[0].value.as < InsnRuleAsmDecl * > ()); }
#line 2005 "./insnsel/parser.cpp"
    break;

  case 92: // patternsDecl: patternsDisjunction
#line 497 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                  { yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(yystack_[0].value.as < InsnGen * > ()); }
#line 2011 "./insnsel/parser.cpp"
    break;

  case 93: // patternsDisjunction: patternsConjunction morePatternsDisjunction
#line 500 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                        {
                  yylhs.value.as < InsnGen * > () = InsnRuleDisjunction::make(yystack_[0].value.as < InsnGen * > (), yystack_[1].value.as < InsnGen * > ());
                }
#line 2019 "./insnsel/parser.cpp"
    break;

  case 94: // morePatternsDisjunction: BAR patternsDisjunction
#line 506 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                        {
                  yylhs.value.as < InsnGen * > () = yystack_[0].value.as < InsnGen * > ();
                }
#line 2027 "./insnsel/parser.cpp"
    break;

  case 95: // morePatternsDisjunction: %empty
#line 509 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnGen * > () = nullptr; }
#line 2033 "./insnsel/parser.cpp"
    break;

  case 96: // patternsConjunction: baseInsnPattern SEMICOLON morePatternsConjunction
#line 513 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                              {
                  yylhs.value.as < InsnGen * > () = InsnRuleConjunction::make(yystack_[2].value.as < InsnGen * > (), yystack_[0].value.as < InsnGen * > ());
                }
#line 2041 "./insnsel/parser.cpp"
    break;

  case 97: // patternsConjunction: baseInsnPattern
#line 516 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                      {
                  yylhs.value.as < InsnGen * > () = static_cast<InsnGen *>(yystack_[0].value.as < InsnGen * > ());
                }
#line 2049 "./insnsel/parser.cpp"
    break;

  case 98: // patternsConjunction: OBRACE patternsDisjunction cbraceSemi optionalPattern morePatternsConjunction
#line 519 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                                                          {
                  if ( yystack_[1].value.as < bool > () ) {
                    yystack_[3].value.as < InsnGen * > () = InsnRuleOptionalPattern::make(yystack_[3].value.as < InsnGen * > ());
                  }
                  yylhs.value.as < InsnGen * > () = InsnRuleConjunction::make(yystack_[3].value.as < InsnGen * > (), yystack_[0].value.as < InsnGen * > ());
                }
#line 2060 "./insnsel/parser.cpp"
    break;

  case 99: // optionalPattern: QUESTIONMARK
#line 528 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                             { yylhs.value.as < bool > () = true; }
#line 2066 "./insnsel/parser.cpp"
    break;

  case 100: // optionalPattern: %empty
#line 529 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < bool > () = false; }
#line 2072 "./insnsel/parser.cpp"
    break;

  case 101: // morePatternsConjunction: %empty
#line 533 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnGen * > () = nullptr; }
#line 2078 "./insnsel/parser.cpp"
    break;

  case 102: // morePatternsConjunction: patternsConjunction
#line 534 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                    { yylhs.value.as < InsnGen * > () = yystack_[0].value.as < InsnGen * > (); }
#line 2084 "./insnsel/parser.cpp"
    break;

  case 103: // baseInsnPattern: simpleInsnPattern optionalPattern
#line 538 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                  {
                  yylhs.value.as < InsnGen * > () = static_cast<InsnGen *>(yystack_[1].value.as < InsnPattern * > ());
                  if ( yystack_[0].value.as < bool > () ) yylhs.value.as < InsnGen * > () = InsnRuleOptionalPattern::make(yylhs.value.as < InsnGen * > ());
                }
#line 2093 "./insnsel/parser.cpp"
    break;

  case 104: // simpleInsnPattern: ID OPAREN insnArgs
#line 545 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                           {
                  yylhs.value.as < InsnPattern * > () = new InsnPattern(yystack_[2].value.as < std::string > (), yystack_[0].value.as < InsnArgPatterns * > ());
                }
#line 2101 "./insnsel/parser.cpp"
    break;

  case 105: // insnArgs: CPAREN
#line 550 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgPatterns * > () = new InsnArgPatterns(); }
#line 2107 "./insnsel/parser.cpp"
    break;

  case 106: // insnArgs: ID insnArgPattern moreInsnArgs
#line 551 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                               {
                   yylhs.value.as < InsnArgPatterns * > () = yystack_[0].value.as < InsnArgPatterns * > ();
                   yylhs.value.as < InsnArgPatterns * > ()->addArgument(yystack_[2].value.as < std::string > (), yystack_[1].value.as < InsnArgPattern > ());
                }
#line 2116 "./insnsel/parser.cpp"
    break;

  case 107: // insnArgDirectionality: EQUAL
#line 558 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                      { yylhs.value.as < InsnArgPattern::Directionality > () = InsnArgPattern::Input; }
#line 2122 "./insnsel/parser.cpp"
    break;

  case 108: // insnArgDirectionality: OUTPUT
#line 559 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgPattern::Directionality > () = InsnArgPattern::Output; }
#line 2128 "./insnsel/parser.cpp"
    break;

  case 109: // insnArgDirectionality: INOUT
#line 560 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                      { yylhs.value.as < InsnArgPattern::Directionality > () = InsnArgPattern::InputOutput; }
#line 2134 "./insnsel/parser.cpp"
    break;

  case 110: // moreInsnArgs: COMMA insnArgs
#line 563 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                               { yylhs.value.as < InsnArgPatterns * > () = yystack_[0].value.as < InsnArgPatterns * > (); }
#line 2140 "./insnsel/parser.cpp"
    break;

  case 111: // moreInsnArgs: CPAREN
#line 564 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgPatterns * > () = new InsnArgPatterns(); }
#line 2146 "./insnsel/parser.cpp"
    break;

  case 112: // insnArgPattern: insnArgDirectionality OPAREN insnArgTypePattern CPAREN insnArgExprPattern
#line 567 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                                          {
                    yylhs.value.as < InsnArgPattern > () = InsnArgPattern(yystack_[4].location, yystack_[4].value.as < InsnArgPattern::Directionality > (), yystack_[2].value.as < std::string > (), yystack_[0].value.as < std::string > ());
                }
#line 2154 "./insnsel/parser.cpp"
    break;

  case 113: // insnArgPattern: insnArgDirectionality insnArgExprPattern
#line 570 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                         {
                    yylhs.value.as < InsnArgPattern > () = InsnArgPattern(yystack_[1].location, yystack_[1].value.as < InsnArgPattern::Directionality > (), yystack_[0].value.as < std::string > ());
                }
#line 2162 "./insnsel/parser.cpp"
    break;

  case 114: // insnArgTypePattern: VARNAME
#line 575 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                            { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2168 "./insnsel/parser.cpp"
    break;

  case 115: // insnArgExprPattern: VARNAME
#line 579 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                        { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2174 "./insnsel/parser.cpp"
    break;

  case 116: // asmDecl: cbraceSemi
#line 583 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < InsnRuleAsmDecl * > () = new InsnRuleAsmDecl(); }
#line 2180 "./insnsel/parser.cpp"
    break;

  case 117: // asmDecl: asmInsn SEMICOLON asmDecl
#line 584 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                          {
                  yystack_[0].value.as < InsnRuleAsmDecl * > ()->generateInsn(*yystack_[2].value.as < AsmInsnCall * > ());
                  delete yystack_[2].value.as < AsmInsnCall * > ();
                  yylhs.value.as < InsnRuleAsmDecl * > () = yystack_[0].value.as < InsnRuleAsmDecl * > ();
        }
#line 2190 "./insnsel/parser.cpp"
    break;

  case 118: // asmInsn: ID OPAREN asmInsnArgs
#line 591 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                      {
                    NncErrorLocation l(yystack_[2].location + yystack_[0].location, "In the generated asm expression");
                    yystack_[0].value.as < AsmInsnCall * > ()->mnemonic(yystack_[2].value.as < std::string > ());
                    yystack_[0].value.as < AsmInsnCall * > ()->errorContext() = NncErrorContextStack::current();
                    yylhs.value.as < AsmInsnCall * > () = yystack_[0].value.as < AsmInsnCall * > ();
                }
#line 2201 "./insnsel/parser.cpp"
    break;

  case 119: // asmInsn: ID
#line 597 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                   {
                  NncErrorLocation l(yystack_[0].location, "In the generated asm expression");
                  yylhs.value.as < AsmInsnCall * > () = new AsmInsnCall(yystack_[0].value.as < std::string > ());
                }
#line 2210 "./insnsel/parser.cpp"
    break;

  case 120: // asmInsnArgs: asmInsnArg moreAsmInsnArgs
#line 603 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                           {
                    NncErrorLocation l(yystack_[1].location, "In the args of the asm instruction");
                    yystack_[1].value.as < AsmInsnArg * > ()->addToCall(*yystack_[0].value.as < AsmInsnCall * > ());
                    delete yystack_[1].value.as < AsmInsnArg * > ();
                    yylhs.value.as < AsmInsnCall * > () = yystack_[0].value.as < AsmInsnCall * > ();
                }
#line 2221 "./insnsel/parser.cpp"
    break;

  case 121: // asmInsnArgs: CPAREN
#line 609 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       {
                  yylhs.value.as < AsmInsnCall * > () = new AsmInsnCall();
                }
#line 2229 "./insnsel/parser.cpp"
    break;

  case 122: // moreAsmInsnArgs: COMMA asmInsnArgs
#line 615 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                  { yylhs.value.as < AsmInsnCall * > () = yystack_[0].value.as < AsmInsnCall * > (); }
#line 2235 "./insnsel/parser.cpp"
    break;

  case 123: // moreAsmInsnArgs: CPAREN
#line 616 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < AsmInsnCall * > () = new AsmInsnCall(); }
#line 2241 "./insnsel/parser.cpp"
    break;

  case 124: // asmInsnArg: asmInsnExpr
#line 619 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                            { yylhs.value.as < AsmInsnArg * > () = static_cast<AsmInsnArg *>(new AsmInsnPositionalArg(std::move(yystack_[0].value.as < template_string > ()))); }
#line 2247 "./insnsel/parser.cpp"
    break;

  case 125: // asmInsnArg: ID EQUAL asmInsnExpr
#line 620 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                     {
                  yylhs.value.as < AsmInsnArg * > () = static_cast<AsmInsnArg *>(new AsmInsnNamedArg(yystack_[2].value.as < std::string > (), std::move(yystack_[0].value.as < template_string > ())));
                }
#line 2255 "./insnsel/parser.cpp"
    break;

  case 126: // asmInsnExpr: VARNAME
#line 625 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                        {
                    yylhs.value.as < template_string > () << yylhs.value.as < template_string > ().var(yystack_[0].value.as < std::string > ());
                }
#line 2263 "./insnsel/parser.cpp"
    break;

  case 127: // asmInsnExpr: STRING
#line 628 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       {
                    template_string s(yystack_[0].value.as < std::string > ());
                    yylhs.value.as < template_string > () = std::move(s);
                }
#line 2272 "./insnsel/parser.cpp"
    break;

  case 128: // paramDecl: ID EQUAL literal SEMICOLON
#line 634 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                           {
                    yylhs.value.as < ParamDecl * > () = new ParamDecl(yystack_[3].value.as < std::string > (), yystack_[1].value.as < Literal * > ());
                }
#line 2280 "./insnsel/parser.cpp"
    break;

  case 129: // literal: STRING
#line 639 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                        { yylhs.value.as < Literal * > () = new StringLiteral(yystack_[0].value.as < std::string > ()); }
#line 2286 "./insnsel/parser.cpp"
    break;

  case 130: // literal: NUMBER
#line 640 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                        { yylhs.value.as < Literal * > () = new NumberLiteral(yystack_[0].value.as < std::uint32_t > ()); }
#line 2292 "./insnsel/parser.cpp"
    break;

  case 131: // literal: ID
#line 641 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                        { if ( yystack_[0].value.as < std::string > () == "true" ) yylhs.value.as < Literal * > () = new NumberLiteral(1);
                          else if ( yystack_[0].value.as < std::string > () == "false" ) yylhs.value.as < Literal * > () = new NumberLiteral(0);
                          else {
                            NncErrorLocation l(yystack_[0].location, "In the literal");
                            throw NncParseError("Invalid literal value " + yystack_[0].value.as < std::string > ());
                          }
                        }
#line 2304 "./insnsel/parser.cpp"
    break;


#line 2308 "./insnsel/parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        std::string msg = YY_("syntax error");
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      yy_lac_discard_ ("error recovery");
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

#if YYDEBUG || 0
  const char *
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytname_[yysymbol];
  }
#endif // #if YYDEBUG || 0





  bool
  parser::yy_lac_check_ (symbol_kind_type yytoken) const
  {
    // Logically, the yylac_stack's lifetime is confined to this function.
    // Clear it, to get rid of potential left-overs from previous call.
    yylac_stack_.clear ();
    // Reduce until we encounter a shift and thereby accept the token.
#if YYDEBUG
    YYCDEBUG << "LAC: checking lookahead " << symbol_name (yytoken) << ':';
#endif
    std::ptrdiff_t lac_top = 0;
    while (true)
      {
        state_type top_state = (yylac_stack_.empty ()
                                ? yystack_[lac_top].state
                                : yylac_stack_.back ());
        int yyrule = yypact_[+top_state];
        if (yy_pact_value_is_default_ (yyrule)
            || (yyrule += yytoken) < 0 || yylast_ < yyrule
            || yycheck_[yyrule] != yytoken)
          {
            // Use the default action.
            yyrule = yydefact_[+top_state];
            if (yyrule == 0)
              {
                YYCDEBUG << " Err\n";
                return false;
              }
          }
        else
          {
            // Use the action from yytable.
            yyrule = yytable_[yyrule];
            if (yy_table_value_is_error_ (yyrule))
              {
                YYCDEBUG << " Err\n";
                return false;
              }
            if (0 < yyrule)
              {
                YYCDEBUG << " S" << yyrule << '\n';
                return true;
              }
            yyrule = -yyrule;
          }
        // By now we know we have to simulate a reduce.
        YYCDEBUG << " R" << yyrule - 1;
        // Pop the corresponding number of values from the stack.
        {
          std::ptrdiff_t yylen = yyr2_[yyrule];
          // First pop from the LAC stack as many tokens as possible.
          std::ptrdiff_t lac_size = std::ptrdiff_t (yylac_stack_.size ());
          if (yylen < lac_size)
            {
              yylac_stack_.resize (std::size_t (lac_size - yylen));
              yylen = 0;
            }
          else if (lac_size)
            {
              yylac_stack_.clear ();
              yylen -= lac_size;
            }
          // Only afterwards look at the main stack.
          // We simulate popping elements by incrementing lac_top.
          lac_top += yylen;
        }
        // Keep top_state in sync with the updated stack.
        top_state = (yylac_stack_.empty ()
                     ? yystack_[lac_top].state
                     : yylac_stack_.back ());
        // Push the resulting state of the reduction.
        state_type state = yy_lr_goto_state_ (top_state, yyr1_[yyrule]);
        YYCDEBUG << " G" << int (state);
        yylac_stack_.push_back (state);
      }
  }

  // Establish the initial context if no initial context currently exists.
  bool
  parser::yy_lac_establish_ (symbol_kind_type yytoken)
  {
    /* Establish the initial context for the current lookahead if no initial
       context is currently established.

       We define a context as a snapshot of the parser stacks.  We define
       the initial context for a lookahead as the context in which the
       parser initially examines that lookahead in order to select a
       syntactic action.  Thus, if the lookahead eventually proves
       syntactically unacceptable (possibly in a later context reached via a
       series of reductions), the initial context can be used to determine
       the exact set of tokens that would be syntactically acceptable in the
       lookahead's place.  Moreover, it is the context after which any
       further semantic actions would be erroneous because they would be
       determined by a syntactically unacceptable token.

       yy_lac_establish_ should be invoked when a reduction is about to be
       performed in an inconsistent state (which, for the purposes of LAC,
       includes consistent states that don't know they're consistent because
       their default reductions have been disabled).

       For parse.lac=full, the implementation of yy_lac_establish_ is as
       follows.  If no initial context is currently established for the
       current lookahead, then check if that lookahead can eventually be
       shifted if syntactic actions continue from the current context.  */
    if (yy_lac_established_)
      return true;
    else
      {
#if YYDEBUG
        YYCDEBUG << "LAC: initial context established for "
                 << symbol_name (yytoken) << '\n';
#endif
        yy_lac_established_ = true;
        return yy_lac_check_ (yytoken);
      }
  }

  // Discard any previous initial lookahead context.
  void
  parser::yy_lac_discard_ (const char* event)
  {
   /* Discard any previous initial lookahead context because of Event,
      which may be a lookahead change or an invalidation of the currently
      established initial context for the current lookahead.

      The most common example of a lookahead change is a shift.  An example
      of both cases is syntax error recovery.  That is, a syntax error
      occurs when the lookahead is syntactically erroneous for the
      currently established initial context, so error recovery manipulates
      the parser stacks to try to find a new initial context in which the
      current lookahead is syntactically acceptable.  If it fails to find
      such a context, it discards the lookahead.  */
    if (yy_lac_established_)
      {
        YYCDEBUG << "LAC: initial context discarded due to "
                 << event << '\n';
        yy_lac_established_ = false;
      }
  }




  const short parser::yypact_ninf_ = -144;

  const signed char parser::yytable_ninf_ = -1;

  const short
  parser::yypact_[] =
  {
      72,  -144,    11,    15,    17,    25,    30,    31,    50,    64,
      66,   106,    72,  -144,  -144,  -144,  -144,  -144,  -144,  -144,
    -144,  -144,    80,    81,    82,    73,    89,    91,    93,    94,
      92,  -144,  -144,  -144,  -144,  -144,    95,    -2,    -3,    -8,
      29,    -7,    -5,  -144,    77,    96,    79,    87,  -144,  -144,
      -2,  -144,  -144,    -3,  -144,  -144,    85,  -144,  -144,    -8,
      99,    83,    90,   104,    86,  -144,  -144,    29,  -144,  -144,
    -144,   -10,  -144,    35,  -144,  -144,    98,   103,  -144,  -144,
     105,   100,    56,  -144,  -144,   102,   107,  -144,     0,   108,
     109,    19,   110,  -144,    97,    40,   101,   111,  -144,  -144,
      35,  -144,  -144,  -144,    77,  -144,  -144,    79,  -144,  -144,
    -144,   112,    85,  -144,     0,   116,   115,  -144,   114,   117,
     118,  -144,  -144,   120,  -144,  -144,   119,    88,   124,  -144,
      -7,  -144,    -1,   113,  -144,  -144,  -144,  -144,  -144,   115,
      -9,  -144,     0,  -144,     0,  -144,  -144,    26,    19,  -144,
    -144,  -144,   122,    40,  -144,  -144,  -144,  -144,  -144,   123,
      57,     1,   118,  -144,    68,  -144,  -144,  -144,  -144,  -144,
    -144,   121,  -144,  -144,    60,  -144,  -144,  -144,  -144,  -144,
    -144,    -1,  -144,   126,   129,   130,   135,     0,  -144,  -144,
    -144,   -18,    61,    52,  -144,    26,  -144,  -144,   125,   125,
     125,    20,   131,  -144,   127,  -144,  -144,  -144,    -9,  -144,
    -144,  -144,   125,   133,   134,   136,  -144,  -144,    20,  -144,
    -144,  -144,   139,  -144,  -144,  -144,  -144,  -144,  -144,   128,
    -144
  };

  const unsigned char
  parser::yydefact_[] =
  {
       0,     3,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    13,    12,     7,     5,     6,     8,     9,
      11,    10,     0,     0,     0,     0,     0,     0,     0,     0,
      43,     1,     4,    22,    21,    23,     0,     0,     0,     0,
       0,    44,     0,    18,     0,    19,     0,     0,    31,    24,
       0,    33,    14,     0,    16,    17,     0,    26,    25,     0,
       0,     0,     0,     0,     0,    81,    80,     0,    90,    45,
      46,     0,    42,     0,    51,    41,    37,     0,    20,    40,
       0,    39,     0,    32,    15,    30,     0,    27,     0,     0,
       0,     0,     0,    82,     0,     0,     0,     0,    54,    52,
       0,    57,    56,    55,     0,    34,    35,     0,   129,   131,
     130,     0,     0,    28,     0,     0,     0,    92,    95,    97,
     100,    88,    89,   119,   116,    91,     0,     0,     0,    50,
      44,    47,     0,     0,    53,    36,    38,   128,    29,     0,
       0,    87,     0,    93,   101,    99,   103,     0,     0,    83,
      84,    85,     0,     0,    49,    63,    64,    60,    65,     0,
       0,     0,   100,   105,     0,   104,    94,   102,    96,   121,
     127,     0,   126,   118,     0,   124,   117,    86,    48,    58,
      62,     0,    59,    67,    69,    71,    75,   101,   107,   108,
     109,     0,     0,     0,   123,     0,   120,    61,     0,     0,
       0,     0,     0,    98,     0,   115,   113,   111,     0,   106,
     125,   122,    74,     0,     0,     0,    77,    76,     0,    79,
      66,   114,     0,   110,    73,    68,    70,    72,    78,     0,
     112
  };

  const short
  parser::yypgoto_[] =
  {
    -144,   145,  -144,  -144,   132,  -144,  -144,   -35,  -144,  -144,
    -144,  -144,  -144,   137,  -144,    49,   138,  -144,    62,    63,
    -144,  -144,  -144,  -144,    37,    18,  -144,    74,  -144,  -144,
      -6,  -144,  -144,  -144,  -144,  -139,  -144,   -46,  -144,  -144,
     140,  -144,  -144,  -144,  -108,  -144,  -143,    14,   -14,  -144,
    -144,   -31,  -144,  -144,  -144,  -144,   -51,    33,  -144,   -16,
    -144,  -144,   -11,   -38,  -144
  };

  const unsigned char
  parser::yydefgoto_[] =
  {
       0,    11,    12,    13,    52,    53,    14,    48,    15,    16,
      17,    18,    19,    58,    59,    86,    49,    50,    77,    80,
      81,    20,    42,    71,    72,   131,    75,    99,   100,   101,
     159,   182,   160,   102,   186,   213,   202,   217,   218,    21,
      66,   152,    67,   116,   117,   143,   118,   146,   168,   119,
     120,   165,   191,   209,   192,   222,   206,   125,   126,   173,
     196,   174,   175,    51,   111
  };

  const unsigned char
  parser::yytable_[] =
  {
      55,   167,    68,    54,    57,    65,   139,   204,    44,   155,
     156,   183,   184,    94,    45,    55,    73,   163,    54,    45,
      45,   114,    74,   205,    57,   157,    69,    70,    56,    68,
     164,    95,    65,    46,   166,   103,    47,    47,   158,   115,
     185,    45,   216,    60,   167,    61,    62,    63,    64,    22,
      96,    45,   169,    23,    97,    24,   124,    98,   123,    47,
     214,   215,   103,    25,   170,   171,   129,   172,    47,    26,
      27,   130,     1,   224,    47,     2,     3,     4,     5,     6,
       7,   141,     8,   180,     9,    10,   194,   207,   181,    28,
     170,   195,   208,   172,   108,   109,   110,   149,   150,   151,
     188,   189,   190,    29,   162,    30,    31,    33,    34,    35,
      37,    36,    38,   124,    39,    40,    76,    41,    79,    82,
      88,    89,    43,    78,    85,    91,   132,    92,    90,   104,
     105,   107,   106,   112,   113,   121,   122,    45,   128,   137,
     127,   140,   142,   161,   144,   147,   148,   145,   153,   177,
     179,   198,   133,   193,   199,   200,   201,    32,   220,   225,
     226,   138,   227,   219,   212,   229,   135,   154,   221,   205,
     136,   178,   228,   203,   134,   197,   187,   223,   230,   211,
     219,   176,   210,     0,     0,    84,     0,     0,    83,     0,
       0,     0,     0,     0,     0,     0,    87,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    93
  };

  const short
  parser::yycheck_[] =
  {
      38,   144,    40,    38,    39,    40,   114,    25,    10,    10,
      11,    10,    11,    23,    22,    53,    21,    26,    53,    22,
      22,    21,    27,    41,    59,    26,    33,    34,    36,    67,
      39,    41,    67,    35,   142,    73,    39,    39,    39,    39,
      39,    22,    22,    14,   187,    16,    17,    18,    19,    38,
      15,    22,    26,    38,    19,    38,    91,    22,    39,    39,
     199,   200,   100,    38,    38,    39,    26,    41,    39,    39,
      39,    31,     0,   212,    39,     3,     4,     5,     6,     7,
       8,   116,    10,    26,    12,    13,    26,    26,    31,    39,
      38,    31,    31,    41,    38,    39,    40,     9,    10,    11,
      32,    33,    34,    39,   139,    39,     0,    27,    27,    27,
      21,    38,    21,   148,    21,    21,    39,    25,    39,    32,
      21,    38,    27,    27,    39,    21,    25,    41,    38,    31,
      27,    31,    27,    31,    27,    27,    27,    22,    41,    27,
      30,    25,    28,    30,    27,    25,    27,    29,    24,    27,
      27,    25,    41,    32,    25,    25,    21,    12,    27,    26,
      26,   112,    26,   201,    39,    26,   104,   130,    41,    41,
     107,   153,   218,   187,   100,   181,   162,   208,   229,   195,
     218,   148,   193,    -1,    -1,    53,    -1,    -1,    50,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67
  };

  const signed char
  parser::yystos_[] =
  {
       0,     0,     3,     4,     5,     6,     7,     8,    10,    12,
      13,    43,    44,    45,    48,    50,    51,    52,    53,    54,
      63,    81,    38,    38,    38,    38,    39,    39,    39,    39,
      39,     0,    43,    27,    27,    27,    38,    21,    21,    21,
      21,    25,    64,    27,    10,    22,    35,    39,    49,    58,
      59,   105,    46,    47,    49,   105,    36,    49,    55,    56,
      14,    16,    17,    18,    19,    49,    82,    84,   105,    33,
      34,    65,    66,    21,    27,    68,    39,    60,    27,    39,
      61,    62,    32,    58,    46,    39,    57,    55,    21,    38,
      38,    21,    41,    82,    23,    41,    15,    19,    22,    69,
      70,    71,    75,   105,    31,    27,    27,    31,    38,    39,
      40,   106,    31,    27,    21,    39,    85,    86,    88,    91,
      92,    27,    27,    39,    49,    99,   100,    30,    41,    26,
      31,    67,    25,    41,    69,    60,    61,    27,    57,    86,
      25,    49,    28,    87,    27,    29,    89,    25,    27,     9,
      10,    11,    83,    24,    66,    10,    11,    26,    39,    72,
      74,    30,    49,    26,    39,    93,    86,    88,    90,    26,
      38,    39,    41,   101,   103,   104,    99,    27,    67,    27,
      26,    31,    73,    10,    11,    39,    76,    89,    32,    33,
      34,    94,    96,    32,    26,    31,   102,    72,    25,    25,
      25,    21,    78,    90,    25,    41,    98,    26,    31,    95,
     104,   101,    39,    77,    77,    77,    22,    79,    80,   105,
      27,    41,    97,    93,    77,    26,    26,    26,    79,    26,
      98
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    42,    43,    43,    43,    44,    44,    44,    44,    44,
      44,    44,    44,    44,    45,    46,    46,    47,    48,    49,
      49,    50,    51,    52,    53,    54,    55,    55,    56,    57,
      57,    58,    58,    59,    59,    59,    60,    60,    61,    61,
      62,    63,    64,    64,    65,    65,    65,    66,    66,    67,
      67,    68,    68,    69,    69,    70,    70,    70,    71,    72,
      72,    73,    73,    74,    74,    74,    75,    76,    76,    76,
      76,    76,    76,    77,    77,    78,    78,    79,    79,    80,
      81,    82,    82,    83,    83,    83,    84,    84,    84,    84,
      84,    84,    85,    86,    87,    87,    88,    88,    88,    89,
      89,    90,    90,    91,    92,    93,    93,    94,    94,    94,
      95,    95,    96,    96,    97,    98,    99,    99,   100,   100,
     101,   101,   102,   102,   103,   103,   104,   104,   105,   106,
     106,   106
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     0,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     2,     1,     1,     4,     1,
       2,     3,     3,     3,     4,     4,     1,     2,     3,     3,
       1,     1,     2,     1,     3,     3,     3,     1,     3,     1,
       1,     4,     2,     0,     0,     1,     1,     3,     5,     2,
       1,     1,     2,     2,     1,     1,     1,     1,     4,     2,
       1,     2,     1,     1,     1,     1,     6,     1,     4,     1,
       4,     1,     4,     2,     1,     0,     2,     1,     2,     1,
       4,     1,     2,     1,     1,     1,     5,     4,     3,     3,
       1,     3,     1,     2,     2,     0,     3,     1,     5,     1,
       0,     0,     1,     2,     3,     1,     3,     1,     1,     1,
       2,     1,     5,     2,     1,     1,     1,     3,     3,     1,
       2,     1,     2,     1,     1,     3,     1,     1,     4,     1,
       1,     1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "EOF", "error", "\"invalid token\"", "ARCHITECTURE", "CHEADER",
  "INCLUDE", "CODE", "REGCLASS", "CTYPE", "REGTYPE", "REGISTER",
  "CONSTANT", "INSNRULE", "INSN", "PATTERN", "CONSTRUCTOR", "IF", "CHECK",
  "ASM", "VAR", "ALIAS", "OBRACE", "CBRACE", "OBRACKET", "CBRACKET",
  "OPAREN", "CPAREN", "SEMICOLON", "BAR", "QUESTIONMARK", "COLON", "COMMA",
  "EQUAL", "OUTPUT", "INOUT", "TYPE", "CLOBBERS", "WHITESPACE", "STRING",
  "ID", "NUMBER", "VARNAME", "$accept", "file", "decl", "ctypeDecl",
  "ctypeBody", "ctypeBodyDecl", "codeDecl", "cbraceSemi", "cheaderDecl",
  "architectureDecl", "includeDecl", "regclassDecl", "registerDecl",
  "registerBody", "regBodyDecl", "clobberRegisterList", "regclassBody",
  "regclassBodyDecl", "regclassRegDecl", "regclassTypeDecl", "regtype",
  "insnDecl", "insnArgsDecl", "insnArgDeclDirection", "insnArgsDeclList",
  "moreInsnArgsDeclList", "insnBody", "insnBodyDecls", "insnBodyDecl",
  "constructorDecl", "constructorArgList", "moreConstructorArgs",
  "constructorArgTy", "insnVarDecl", "insnVarTyDecl", "rtlType",
  "insnVarDeclBody", "insnVarDeclBodyDecls", "insnVarDeclBodyDecl",
  "insnRuleDecl", "insnRuleBody", "insnRuleVarType", "insnRuleBodyDecl",
  "patternsDecl", "patternsDisjunction", "morePatternsDisjunction",
  "patternsConjunction", "optionalPattern", "morePatternsConjunction",
  "baseInsnPattern", "simpleInsnPattern", "insnArgs",
  "insnArgDirectionality", "moreInsnArgs", "insnArgPattern",
  "insnArgTypePattern", "insnArgExprPattern", "asmDecl", "asmInsn",
  "asmInsnArgs", "moreAsmInsnArgs", "asmInsnArg", "asmInsnExpr",
  "paramDecl", "literal", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   167,   167,   168,   169,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   183,   188,   193,   196,   199,   205,
     206,   209,   215,   220,   227,   234,   242,   243,   250,   254,
     259,   265,   266,   273,   274,   275,   279,   284,   292,   297,
     304,   308,   317,   318,   322,   323,   324,   328,   333,   341,
     342,   345,   346,   349,   354,   357,   358,   359,   363,   372,
     376,   380,   381,   385,   386,   387,   394,   402,   403,   408,
     409,   414,   421,   432,   438,   444,   445,   448,   449,   458,
     461,   468,   469,   479,   480,   481,   485,   490,   491,   492,
     493,   494,   497,   500,   506,   509,   513,   516,   519,   528,
     529,   533,   534,   538,   545,   550,   551,   558,   559,   560,
     563,   564,   567,   570,   575,   579,   583,   584,   591,   597,
     603,   609,   615,   616,   619,   620,   625,   628,   634,   639,
     640,   641
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


#line 10 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
} // nnc
#line 2931 "./insnsel/parser.cpp"

#line 650 "/home/tathougies/Projects/nnc/./insnsel/parser.y"


void nnc::parser::error(const location_type &l, const std::string &m) {
    throw NncParseError(l, m);
}

ArchDescLexer::~ArchDescLexer() {
}

nnc::parser::token_type ArchDescLexer::token_type(const nnc::position &offset) {
  std::uint64_t off(translate_position(offset));
  return token_type(off);
}
