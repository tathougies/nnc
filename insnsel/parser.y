%require "3.2"
%language "c++"

%define api.value.type variant
%define api.token.constructor
%locations
%define parse.lac full

%define api.token.prefix TOK_
%define api.namespace nnc

%parse-param {ArchDescLexer &lexer} {ArchDescBuilder &builder}

%token ARCHITECTURE
%token CHEADER
%token INCLUDE
%token CODE
%token REGCLASS
%token CTYPE
%token REGTYPE
%token REGISTER
%token CONSTANT
%token INSNRULE
%token INSN
%token PATTERN
%token CONSTRUCTOR
%token IF
%token CHECK
%token ASM
%token VAR
%token ALIAS
%token OBRACE
%token CBRACE
%token OBRACKET
%token CBRACKET
%token OPAREN
%token CPAREN
%token SEMICOLON
%token BAR
%token QUESTIONMARK
%token COLON
%token COMMA
%token EQUAL
%token OUTPUT
%token INOUT
%token TYPE
%token CLOBBERS
%token WHITESPACE /* Not used */
%token EOF 0

%token <std::string> STRING
%token <std::string> ID
%token <std::uint32_t> NUMBER
%token <std::string> VARNAME

%nterm  <RegClassFactory *>    regclassBody
%nterm  <RegClassModifier *>   regclassBodyDecl
%nterm  <ParamDecl *>          paramDecl
%nterm  <Literal *>            literal
%nterm  <RegMemberDecl *>      regclassRegDecl
%nterm  <RegClassTypeDecl *>   regclassTypeDecl
%nterm  <RegType>              regtype
%nterm  <ClobberListDecl *>    clobberRegisterList
%nterm  <RegisterModifier *>   regBodyDecl
%nterm  <RegisterFactory *>    registerBody

%nterm <InsnRule *> insnRuleDecl
%nterm <InsnRuleFactory *> insnRuleBody
%nterm <InsnRuleModifier *> insnRuleBodyDecl
%nterm <InsnVariable::Type> insnRuleVarType

%nterm <InsnRuleModifier *> patternsDecl
%nterm <InsnGen *> patternsDisjunction
%nterm <InsnGen *> morePatternsDisjunction
%nterm <InsnGen *> patternsConjunction
%nterm <InsnGen *> morePatternsConjunction
%nterm <bool> optionalPattern
%nterm <InsnGen *> baseInsnPattern
%nterm <InsnPattern *> simpleInsnPattern
%nterm <InsnArgPatterns *>    insnArgs
%nterm <InsnArgPatterns *>    moreInsnArgs
%nterm <InsnArgPattern>       insnArgPattern
%nterm <InsnArgPattern::Directionality> insnArgDirectionality
%nterm <std::string> insnArgTypePattern
%nterm <std::string> insnArgExprPattern

%nterm <template_string> asmInsnExpr
%nterm <AsmInsnCall *> asmInsnArgs
%nterm <AsmInsnCall *> moreAsmInsnArgs
%nterm <AsmInsnCall *> asmInsn
%nterm <AsmInsnArg *> asmInsnArg
%nterm <InsnRuleAsmDecl *> asmDecl

%nterm <Insn *> insnDecl
%nterm <InsnFactory *> insnBody
%nterm <InsnArgsModifier *> insnArgsDecl
%nterm <InsnArgsModifier *> insnArgsDeclList
%nterm <InsnArgsModifier *> moreInsnArgsDeclList
%nterm <InsnModifier *> insnBodyDecl
%nterm <InsnVarDecl *> insnVarDecl
%nterm <InsnVarType *> insnVarTyDecl
%nterm <InsnVarDeclFactory *> insnVarDeclBody
%nterm <InsnVarDeclFactory *> insnVarDeclBodyDecls
%nterm <InsnVarDeclModifier *> insnVarDeclBodyDecl
%nterm <InsnFactory *> insnBodyDecls
%nterm <InsnArgDecl::Direction> insnArgDeclDirection
%nterm <InsnRtlType> rtlType
%nterm <ConstructorDecl *> constructorDecl
%nterm <ConstructorDecl *> constructorArgList
%nterm <ConstructorDecl *> moreConstructorArgs
%nterm <InsnVarType *> constructorArgTy

%nterm <CTypeModifier *> ctypeBodyDecl
%nterm <CType *> ctypeBody

