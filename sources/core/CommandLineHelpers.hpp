#ifndef COMMANDLINEHELPERS_HPP
#define COMMANDLINEHELPERS_HPP
#include <clang/Tooling/Tooling.h>
#include <string>

/*!
 * \brief Make extras arguments.
 *
 * Since clang driver hardcode the standard includes directories path
 * using the location of the binary (assuming the binary is located in the
 * 'normal' location for a clang tool), we need to hardcode ourself theses
 * paths because this binary will probably not located in the right directory...
 *
 * If this executable has not been compiled by the user, the user himself must pass as arguments
 * the paths to the standard includes directories provided by llvm.
 */
clang::tooling::CommandLineArguments makeLlvmIncludeArguments();

#endif // COMMANDLINEHELPERS_HPP
