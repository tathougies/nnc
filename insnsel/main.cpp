#include "parser.hpp"
#include <iostream>
#include <string>
#include <span>
#include <filesystem>
#include <fstream>

struct cliopts {
  cliopts() : dumpDeps(false), showHelp(false), npos(0) {
  }

  void parse(int argc, const char **argv) {
    std::span<const char *> args(argv + 1, argc -1);

    for ( decltype(args)::iterator i(args.begin());
          i != args.end();
         ++i ) {
      std::string arg(*i);
      if ( arg[0] == '-' ) {
        if ( arg[1] == '-' ) {
          std::string longopt(arg.begin() + 2, arg.end());
          parseLongOpt(longopt, i);
        } else {
          std::string shortopts(arg.begin() + 1, arg.end());
          for ( std::string::iterator opt(shortopts.begin());
                opt != shortopts.end();
                ++opt ) {
            if ( opt == (shortopts.end() - 1) )
              parseShortOpt(*opt, i);
            else
              parseShortOpt(*opt);
          }
        }
      } else {
        if ( npos == 0 ) inputPath = std::move(arg);
        else if (npos == 1) outputPath = std::move(arg);
        else {
          std::cerr << "Parsing CLI opts failed: only two positional arguments expected" << std::endl;
          exit(1);
        }
        npos++;
      }
    }
  }

  void parseLongOpt(const std::string &opt,
                    std::span<const char*>::iterator &arg) {
    if ( opt == "help" ) showHelp = true;
    else if ( opt == "dump-deps" ) dumpDeps = true;
    else if ( opt == "deps-prefix" ) relPath = *(++arg);
    else {
      std::cerr << "Long option '" << opt << "' not valid" << std::endl;
      exit(1);
    }
  }

  void parseShortOpt(char c) {
    switch ( c ) {
    case 'h': showHelp = true; break;
    case 'M': dumpDeps = true; break;
    default:
      std::cerr << "short option '" << c << "' not defined, or expects an argument" << std::endl;
      break;
    }
  }

  void parseShortOpt(char c, std::span<const char*>::iterator &arg) {
    switch ( c ) {
    case 'o':
      outputPath = *(++arg);
      npos = 2;
      arg ++;
      break;

    default:
      parseShortOpt(c);
    }
  }

  int npos;
  std::filesystem::path inputPath;
  std::filesystem::path outputPath;
  std::filesystem::path relPath;
  bool dumpDeps;
  bool showHelp;
};

int main(int argc, const char **argv) {
  cliopts opts;

  opts.parse(argc, argv);

  ArchDescBuilder builder;
  try {
    builder.includeFile(opts.inputPath);
    if ( opts.dumpDeps ) {
      std::filesystem::path parentPath(opts.outputPath.parent_path());

      if ( !parentPath.empty() ) {
        std::error_code ec;
        std::filesystem::create_directories(opts.outputPath.parent_path(), ec);

        if ( ec ) {
          std::cerr << "Could not create " << opts.outputPath.parent_path() << ": " << ec << std::endl;
          return 0;
        }
      }

      std::fstream out(opts.outputPath.native(), std::fstream::out);
      DepsDumper deps(builder, opts.relPath, out);
      builder.generate(deps);
      return 0;
    }

    builder.generate(opts.outputPath);
  } catch ( NncCompileError &e ) {
      NncSimpleErrorReporter reporter;
      e.printError(reporter);
  }

  return 0;
}