%code requires {
    #include "ast.hpp"
    #include "archdesc.hpp"
    #include "error.hpp"
    #include "templatestring.hpp"
    #include "insn.hpp"
    class ArchDescLexer;
}

%code provides {
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

}

%code {
  #define yylex(x) lexer.lex()
}

%%

file:           %empty
        |       EOF
        |       decl file
        ;

decl:           architectureDecl
        |       includeDecl
        |       cheaderDecl
        |       regclassDecl
        |       registerDecl
        |       insnRuleDecl
        |       insnDecl
        |       codeDecl
        |       ctypeDecl
        ;

ctypeDecl:      CTYPE ID[tyName] OBRACE ctypeBody[ty] {
                    builder.addCType($tyName, $ty);
                }
        ;

ctypeBody:      ctypeBodyDecl[decl] ctypeBody[body] {
                    $decl->modify(*$body);
                    delete $decl;
                    $$ = $body;
                }
        |       cbraceSemi { $$ = new CType(); }
        ;

ctypeBodyDecl:  paramDecl[param] { $$ = static_cast<CTypeModifier *>($param); }
        ;

codeDecl:       CODE STRING STRING SEMICOLON
                {
                  builder.addCodeSection($2, $3);
                }
        ;

cbraceSemi:     CBRACE
        |       CBRACE SEMICOLON
        ;

cheaderDecl:    CHEADER STRING SEMICOLON {
                    builder.addCHeader($2);
                }
        ;

architectureDecl:
                ARCHITECTURE STRING SEMICOLON {
                    builder.setArchitecture($2);
                }
        ;

includeDecl:    INCLUDE STRING SEMICOLON {
                    NncErrorLocation l(@2, "Included from here");
                    builder.includeFile(lexer.filename(), $2);
                }
        ;


regclassDecl:   REGCLASS ID OBRACE regclassBody {
                    auto newRegClass($4->build($2));
                    delete $4;
                    builder.addRegisterClass(newRegClass);
                }
        ;

registerDecl:    REGISTER ID OBRACE registerBody {
                     auto newRegister($4->build($2));
                     delete $4;

                     builder.addRegister(newRegister);
                 }
        ;

registerBody:   cbraceSemi { $$ = new RegisterFactory(); }
        |       regBodyDecl registerBody {
                  $1->modify(*$2);
                  delete $1;
                  $$ = $2;
                }
        ;

regBodyDecl:     CLOBBERS clobberRegisterList SEMICOLON { $$ = static_cast<RegisterModifier *>($2); }
        ;

clobberRegisterList:
                ID COMMA clobberRegisterList
                {
                  $3->addRegister($1);
                  $$ = $3;
                }
        |       ID {
                    $$ = new ClobberListDecl();
                    $$->addRegister($1);
                }
        ;

regclassBody:   cbraceSemi { $$ = new RegClassFactory(); }
        |       regclassBodyDecl regclassBody {
                  $1->modify(*($2));
                  delete $1;
                  $$ = $2;
                }
        ;

regclassBodyDecl:   paramDecl { $$ = static_cast<RegClassModifier *>($1); }
        |           REGISTER regclassRegDecl SEMICOLON { $$ = static_cast<RegClassModifier *>($2); }
        |           TYPE regclassTypeDecl SEMICOLON { $$ =static_cast<RegClassModifier *>($2); }
        ;

regclassRegDecl:
                ID COMMA regclassRegDecl
                {
                    $3->addRegister($1);
                    $$ = $3;
                }
        |       ID
                {
                    $$ = new RegMemberDecl();
                    $$->addRegister($1);
                }
        ;

regclassTypeDecl:
                regtype COMMA regclassTypeDecl
                {
                    $3->addType($1);
                    $$ = $3;
                }
        |       regtype
                {
                    $$ = new RegClassTypeDecl();
                    $$->addType($1);
                }
        ;

regtype:    ID { NncErrorLocation l(@1, "While parsing the register type");
                 $$ = RegType::fromString($1); }
        ;

insnDecl:       INSN ID[name] insnArgsDecl insnBody {
                    $insnArgsDecl->modify(*$insnBody);
                    delete $insnArgsDecl;

                    NncErrorLocation l(@INSN + @insnBody, "In the instruction declaration for " + $name);
                    builder.addInsn($insnBody->build($name));
                }
        ;

