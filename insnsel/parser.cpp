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
#line 172 "/home/tathougies/Projects/nnc/./insnsel/parser.y"

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

      case symbol_kind::S_registerBody: // registerBody
        value.YY_MOVE_OR_COPY< CompositeRegisterModifier * > (YY_MOVE (that.value));
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

      case symbol_kind::S_intersectDecl: // intersectDecl
        value.YY_MOVE_OR_COPY< IntersectDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_literal: // literal
        value.YY_MOVE_OR_COPY< Literal * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        value.YY_MOVE_OR_COPY< ParamDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassRange: // regclassRange
      case symbol_kind::S_regclassRangeMore: // regclassRangeMore
        value.YY_MOVE_OR_COPY< RangedRegMemberDeclarer * > (YY_MOVE (that.value));
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
      case symbol_kind::S_regclassRegDeclMore: // regclassRegDeclMore
        value.YY_MOVE_OR_COPY< RegMemberDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassRegDeclMember: // regclassRegDeclMember
        value.YY_MOVE_OR_COPY< RegMemberDeclarer * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regtype: // regtype
        value.YY_MOVE_OR_COPY< RegType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        value.YY_MOVE_OR_COPY< RegisterModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_optionalPatternSemicolon: // optionalPatternSemicolon
      case symbol_kind::S_optionalPattern: // optionalPattern
        value.YY_MOVE_OR_COPY< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_postfixIfDecl: // postfixIfDecl
      case symbol_kind::S_asmConditional: // asmConditional
        value.YY_MOVE_OR_COPY< std::optional<template_string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassRangeElem: // regclassRangeElem
        value.YY_MOVE_OR_COPY< std::pair<int, int>  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_optionalRegClass: // optionalRegClass
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

      case symbol_kind::S_registerBody: // registerBody
        value.move< CompositeRegisterModifier * > (YY_MOVE (that.value));
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

      case symbol_kind::S_intersectDecl: // intersectDecl
        value.move< IntersectDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_literal: // literal
        value.move< Literal * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        value.move< ParamDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassRange: // regclassRange
      case symbol_kind::S_regclassRangeMore: // regclassRangeMore
        value.move< RangedRegMemberDeclarer * > (YY_MOVE (that.value));
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
      case symbol_kind::S_regclassRegDeclMore: // regclassRegDeclMore
        value.move< RegMemberDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassRegDeclMember: // regclassRegDeclMember
        value.move< RegMemberDeclarer * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regtype: // regtype
        value.move< RegType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        value.move< RegisterModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_optionalPatternSemicolon: // optionalPatternSemicolon
      case symbol_kind::S_optionalPattern: // optionalPattern
        value.move< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_postfixIfDecl: // postfixIfDecl
      case symbol_kind::S_asmConditional: // asmConditional
        value.move< std::optional<template_string> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassRangeElem: // regclassRangeElem
        value.move< std::pair<int, int>  > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_optionalRegClass: // optionalRegClass
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

      case symbol_kind::S_registerBody: // registerBody
        value.copy< CompositeRegisterModifier * > (that.value);
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

      case symbol_kind::S_intersectDecl: // intersectDecl
        value.copy< IntersectDecl * > (that.value);
        break;

      case symbol_kind::S_literal: // literal
        value.copy< Literal * > (that.value);
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        value.copy< ParamDecl * > (that.value);
        break;

      case symbol_kind::S_regclassRange: // regclassRange
      case symbol_kind::S_regclassRangeMore: // regclassRangeMore
        value.copy< RangedRegMemberDeclarer * > (that.value);
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
      case symbol_kind::S_regclassRegDeclMore: // regclassRegDeclMore
        value.copy< RegMemberDecl * > (that.value);
        break;

      case symbol_kind::S_regclassRegDeclMember: // regclassRegDeclMember
        value.copy< RegMemberDeclarer * > (that.value);
        break;

      case symbol_kind::S_regtype: // regtype
        value.copy< RegType > (that.value);
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        value.copy< RegisterModifier * > (that.value);
        break;

      case symbol_kind::S_optionalPatternSemicolon: // optionalPatternSemicolon
      case symbol_kind::S_optionalPattern: // optionalPattern
        value.copy< bool > (that.value);
        break;

      case symbol_kind::S_postfixIfDecl: // postfixIfDecl
      case symbol_kind::S_asmConditional: // asmConditional
        value.copy< std::optional<template_string> > (that.value);
        break;

      case symbol_kind::S_regclassRangeElem: // regclassRangeElem
        value.copy< std::pair<int, int>  > (that.value);
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_optionalRegClass: // optionalRegClass
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

      case symbol_kind::S_registerBody: // registerBody
        value.move< CompositeRegisterModifier * > (that.value);
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

      case symbol_kind::S_intersectDecl: // intersectDecl
        value.move< IntersectDecl * > (that.value);
        break;

      case symbol_kind::S_literal: // literal
        value.move< Literal * > (that.value);
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        value.move< ParamDecl * > (that.value);
        break;

      case symbol_kind::S_regclassRange: // regclassRange
      case symbol_kind::S_regclassRangeMore: // regclassRangeMore
        value.move< RangedRegMemberDeclarer * > (that.value);
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
      case symbol_kind::S_regclassRegDeclMore: // regclassRegDeclMore
        value.move< RegMemberDecl * > (that.value);
        break;

      case symbol_kind::S_regclassRegDeclMember: // regclassRegDeclMember
        value.move< RegMemberDeclarer * > (that.value);
        break;

      case symbol_kind::S_regtype: // regtype
        value.move< RegType > (that.value);
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        value.move< RegisterModifier * > (that.value);
        break;

      case symbol_kind::S_optionalPatternSemicolon: // optionalPatternSemicolon
      case symbol_kind::S_optionalPattern: // optionalPattern
        value.move< bool > (that.value);
        break;

      case symbol_kind::S_postfixIfDecl: // postfixIfDecl
      case symbol_kind::S_asmConditional: // asmConditional
        value.move< std::optional<template_string> > (that.value);
        break;

      case symbol_kind::S_regclassRangeElem: // regclassRangeElem
        value.move< std::pair<int, int>  > (that.value);
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_optionalRegClass: // optionalRegClass
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

      case symbol_kind::S_registerBody: // registerBody
        yylhs.value.emplace< CompositeRegisterModifier * > ();
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

      case symbol_kind::S_intersectDecl: // intersectDecl
        yylhs.value.emplace< IntersectDecl * > ();
        break;

      case symbol_kind::S_literal: // literal
        yylhs.value.emplace< Literal * > ();
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        yylhs.value.emplace< ParamDecl * > ();
        break;

      case symbol_kind::S_regclassRange: // regclassRange
      case symbol_kind::S_regclassRangeMore: // regclassRangeMore
        yylhs.value.emplace< RangedRegMemberDeclarer * > ();
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
      case symbol_kind::S_regclassRegDeclMore: // regclassRegDeclMore
        yylhs.value.emplace< RegMemberDecl * > ();
        break;

      case symbol_kind::S_regclassRegDeclMember: // regclassRegDeclMember
        yylhs.value.emplace< RegMemberDeclarer * > ();
        break;

      case symbol_kind::S_regtype: // regtype
        yylhs.value.emplace< RegType > ();
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        yylhs.value.emplace< RegisterModifier * > ();
        break;

      case symbol_kind::S_optionalPatternSemicolon: // optionalPatternSemicolon
      case symbol_kind::S_optionalPattern: // optionalPattern
        yylhs.value.emplace< bool > ();
        break;

      case symbol_kind::S_postfixIfDecl: // postfixIfDecl
      case symbol_kind::S_asmConditional: // asmConditional
        yylhs.value.emplace< std::optional<template_string> > ();
        break;

      case symbol_kind::S_regclassRangeElem: // regclassRangeElem
        yylhs.value.emplace< std::pair<int, int>  > ();
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_optionalRegClass: // optionalRegClass
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
  case 5: // decl: paramDecl
#line 183 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { builder.param(yystack_[0].value.as < ParamDecl * > ()->name(), yystack_[0].value.as < ParamDecl * > ()->value()); delete yystack_[0].value.as < ParamDecl * > (); }
#line 1549 "./insnsel/parser.cpp"
    break;

  case 14: // ctypeDecl: CTYPE ID OBRACE ctypeBody
#line 194 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                      {
                    builder.addCType(yystack_[2].value.as < std::string > (), yystack_[0].value.as < CType * > ());
                }
#line 1557 "./insnsel/parser.cpp"
    break;

  case 15: // ctypeBody: ctypeBodyDecl ctypeBody
#line 199 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                    {
                    yystack_[1].value.as < CTypeModifier * > ()->modify(*yystack_[0].value.as < CType * > ());
                    delete yystack_[1].value.as < CTypeModifier * > ();
                    yylhs.value.as < CType * > () = yystack_[0].value.as < CType * > ();
                }
#line 1567 "./insnsel/parser.cpp"
    break;

  case 16: // ctypeBody: cbraceSemi
