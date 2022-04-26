// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file ./insnsel/parser.hpp
 ** Define the nnc::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_INSNSEL_PARSER_HPP_INCLUDED
# define YY_YY_INSNSEL_PARSER_HPP_INCLUDED
// "%code requires" blocks.
#line 116 "/home/tathougies/Projects/nnc/./insnsel/parser.y"

    #include "ast.hpp"
    #include "archdesc.hpp"
    #include "error.hpp"
    #include "templatestring.hpp"
    #include "insn.hpp"
    class ArchDescLexer;

#line 58 "./insnsel/parser.hpp"


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif
# include "location.hh"


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

#line 10 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
namespace nnc {
#line 194 "./insnsel/parser.hpp"




  /// A Bison parser.
  class parser
  {
  public:
#ifdef YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define YYSTYPE in C++, use %define api.value.type"
# endif
    typedef YYSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
    {
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {}

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // asmInsnArg
      char dummy1[sizeof (AsmInsnArg *)];

      // asmInsn
      // asmInsnArgs
      // moreAsmInsnArgs
      char dummy2[sizeof (AsmInsnCall *)];

      // ctypeBody
      char dummy3[sizeof (CType *)];

      // ctypeBodyDecl
      char dummy4[sizeof (CTypeModifier *)];

      // clobberRegisterList
      char dummy5[sizeof (ClobberListDecl *)];

      // constructorDecl
      // constructorArgList
      // moreConstructorArgs
      char dummy6[sizeof (ConstructorDecl *)];

      // insnDecl
      char dummy7[sizeof (Insn *)];

      // insnArgDeclDirection
      char dummy8[sizeof (InsnArgDecl::Direction)];

      // insnArgPattern
      char dummy9[sizeof (InsnArgPattern)];

      // insnArgDirectionality
      char dummy10[sizeof (InsnArgPattern::Directionality)];

      // insnArgs
      // moreInsnArgs
      char dummy11[sizeof (InsnArgPatterns *)];

      // insnArgsDecl
      // insnArgsDeclList
      // moreInsnArgsDeclList
      char dummy12[sizeof (InsnArgsModifier *)];

      // insnBody
      // insnBodyDecls
      char dummy13[sizeof (InsnFactory *)];

      // patternsDisjunction
      // morePatternsDisjunction
      // patternsConjunction
      // morePatternsConjunction
      // baseInsnPattern
      char dummy14[sizeof (InsnGen *)];

      // insnBodyDecl
      char dummy15[sizeof (InsnModifier *)];

      // simpleInsnPattern
      char dummy16[sizeof (InsnPattern *)];

      // rtlType
      char dummy17[sizeof (InsnRtlType)];

      // insnRuleDecl
      char dummy18[sizeof (InsnRule *)];

      // asmDecl
      char dummy19[sizeof (InsnRuleAsmDecl *)];

      // insnRuleBody
      char dummy20[sizeof (InsnRuleFactory *)];

      // insnRuleBodyDecl
      // patternsDecl
      char dummy21[sizeof (InsnRuleModifier *)];

      // insnVarDecl
      char dummy22[sizeof (InsnVarDecl *)];

      // insnVarDeclBody
      // insnVarDeclBodyDecls
      char dummy23[sizeof (InsnVarDeclFactory *)];

      // insnVarDeclBodyDecl
      char dummy24[sizeof (InsnVarDeclModifier *)];

      // constructorArgTy
      // insnVarTyDecl
      char dummy25[sizeof (InsnVarType *)];

      // insnRuleVarType
      char dummy26[sizeof (InsnVariable::Type)];

      // literal
      char dummy27[sizeof (Literal *)];

      // paramDecl
      char dummy28[sizeof (ParamDecl *)];

      // regclassBody
      char dummy29[sizeof (RegClassFactory *)];

      // regclassBodyDecl
      char dummy30[sizeof (RegClassModifier *)];

      // regclassTypeDecl
      char dummy31[sizeof (RegClassTypeDecl *)];

      // regclassRegDecl
      char dummy32[sizeof (RegMemberDecl *)];

      // regtype
      char dummy33[sizeof (RegType)];

      // registerBody
      char dummy34[sizeof (RegisterFactory *)];

      // regBodyDecl
      char dummy35[sizeof (RegisterModifier *)];

      // optionalPattern
      char dummy36[sizeof (bool)];

      // STRING
      // ID
      // VARNAME
      // insnArgTypePattern
      // insnArgExprPattern
      char dummy37[sizeof (std::string)];

      // NUMBER
      char dummy38[sizeof (std::uint32_t)];

      // asmInsnExpr
      char dummy39[sizeof (template_string)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        TOK_YYEMPTY = -2,
    TOK_EOF = 0,                   // EOF
    TOK_YYerror = 256,             // error
    TOK_YYUNDEF = 257,             // "invalid token"
    TOK_ARCHITECTURE = 258,        // ARCHITECTURE
    TOK_CHEADER = 259,             // CHEADER
    TOK_INCLUDE = 260,             // INCLUDE
    TOK_CODE = 261,                // CODE
    TOK_REGCLASS = 262,            // REGCLASS
    TOK_CTYPE = 263,               // CTYPE
    TOK_REGTYPE = 264,             // REGTYPE
    TOK_REGISTER = 265,            // REGISTER
    TOK_CONSTANT = 266,            // CONSTANT
    TOK_INSNRULE = 267,            // INSNRULE
    TOK_INSN = 268,                // INSN
    TOK_PATTERN = 269,             // PATTERN
    TOK_CONSTRUCTOR = 270,         // CONSTRUCTOR
    TOK_IF = 271,                  // IF
    TOK_CHECK = 272,               // CHECK
    TOK_ASM = 273,                 // ASM
    TOK_VAR = 274,                 // VAR
    TOK_ALIAS = 275,               // ALIAS
    TOK_OBRACE = 276,              // OBRACE
    TOK_CBRACE = 277,              // CBRACE
    TOK_OBRACKET = 278,            // OBRACKET
    TOK_CBRACKET = 279,            // CBRACKET
    TOK_OPAREN = 280,              // OPAREN
    TOK_CPAREN = 281,              // CPAREN
    TOK_SEMICOLON = 282,           // SEMICOLON
    TOK_BAR = 283,                 // BAR
    TOK_QUESTIONMARK = 284,        // QUESTIONMARK
    TOK_COLON = 285,               // COLON
    TOK_COMMA = 286,               // COMMA
    TOK_EQUAL = 287,               // EQUAL
    TOK_OUTPUT = 288,              // OUTPUT
    TOK_INOUT = 289,               // INOUT
    TOK_TYPE = 290,                // TYPE
    TOK_CLOBBERS = 291,            // CLOBBERS
    TOK_WHITESPACE = 292,          // WHITESPACE
    TOK_STRING = 293,              // STRING
    TOK_ID = 294,                  // ID
    TOK_NUMBER = 295,              // NUMBER
    TOK_VARNAME = 296              // VARNAME
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 42, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // EOF
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_ARCHITECTURE = 3,                      // ARCHITECTURE
        S_CHEADER = 4,                           // CHEADER
        S_INCLUDE = 5,                           // INCLUDE
        S_CODE = 6,                              // CODE
        S_REGCLASS = 7,                          // REGCLASS
        S_CTYPE = 8,                             // CTYPE
        S_REGTYPE = 9,                           // REGTYPE
        S_REGISTER = 10,                         // REGISTER
        S_CONSTANT = 11,                         // CONSTANT
        S_INSNRULE = 12,                         // INSNRULE
        S_INSN = 13,                             // INSN
        S_PATTERN = 14,                          // PATTERN
        S_CONSTRUCTOR = 15,                      // CONSTRUCTOR
        S_IF = 16,                               // IF
        S_CHECK = 17,                            // CHECK
        S_ASM = 18,                              // ASM
        S_VAR = 19,                              // VAR
        S_ALIAS = 20,                            // ALIAS
        S_OBRACE = 21,                           // OBRACE
        S_CBRACE = 22,                           // CBRACE
        S_OBRACKET = 23,                         // OBRACKET
        S_CBRACKET = 24,                         // CBRACKET
        S_OPAREN = 25,                           // OPAREN
        S_CPAREN = 26,                           // CPAREN
        S_SEMICOLON = 27,                        // SEMICOLON
        S_BAR = 28,                              // BAR
        S_QUESTIONMARK = 29,                     // QUESTIONMARK
        S_COLON = 30,                            // COLON
        S_COMMA = 31,                            // COMMA
        S_EQUAL = 32,                            // EQUAL
        S_OUTPUT = 33,                           // OUTPUT
        S_INOUT = 34,                            // INOUT
        S_TYPE = 35,                             // TYPE
        S_CLOBBERS = 36,                         // CLOBBERS
        S_WHITESPACE = 37,                       // WHITESPACE
        S_STRING = 38,                           // STRING
        S_ID = 39,                               // ID
        S_NUMBER = 40,                           // NUMBER
        S_VARNAME = 41,                          // VARNAME
        S_YYACCEPT = 42,                         // $accept
        S_file = 43,                             // file
        S_decl = 44,                             // decl
        S_ctypeDecl = 45,                        // ctypeDecl
        S_ctypeBody = 46,                        // ctypeBody
        S_ctypeBodyDecl = 47,                    // ctypeBodyDecl
        S_codeDecl = 48,                         // codeDecl
        S_cbraceSemi = 49,                       // cbraceSemi
        S_cheaderDecl = 50,                      // cheaderDecl
        S_architectureDecl = 51,                 // architectureDecl
        S_includeDecl = 52,                      // includeDecl
        S_regclassDecl = 53,                     // regclassDecl
        S_registerDecl = 54,                     // registerDecl
        S_registerBody = 55,                     // registerBody
        S_regBodyDecl = 56,                      // regBodyDecl
        S_clobberRegisterList = 57,              // clobberRegisterList
        S_regclassBody = 58,                     // regclassBody
        S_regclassBodyDecl = 59,                 // regclassBodyDecl
        S_regclassRegDecl = 60,                  // regclassRegDecl
        S_regclassTypeDecl = 61,                 // regclassTypeDecl
        S_regtype = 62,                          // regtype
        S_insnDecl = 63,                         // insnDecl
        S_insnArgsDecl = 64,                     // insnArgsDecl
        S_insnArgDeclDirection = 65,             // insnArgDeclDirection
        S_insnArgsDeclList = 66,                 // insnArgsDeclList
        S_moreInsnArgsDeclList = 67,             // moreInsnArgsDeclList
        S_insnBody = 68,                         // insnBody
        S_insnBodyDecls = 69,                    // insnBodyDecls
        S_insnBodyDecl = 70,                     // insnBodyDecl
        S_constructorDecl = 71,                  // constructorDecl
        S_constructorArgList = 72,               // constructorArgList
        S_moreConstructorArgs = 73,              // moreConstructorArgs
        S_constructorArgTy = 74,                 // constructorArgTy
        S_insnVarDecl = 75,                      // insnVarDecl
        S_insnVarTyDecl = 76,                    // insnVarTyDecl
        S_rtlType = 77,                          // rtlType
        S_insnVarDeclBody = 78,                  // insnVarDeclBody
        S_insnVarDeclBodyDecls = 79,             // insnVarDeclBodyDecls
        S_insnVarDeclBodyDecl = 80,              // insnVarDeclBodyDecl
        S_insnRuleDecl = 81,                     // insnRuleDecl
        S_insnRuleBody = 82,                     // insnRuleBody
        S_insnRuleVarType = 83,                  // insnRuleVarType
        S_insnRuleBodyDecl = 84,                 // insnRuleBodyDecl
        S_patternsDecl = 85,                     // patternsDecl
        S_patternsDisjunction = 86,              // patternsDisjunction
        S_morePatternsDisjunction = 87,          // morePatternsDisjunction
        S_patternsConjunction = 88,              // patternsConjunction
        S_optionalPattern = 89,                  // optionalPattern
        S_morePatternsConjunction = 90,          // morePatternsConjunction
        S_baseInsnPattern = 91,                  // baseInsnPattern
        S_simpleInsnPattern = 92,                // simpleInsnPattern
        S_insnArgs = 93,                         // insnArgs
        S_insnArgDirectionality = 94,            // insnArgDirectionality
        S_moreInsnArgs = 95,                     // moreInsnArgs
        S_insnArgPattern = 96,                   // insnArgPattern
        S_insnArgTypePattern = 97,               // insnArgTypePattern
        S_insnArgExprPattern = 98,               // insnArgExprPattern
        S_asmDecl = 99,                          // asmDecl
        S_asmInsn = 100,                         // asmInsn
        S_asmInsnArgs = 101,                     // asmInsnArgs
        S_moreAsmInsnArgs = 102,                 // moreAsmInsnArgs
        S_asmInsnArg = 103,                      // asmInsnArg
        S_asmInsnExpr = 104,                     // asmInsnExpr
        S_paramDecl = 105,                       // paramDecl
        S_literal = 106                          // literal
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value ()
        , location (std::move (that.location))
      {
        switch (this->kind ())
    {
      case symbol_kind::S_asmInsnArg: // asmInsnArg
        value.move< AsmInsnArg * > (std::move (that.value));
        break;

      case symbol_kind::S_asmInsn: // asmInsn
      case symbol_kind::S_asmInsnArgs: // asmInsnArgs
      case symbol_kind::S_moreAsmInsnArgs: // moreAsmInsnArgs
        value.move< AsmInsnCall * > (std::move (that.value));
        break;

      case symbol_kind::S_ctypeBody: // ctypeBody
        value.move< CType * > (std::move (that.value));
        break;

      case symbol_kind::S_ctypeBodyDecl: // ctypeBodyDecl
        value.move< CTypeModifier * > (std::move (that.value));
        break;

      case symbol_kind::S_clobberRegisterList: // clobberRegisterList
        value.move< ClobberListDecl * > (std::move (that.value));
        break;

      case symbol_kind::S_constructorDecl: // constructorDecl
      case symbol_kind::S_constructorArgList: // constructorArgList
      case symbol_kind::S_moreConstructorArgs: // moreConstructorArgs
        value.move< ConstructorDecl * > (std::move (that.value));
        break;

      case symbol_kind::S_insnDecl: // insnDecl
        value.move< Insn * > (std::move (that.value));
        break;

      case symbol_kind::S_insnArgDeclDirection: // insnArgDeclDirection
        value.move< InsnArgDecl::Direction > (std::move (that.value));
        break;

      case symbol_kind::S_insnArgPattern: // insnArgPattern
        value.move< InsnArgPattern > (std::move (that.value));
        break;

      case symbol_kind::S_insnArgDirectionality: // insnArgDirectionality
        value.move< InsnArgPattern::Directionality > (std::move (that.value));
        break;

      case symbol_kind::S_insnArgs: // insnArgs
      case symbol_kind::S_moreInsnArgs: // moreInsnArgs
        value.move< InsnArgPatterns * > (std::move (that.value));
        break;

      case symbol_kind::S_insnArgsDecl: // insnArgsDecl
      case symbol_kind::S_insnArgsDeclList: // insnArgsDeclList
      case symbol_kind::S_moreInsnArgsDeclList: // moreInsnArgsDeclList
        value.move< InsnArgsModifier * > (std::move (that.value));
        break;

      case symbol_kind::S_insnBody: // insnBody
      case symbol_kind::S_insnBodyDecls: // insnBodyDecls
        value.move< InsnFactory * > (std::move (that.value));
        break;

      case symbol_kind::S_patternsDisjunction: // patternsDisjunction
      case symbol_kind::S_morePatternsDisjunction: // morePatternsDisjunction
      case symbol_kind::S_patternsConjunction: // patternsConjunction
      case symbol_kind::S_morePatternsConjunction: // morePatternsConjunction
      case symbol_kind::S_baseInsnPattern: // baseInsnPattern
        value.move< InsnGen * > (std::move (that.value));
        break;

      case symbol_kind::S_insnBodyDecl: // insnBodyDecl
        value.move< InsnModifier * > (std::move (that.value));
        break;

      case symbol_kind::S_simpleInsnPattern: // simpleInsnPattern
        value.move< InsnPattern * > (std::move (that.value));
        break;

      case symbol_kind::S_rtlType: // rtlType
        value.move< InsnRtlType > (std::move (that.value));
        break;

      case symbol_kind::S_insnRuleDecl: // insnRuleDecl
        value.move< InsnRule * > (std::move (that.value));
        break;

      case symbol_kind::S_asmDecl: // asmDecl
        value.move< InsnRuleAsmDecl * > (std::move (that.value));
        break;

      case symbol_kind::S_insnRuleBody: // insnRuleBody
        value.move< InsnRuleFactory * > (std::move (that.value));
        break;

      case symbol_kind::S_insnRuleBodyDecl: // insnRuleBodyDecl
      case symbol_kind::S_patternsDecl: // patternsDecl
        value.move< InsnRuleModifier * > (std::move (that.value));
        break;

      case symbol_kind::S_insnVarDecl: // insnVarDecl
        value.move< InsnVarDecl * > (std::move (that.value));
        break;

      case symbol_kind::S_insnVarDeclBody: // insnVarDeclBody
      case symbol_kind::S_insnVarDeclBodyDecls: // insnVarDeclBodyDecls
        value.move< InsnVarDeclFactory * > (std::move (that.value));
        break;

      case symbol_kind::S_insnVarDeclBodyDecl: // insnVarDeclBodyDecl
        value.move< InsnVarDeclModifier * > (std::move (that.value));
        break;

      case symbol_kind::S_constructorArgTy: // constructorArgTy
      case symbol_kind::S_insnVarTyDecl: // insnVarTyDecl
        value.move< InsnVarType * > (std::move (that.value));
        break;

      case symbol_kind::S_insnRuleVarType: // insnRuleVarType
        value.move< InsnVariable::Type > (std::move (that.value));
        break;

      case symbol_kind::S_literal: // literal
        value.move< Literal * > (std::move (that.value));
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        value.move< ParamDecl * > (std::move (that.value));
        break;

      case symbol_kind::S_regclassBody: // regclassBody
        value.move< RegClassFactory * > (std::move (that.value));
        break;

      case symbol_kind::S_regclassBodyDecl: // regclassBodyDecl
        value.move< RegClassModifier * > (std::move (that.value));
        break;

      case symbol_kind::S_regclassTypeDecl: // regclassTypeDecl
        value.move< RegClassTypeDecl * > (std::move (that.value));
        break;

      case symbol_kind::S_regclassRegDecl: // regclassRegDecl
        value.move< RegMemberDecl * > (std::move (that.value));
        break;

      case symbol_kind::S_regtype: // regtype
        value.move< RegType > (std::move (that.value));
        break;

      case symbol_kind::S_registerBody: // registerBody
        value.move< RegisterFactory * > (std::move (that.value));
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        value.move< RegisterModifier * > (std::move (that.value));
        break;

      case symbol_kind::S_optionalPattern: // optionalPattern
        value.move< bool > (std::move (that.value));
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_insnArgTypePattern: // insnArgTypePattern
      case symbol_kind::S_insnArgExprPattern: // insnArgExprPattern
        value.move< std::string > (std::move (that.value));
        break;

      case symbol_kind::S_NUMBER: // NUMBER
        value.move< std::uint32_t > (std::move (that.value));
        break;

      case symbol_kind::S_asmInsnExpr: // asmInsnExpr
        value.move< template_string > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AsmInsnArg *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AsmInsnArg *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, AsmInsnCall *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const AsmInsnCall *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, CType *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const CType *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, CTypeModifier *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const CTypeModifier *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ClobberListDecl *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ClobberListDecl *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ConstructorDecl *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ConstructorDecl *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, Insn *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const Insn *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnArgDecl::Direction&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnArgDecl::Direction& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnArgPattern&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnArgPattern& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnArgPattern::Directionality&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnArgPattern::Directionality& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnArgPatterns *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnArgPatterns *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnArgsModifier *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnArgsModifier *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnFactory *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnFactory *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnGen *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnGen *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnModifier *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnModifier *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnPattern *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnPattern *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnRtlType&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnRtlType& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnRule *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnRule *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnRuleAsmDecl *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnRuleAsmDecl *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnRuleFactory *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnRuleFactory *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnRuleModifier *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnRuleModifier *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnVarDecl *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnVarDecl *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnVarDeclFactory *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnVarDeclFactory *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnVarDeclModifier *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnVarDeclModifier *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnVarType *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnVarType *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, InsnVariable::Type&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const InsnVariable::Type& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, Literal *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const Literal *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, ParamDecl *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const ParamDecl *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, RegClassFactory *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const RegClassFactory *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, RegClassModifier *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const RegClassModifier *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, RegClassTypeDecl *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const RegClassTypeDecl *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, RegMemberDecl *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const RegMemberDecl *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, RegType&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const RegType& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, RegisterFactory *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const RegisterFactory *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, RegisterModifier *&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const RegisterModifier *& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, bool&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const bool& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::string&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::string& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::uint32_t&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::uint32_t& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, template_string&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const template_string& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
    {
      case symbol_kind::S_asmInsnArg: // asmInsnArg
        value.template destroy< AsmInsnArg * > ();
        break;

      case symbol_kind::S_asmInsn: // asmInsn
      case symbol_kind::S_asmInsnArgs: // asmInsnArgs
      case symbol_kind::S_moreAsmInsnArgs: // moreAsmInsnArgs
        value.template destroy< AsmInsnCall * > ();
        break;

      case symbol_kind::S_ctypeBody: // ctypeBody
        value.template destroy< CType * > ();
        break;

      case symbol_kind::S_ctypeBodyDecl: // ctypeBodyDecl
        value.template destroy< CTypeModifier * > ();
        break;

      case symbol_kind::S_clobberRegisterList: // clobberRegisterList
        value.template destroy< ClobberListDecl * > ();
        break;

      case symbol_kind::S_constructorDecl: // constructorDecl
      case symbol_kind::S_constructorArgList: // constructorArgList
      case symbol_kind::S_moreConstructorArgs: // moreConstructorArgs
        value.template destroy< ConstructorDecl * > ();
        break;

      case symbol_kind::S_insnDecl: // insnDecl
        value.template destroy< Insn * > ();
        break;

      case symbol_kind::S_insnArgDeclDirection: // insnArgDeclDirection
        value.template destroy< InsnArgDecl::Direction > ();
        break;

      case symbol_kind::S_insnArgPattern: // insnArgPattern
        value.template destroy< InsnArgPattern > ();
        break;

      case symbol_kind::S_insnArgDirectionality: // insnArgDirectionality
        value.template destroy< InsnArgPattern::Directionality > ();
        break;

      case symbol_kind::S_insnArgs: // insnArgs
      case symbol_kind::S_moreInsnArgs: // moreInsnArgs
        value.template destroy< InsnArgPatterns * > ();
        break;

      case symbol_kind::S_insnArgsDecl: // insnArgsDecl
      case symbol_kind::S_insnArgsDeclList: // insnArgsDeclList
      case symbol_kind::S_moreInsnArgsDeclList: // moreInsnArgsDeclList
        value.template destroy< InsnArgsModifier * > ();
        break;

      case symbol_kind::S_insnBody: // insnBody
      case symbol_kind::S_insnBodyDecls: // insnBodyDecls
        value.template destroy< InsnFactory * > ();
        break;

      case symbol_kind::S_patternsDisjunction: // patternsDisjunction
      case symbol_kind::S_morePatternsDisjunction: // morePatternsDisjunction
      case symbol_kind::S_patternsConjunction: // patternsConjunction
      case symbol_kind::S_morePatternsConjunction: // morePatternsConjunction
      case symbol_kind::S_baseInsnPattern: // baseInsnPattern
        value.template destroy< InsnGen * > ();
        break;

      case symbol_kind::S_insnBodyDecl: // insnBodyDecl
        value.template destroy< InsnModifier * > ();
        break;

      case symbol_kind::S_simpleInsnPattern: // simpleInsnPattern
        value.template destroy< InsnPattern * > ();
        break;

      case symbol_kind::S_rtlType: // rtlType
        value.template destroy< InsnRtlType > ();
        break;

      case symbol_kind::S_insnRuleDecl: // insnRuleDecl
        value.template destroy< InsnRule * > ();
        break;

      case symbol_kind::S_asmDecl: // asmDecl
        value.template destroy< InsnRuleAsmDecl * > ();
        break;

      case symbol_kind::S_insnRuleBody: // insnRuleBody
        value.template destroy< InsnRuleFactory * > ();
        break;

      case symbol_kind::S_insnRuleBodyDecl: // insnRuleBodyDecl
      case symbol_kind::S_patternsDecl: // patternsDecl
        value.template destroy< InsnRuleModifier * > ();
        break;

      case symbol_kind::S_insnVarDecl: // insnVarDecl
        value.template destroy< InsnVarDecl * > ();
        break;

      case symbol_kind::S_insnVarDeclBody: // insnVarDeclBody
      case symbol_kind::S_insnVarDeclBodyDecls: // insnVarDeclBodyDecls
        value.template destroy< InsnVarDeclFactory * > ();
        break;

      case symbol_kind::S_insnVarDeclBodyDecl: // insnVarDeclBodyDecl
        value.template destroy< InsnVarDeclModifier * > ();
        break;

      case symbol_kind::S_constructorArgTy: // constructorArgTy
      case symbol_kind::S_insnVarTyDecl: // insnVarTyDecl
        value.template destroy< InsnVarType * > ();
        break;

      case symbol_kind::S_insnRuleVarType: // insnRuleVarType
        value.template destroy< InsnVariable::Type > ();
        break;

      case symbol_kind::S_literal: // literal
        value.template destroy< Literal * > ();
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        value.template destroy< ParamDecl * > ();
        break;

      case symbol_kind::S_regclassBody: // regclassBody
        value.template destroy< RegClassFactory * > ();
        break;

      case symbol_kind::S_regclassBodyDecl: // regclassBodyDecl
        value.template destroy< RegClassModifier * > ();
        break;

      case symbol_kind::S_regclassTypeDecl: // regclassTypeDecl
        value.template destroy< RegClassTypeDecl * > ();
        break;

      case symbol_kind::S_regclassRegDecl: // regclassRegDecl
        value.template destroy< RegMemberDecl * > ();
        break;

      case symbol_kind::S_regtype: // regtype
        value.template destroy< RegType > ();
        break;

      case symbol_kind::S_registerBody: // registerBody
        value.template destroy< RegisterFactory * > ();
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        value.template destroy< RegisterModifier * > ();
        break;

      case symbol_kind::S_optionalPattern: // optionalPattern
        value.template destroy< bool > ();
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_insnArgTypePattern: // insnArgTypePattern
      case symbol_kind::S_insnArgExprPattern: // insnArgExprPattern
        value.template destroy< std::string > ();
        break;

      case symbol_kind::S_NUMBER: // NUMBER
        value.template destroy< std::uint32_t > ();
        break;

      case symbol_kind::S_asmInsnExpr: // asmInsnExpr
        value.template destroy< template_string > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

#if YYDEBUG || 0
      /// The user-facing name of this symbol.
      const char *name () const YY_NOEXCEPT
      {
        return parser::symbol_name (this->kind ());
      }
#endif // #if YYDEBUG || 0


      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, std::string v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const std::string& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, std::uint32_t v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const std::uint32_t& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
    };

    /// Build a parser object.
    parser (ArchDescLexer &lexer_yyarg, ArchDescBuilder &builder_yyarg);
    virtual ~parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    parser (const parser&) = delete;
    /// Non copyable.
    parser& operator= (const parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

#if YYDEBUG || 0
    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static const char *symbol_name (symbol_kind_type yysymbol);
#endif // #if YYDEBUG || 0


    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EOF (location_type l)
      {
        return symbol_type (token::TOK_EOF, std::move (l));
      }
#else
      static
      symbol_type
      make_EOF (const location_type& l)
      {
        return symbol_type (token::TOK_EOF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYerror (location_type l)
      {
        return symbol_type (token::TOK_YYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_YYerror (const location_type& l)
      {
        return symbol_type (token::TOK_YYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYUNDEF (location_type l)
      {
        return symbol_type (token::TOK_YYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYUNDEF (const location_type& l)
      {
        return symbol_type (token::TOK_YYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ARCHITECTURE (location_type l)
      {
        return symbol_type (token::TOK_ARCHITECTURE, std::move (l));
      }
#else
      static
      symbol_type
      make_ARCHITECTURE (const location_type& l)
      {
        return symbol_type (token::TOK_ARCHITECTURE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CHEADER (location_type l)
      {
        return symbol_type (token::TOK_CHEADER, std::move (l));
      }
#else
      static
      symbol_type
      make_CHEADER (const location_type& l)
      {
        return symbol_type (token::TOK_CHEADER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INCLUDE (location_type l)
      {
        return symbol_type (token::TOK_INCLUDE, std::move (l));
      }
#else
      static
      symbol_type
      make_INCLUDE (const location_type& l)
      {
        return symbol_type (token::TOK_INCLUDE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CODE (location_type l)
      {
        return symbol_type (token::TOK_CODE, std::move (l));
      }
#else
      static
      symbol_type
      make_CODE (const location_type& l)
      {
        return symbol_type (token::TOK_CODE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_REGCLASS (location_type l)
      {
        return symbol_type (token::TOK_REGCLASS, std::move (l));
      }
#else
      static
      symbol_type
      make_REGCLASS (const location_type& l)
      {
        return symbol_type (token::TOK_REGCLASS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CTYPE (location_type l)
      {
        return symbol_type (token::TOK_CTYPE, std::move (l));
      }
#else
      static
      symbol_type
      make_CTYPE (const location_type& l)
      {
        return symbol_type (token::TOK_CTYPE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_REGTYPE (location_type l)
      {
        return symbol_type (token::TOK_REGTYPE, std::move (l));
      }
#else
      static
      symbol_type
      make_REGTYPE (const location_type& l)
      {
        return symbol_type (token::TOK_REGTYPE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_REGISTER (location_type l)
      {
        return symbol_type (token::TOK_REGISTER, std::move (l));
      }
#else
      static
      symbol_type
      make_REGISTER (const location_type& l)
      {
        return symbol_type (token::TOK_REGISTER, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONSTANT (location_type l)
      {
        return symbol_type (token::TOK_CONSTANT, std::move (l));
      }
#else
      static
      symbol_type
      make_CONSTANT (const location_type& l)
      {
        return symbol_type (token::TOK_CONSTANT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INSNRULE (location_type l)
      {
        return symbol_type (token::TOK_INSNRULE, std::move (l));
      }
#else
      static
      symbol_type
      make_INSNRULE (const location_type& l)
      {
        return symbol_type (token::TOK_INSNRULE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INSN (location_type l)
      {
        return symbol_type (token::TOK_INSN, std::move (l));
      }
#else
      static
      symbol_type
      make_INSN (const location_type& l)
      {
        return symbol_type (token::TOK_INSN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PATTERN (location_type l)
      {
        return symbol_type (token::TOK_PATTERN, std::move (l));
      }
#else
      static
      symbol_type
      make_PATTERN (const location_type& l)
      {
        return symbol_type (token::TOK_PATTERN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONSTRUCTOR (location_type l)
      {
        return symbol_type (token::TOK_CONSTRUCTOR, std::move (l));
      }
#else
      static
      symbol_type
      make_CONSTRUCTOR (const location_type& l)
      {
        return symbol_type (token::TOK_CONSTRUCTOR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IF (location_type l)
      {
        return symbol_type (token::TOK_IF, std::move (l));
      }
#else
      static
      symbol_type
      make_IF (const location_type& l)
      {
        return symbol_type (token::TOK_IF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CHECK (location_type l)
      {
        return symbol_type (token::TOK_CHECK, std::move (l));
      }
#else
      static
      symbol_type
      make_CHECK (const location_type& l)
      {
        return symbol_type (token::TOK_CHECK, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ASM (location_type l)
      {
        return symbol_type (token::TOK_ASM, std::move (l));
      }
#else
      static
      symbol_type
      make_ASM (const location_type& l)
      {
        return symbol_type (token::TOK_ASM, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VAR (location_type l)
      {
        return symbol_type (token::TOK_VAR, std::move (l));
      }
#else
      static
      symbol_type
      make_VAR (const location_type& l)
      {
        return symbol_type (token::TOK_VAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ALIAS (location_type l)
      {
        return symbol_type (token::TOK_ALIAS, std::move (l));
      }
#else
      static
      symbol_type
      make_ALIAS (const location_type& l)
      {
        return symbol_type (token::TOK_ALIAS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OBRACE (location_type l)
      {
        return symbol_type (token::TOK_OBRACE, std::move (l));
      }
#else
      static
      symbol_type
      make_OBRACE (const location_type& l)
      {
        return symbol_type (token::TOK_OBRACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CBRACE (location_type l)
      {
        return symbol_type (token::TOK_CBRACE, std::move (l));
      }
#else
      static
      symbol_type
      make_CBRACE (const location_type& l)
      {
        return symbol_type (token::TOK_CBRACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OBRACKET (location_type l)
      {
        return symbol_type (token::TOK_OBRACKET, std::move (l));
      }
#else
      static
      symbol_type
      make_OBRACKET (const location_type& l)
      {
        return symbol_type (token::TOK_OBRACKET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CBRACKET (location_type l)
      {
        return symbol_type (token::TOK_CBRACKET, std::move (l));
      }
#else
      static
      symbol_type
      make_CBRACKET (const location_type& l)
      {
        return symbol_type (token::TOK_CBRACKET, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OPAREN (location_type l)
      {
        return symbol_type (token::TOK_OPAREN, std::move (l));
      }
#else
      static
      symbol_type
      make_OPAREN (const location_type& l)
      {
        return symbol_type (token::TOK_OPAREN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CPAREN (location_type l)
      {
        return symbol_type (token::TOK_CPAREN, std::move (l));
      }
#else
      static
      symbol_type
      make_CPAREN (const location_type& l)
      {
        return symbol_type (token::TOK_CPAREN, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SEMICOLON (location_type l)
      {
        return symbol_type (token::TOK_SEMICOLON, std::move (l));
      }
#else
      static
      symbol_type
      make_SEMICOLON (const location_type& l)
      {
        return symbol_type (token::TOK_SEMICOLON, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BAR (location_type l)
      {
        return symbol_type (token::TOK_BAR, std::move (l));
      }
#else
      static
      symbol_type
      make_BAR (const location_type& l)
      {
        return symbol_type (token::TOK_BAR, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_QUESTIONMARK (location_type l)
      {
        return symbol_type (token::TOK_QUESTIONMARK, std::move (l));
      }
#else
      static
      symbol_type
      make_QUESTIONMARK (const location_type& l)
      {
        return symbol_type (token::TOK_QUESTIONMARK, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COLON (location_type l)
      {
        return symbol_type (token::TOK_COLON, std::move (l));
      }
#else
      static
      symbol_type
      make_COLON (const location_type& l)
      {
        return symbol_type (token::TOK_COLON, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_COMMA (location_type l)
      {
        return symbol_type (token::TOK_COMMA, std::move (l));
      }
#else
      static
      symbol_type
      make_COMMA (const location_type& l)
      {
        return symbol_type (token::TOK_COMMA, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EQUAL (location_type l)
      {
        return symbol_type (token::TOK_EQUAL, std::move (l));
      }
#else
      static
      symbol_type
      make_EQUAL (const location_type& l)
      {
        return symbol_type (token::TOK_EQUAL, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OUTPUT (location_type l)
      {
        return symbol_type (token::TOK_OUTPUT, std::move (l));
      }
#else
      static
      symbol_type
      make_OUTPUT (const location_type& l)
      {
        return symbol_type (token::TOK_OUTPUT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INOUT (location_type l)
      {
        return symbol_type (token::TOK_INOUT, std::move (l));
      }
#else
      static
      symbol_type
      make_INOUT (const location_type& l)
      {
        return symbol_type (token::TOK_INOUT, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TYPE (location_type l)
      {
        return symbol_type (token::TOK_TYPE, std::move (l));
      }
#else
      static
      symbol_type
      make_TYPE (const location_type& l)
      {
        return symbol_type (token::TOK_TYPE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CLOBBERS (location_type l)
      {
        return symbol_type (token::TOK_CLOBBERS, std::move (l));
      }
#else
      static
      symbol_type
      make_CLOBBERS (const location_type& l)
      {
        return symbol_type (token::TOK_CLOBBERS, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_WHITESPACE (location_type l)
      {
        return symbol_type (token::TOK_WHITESPACE, std::move (l));
      }
#else
      static
      symbol_type
      make_WHITESPACE (const location_type& l)
      {
        return symbol_type (token::TOK_WHITESPACE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRING (std::string v, location_type l)
      {
        return symbol_type (token::TOK_STRING, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_STRING (const std::string& v, const location_type& l)
      {
        return symbol_type (token::TOK_STRING, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ID (std::string v, location_type l)
      {
        return symbol_type (token::TOK_ID, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ID (const std::string& v, const location_type& l)
      {
        return symbol_type (token::TOK_ID, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NUMBER (std::uint32_t v, location_type l)
      {
        return symbol_type (token::TOK_NUMBER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_NUMBER (const std::uint32_t& v, const location_type& l)
      {
        return symbol_type (token::TOK_NUMBER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_VARNAME (std::string v, location_type l)
      {
        return symbol_type (token::TOK_VARNAME, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_VARNAME (const std::string& v, const location_type& l)
      {
        return symbol_type (token::TOK_VARNAME, v, l);
      }
#endif


  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    parser (const parser&);
    /// Non copyable.
    parser& operator= (const parser&);
#endif

    /// Check the lookahead yytoken.
    /// \returns  true iff the token will be eventually shifted.
    bool yy_lac_check_ (symbol_kind_type yytoken) const;
    /// Establish the initial context if no initial context currently exists.
    /// \returns  true iff the token will be eventually shifted.
    bool yy_lac_establish_ (symbol_kind_type yytoken);
    /// Discard any previous initial lookahead context because of event.
    /// \param event  the event which caused the lookahead to be discarded.
    ///               Only used for debbuging output.
    void yy_lac_discard_ (const char* event);

    /// Stored state numbers (used for stacks).
    typedef unsigned char state_type;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const short yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;

#if YYDEBUG || 0
    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#endif // #if YYDEBUG || 0


    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const unsigned char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const unsigned char yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const unsigned char yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const signed char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const signed char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;
    /// The stack for LAC.
    /// Logically, the yy_lac_stack's lifetime is confined to the function
    /// yy_lac_check_. We just store it as a member of this class to hold
    /// on to the memory and to avoid frequent reallocations.
    /// Since yy_lac_check_ is const, this member must be mutable.
    mutable std::vector<state_type> yylac_stack_;
    /// Whether an initial LAC context was established.
    bool yy_lac_established_;


    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 207,     ///< Last index in yytable_.
      yynnts_ = 65,  ///< Number of nonterminal symbols.
      yyfinal_ = 31 ///< Termination state number.
    };


    // User arguments.
    ArchDescLexer &lexer;
    ArchDescBuilder &builder;

  };

  inline
  parser::symbol_kind_type
  parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41
    };
    // Last valid token kind.
    const int code_max = 296;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

  // basic_symbol.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
    {
      case symbol_kind::S_asmInsnArg: // asmInsnArg
        value.copy< AsmInsnArg * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_asmInsn: // asmInsn
      case symbol_kind::S_asmInsnArgs: // asmInsnArgs
      case symbol_kind::S_moreAsmInsnArgs: // moreAsmInsnArgs
        value.copy< AsmInsnCall * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ctypeBody: // ctypeBody
        value.copy< CType * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_ctypeBodyDecl: // ctypeBodyDecl
        value.copy< CTypeModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_clobberRegisterList: // clobberRegisterList
        value.copy< ClobberListDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_constructorDecl: // constructorDecl
      case symbol_kind::S_constructorArgList: // constructorArgList
      case symbol_kind::S_moreConstructorArgs: // moreConstructorArgs
        value.copy< ConstructorDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnDecl: // insnDecl
        value.copy< Insn * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgDeclDirection: // insnArgDeclDirection
        value.copy< InsnArgDecl::Direction > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgPattern: // insnArgPattern
        value.copy< InsnArgPattern > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgDirectionality: // insnArgDirectionality
        value.copy< InsnArgPattern::Directionality > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgs: // insnArgs
      case symbol_kind::S_moreInsnArgs: // moreInsnArgs
        value.copy< InsnArgPatterns * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnArgsDecl: // insnArgsDecl
      case symbol_kind::S_insnArgsDeclList: // insnArgsDeclList
      case symbol_kind::S_moreInsnArgsDeclList: // moreInsnArgsDeclList
        value.copy< InsnArgsModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnBody: // insnBody
      case symbol_kind::S_insnBodyDecls: // insnBodyDecls
        value.copy< InsnFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_patternsDisjunction: // patternsDisjunction
      case symbol_kind::S_morePatternsDisjunction: // morePatternsDisjunction
      case symbol_kind::S_patternsConjunction: // patternsConjunction
      case symbol_kind::S_morePatternsConjunction: // morePatternsConjunction
      case symbol_kind::S_baseInsnPattern: // baseInsnPattern
        value.copy< InsnGen * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnBodyDecl: // insnBodyDecl
        value.copy< InsnModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_simpleInsnPattern: // simpleInsnPattern
        value.copy< InsnPattern * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_rtlType: // rtlType
        value.copy< InsnRtlType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnRuleDecl: // insnRuleDecl
        value.copy< InsnRule * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_asmDecl: // asmDecl
        value.copy< InsnRuleAsmDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnRuleBody: // insnRuleBody
        value.copy< InsnRuleFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnRuleBodyDecl: // insnRuleBodyDecl
      case symbol_kind::S_patternsDecl: // patternsDecl
        value.copy< InsnRuleModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnVarDecl: // insnVarDecl
        value.copy< InsnVarDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnVarDeclBody: // insnVarDeclBody
      case symbol_kind::S_insnVarDeclBodyDecls: // insnVarDeclBodyDecls
        value.copy< InsnVarDeclFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnVarDeclBodyDecl: // insnVarDeclBodyDecl
        value.copy< InsnVarDeclModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_constructorArgTy: // constructorArgTy
      case symbol_kind::S_insnVarTyDecl: // insnVarTyDecl
        value.copy< InsnVarType * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_insnRuleVarType: // insnRuleVarType
        value.copy< InsnVariable::Type > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_literal: // literal
        value.copy< Literal * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        value.copy< ParamDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassBody: // regclassBody
        value.copy< RegClassFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassBodyDecl: // regclassBodyDecl
        value.copy< RegClassModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassTypeDecl: // regclassTypeDecl
        value.copy< RegClassTypeDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regclassRegDecl: // regclassRegDecl
        value.copy< RegMemberDecl * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regtype: // regtype
        value.copy< RegType > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_registerBody: // registerBody
        value.copy< RegisterFactory * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        value.copy< RegisterModifier * > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_optionalPattern: // optionalPattern
        value.copy< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_insnArgTypePattern: // insnArgTypePattern
      case symbol_kind::S_insnArgExprPattern: // insnArgExprPattern
        value.copy< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUMBER: // NUMBER
        value.copy< std::uint32_t > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_asmInsnExpr: // asmInsnExpr
        value.copy< template_string > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  parser::symbol_kind_type
  parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_asmInsnArg: // asmInsnArg
        value.move< AsmInsnArg * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_asmInsn: // asmInsn
      case symbol_kind::S_asmInsnArgs: // asmInsnArgs
      case symbol_kind::S_moreAsmInsnArgs: // moreAsmInsnArgs
        value.move< AsmInsnCall * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_ctypeBody: // ctypeBody
        value.move< CType * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_ctypeBodyDecl: // ctypeBodyDecl
        value.move< CTypeModifier * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_clobberRegisterList: // clobberRegisterList
        value.move< ClobberListDecl * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_constructorDecl: // constructorDecl
      case symbol_kind::S_constructorArgList: // constructorArgList
      case symbol_kind::S_moreConstructorArgs: // moreConstructorArgs
        value.move< ConstructorDecl * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnDecl: // insnDecl
        value.move< Insn * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnArgDeclDirection: // insnArgDeclDirection
        value.move< InsnArgDecl::Direction > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnArgPattern: // insnArgPattern
        value.move< InsnArgPattern > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnArgDirectionality: // insnArgDirectionality
        value.move< InsnArgPattern::Directionality > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnArgs: // insnArgs
      case symbol_kind::S_moreInsnArgs: // moreInsnArgs
        value.move< InsnArgPatterns * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnArgsDecl: // insnArgsDecl
      case symbol_kind::S_insnArgsDeclList: // insnArgsDeclList
      case symbol_kind::S_moreInsnArgsDeclList: // moreInsnArgsDeclList
        value.move< InsnArgsModifier * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnBody: // insnBody
      case symbol_kind::S_insnBodyDecls: // insnBodyDecls
        value.move< InsnFactory * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_patternsDisjunction: // patternsDisjunction
      case symbol_kind::S_morePatternsDisjunction: // morePatternsDisjunction
      case symbol_kind::S_patternsConjunction: // patternsConjunction
      case symbol_kind::S_morePatternsConjunction: // morePatternsConjunction
      case symbol_kind::S_baseInsnPattern: // baseInsnPattern
        value.move< InsnGen * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnBodyDecl: // insnBodyDecl
        value.move< InsnModifier * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_simpleInsnPattern: // simpleInsnPattern
        value.move< InsnPattern * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_rtlType: // rtlType
        value.move< InsnRtlType > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnRuleDecl: // insnRuleDecl
        value.move< InsnRule * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_asmDecl: // asmDecl
        value.move< InsnRuleAsmDecl * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnRuleBody: // insnRuleBody
        value.move< InsnRuleFactory * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnRuleBodyDecl: // insnRuleBodyDecl
      case symbol_kind::S_patternsDecl: // patternsDecl
        value.move< InsnRuleModifier * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnVarDecl: // insnVarDecl
        value.move< InsnVarDecl * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnVarDeclBody: // insnVarDeclBody
      case symbol_kind::S_insnVarDeclBodyDecls: // insnVarDeclBodyDecls
        value.move< InsnVarDeclFactory * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnVarDeclBodyDecl: // insnVarDeclBodyDecl
        value.move< InsnVarDeclModifier * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_constructorArgTy: // constructorArgTy
      case symbol_kind::S_insnVarTyDecl: // insnVarTyDecl
        value.move< InsnVarType * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_insnRuleVarType: // insnRuleVarType
        value.move< InsnVariable::Type > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_literal: // literal
        value.move< Literal * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_paramDecl: // paramDecl
        value.move< ParamDecl * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_regclassBody: // regclassBody
        value.move< RegClassFactory * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_regclassBodyDecl: // regclassBodyDecl
        value.move< RegClassModifier * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_regclassTypeDecl: // regclassTypeDecl
        value.move< RegClassTypeDecl * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_regclassRegDecl: // regclassRegDecl
        value.move< RegMemberDecl * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_regtype: // regtype
        value.move< RegType > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_registerBody: // registerBody
        value.move< RegisterFactory * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_regBodyDecl: // regBodyDecl
        value.move< RegisterModifier * > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_optionalPattern: // optionalPattern
        value.move< bool > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_ID: // ID
      case symbol_kind::S_VARNAME: // VARNAME
      case symbol_kind::S_insnArgTypePattern: // insnArgTypePattern
      case symbol_kind::S_insnArgExprPattern: // insnArgExprPattern
        value.move< std::string > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_NUMBER: // NUMBER
        value.move< std::uint32_t > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_asmInsnExpr: // asmInsnExpr
        value.move< template_string > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  inline
  parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  inline
  parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  inline
  parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  inline
  parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  inline
  void
  parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  inline
  void
  parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  inline
  parser::symbol_kind_type
  parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  inline
  parser::symbol_kind_type
  parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


#line 10 "/home/tathougies/Projects/nnc/./insnsel/parser.y"
} // nnc
#line 3357 "./insnsel/parser.hpp"


// "%code provides" blocks.
#line 125 "/home/tathougies/Projects/nnc/./insnsel/parser.y"

    class LexedToken {
    public:
        enum token_type {
            NNC_AD_KEYWORD,
            NNC_AD_ID,
            NNC_AD_STRING,
            NNC_AD_NUMBER,
            NNC_AD_PUNCTUATION,
            NNC_AD_WHITESPACE
        };

        nnc::position startLoc, endLoc;
        std::uint64_t startPos, endPos;

        token_type token_type;
    };

    class ArchDescLexer {
    public:
        virtual ~ArchDescLexer();

        virtual nnc::parser::symbol_type lex() =0;
        virtual nnc::parser::token_type token_type(std::uint64_t offset) =0;
        nnc::parser::token_type token_type(const nnc::position &offset);

        virtual std::uint64_t translate_position(const nnc::position &offset) =0;
        virtual const std::string &filename() const =0;
    };

    #include <memory>
    #include <iostream>
    std::unique_ptr<ArchDescLexer> makeLexer(const std::string &filename, std::istream &in);


#line 3397 "./insnsel/parser.hpp"


#endif // !YY_YY_INSNSEL_PARSER_HPP_INCLUDED