insnArgsDecl:   OPAREN insnArgsDeclList { $$ = $2; }
        |       %empty { $$ = new InsnArgsModifier(); }
        ;

insnArgDeclDirection:
                %empty { $$ = InsnArgDecl::Input; }
        |       OUTPUT { $$ = InsnArgDecl::Output; }
        |       INOUT  { $$ = InsnArgDecl::InputOutput; }
        ;

insnArgsDeclList:
                insnArgDeclDirection VARNAME moreInsnArgsDeclList {
                  NncErrorLocation l(@1 + @2, "In the insn arg declaration");
                  $3->addArgument($2).direction($1);
                  $$ = $3;
                }
        |       insnArgDeclDirection OBRACKET VARNAME CBRACKET[cbracket] moreInsnArgsDeclList {
                  NncErrorLocation l(@1 + @cbracket, "In the insn arg declaration");
                  $5->addArgument($3).direction($1).makeOptional();
                  $$ = $5;
                }
        ;

moreInsnArgsDeclList:
                COMMA insnArgsDeclList { $$ = $2; }
        |       CPAREN { $$ = new InsnArgsModifier(); }
        ;

insnBody:       SEMICOLON { $$ = new InsnFactory(); }
        |       OBRACE insnBodyDecls { $$ = $2; }
        ;

insnBodyDecls:  insnBodyDecl insnBodyDecls {
                    $1->modify(*$2);
                    delete $1;
                    $$ = $2;
                }
        |       CBRACE { $$ = new InsnFactory(); }
        ;

insnBodyDecl:   paramDecl { $$ = static_cast<InsnModifier *>($1); }
        |       insnVarDecl { $$ = static_cast<InsnModifier *>($1); }
        |       constructorDecl { $$ = static_cast<InsnModifier *>($1); }
        ;

constructorDecl:
                CONSTRUCTOR OPAREN constructorArgList[args] SEMICOLON
                {
                    NncErrorLocation l(@CONSTRUCTOR + @args, "In the constructor declaration");
                    $args->errorContext() = NncErrorContextStack::current();
                    $$ = $args;
                }
        ;

constructorArgList:
                constructorArgTy[arg] moreConstructorArgs[rest] {
                  $rest->addType($arg);
                  $$ = $rest;
                }
        |       CPAREN { $$ = new ConstructorDecl(); }
        ;

moreConstructorArgs:
                COMMA constructorArgList[args] { $$ = $args; }
        |       CPAREN { $$ = new ConstructorDecl(); }
        ;

constructorArgTy:
                 REGISTER { $$ = new InsnRegisterVarType(); }
        |        CONSTANT { $$ = new InsnConstantVarType(); }
        |        ID[ctypeName] {
                   NncErrorLocation l(@ctypeName, "In constructor arg type declaration");
                   CType &ty(builder.cType($ctypeName));
                   $$ = static_cast<InsnVarType *>(new CType(ty));
                 }
        ;

insnVarDecl:    VAR VARNAME[name] COLON insnVarTyDecl[tyDecl] insnVarDeclBody[body] SEMICOLON
                {
                  NncErrorLocation l(@1 + @6, "In the var decl");
                  $$ = $body->build($name, $tyDecl);
                  delete $body;
                }
        ;

insnVarTyDecl:  REGISTER { $$ = new InsnRegisterVarType(); }
        |       REGISTER OPAREN rtlType CPAREN {
                  auto newTy(new InsnRegisterVarType());
                  newTy->setRtlType($rtlType);
                  $$ = static_cast<InsnVarType *>(newTy);
                }
        |       CONSTANT { $$ = new InsnConstantVarType(); }
        |       CONSTANT OPAREN rtlType CPAREN {
                  auto newTy(new InsnConstantVarType());
                  newTy->setRtlType($rtlType);
                  $$ = static_cast<InsnVarType *>(newTy);
                }
        |       ID[ctypeName] {
                  NncErrorLocation l(@ctypeName, "In the var type declaration");
                  CType &ty(builder.cType($ctypeName));
                  if ( ty.acceptsRtlType() )
                    throw NncParseError(@ctypeName, "Type " + $ctypeName + " requires an RTL type parameter");
                  $$ = static_cast<InsnVarType *>(new CType(ty));
                }
        |       ID[ctypeName] OPAREN rtlType CPAREN {
                  NncErrorLocation l(@ctypeName, "In the var type declaration");
                  CType &ty(builder.cType($ctypeName));
                  if ( !ty.acceptsRtlType() )
                    throw NncParseError(@ctypeName, "Type " + $ctypeName + " does not accept RTL type parameters");
                  auto newType(new CType(ty));
                  newType->setRtlType($rtlType);
                  $$ = static_cast<InsnVarType *>(newType);
                }
        ;