#line 204 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < CType * > () = new CType(); }
#line 1573 "./insnsel/parser.cpp"
    break;

  case 17: // ctypeBodyDecl: paramDecl
#line 207 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                 { yylhs.value.as < CTypeModifier * > () = static_cast<CTypeModifier *>(yystack_[0].value.as < ParamDecl * > ()); }
#line 1579 "./insnsel/parser.cpp"
    break;

  case 18: // codeDecl: CODE STRING STRING SEMICOLON
#line 211 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                  builder.addCodeSection(yystack_[2].value.as < std::string > (), yystack_[1].value.as < std::string > ());
                }
#line 1587 "./insnsel/parser.cpp"
    break;

  case 21: // cheaderDecl: CHEADER STRING SEMICOLON
#line 220 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                         {
                    builder.addCHeader(yystack_[1].value.as < std::string > ());
                }
#line 1595 "./insnsel/parser.cpp"
    break;

  case 22: // includeDecl: INCLUDE STRING SEMICOLON
#line 225 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                         {
                    NncErrorLocation l(yystack_[1].location, "Included from here");
                    builder.includeFile(lexer.filename(), yystack_[1].value.as < std::string > ());
                }
#line 1604 "./insnsel/parser.cpp"
    break;

  case 23: // regclassDecl: REGCLASS ID OBRACE regclassBody
#line 232 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                {
                    auto newRegClass(yystack_[0].value.as < RegClassFactory * > ()->build(yystack_[2].value.as < std::string > ()));
                    delete yystack_[0].value.as < RegClassFactory * > ();
                    builder.addRegisterClass(newRegClass);
                }
#line 1614 "./insnsel/parser.cpp"
    break;

  case 24: // registerDecl: REGISTER ID OBRACE registerBody
#line 239 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                          {
                     yystack_[0].value.as < CompositeRegisterModifier * > ()->modify(builder.getRegister(yystack_[2].value.as < std::string > ()));
                     delete yystack_[0].value.as < CompositeRegisterModifier * > ();
                 }
#line 1623 "./insnsel/parser.cpp"
    break;

  case 25: // registerBody: cbraceSemi
#line 245 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < CompositeRegisterModifier * > () = new CompositeRegisterModifier(); }
#line 1629 "./insnsel/parser.cpp"
    break;

  case 26: // registerBody: regBodyDecl registerBody
#line 246 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                               {
                  yystack_[0].value.as < CompositeRegisterModifier * > ()->addModifier(yystack_[1].value.as < RegisterModifier * > ());
                  yylhs.value.as < CompositeRegisterModifier * > () = yystack_[0].value.as < CompositeRegisterModifier * > ();
                }
#line 1638 "./insnsel/parser.cpp"
    break;

  case 27: // regBodyDecl: CLOBBERS clobberRegisterList SEMICOLON
#line 252 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                        { yylhs.value.as < RegisterModifier * > () = static_cast<RegisterModifier *>(yystack_[1].value.as < ClobberListDecl * > ()); }
#line 1644 "./insnsel/parser.cpp"
    break;

  case 28: // clobberRegisterList: ID COMMA clobberRegisterList
#line 257 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                  yystack_[0].value.as < ClobberListDecl * > ()->addRegister(yystack_[2].value.as < std::string > ());
                  yylhs.value.as < ClobberListDecl * > () = yystack_[0].value.as < ClobberListDecl * > ();
                }
#line 1653 "./insnsel/parser.cpp"
    break;

  case 29: // clobberRegisterList: ID
#line 261 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                   {
                    yylhs.value.as < ClobberListDecl * > () = new ClobberListDecl();
                    yylhs.value.as < ClobberListDecl * > ()->addRegister(yystack_[0].value.as < std::string > ());
                }
#line 1662 "./insnsel/parser.cpp"
    break;

  case 30: // regclassBody: cbraceSemi
#line 267 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < RegClassFactory * > () = new RegClassFactory(); }
#line 1668 "./insnsel/parser.cpp"
    break;

  case 31: // regclassBody: regclassBodyDecl regclassBody
#line 268 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                              {
                  yystack_[1].value.as < RegClassModifier * > ()->modify(*(yystack_[0].value.as < RegClassFactory * > ()));
                  delete yystack_[1].value.as < RegClassModifier * > ();
                  yylhs.value.as < RegClassFactory * > () = yystack_[0].value.as < RegClassFactory * > ();
                }
#line 1678 "./insnsel/parser.cpp"
    break;

  case 32: // regclassBodyDecl: paramDecl
#line 275 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                              { yylhs.value.as < RegClassModifier * > () = static_cast<RegClassModifier *>(yystack_[0].value.as < ParamDecl * > ()); }
#line 1684 "./insnsel/parser.cpp"
    break;

  case 33: // regclassBodyDecl: REGISTER regclassRegDecl SEMICOLON
#line 276 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                       { yylhs.value.as < RegClassModifier * > () = static_cast<RegClassModifier *>(yystack_[1].value.as < RegMemberDecl * > ()); }
#line 1690 "./insnsel/parser.cpp"
    break;

  case 34: // regclassBodyDecl: TYPE regclassTypeDecl SEMICOLON
#line 277 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                    { yylhs.value.as < RegClassModifier * > () =static_cast<RegClassModifier *>(yystack_[1].value.as < RegClassTypeDecl * > ()); }
#line 1696 "./insnsel/parser.cpp"
    break;

  case 35: // regclassRegDecl: regclassRegDeclMember regclassRegDeclMore
#line 281 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                    yystack_[1].value.as < RegMemberDeclarer * > ()->modify(*yystack_[0].value.as < RegMemberDecl * > ());
                    delete yystack_[1].value.as < RegMemberDeclarer * > ();
                    yylhs.value.as < RegMemberDecl * > () = yystack_[0].value.as < RegMemberDecl * > ();
                }
#line 1706 "./insnsel/parser.cpp"
    break;

  case 36: // regclassRegDeclMember: ID OBRACKET regclassRange CBRACKET
#line 290 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                    yystack_[1].value.as < RangedRegMemberDeclarer * > ()->setName(yystack_[3].value.as < std::string > ());
                    yylhs.value.as < RegMemberDeclarer * > () = yystack_[1].value.as < RangedRegMemberDeclarer * > ();
                }
#line 1715 "./insnsel/parser.cpp"
    break;

  case 37: // regclassRegDeclMember: ID
#line 294 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                   { yylhs.value.as < RegMemberDeclarer * > () = new SingleRegMemberDeclarer(yystack_[0].value.as < std::string > ()); }
#line 1721 "./insnsel/parser.cpp"
    break;

  case 38: // regclassRange: regclassRangeElem regclassRangeMore
#line 298 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                    yystack_[0].value.as < RangedRegMemberDeclarer * > ()->addRange(yystack_[1].value.as < std::pair<int, int>  > ().first, yystack_[1].value.as < std::pair<int, int>  > ().second);
                    yylhs.value.as < RangedRegMemberDeclarer * > () = yystack_[0].value.as < RangedRegMemberDeclarer * > ();
                }
#line 1730 "./insnsel/parser.cpp"
    break;

  case 39: // regclassRangeMore: COMMA regclassRange
#line 306 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                 { yylhs.value.as < RangedRegMemberDeclarer * > () = yystack_[0].value.as < RangedRegMemberDeclarer * > (); }
#line 1736 "./insnsel/parser.cpp"
    break;

  case 40: // regclassRangeMore: %empty
#line 307 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                        { yylhs.value.as < RangedRegMemberDeclarer * > () = new RangedRegMemberDeclarer(); }
#line 1742 "./insnsel/parser.cpp"
    break;

  case 41: // regclassRangeElem: NUMBER DASH NUMBER
#line 312 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                 {
                     NncErrorLocation l(yystack_[2].location+yystack_[0].location, "While parsing the register name range");
                     if ( yystack_[2].value.as < std::uint32_t > () > yystack_[0].value.as < std::uint32_t > () ) {
                         throw NncParseError(yystack_[2].location, "Start of register name range is greater than the end");
                     }

                     yylhs.value.as < std::pair<int, int>  > () = std::make_pair(yystack_[2].value.as < std::uint32_t > (), yystack_[0].value.as < std::uint32_t > ());
                }
#line 1755 "./insnsel/parser.cpp"
    break;

  case 42: // regclassRangeElem: NUMBER
#line 320 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < std::pair<int, int>  > () = std::make_pair(yystack_[0].value.as < std::uint32_t > (), yystack_[0].value.as < std::uint32_t > ()); }
#line 1761 "./insnsel/parser.cpp"
    break;

  case 43: // regclassRegDeclMore: COMMA regclassRegDecl
#line 324 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                      { yylhs.value.as < RegMemberDecl * > () = yystack_[0].value.as < RegMemberDecl * > (); }
#line 1767 "./insnsel/parser.cpp"
    break;

  case 44: // regclassRegDeclMore: %empty
