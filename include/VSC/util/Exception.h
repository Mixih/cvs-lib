#ifndef VSC_EXCEPTION_H_
#define VSC_EXCEPTION_H_

#include <stdexcept>

namespace vsc {

/**
 * An exception type for vulkan runtime errors.
 */
class VkRuntimeError : public std::runtime_error {
public:
    template <typename... Args>
    explicit VkRuntimeError(Args&&... args)
        : std::runtime_error(std::forward<Args>(args)...) {}
};

} // namespace vsc

#endif /* VSC_EXCEPTION_H_ */
