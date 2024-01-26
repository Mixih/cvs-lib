#ifndef VSC_SUPPRESSIONS_H_
#define VSC_SUPPRESSIONS_H_

namespace vsc {

/**
 * Used to mark a function parameter as intentionally unused to squash the compiler
 * warning.
 */
template <typename T> void unused(T&){};

} // namespace vsc

#endif /* VSC_SUPPRESSIONS_H_ */