#line 325 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < RegMemberDecl * > () = new RegMemberDecl(); }
#line 1773 "./insnsel/parser.cpp"
    break;

  case 45: // regclassTypeDecl: regtype COMMA regclassTypeDecl
#line 330 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                    yystack_[0].value.as < RegClassTypeDecl * > ()->addType(yystack_[2].value.as < RegType > ());
                    yylhs.value.as < RegClassTypeDecl * > () = yystack_[0].value.as < RegClassTypeDecl * > ();
                }
#line 1782 "./insnsel/parser.cpp"
    break;

  case 46: // regclassTypeDecl: regtype
#line 335 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                    yylhs.value.as < RegClassTypeDecl * > () = new RegClassTypeDecl();
                    yylhs.value.as < RegClassTypeDecl * > ()->addType(yystack_[0].value.as < RegType > ());
                }
#line 1791 "./insnsel/parser.cpp"
    break;

  case 47: // regtype: ID
#line 341 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
               { NncErrorLocation l(yystack_[0].location, "While parsing the register type");
                 yylhs.value.as < RegType > () = RegType::fromString(yystack_[0].value.as < std::string > ()); }
#line 1798 "./insnsel/parser.cpp"
    break;

  case 48: // insnDecl: INSN ID insnArgsDecl insnBody
#line 345 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                    {
                    yystack_[1].value.as < InsnArgsModifier * > ()->modify(*yystack_[0].value.as < InsnFactory * > ());
                    delete yystack_[1].value.as < InsnArgsModifier * > ();

                    NncErrorLocation l(yystack_[3].location + yystack_[0].location, "In the instruction declaration for " + yystack_[2].value.as < std::string > ());
                    builder.addInsn(yystack_[0].value.as < InsnFactory * > ()->build(yystack_[2].value.as < std::string > ()));
                }
#line 1810 "./insnsel/parser.cpp"
    break;

  case 49: // insnArgsDecl: OPAREN insnArgsDeclList
#line 354 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                        { yylhs.value.as < InsnArgsModifier * > () = yystack_[0].value.as < InsnArgsModifier * > (); }
#line 1816 "./insnsel/parser.cpp"
    break;

  case 50: // insnArgsDecl: %empty
#line 355 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgsModifier * > () = new InsnArgsModifier(); }
#line 1822 "./insnsel/parser.cpp"
    break;

  case 51: // insnArgDeclDirection: %empty
#line 359 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgDecl::Direction > () = InsnArgDecl::Input; }
#line 1828 "./insnsel/parser.cpp"
    break;

  case 52: // insnArgDeclDirection: OUTPUT
#line 360 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgDecl::Direction > () = InsnArgDecl::Output; }
#line 1834 "./insnsel/parser.cpp"
    break;

  case 53: // insnArgDeclDirection: INOUT
#line 361 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgDecl::Direction > () = InsnArgDecl::InputOutput; }
#line 1840 "./insnsel/parser.cpp"
    break;

  case 54: // insnArgsDeclList: insnArgDeclDirection VARNAME moreInsnArgsDeclList
#line 365 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                  {
                  NncErrorLocation l(yystack_[2].location + yystack_[1].location, "In the insn arg declaration");
                  yystack_[0].value.as < InsnArgsModifier * > ()->addArgument(yystack_[1].value.as < std::string > ()).direction(yystack_[2].value.as < InsnArgDecl::Direction > ());
                  yylhs.value.as < InsnArgsModifier * > () = yystack_[0].value.as < InsnArgsModifier * > ();
                }
#line 1850 "./insnsel/parser.cpp"
    break;

  case 55: // insnArgsDeclList: insnArgDeclDirection OBRACKET VARNAME CBRACKET moreInsnArgsDeclList
#line 370 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                                              {
                  NncErrorLocation l(yystack_[4].location + yystack_[1].location, "In the insn arg declaration");
                  yystack_[0].value.as < InsnArgsModifier * > ()->addArgument(yystack_[2].value.as < std::string > ()).direction(yystack_[4].value.as < InsnArgDecl::Direction > ()).makeOptional();
                  yylhs.value.as < InsnArgsModifier * > () = yystack_[0].value.as < InsnArgsModifier * > ();
                }
#line 1860 "./insnsel/parser.cpp"
    break;

  case 56: // moreInsnArgsDeclList: COMMA insnArgsDeclList
#line 378 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                       { yylhs.value.as < InsnArgsModifier * > () = yystack_[0].value.as < InsnArgsModifier * > (); }
#line 1866 "./insnsel/parser.cpp"
    break;

  case 57: // moreInsnArgsDeclList: CPAREN
#line 379 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgsModifier * > () = new InsnArgsModifier(); }
#line 1872 "./insnsel/parser.cpp"
    break;

  case 58: // insnBody: SEMICOLON
#line 382 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { yylhs.value.as < InsnFactory * > () = new InsnFactory(); }
#line 1878 "./insnsel/parser.cpp"
    break;

  case 59: // insnBody: OBRACE insnBodyDecls
#line 383 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                     { yylhs.value.as < InsnFactory * > () = yystack_[0].value.as < InsnFactory * > (); }
#line 1884 "./insnsel/parser.cpp"
    break;

  case 60: // insnBodyDecls: insnBodyDecl insnBodyDecls
#line 386 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                           {
                    yystack_[1].value.as < InsnModifier * > ()->modify(*yystack_[0].value.as < InsnFactory * > ());
                    delete yystack_[1].value.as < InsnModifier * > ();
                    yylhs.value.as < InsnFactory * > () = yystack_[0].value.as < InsnFactory * > ();
                }
#line 1894 "./insnsel/parser.cpp"
    break;

  case 61: // insnBodyDecls: CBRACE
#line 391 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnFactory * > () = new InsnFactory(); }
#line 1900 "./insnsel/parser.cpp"
    break;

  case 62: // insnBodyDecl: paramDecl
#line 394 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { yylhs.value.as < InsnModifier * > () = static_cast<InsnModifier *>(yystack_[0].value.as < ParamDecl * > ()); }
#line 1906 "./insnsel/parser.cpp"
    break;

  case 63: // insnBodyDecl: insnVarDecl
#line 395 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                            { yylhs.value.as < InsnModifier * > () = static_cast<InsnModifier *>(yystack_[0].value.as < InsnVarDecl * > ()); }
#line 1912 "./insnsel/parser.cpp"
    break;

  case 64: // insnBodyDecl: constructorDecl
#line 396 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                { yylhs.value.as < InsnModifier * > () = static_cast<InsnModifier *>(yystack_[0].value.as < ConstructorDecl * > ()); }
#line 1918 "./insnsel/parser.cpp"
    break;

  case 65: // insnBodyDecl: intersectDecl
#line 397 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                              { yylhs.value.as < InsnModifier * > () = static_cast<InsnModifier *>(yystack_[0].value.as < IntersectDecl * > ()); }
#line 1924 "./insnsel/parser.cpp"
    break;

  case 66: // intersectDecl: INTERSECT OPAREN VARNAME COMMA VARNAME COMMA ID CPAREN postfixIfDecl SEMICOLON
#line 401 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                    yylhs.value.as < IntersectDecl * > () = new IntersectDecl(yystack_[7].value.as < std::string > (), yystack_[5].value.as < std::string > (), yystack_[3].value.as < std::string > ());
                    if ( yystack_[1].value.as < std::optional<template_string> > () )
                        yylhs.value.as < IntersectDecl * > ()->setCondition(*yystack_[1].value.as < std::optional<template_string> > ());
                }
#line 1934 "./insnsel/parser.cpp"
    break;

  case 67: // postfixIfDecl: IF STRING
#line 408 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           {
                     yylhs.value.as < std::optional<template_string> > () = template_string(yystack_[0].value.as < std::string > ());
                 }
#line 1942 "./insnsel/parser.cpp"
    break;

  case 68: // postfixIfDecl: %empty
#line 411 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                        { }
#line 1948 "./insnsel/parser.cpp"
    break;

  case 69: // constructorDecl: CONSTRUCTOR OPAREN constructorArgList SEMICOLON
#line 416 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                    NncErrorLocation l(yystack_[3].location + yystack_[1].location, "In the constructor declaration");
                    yystack_[1].value.as < ConstructorDecl * > ()->errorContext() = NncErrorContextStack::current();
                    yylhs.value.as < ConstructorDecl * > () = yystack_[1].value.as < ConstructorDecl * > ();
                }
#line 1958 "./insnsel/parser.cpp"
    break;

  case 70: // constructorArgList: constructorArgTy moreConstructorArgs
#line 424 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                {
                  yystack_[0].value.as < ConstructorDecl * > ()->addType(yystack_[1].value.as < InsnVarType * > ());
                  yylhs.value.as < ConstructorDecl * > () = yystack_[0].value.as < ConstructorDecl * > ();
                }
#line 1967 "./insnsel/parser.cpp"
    break;

  case 71: // constructorArgList: CPAREN