rtlType:        ID rtlType {
                    NncErrorLocation l(@1 + @2, "In the RTL type");
                    if ( $1 != "ptr" )
                      throw NncParseError("Expected ptr");
                    $$ = $2.ptr();
                }
        |       ID {
                   NncErrorLocation l(@1, "In the primitive RTL type");
                   $$ = InsnRtlType($1);
                }
        ;

insnVarDeclBody:    %empty { $$ = new InsnVarDeclFactory(); }
        |           OBRACE insnVarDeclBodyDecls { $$ = $2; }
        ;

insnVarDeclBodyDecls: CBRACE { $$ = new InsnVarDeclFactory(); }
        |             insnVarDeclBodyDecl insnVarDeclBodyDecls
                      {
                        $1->modify(*$2);
                        delete $1;
                        $$ = $2;
                      }
        ;

insnVarDeclBodyDecl:
                paramDecl { $$ = static_cast<InsnVarDeclModifier *>($1); }
        ;

insnRuleDecl:   INSNRULE ID OBRACE insnRuleBody {
                    NncErrorLocation l(@1 + @4, "In the instruction rule");
                    builder.addInsnRule($4->build($2));
                    delete $4;
                }
        ;

insnRuleBody:   cbraceSemi { $$ = new InsnRuleFactory(); }
        |       insnRuleBodyDecl insnRuleBody {
                  if ( $1 ) {
                    $1->modify(*$2);
                    if ( $1->shouldDeleteInsnRuleModifierAfterModify() ) delete $1;
                  }
                  $$ = $2;
                }
        ;

insnRuleVarType:
                REGTYPE  { $$ = InsnVariable::RegTypeType; }
        |       REGISTER { $$ = InsnVariable::RegisterType; }
        |       CONSTANT { $$ = InsnVariable::ConstantType; }
        ;

insnRuleBodyDecl:
                 VAR VARNAME COLON insnRuleVarType SEMICOLON
                {
                   auto decl(new InsnRuleVarDecl(@2, $2, $4));
                   $$ = static_cast<InsnRuleModifier *>(decl);
                }
        |        PATTERN OBRACE patternsDecl cbraceSemi { $$ = static_cast<InsnRuleModifier *>($3); }
        |        IF STRING SEMICOLON { $$ = static_cast<InsnRuleModifier *>(new InsnCondDecl($2)); }
        |        CHECK STRING SEMICOLON { $$ = static_cast<InsnRuleModifier *>(new InsnCheckDecl(@CHECK, $2)); }
        |        paramDecl { $$ = static_cast<InsnRuleModifier *>($1); }
        |        ASM OBRACE asmDecl { $$ = static_cast<InsnRuleModifier *>($3); }
        ;

patternsDecl: patternsDisjunction { $$ = static_cast<InsnRuleModifier *>($patternsDisjunction); }

patternsDisjunction:
                patternsConjunction[head] morePatternsDisjunction[rest] {
                  $$ = InsnRuleDisjunction::make($rest, $head);
                }
                ;

morePatternsDisjunction:
                BAR patternsDisjunction {
                  $$ = $patternsDisjunction;
                }
        |       %empty { $$ = nullptr; }
        ;

patternsConjunction:
                baseInsnPattern[head] SEMICOLON morePatternsConjunction[rest] {
                  $$ = InsnRuleConjunction::make($head, $rest);
                }
        |       baseInsnPattern[insn] {
                  $$ = static_cast<InsnGen *>($insn);
                }
        |       OBRACE patternsDisjunction[head] cbraceSemi optionalPattern morePatternsConjunction[rest] {
                  if ( $optionalPattern ) {
                    $head = InsnRuleOptionalPattern::make($head);
                  }
                  $$ = InsnRuleConjunction::make($head, $rest);
                }
        ;

