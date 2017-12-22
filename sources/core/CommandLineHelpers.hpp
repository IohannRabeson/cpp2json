#ifndef COMMANDLINEHELPERS_HPP
#define COMMANDLINEHELPERS_HPP
#include <clang/Tooling/Tooling.h>
#include <string>

/*!
 * \brief Make extras arguments.
 *
 * Clang assume the standard library directory can be found using a relative path.
 * So if you change the location of cpp2json and try to use it, path to the STL will be not found.
 * To fix that, we need to hardcode the path to the LLVM installation used to build the program and add it
 * to the include path using -I. This is a dirty solution.
 * TODO: find something else to fix theses standard includes locations.
 * Possible solutions:
 *  - bundle the clang standard library -> write a CMake rule to copy the required includes into our own location and
 *  deploy it with binaries.
 *  - hardcode all possible standard locations of clang.
 *  - do nothing, just print an error then quit.
 */
clang::tooling::CommandLineArguments makeLlvmIncludeArguments();

#endif // COMMANDLINEHELPERS_HPP