#line 428 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < ConstructorDecl * > () = new ConstructorDecl(); }
#line 1973 "./insnsel/parser.cpp"
    break;

  case 72: // moreConstructorArgs: COMMA constructorArgList
#line 432 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                               { yylhs.value.as < ConstructorDecl * > () = yystack_[0].value.as < ConstructorDecl * > (); }
#line 1979 "./insnsel/parser.cpp"
    break;

  case 73: // moreConstructorArgs: CPAREN
#line 433 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < ConstructorDecl * > () = new ConstructorDecl(); }
#line 1985 "./insnsel/parser.cpp"
    break;

  case 74: // constructorArgTy: REGISTER
#line 437 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { yylhs.value.as < InsnVarType * > () = new InsnRegisterVarType(); }
#line 1991 "./insnsel/parser.cpp"
    break;

  case 75: // constructorArgTy: CONSTANT
#line 438 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { yylhs.value.as < InsnVarType * > () = new InsnConstantVarType(); }
#line 1997 "./insnsel/parser.cpp"
    break;

  case 76: // constructorArgTy: ID
#line 439 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                               {
                   NncErrorLocation l(yystack_[0].location, "In constructor arg type declaration");
                   CType &ty(builder.cType(yystack_[0].value.as < std::string > ()));
                   yylhs.value.as < InsnVarType * > () = static_cast<InsnVarType *>(new CType(ty));
                 }
#line 2007 "./insnsel/parser.cpp"
    break;

  case 77: // insnVarDecl: VAR VARNAME COLON insnVarTyDecl insnVarDeclBody SEMICOLON
#line 447 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                  NncErrorLocation l(yystack_[5].location + yystack_[0].location, "In the var decl");
                  yylhs.value.as < InsnVarDecl * > () = yystack_[1].value.as < InsnVarDeclFactory * > ()->build(yystack_[4].value.as < std::string > (), yystack_[2].value.as < InsnVarType * > ());
                  delete yystack_[1].value.as < InsnVarDeclFactory * > ();
                }
#line 2017 "./insnsel/parser.cpp"
    break;

  case 78: // optionalRegClass: COMMA ID
#line 455 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2023 "./insnsel/parser.cpp"
    break;

  case 80: // insnVarTyDecl: REGISTER
#line 459 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                         { yylhs.value.as < InsnVarType * > () = new InsnRegisterVarType(); }
#line 2029 "./insnsel/parser.cpp"
    break;

  case 81: // insnVarTyDecl: REGISTER OPAREN rtlType optionalRegClass CPAREN
#line 460 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                {
                  auto newTy(new InsnRegisterVarType());
                  newTy->setRtlType(yystack_[2].value.as < InsnRtlType > ());
                  if ( !yystack_[1].value.as < std::string > ().empty() )
                      newTy->setRegClass(yystack_[1].value.as < std::string > ());
                  yylhs.value.as < InsnVarType * > () = static_cast<InsnVarType *>(newTy);
                }
#line 2041 "./insnsel/parser.cpp"
    break;

  case 82: // insnVarTyDecl: CONSTANT
#line 467 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                         { yylhs.value.as < InsnVarType * > () = new InsnConstantVarType(); }
#line 2047 "./insnsel/parser.cpp"
    break;

  case 83: // insnVarTyDecl: CONSTANT OPAREN rtlType CPAREN
#line 468 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                               {
                  auto newTy(new InsnConstantVarType());
                  newTy->setRtlType(yystack_[1].value.as < InsnRtlType > ());
                  yylhs.value.as < InsnVarType * > () = static_cast<InsnVarType *>(newTy);
                }
#line 2057 "./insnsel/parser.cpp"
    break;

  case 84: // insnVarTyDecl: ID
#line 473 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                              {
                  NncErrorLocation l(yystack_[0].location, "In the var type declaration");
                  CType &ty(builder.cType(yystack_[0].value.as < std::string > ()));
                  if ( ty.acceptsRtlType() )
                    throw NncParseError(yystack_[0].location, "Type " + yystack_[0].value.as < std::string > () + " requires an RTL type parameter");
                  yylhs.value.as < InsnVarType * > () = static_cast<InsnVarType *>(new CType(ty));
                }
#line 2069 "./insnsel/parser.cpp"
    break;

  case 85: // insnVarTyDecl: ID OPAREN rtlType optionalRegClass CPAREN
#line 480 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                     {
                  NncErrorLocation l(yystack_[4].location, "In the var type declaration");
                  CType &ty(builder.cType(yystack_[4].value.as < std::string > ()));
                  if ( !ty.acceptsRtlType() )
                    throw NncParseError(yystack_[4].location, "Type " + yystack_[4].value.as < std::string > () + " does not accept RTL type parameters");
                  auto newType(new CType(ty));
                  newType->setRtlType(yystack_[2].value.as < InsnRtlType > ());
                  if ( !yystack_[1].value.as < std::string > ().empty() )
                      newType->setRegClass(yystack_[1].value.as < std::string > ());
                  yylhs.value.as < InsnVarType * > () = static_cast<InsnVarType *>(newType);
                }
#line 2085 "./insnsel/parser.cpp"
    break;

  case 86: // rtlType: ID rtlType
#line 493 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           {
                    NncErrorLocation l(yystack_[1].location + yystack_[0].location, "In the RTL type");
                    if ( yystack_[1].value.as < std::string > () != "ptr" )
                      throw NncParseError("Expected ptr");
                    yylhs.value.as < InsnRtlType > () = yystack_[0].value.as < InsnRtlType > ().ptr();
                }
#line 2096 "./insnsel/parser.cpp"
    break;

  case 87: // rtlType: ID
#line 499 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                   {
                   NncErrorLocation l(yystack_[0].location, "In the primitive RTL type");
                   yylhs.value.as < InsnRtlType > () = InsnRtlType(yystack_[0].value.as < std::string > ());
                }
#line 2105 "./insnsel/parser.cpp"
    break;

  case 88: // insnVarDeclBody: %empty
#line 505 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < InsnVarDeclFactory * > () = new InsnVarDeclFactory(); }
#line 2111 "./insnsel/parser.cpp"
    break;

  case 89: // insnVarDeclBody: OBRACE insnVarDeclBodyDecls
#line 506 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                { yylhs.value.as < InsnVarDeclFactory * > () = yystack_[0].value.as < InsnVarDeclFactory * > (); }
#line 2117 "./insnsel/parser.cpp"
    break;

  case 90: // insnVarDeclBodyDecls: CBRACE
#line 509 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                             { yylhs.value.as < InsnVarDeclFactory * > () = new InsnVarDeclFactory(); }
#line 2123 "./insnsel/parser.cpp"
    break;

  case 91: // insnVarDeclBodyDecls: insnVarDeclBodyDecl insnVarDeclBodyDecls
#line 511 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                      {
                        yystack_[1].value.as < InsnVarDeclModifier * > ()->modify(*yystack_[0].value.as < InsnVarDeclFactory * > ());
                        delete yystack_[1].value.as < InsnVarDeclModifier * > ();
                        yylhs.value.as < InsnVarDeclFactory * > () = yystack_[0].value.as < InsnVarDeclFactory * > ();
                      }
#line 2133 "./insnsel/parser.cpp"
    break;

  case 92: // insnVarDeclBodyDecl: paramDecl
#line 519 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { yylhs.value.as < InsnVarDeclModifier * > () = static_cast<InsnVarDeclModifier *>(yystack_[0].value.as < ParamDecl * > ()); }
#line 2139 "./insnsel/parser.cpp"
    break;

  case 93: // insnRuleDecl: INSNRULE ID OBRACE insnRuleBody
#line 522 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                {
                    NncErrorLocation l(yystack_[3].location + yystack_[0].location, "In the instruction rule");
                    builder.addInsnRule(yystack_[0].value.as < InsnRuleFactory * > ()->build(yystack_[2].value.as < std::string > ()));
                    delete yystack_[0].value.as < InsnRuleFactory * > ();
                }
#line 2149 "./insnsel/parser.cpp"
    break;

  case 94: // insnRuleBody: cbraceSemi
#line 529 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < InsnRuleFactory * > () = new InsnRuleFactory(); }
#line 2155 "./insnsel/parser.cpp"
    break;

  case 95: // insnRuleBody: insnRuleBodyDecl insnRuleBody
#line 530 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                              {
                  if ( yystack_[1].value.as < InsnRuleModifier * > () ) {
                    yystack_[1].value.as < InsnRuleModifier * > ()->modify(*yystack_[0].value.as < InsnRuleFactory * > ());
                    if ( yystack_[1].value.as < InsnRuleModifier * > ()->shouldDeleteInsnRuleModifierAfterModify() ) delete yystack_[1].value.as < InsnRuleModifier * > ();
                  }
                  yylhs.value.as < InsnRuleFactory * > () = yystack_[0].value.as < InsnRuleFactory * > ();
                }