optionalPattern:
                QUESTIONMARK { $$ = true; }
        |       %empty { $$ = false; }
        ;

morePatternsConjunction:
                %empty { $$ = nullptr; }
        |       patternsConjunction { $$ = $patternsConjunction; }
        ;

baseInsnPattern:
                simpleInsnPattern optionalPattern {
                  $$ = static_cast<InsnGen *>($simpleInsnPattern);
                  if ( $optionalPattern ) $$ = InsnRuleOptionalPattern::make($$);
                }
        ;

simpleInsnPattern:
                ID[insnId] OPAREN insnArgs {
                  $$ = new InsnPattern($insnId, $insnArgs);
                }
        ;

insnArgs:       CPAREN { $$ = new InsnArgPatterns(); }
        |       ID insnArgPattern moreInsnArgs {
                   $$ = $3;
                   $$->addArgument($1, $2);
                }
        ;

insnArgDirectionality:
                EQUAL { $$ = InsnArgPattern::Input; }
        |       OUTPUT { $$ = InsnArgPattern::Output; }
        |       INOUT { $$ = InsnArgPattern::InputOutput; }
        ;

moreInsnArgs:   COMMA insnArgs { $$ = $2; }
        |       CPAREN { $$ = new InsnArgPatterns(); }
        ;

insnArgPattern: insnArgDirectionality OPAREN insnArgTypePattern CPAREN insnArgExprPattern {
                    $$ = InsnArgPattern(@1, $1, $3, $5);
                }
        |       insnArgDirectionality insnArgExprPattern {
                    $$ = InsnArgPattern(@1, $1, $2);
                }
        ;

insnArgTypePattern: VARNAME { $$ = $1; }
        ;

insnArgExprPattern:
                VARNAME { $$ = $1; }
        ;


asmDecl:        cbraceSemi { $$ = new InsnRuleAsmDecl(); }
        |       asmInsn SEMICOLON asmDecl {
                  $3->generateInsn(*$1);
                  delete $1;
                  $$ = $3;
        }
        ;

asmInsn:        ID OPAREN asmInsnArgs {
                    NncErrorLocation l(@1 + @3, "In the generated asm expression");
                    $3->mnemonic($1);
                    $3->errorContext() = NncErrorContextStack::current();
                    $$ = $3;
                }
        |       ID {
                  NncErrorLocation l(@1, "In the generated asm expression");
                  $$ = new AsmInsnCall($1);
                }
        ;

asmInsnArgs:    asmInsnArg moreAsmInsnArgs {
                    NncErrorLocation l(@1, "In the args of the asm instruction");
                    $1->addToCall(*$2);
                    delete $1;
                    $$ = $2;
                }
        |       CPAREN {
                  $$ = new AsmInsnCall();
                }
        ;

moreAsmInsnArgs:
                COMMA asmInsnArgs { $$ = $2; }
        |       CPAREN { $$ = new AsmInsnCall(); }
        ;

asmInsnArg:     asmInsnExpr { $$ = static_cast<AsmInsnArg *>(new AsmInsnPositionalArg(std::move($1))); }
        |       ID EQUAL asmInsnExpr {
                  $$ = static_cast<AsmInsnArg *>(new AsmInsnNamedArg($1, std::move($3)));
                }
        ;

asmInsnExpr:    VARNAME {
                    $$ << $$.var($1);
                }
        |       STRING {
                    template_string s($1);
                    $$ = std::move(s);
                }
        ;

paramDecl:      ID EQUAL literal SEMICOLON {
                    $$ = new ParamDecl($1, $3);
                }
        ;

literal:         STRING { $$ = new StringLiteral($1); }
        |        NUMBER { $$ = new NumberLiteral($1); }
        |        ID     { if ( $1 == "true" ) $$ = new NumberLiteral(1);
                          else if ( $1 == "false" ) $$ = new NumberLiteral(0);
                          else {
                            NncErrorLocation l(@1, "In the literal");
                            throw NncParseError("Invalid literal value " + $1);
                          }
                        }
        ;

%%

void nnc::parser::error(const location_type &l, const std::string &m) {
    throw NncParseError(l, m);
}

ArchDescLexer::~ArchDescLexer() {
}

nnc::parser::token_type ArchDescLexer::token_type(const nnc::position &offset) {
  std::uint64_t off(translate_position(offset));
  return token_type(off);
}