#line 2167 "./insnsel/parser.cpp"
    break;

  case 96: // insnRuleVarType: REGTYPE
#line 540 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                         { yylhs.value.as < InsnVariable::Type > () = InsnVariable::RegTypeType; }
#line 2173 "./insnsel/parser.cpp"
    break;

  case 97: // insnRuleVarType: REGISTER
#line 541 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                         { yylhs.value.as < InsnVariable::Type > () = InsnVariable::RegisterType; }
#line 2179 "./insnsel/parser.cpp"
    break;

  case 98: // insnRuleVarType: CONSTANT
#line 542 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                         { yylhs.value.as < InsnVariable::Type > () = InsnVariable::ConstantType; }
#line 2185 "./insnsel/parser.cpp"
    break;

  case 99: // insnRuleBodyDecl: VAR VARNAME COLON insnRuleVarType SEMICOLON
#line 547 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                {
                   auto decl(new InsnRuleVarDecl(yystack_[3].location, yystack_[3].value.as < std::string > (), yystack_[1].value.as < InsnVariable::Type > ()));
                   yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(decl);
                }
#line 2194 "./insnsel/parser.cpp"
    break;

  case 100: // insnRuleBodyDecl: PATTERN OBRACE patternsDecl cbraceSemi
#line 551 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                        { yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(yystack_[1].value.as < InsnRuleModifier * > ()); }
#line 2200 "./insnsel/parser.cpp"
    break;

  case 101: // insnRuleBodyDecl: IF STRING SEMICOLON
#line 552 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                     { yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(new InsnCondDecl(yystack_[1].value.as < std::string > ())); }
#line 2206 "./insnsel/parser.cpp"
    break;

  case 102: // insnRuleBodyDecl: CHECK STRING SEMICOLON
#line 553 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                        { yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(new InsnCheckDecl(yystack_[2].location, yystack_[1].value.as < std::string > ())); }
#line 2212 "./insnsel/parser.cpp"
    break;

  case 103: // insnRuleBodyDecl: paramDecl
#line 554 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(yystack_[0].value.as < ParamDecl * > ()); }
#line 2218 "./insnsel/parser.cpp"
    break;

  case 104: // insnRuleBodyDecl: ASM OBRACE asmDecl
#line 555 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                    { yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(yystack_[0].value.as < InsnRuleAsmDecl * > ()); }
#line 2224 "./insnsel/parser.cpp"
    break;

  case 105: // insnRuleBodyDecl: ALIAS VARNAME VARNAME SEMICOLON
#line 556 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                              {
                     yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(new InsnAliasDecl(yystack_[2].value.as < std::string > (), yystack_[1].value.as < std::string > ()));
                 }
#line 2232 "./insnsel/parser.cpp"
    break;

  case 106: // patternsDecl: patternsDisjunction
#line 561 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                  { yylhs.value.as < InsnRuleModifier * > () = static_cast<InsnRuleModifier *>(yystack_[0].value.as < InsnGen * > ()); }
#line 2238 "./insnsel/parser.cpp"
    break;

  case 107: // patternsDisjunction: patternsConjunction morePatternsDisjunction
#line 564 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                        {
                  yylhs.value.as < InsnGen * > () = InsnRuleDisjunction::make(yystack_[0].value.as < InsnGen * > (), yystack_[1].value.as < InsnGen * > ());
                }
#line 2246 "./insnsel/parser.cpp"
    break;

  case 108: // morePatternsDisjunction: BAR patternsDisjunction
#line 570 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                        {
                  yylhs.value.as < InsnGen * > () = yystack_[0].value.as < InsnGen * > ();
                }
#line 2254 "./insnsel/parser.cpp"
    break;

  case 109: // morePatternsDisjunction: %empty
#line 573 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnGen * > () = nullptr; }
#line 2260 "./insnsel/parser.cpp"
    break;

  case 110: // patternsConjunction: baseInsnPattern SEMICOLON morePatternsConjunction
#line 577 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                              {
                  yylhs.value.as < InsnGen * > () = InsnRuleConjunction::make(yystack_[2].value.as < InsnGen * > (), yystack_[0].value.as < InsnGen * > ());
                }
#line 2268 "./insnsel/parser.cpp"
    break;

  case 111: // patternsConjunction: baseInsnPattern
#line 580 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                      {
                  yylhs.value.as < InsnGen * > () = static_cast<InsnGen *>(yystack_[0].value.as < InsnGen * > ());
                }
#line 2276 "./insnsel/parser.cpp"
    break;

  case 112: // patternsConjunction: OBRACE patternsDisjunction CBRACE optionalPatternSemicolon morePatternsConjunction
#line 583 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                                                                         {
                  if ( yystack_[1].value.as < bool > () ) {
                    yystack_[3].value.as < InsnGen * > () = InsnRuleOptionalPattern::make(yystack_[3].value.as < InsnGen * > ());
                  }
                  yylhs.value.as < InsnGen * > () = InsnRuleConjunction::make(yystack_[3].value.as < InsnGen * > (), yystack_[0].value.as < InsnGen * > ());
                }
#line 2287 "./insnsel/parser.cpp"
    break;

  case 113: // patternsConjunction: OBRACKET patternsDisjunction CBRACKET optionalPatternSemicolon morePatternsConjunction
#line 589 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                                                                             {
                    if ( yystack_[1].value.as < bool > () ) {
                        yystack_[3].value.as < InsnGen * > () = InsnRuleOptionalPattern::make(yystack_[3].value.as < InsnGen * > ());
                    }
                    auto head(new InsnRuleHiddenPattern(yystack_[3].value.as < InsnGen * > ()));
                    yylhs.value.as < InsnGen * > () = InsnRuleConjunction::make(head, yystack_[0].value.as < InsnGen * > ());
                }
#line 2299 "./insnsel/parser.cpp"
    break;

  case 114: // optionalPatternSemicolon: optionalPattern
#line 599 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                { yylhs.value.as < bool > () = yystack_[0].value.as < bool > (); }
#line 2305 "./insnsel/parser.cpp"
    break;

  case 115: // optionalPatternSemicolon: optionalPattern SEMICOLON
#line 600 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                          { yylhs.value.as < bool > () = yystack_[1].value.as < bool > (); }
#line 2311 "./insnsel/parser.cpp"
    break;

  case 116: // optionalPattern: QUESTIONMARK
#line 604 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                             { yylhs.value.as < bool > () = true; }
#line 2317 "./insnsel/parser.cpp"
    break;

  case 117: // optionalPattern: %empty
#line 605 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < bool > () = false; }
#line 2323 "./insnsel/parser.cpp"
    break;

  case 118: // morePatternsConjunction: %empty
#line 609 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnGen * > () = nullptr; }
#line 2329 "./insnsel/parser.cpp"
    break;

  case 119: // morePatternsConjunction: patternsConjunction
#line 610 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                    { yylhs.value.as < InsnGen * > () = yystack_[0].value.as < InsnGen * > (); }
#line 2335 "./insnsel/parser.cpp"
    break;

  case 120: // baseInsnPattern: simpleInsnPattern optionalPattern
#line 614 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                  {
                  yylhs.value.as < InsnGen * > () = static_cast<InsnGen *>(yystack_[1].value.as < InsnPattern * > ());
                  if ( yystack_[0].value.as < bool > () ) yylhs.value.as < InsnGen * > () = InsnRuleOptionalPattern::make(yylhs.value.as < InsnGen * > ());
                }
#line 2344 "./insnsel/parser.cpp"
    break;

  case 121: // simpleInsnPattern: ID OPAREN insnArgs
#line 621 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                           {
                  yylhs.value.as < InsnPattern * > () = new InsnPattern(yystack_[2].value.as < std::string > (), yystack_[0].value.as < InsnArgPatterns * > ());
                }
#line 2352 "./insnsel/parser.cpp"
    break;

  case 122: // insnArgs: CPAREN
#line 626 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgPatterns * > () = new InsnArgPatterns(); }
#line 2358 "./insnsel/parser.cpp"
    break;

  case 123: // insnArgs: ID insnArgPattern moreInsnArgs
#line 627 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                               {
                   yylhs.value.as < InsnArgPatterns * > () = yystack_[0].value.as < InsnArgPatterns * > ();
                   yylhs.value.as < InsnArgPatterns * > ()->addArgument(yystack_[2].value.as < std::string > (), yystack_[1].value.as < InsnArgPattern > ());
                }
#line 2367 "./insnsel/parser.cpp"
    break;

  case 124: // insnArgDirectionality: EQUAL
#line 634 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                      { yylhs.value.as < InsnArgPattern::Directionality > () = InsnArgPattern::Input; }
#line 2373 "./insnsel/parser.cpp"
    break;

  case 125: // insnArgDirectionality: OUTPUT
#line 635 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgPattern::Directionality > () = InsnArgPattern::Output; }
#line 2379 "./insnsel/parser.cpp"
    break;

  case 126: // insnArgDirectionality: INOUT
#line 636 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                      { yylhs.value.as < InsnArgPattern::Directionality > () = InsnArgPattern::InputOutput; }
#line 2385 "./insnsel/parser.cpp"
    break;

  case 127: // moreInsnArgs: COMMA insnArgs
#line 639 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                               { yylhs.value.as < InsnArgPatterns * > () = yystack_[0].value.as < InsnArgPatterns * > (); }
#line 2391 "./insnsel/parser.cpp"
    break;

  case 128: // moreInsnArgs: CPAREN
#line 640 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < InsnArgPatterns * > () = new InsnArgPatterns(); }
#line 2397 "./insnsel/parser.cpp"
    break;

  case 129: // insnArgPattern: insnArgDirectionality OPAREN insnArgTypePattern CPAREN insnArgExprPattern
#line 643 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                                                          {
                    yylhs.value.as < InsnArgPattern > () = InsnArgPattern(yystack_[4].location, yystack_[4].value.as < InsnArgPattern::Directionality > (), yystack_[2].value.as < std::string > (), yystack_[0].value.as < std::string > ());
                }
#line 2405 "./insnsel/parser.cpp"
    break;

  case 130: // insnArgPattern: insnArgDirectionality insnArgExprPattern
#line 646 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                         {
                    yylhs.value.as < InsnArgPattern > () = InsnArgPattern(yystack_[1].location, yystack_[1].value.as < InsnArgPattern::Directionality > (), yystack_[0].value.as < std::string > ());
                }
#line 2413 "./insnsel/parser.cpp"
    break;

  case 131: // insnArgTypePattern: VARNAME
#line 651 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                            { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2419 "./insnsel/parser.cpp"
    break;

  case 132: // insnArgExprPattern: VARNAME
#line 655 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                        { yylhs.value.as < std::string > () = yystack_[0].value.as < std::string > (); }
#line 2425 "./insnsel/parser.cpp"
    break;

  case 133: // asmDecl: cbraceSemi
#line 659 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                           { yylhs.value.as < InsnRuleAsmDecl * > () = new InsnRuleAsmDecl(); }
#line 2431 "./insnsel/parser.cpp"
    break;

  case 134: // asmDecl: asmInsn SEMICOLON asmDecl
#line 660 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                          {
                  yystack_[0].value.as < InsnRuleAsmDecl * > ()->generateInsn(*yystack_[2].value.as < AsmInsnCall * > ());
                  delete yystack_[2].value.as < AsmInsnCall * > ();
                  yylhs.value.as < InsnRuleAsmDecl * > () = yystack_[0].value.as < InsnRuleAsmDecl * > ();
        }
#line 2441 "./insnsel/parser.cpp"
    break;

  case 135: // asmConditional: IF STRING
#line 667 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                          { yylhs.value.as < std::optional<template_string> > () = template_string(yystack_[0].value.as < std::string > ()); }
#line 2447 "./insnsel/parser.cpp"
    break;

  case 137: // asmInsn: ID OPAREN asmInsnArgs asmConditional
#line 671 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                                     {
                    NncErrorLocation l(yystack_[3].location + yystack_[1].location, "In the generated asm expression");
                    yystack_[1].value.as < AsmInsnCall * > ()->mnemonic(yystack_[3].value.as < std::string > ());
                    yystack_[1].value.as < AsmInsnCall * > ()->errorContext() = NncErrorContextStack::current();
                    yylhs.value.as < AsmInsnCall * > () = yystack_[1].value.as < AsmInsnCall * > ();
                    if ( yystack_[0].value.as < std::optional<template_string> > () )
                        yylhs.value.as < AsmInsnCall * > ()->addCondition(*yystack_[0].value.as < std::optional<template_string> > ());
                }
#line 2460 "./insnsel/parser.cpp"
    break;

  case 138: // asmInsn: ID asmConditional
#line 679 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                  {
                  NncErrorLocation l(yystack_[1].location, "In the generated asm expression");
                  yylhs.value.as < AsmInsnCall * > () = new AsmInsnCall(yystack_[1].value.as < std::string > ());
                  if ( yystack_[0].value.as < std::optional<template_string> > () )
                      yylhs.value.as < AsmInsnCall * > ()->addCondition(*yystack_[0].value.as < std::optional<template_string> > ());
                }
#line 2471 "./insnsel/parser.cpp"
    break;

  case 139: // asmInsn: STRING
#line 685 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       {
                  NncErrorLocation l(yystack_[0].location, "In the generated asm expression");
                  yylhs.value.as < AsmInsnCall * > () = new AsmInsnCall(template_string(yystack_[0].value.as < std::string > ()));
                }
#line 2480 "./insnsel/parser.cpp"
    break;

  case 140: // asmInsnArgs: asmInsnArg moreAsmInsnArgs
#line 691 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                           {
                    NncErrorLocation l(yystack_[1].location, "In the args of the asm instruction");
                    yystack_[1].value.as < AsmInsnArg * > ()->addToCall(*yystack_[0].value.as < AsmInsnCall * > ());
                    delete yystack_[1].value.as < AsmInsnArg * > ();
                    yylhs.value.as < AsmInsnCall * > () = yystack_[0].value.as < AsmInsnCall * > ();
                }
#line 2491 "./insnsel/parser.cpp"
    break;

  case 141: // asmInsnArgs: CPAREN
#line 697 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       {
                  yylhs.value.as < AsmInsnCall * > () = new AsmInsnCall();
                }
#line 2499 "./insnsel/parser.cpp"
    break;

  case 142: // moreAsmInsnArgs: COMMA asmInsnArgs
#line 703 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                  { yylhs.value.as < AsmInsnCall * > () = yystack_[0].value.as < AsmInsnCall * > (); }
#line 2505 "./insnsel/parser.cpp"
    break;

  case 143: // moreAsmInsnArgs: CPAREN
#line 704 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       { yylhs.value.as < AsmInsnCall * > () = new AsmInsnCall(); }
#line 2511 "./insnsel/parser.cpp"
    break;

  case 144: // asmInsnArg: asmInsnExpr
#line 707 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                            { yylhs.value.as < AsmInsnArg * > () = static_cast<AsmInsnArg *>(new AsmInsnPositionalArg(std::move(yystack_[0].value.as < template_string > ()))); }
#line 2517 "./insnsel/parser.cpp"
    break;

  case 145: // asmInsnArg: ID EQUAL asmInsnExpr
#line 708 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                     {
                  yylhs.value.as < AsmInsnArg * > () = static_cast<AsmInsnArg *>(new AsmInsnNamedArg(yystack_[2].value.as < std::string > (), std::move(yystack_[0].value.as < template_string > ())));
                }
#line 2525 "./insnsel/parser.cpp"
    break;

  case 146: // asmInsnExpr: VARNAME
#line 713 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                        {
                    yylhs.value.as < template_string > () << yylhs.value.as < template_string > ().var(yystack_[0].value.as < std::string > ());
                }
#line 2533 "./insnsel/parser.cpp"
    break;

  case 147: // asmInsnExpr: STRING
#line 716 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                       {
                    template_string s(yystack_[0].value.as < std::string > ());
                    yylhs.value.as < template_string > () = std::move(s);
                }
#line 2542 "./insnsel/parser.cpp"
    break;

  case 148: // paramDecl: ID EQUAL literal SEMICOLON
#line 722 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                                           {
                    yylhs.value.as < ParamDecl * > () = new ParamDecl(yystack_[3].value.as < std::string > (), yystack_[1].value.as < Literal * > ());
                }
#line 2550 "./insnsel/parser.cpp"
    break;

  case 149: // literal: STRING
#line 727 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                        { yylhs.value.as < Literal * > () = new StringLiteral(yystack_[0].value.as < std::string > ()); }
#line 2556 "./insnsel/parser.cpp"
    break;

  case 150: // literal: NUMBER
#line 728 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                        { yylhs.value.as < Literal * > () = new NumberLiteral(yystack_[0].value.as < std::uint32_t > ()); }
#line 2562 "./insnsel/parser.cpp"
    break;

  case 151: // literal: ID
#line 729 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
                        { if ( yystack_[0].value.as < std::string > () == "true" ) yylhs.value.as < Literal * > () = new NumberLiteral(1);
                          else if ( yystack_[0].value.as < std::string > () == "false" ) yylhs.value.as < Literal * > () = new NumberLiteral(0);
                          else {
                            NncErrorLocation l(yystack_[0].location, "In the literal");
                            throw NncParseError("Invalid literal value " + yystack_[0].value.as < std::string > ());
                          }
                        }
#line 2574 "./insnsel/parser.cpp"
    break;


#line 2578 "./insnsel/parser.cpp"

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




  const short parser::yypact_ninf_ = -188;

  const signed char parser::yytable_ninf_ = -1;

  const short
  parser::yypact_[] =
  {
      38,  -188,   -15,    18,    37,    43,    54,    58,    60,    77,
      47,   107,    38,  -188,  -188,  -188,  -188,  -188,  -188,  -188,
    -188,  -188,    91,    92,    82,    99,   102,   103,   104,    97,
      65,  -188,  -188,  -188,  -188,   100,    -1,    -8,    32,    68,
      81,    34,  -188,  -188,  -188,   105,  -188,    86,   106,    88,
    -188,  -188,    -1,  -188,  -188,    -8,  -188,  -188,    90,  -188,
    -188,    32,   110,    95,    96,   115,    98,   108,  -188,  -188,
      68,  -188,  -188,  -188,   -13,  -188,    52,  -188,  -188,  -188,
     114,   111,   109,  -188,  -188,   112,   113,  -188,  -188,   116,
     117,  -188,   -14,   119,   120,    -9,   118,   121,  -188,   122,
      67,   126,   127,   123,  -188,  -188,    52,  -188,  -188,  -188,
    -188,   125,  -188,    86,  -188,  -188,    88,    90,  -188,   -14,
     -14,   128,   132,  -188,   129,   131,   130,  -188,  -188,  -188,
      40,  -188,  -188,   134,   101,   135,   143,  -188,    81,  -188,
      42,   133,   124,  -188,   139,   145,   138,  -188,  -188,  -188,
     137,   147,    51,  -188,   -14,  -188,   -14,  -188,  -188,   140,
      33,  -188,    -9,  -188,  -188,  -188,   146,  -188,    67,  -188,
    -188,  -188,  -188,  -188,   149,    69,   142,     8,   136,  -188,
     125,  -188,   130,   130,  -188,    79,  -188,  -188,  -188,  -188,
    -188,  -188,  -188,   148,  -188,   156,    70,  -188,  -188,  -188,
    -188,  -188,  -188,    42,  -188,   141,   153,   155,   157,   163,
    -188,  -188,   -14,   158,   -14,  -188,  -188,  -188,    11,    71,
     -19,  -188,  -188,    33,  -188,  -188,   154,   150,   150,   150,
      -7,   160,  -188,  -188,  -188,   151,  -188,  -188,  -188,    51,
    -188,  -188,  -188,   152,   150,   159,   162,   159,  -188,  -188,
      -7,  -188,  -188,  -188,   169,  -188,   170,  -188,   161,   171,
    -188,   172,  -188,   164,   173,  -188,  -188,  -188,  -188,   165,
     175,  -188,  -188
  };

  const unsigned char
  parser::yydefact_[] =
  {
       0,     3,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    13,    12,     7,     6,     8,     9,    11,
      10,     5,     0,     0,     0,     0,     0,     0,     0,    50,
       0,     1,     4,    21,    22,     0,     0,     0,     0,     0,
      51,     0,   149,   151,   150,     0,    18,     0,    19,     0,
      30,    23,     0,    32,    14,     0,    16,    17,     0,    25,
      24,     0,     0,     0,     0,     0,     0,     0,    94,    93,
       0,   103,    52,    53,     0,    49,     0,    58,    48,   148,
      37,     0,    44,    20,    47,     0,    46,    31,    15,    29,
       0,    26,     0,     0,     0,     0,     0,     0,    95,     0,
       0,     0,     0,     0,    61,    59,     0,    65,    64,    63,
      62,     0,    33,     0,    35,    34,     0,     0,    27,     0,
       0,     0,     0,   106,   109,   111,   117,   101,   102,   139,
     136,   133,   104,     0,     0,     0,     0,    57,    51,    54,
       0,     0,     0,    60,    42,     0,    40,    43,    45,    28,
       0,     0,     0,   100,     0,   107,   118,   116,   120,     0,
       0,   138,     0,    96,    97,    98,     0,   105,     0,    56,
      74,    75,    71,    76,     0,     0,     0,     0,     0,    36,
       0,    38,   117,   117,   122,     0,   121,   108,   119,   110,
     135,   141,   147,     0,   146,   136,     0,   144,   134,    99,
      55,    69,    73,     0,    70,     0,    80,    82,    84,    88,
      41,    39,   118,   114,   118,   124,   125,   126,     0,     0,
       0,   137,   143,     0,   140,    72,     0,     0,     0,     0,
       0,     0,   112,   115,   113,     0,   132,   130,   128,     0,
     123,   145,   142,     0,    87,    79,     0,    79,    90,    89,
       0,    92,    77,   131,     0,   127,     0,    86,     0,     0,
      83,     0,    91,     0,    68,    78,    81,    85,   129,     0,
       0,    67,    66
  };

  const short
  parser::yypgoto_[] =
  {
    -188,   144,  -188,  -188,    87,  -188,  -188,   -33,  -188,  -188,
    -188,  -188,   166,  -188,    26,   167,  -188,    94,  -188,    19,
    -188,  -188,  -188,    84,  -188,  -188,  -188,  -188,    72,    35,
    -188,   168,  -188,  -188,  -188,  -188,     2,  -188,  -188,  -188,
     -39,  -188,  -165,  -188,   -41,  -188,  -188,   174,  -188,  -188,
    -188,  -108,  -188,  -154,    28,    89,  -187,  -188,  -188,   -27,
    -188,  -188,  -188,  -188,   -50,    55,    21,  -188,    -5,  -188,
    -188,     0,   -36,  -188
  };

  const short
  parser::yydefgoto_[] =
  {
       0,    11,    12,    13,    54,    55,    14,    50,    15,    16,
      17,    18,    60,    61,    90,    51,    52,    81,    82,   145,
     181,   146,   114,    85,    86,    19,    41,    74,    75,   139,
      78,   105,   106,   107,   270,   108,   174,   204,   175,   109,
     259,   209,   245,   231,   249,   250,    20,    69,   166,    70,
     122,   123,   155,   124,   212,   213,   189,   125,   126,   186,
     218,   240,   219,   254,   237,   132,   161,   133,   195,   224,
     196,   197,    21,    45
  };

  const short
  parser::yytable_[] =
  {
      53,    57,   188,    71,    56,    59,    68,   119,    47,   120,
      99,   150,   151,    48,    48,   248,    53,   206,   207,    57,
     192,    48,    56,   194,    22,   232,   121,   234,    59,   100,
     129,   130,    10,    10,    71,    49,   235,    68,     1,    10,
     110,     2,     3,     4,     5,     6,   187,     7,   208,     8,
       9,   170,   171,   236,    48,    76,   159,    23,   188,   191,
     188,    77,   131,   246,   247,   160,   101,   102,   172,    58,
     110,   103,   192,   193,   104,   194,    24,   184,    10,   257,
      30,    62,   173,    25,    63,    64,    65,    66,    67,   153,
      48,   185,    10,   137,    26,   202,   222,   238,    27,   138,
      28,   203,   223,   239,    42,    43,    44,    31,    10,   163,
     164,   165,   215,   216,   217,    72,    73,    29,    33,    34,
      36,    35,    40,    37,    38,    39,    80,    46,    84,   131,
      89,    92,    79,    83,    93,    94,    95,   111,   112,   115,
      96,   113,    88,   149,   118,   116,   127,   128,   117,   134,
      97,   140,   141,   152,    48,   177,    32,   154,   156,   182,
     157,   162,   167,   135,   136,   142,   144,   168,   178,   179,
     180,   183,   159,   199,   205,   176,   201,   210,   227,   190,
     228,   220,   229,   226,   230,   233,   243,   252,   260,   269,
     244,   258,   256,   253,   251,   263,   264,   266,   267,   211,
     148,   265,   272,   200,   271,   225,   236,   147,   261,   262,
     169,   214,   255,   268,   251,   158,   221,   198,   242,    87,
     241,     0,     0,     0,     0,     0,     0,    91,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    98,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   143
  };

  const short
  parser::yycheck_[] =
  {
      36,    37,   156,    39,    37,    38,    39,    21,     9,    23,
      23,   119,   120,    22,    22,    22,    52,     9,    10,    55,
      39,    22,    55,    42,    39,   212,    40,   214,    61,    42,
      39,    40,    40,    40,    70,    36,    25,    70,     0,    40,
      76,     3,     4,     5,     6,     7,   154,     9,    40,    11,
      12,     9,    10,    42,    22,    21,    16,    39,   212,    26,
     214,    27,    95,   228,   229,    25,    14,    15,    26,    37,
     106,    19,    39,    40,    22,    42,    39,    26,    40,   244,
      33,    13,    40,    40,    16,    17,    18,    19,    20,   122,
      22,    40,    40,    26,    40,    26,    26,    26,    40,    32,
      40,    32,    32,    32,    39,    40,    41,     0,    40,     8,
       9,    10,    33,    34,    35,    34,    35,    40,    27,    27,
      21,    39,    25,    21,    21,    21,    40,    27,    40,   162,
      40,    21,    27,    27,    39,    39,    21,    23,    27,    27,
      42,    32,    55,   117,    27,    32,    27,    27,    32,    31,
      42,    25,    25,    25,    22,    31,    12,    28,    27,    22,
      30,    27,    27,    42,    42,    42,    41,    24,    29,    24,
      32,    24,    16,    27,    32,    42,    27,    41,    25,    39,
      25,    33,    25,    42,    21,    27,    32,    27,    26,    16,
      40,    32,    40,    42,   230,    26,    26,    26,    26,   180,
     116,    40,    27,   168,    39,   203,    42,   113,   247,   250,
     138,   183,   239,   263,   250,   126,   195,   162,   223,    52,
     220,    -1,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    70,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   106
  };

  const signed char
  parser::yystos_[] =
  {
       0,     0,     3,     4,     5,     6,     7,     9,    11,    12,
      40,    44,    45,    46,    49,    51,    52,    53,    54,    68,
      89,   115,    39,    39,    39,    40,    40,    40,    40,    40,
      33,     0,    44,    27,    27,    39,    21,    21,    21,    21,
      25,    69,    39,    40,    41,   116,    27,     9,    22,    36,
      50,    58,    59,   115,    47,    48,    50,   115,    37,    50,
      55,    56,    13,    16,    17,    18,    19,    20,    50,    90,
      92,   115,    34,    35,    70,    71,    21,    27,    73,    27,
      40,    60,    61,    27,    40,    66,    67,    58,    47,    40,
      57,    55,    21,    39,    39,    21,    42,    42,    90,    23,
      42,    14,    15,    19,    22,    74,    75,    76,    78,    82,
     115,    23,    27,    32,    65,    27,    32,    32,    27,    21,
      23,    40,    93,    94,    96,   100,   101,    27,    27,    39,
      40,    50,   108,   110,    31,    42,    42,    26,    32,    72,
      25,    25,    42,    74,    41,    62,    64,    60,    66,    57,
      94,    94,    25,    50,    28,    95,    27,    30,    98,    16,
      25,   109,    27,     8,     9,    10,    91,    27,    24,    71,
       9,    10,    26,    40,    79,    81,    42,    31,    29,    24,
      32,    63,    22,    24,    26,    40,   102,    94,    96,    99,
      39,    26,    39,    40,    42,   111,   113,   114,   108,    27,
      72,    27,    26,    32,    80,    32,     9,    10,    40,    84,
      41,    62,    97,    98,    97,    33,    34,    35,   103,   105,
      33,   109,    26,    32,   112,    79,    42,    25,    25,    25,
      21,    86,    99,    27,    99,    25,    42,   107,    26,    32,
     104,   114,   111,    32,    40,    85,    85,    85,    22,    87,
      88,   115,    27,    42,   106,   102,    40,    85,    32,    83,
      26,    83,    87,    26,    26,    40,    26,    26,   107,    16,
      77,    39,    27
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    43,    44,    44,    44,    45,    45,    45,    45,    45,
      45,    45,    45,    45,    46,    47,    47,    48,    49,    50,
      50,    51,    52,    53,    54,    55,    55,    56,    57,    57,
      58,    58,    59,    59,    59,    60,    61,    61,    62,    63,
      63,    64,    64,    65,    65,    66,    66,    67,    68,    69,
      69,    70,    70,    70,    71,    71,    72,    72,    73,    73,
      74,    74,    75,    75,    75,    75,    76,    77,    77,    78,
      79,    79,    80,    80,    81,    81,    81,    82,    83,    83,
      84,    84,    84,    84,    84,    84,    85,    85,    86,    86,
      87,    87,    88,    89,    90,    90,    91,    91,    91,    92,
      92,    92,    92,    92,    92,    92,    93,    94,    95,    95,
      96,    96,    96,    96,    97,    97,    98,    98,    99,    99,
     100,   101,   102,   102,   103,   103,   103,   104,   104,   105,
     105,   106,   107,   108,   108,   109,   109,   110,   110,   110,
     111,   111,   112,   112,   113,   113,   114,   114,   115,   116,
     116,   116
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     0,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     2,     1,     1,     4,     1,
       2,     3,     3,     4,     4,     1,     2,     3,     3,     1,
       1,     2,     1,     3,     3,     2,     4,     1,     2,     2,
       0,     3,     1,     2,     0,     3,     1,     1,     4,     2,
       0,     0,     1,     1,     3,     5,     2,     1,     1,     2,
       2,     1,     1,     1,     1,     1,    10,     2,     0,     4,
       2,     1,     2,     1,     1,     1,     1,     6,     2,     0,
       1,     5,     1,     4,     1,     5,     2,     1,     0,     2,
       1,     2,     1,     4,     1,     2,     1,     1,     1,     5,
       4,     3,     3,     1,     3,     4,     1,     2,     2,     0,
       3,     1,     5,     5,     1,     2,     1,     0,     0,     1,
       2,     3,     1,     3,     1,     1,     1,     2,     1,     5,
       2,     1,     1,     1,     3,     2,     0,     4,     2,     1,
       2,     1,     2,     1,     1,     3,     1,     1,     4,     1,
       1,     1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "EOF", "error", "\"invalid token\"", "CHEADER", "INCLUDE", "CODE",
  "REGCLASS", "CTYPE", "REGTYPE", "REGISTER", "CONSTANT", "INSNRULE",
  "INSN", "PATTERN", "CONSTRUCTOR", "INTERSECT", "IF", "CHECK", "ASM",
  "VAR", "ALIAS", "OBRACE", "CBRACE", "OBRACKET", "CBRACKET", "OPAREN",
  "CPAREN", "SEMICOLON", "BAR", "DASH", "QUESTIONMARK", "COLON", "COMMA",
  "EQUAL", "OUTPUT", "INOUT", "TYPE", "CLOBBERS", "WHITESPACE", "STRING",
  "ID", "NUMBER", "VARNAME", "$accept", "file", "decl", "ctypeDecl",
  "ctypeBody", "ctypeBodyDecl", "codeDecl", "cbraceSemi", "cheaderDecl",
  "includeDecl", "regclassDecl", "registerDecl", "registerBody",
  "regBodyDecl", "clobberRegisterList", "regclassBody", "regclassBodyDecl",
  "regclassRegDecl", "regclassRegDeclMember", "regclassRange",
  "regclassRangeMore", "regclassRangeElem", "regclassRegDeclMore",
  "regclassTypeDecl", "regtype", "insnDecl", "insnArgsDecl",
  "insnArgDeclDirection", "insnArgsDeclList", "moreInsnArgsDeclList",
  "insnBody", "insnBodyDecls", "insnBodyDecl", "intersectDecl",
  "postfixIfDecl", "constructorDecl", "constructorArgList",
  "moreConstructorArgs", "constructorArgTy", "insnVarDecl",
  "optionalRegClass", "insnVarTyDecl", "rtlType", "insnVarDeclBody",
  "insnVarDeclBodyDecls", "insnVarDeclBodyDecl", "insnRuleDecl",
  "insnRuleBody", "insnRuleVarType", "insnRuleBodyDecl", "patternsDecl",
  "patternsDisjunction", "morePatternsDisjunction", "patternsConjunction",
  "optionalPatternSemicolon", "optionalPattern", "morePatternsConjunction",
  "baseInsnPattern", "simpleInsnPattern", "insnArgs",
  "insnArgDirectionality", "moreInsnArgs", "insnArgPattern",
  "insnArgTypePattern", "insnArgExprPattern", "asmDecl", "asmConditional",
  "asmInsn", "asmInsnArgs", "moreAsmInsnArgs", "asmInsnArg", "asmInsnExpr",
  "paramDecl", "literal", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   178,   178,   179,   180,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   194,   199,   204,   207,   210,   216,
     217,   220,   225,   232,   239,   245,   246,   252,   256,   261,
     267,   268,   275,   276,   277,   280,   289,   294,   297,   305,
     307,   311,   320,   324,   325,   329,   334,   341,   345,   354,
     355,   359,   360,   361,   365,   370,   378,   379,   382,   383,
     386,   391,   394,   395,   396,   397,   400,   408,   411,   415,
     424,   428,   432,   433,   437,   438,   439,   446,   455,   456,
     459,   460,   467,   468,   473,   480,   493,   499,   505,   506,
     509,   510,   519,   522,   529,   530,   540,   541,   542,   546,
     551,   552,   553,   554,   555,   556,   561,   564,   570,   573,
     577,   580,   583,   589,   599,   600,   604,   605,   609,   610,
     614,   621,   626,   627,   634,   635,   636,   639,   640,   643,
     646,   651,   655,   659,   660,   667,   668,   671,   679,   685,
     691,   697,   703,   704,   707,   708,   713,   716,   722,   727,
     728,   729
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
#line 3238 "./insnsel/parser.cpp"

#line 738 "/home/tathougies/Projects/nnc/./insnsel/parser.y"


void nnc::parser::error(const location_type &l, const std::string &m) {
    throw NncParseError(l, m);
}

ArchDescLexer::~ArchDescLexer() {
}

nnc::parser::token_type ArchDescLexer::token_type(const nnc::position &offset) {
  std::uint64_t off(translate_position(offset));
  return token_type(off);
}
