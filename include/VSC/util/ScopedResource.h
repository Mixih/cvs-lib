/** @file
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */
#ifndef VSC_SCOPED_RESOURCE_H_
#define VSC_SCOPED_RESOURCE_H_

#include <utility>

#include "VSC/util/Concept.h"

namespace vsc {

struct Nothing {};

template <typename U, typename AuxT> struct DestroyerFun {
    using Type = void (*)(U&, AuxT);
};
template <typename U> struct DestroyerFun<U, Nothing> {
    using Type = void (*)(U&);
};
template <typename U, typename AuxT> struct DestroyerFun<U*, AuxT> {
    using Type = void (*)(U*, AuxT);
};
template <typename U> struct DestroyerFun<U*, Nothing> {
    using Type = void (*)(U*);
};

/**
 * Scope managed resource (RAII) wrapper class
 *
 * Use this class to wrap resources that must have a "tear-down" procedule called at the
 * end of its life reliablely, using the SBRM CPP pattern.
 *
 * Note: Prefer the initializing constructors when possible. A non-initializing
 * constructor has been provided, but using it can lead to uninitialized resource bugs.
 * You have been warned!
 * @tparam T type to wrap
 */
template <typename T, typename AuxType = Nothing> class ScopedResource {
private:
    DestroyerFun<T, AuxType>::Type destroyResource;
    bool resourceValid;
    T managedResource;
    AuxType aux;

    void releaseResource() {
        if (resourceValid) {
            destroyResource(managedResource, aux);
            resourceValid = false;
        }
    }

    void releaseResource()
        requires(std::is_same_v<AuxType, Nothing>)
    {
        if (resourceValid) {
            destroyResource(managedResource);
            resourceValid = false;
        }
    }

public:
    /**
     * Create a uninitialized managed resource with the given deleter lambda.
     * @param destroyResource callable that will be called automatically to release
     *                        resource at end-of-life.
     */
    ScopedResource(DestroyerFun<T, AuxType>::Type destroyResource);
    /**
     * Create a uninitialized managed resource with the given deleter lambda.
     * @param destroyResource callable that will be called automatically to release
     *                        resource at end-of-life.
     */
    ScopedResource(DestroyerFun<T, AuxType>::Type destroyResource, AuxType aux);
    /**
     * Create an initialized managed resource by taking ownership of an existing
     * resource.
     * @param resource resource to take ownership of.
     * @param destroyResource callable that will be called automatically to release
     *                        resource at end-of-life.
     */
    ScopedResource(T&& resource, DestroyerFun<T, AuxType>::Type destroyResource,
                   bool valid = true);
    /**
     * Create an initialized managed resource by taking ownership of an existing
     * resource.
     * @param resource resource to take ownership of.
     * @param destroyResource callable that will be called automatically to release
     *                        resource at end-of-life.
     */
    ScopedResource(T&& resource, DestroyerFun<T, AuxType>::Type destroyResource,
                   AuxType aux, bool valid = true);
    /**
     * RAII destructor semantics.
     */
    ~ScopedResource() { releaseResource(); }
    /**
     * Move constructor - takes ownership
     */
    ScopedResource(ScopedResource&& other);
    /**
     * Move assignment - takes ownership
     */
    ScopedResource& operator=(ScopedResource&& other);

    /**
     * Take ownership of a resource, releasing the current one if one is already
     * acquired.
     */
    void takeOwnership(T&& resource);

    /**
     * This forwards the wrapped type in most circumstances. If the autoconversion
     * selects the wrong type, use the get() method to get the underlying resource.
     *
     * WARNING: This is unchecked for performance reasons. DO NOT ATTEMPT TO ACCESS
     * AN UN-INITIALIZED RESOURCE.
     */
    operator T&() { return managedResource; }
    T& get() { return managedResource; }
    const T& get() const { return managedResource; }

    // disable all forms of copy
    ScopedResource(const ScopedResource&) = delete;
    ScopedResource& operator=(const ScopedResource&) = delete;
};

/**
 * Factory function for ScopedResource without auxiliary data.
 *
 * @tparam T underlying resource type.
 * @param destroyResource callable that is used to release the resource.
 * @param createResource callale that is used to create the resource.
 * @param args arguments for the creator callable.
 */
template <typename T, typename... Args, ResourceCreator<T, Args...> CreatorFun>
ScopedResource<T, Nothing>
makeScoped(typename DestroyerFun<T, Nothing>::Type destroyResource,
           CreatorFun createResource, Args&&... args);

/**
 * Factory function for ScopedResource with auxiliary data.
 *
 * @tparam T underlying resource type.
 * @param aux auxiliary data passed to destructor callable
 * @param destroyResource callable that is used to release the resource.
 * @param createResource callale that is used to create the resource.
 * @param args arguments for the creator callable.
 */
template <typename T, typename AuxT, typename... Args,
          ResourceCreator<T, Args...> CreatorFun>
ScopedResource<T, AuxT> inline makeScoped(
    AuxT aux, typename DestroyerFun<T, AuxT>::Type destroyResource,
    CreatorFun createResource, Args&&... args);

///////////////////////////////////////////////////////////////////////////////
// Template Method Implementations
///////////////////////////////////////////////////////////////////////////////

// Begin ScopedResource Implementations
template <typename T, typename AuxType>
inline ScopedResource<T, AuxType>::ScopedResource(
    DestroyerFun<T, AuxType>::Type destroyResource)
    : destroyResource(destroyResource), resourceValid{false} {
}

template <typename T, typename AuxType>
inline ScopedResource<T, AuxType>::ScopedResource(
    DestroyerFun<T, AuxType>::Type destroyResource, AuxType aux)
    : destroyResource(destroyResource), resourceValid{false}, aux(std::move(aux)) {
}

template <typename T, typename AuxType>
inline ScopedResource<T, AuxType>::ScopedResource(
    T&& resource, DestroyerFun<T, AuxType>::Type destroyResource, bool valid)
    : destroyResource(destroyResource),
      resourceValid{valid},
      managedResource(std::move(resource)) {
}

template <typename T, typename AuxType>
inline ScopedResource<T, AuxType>::ScopedResource(
    T&& resource, DestroyerFun<T, AuxType>::Type destroyResource, AuxType aux, bool valid)
    : destroyResource(destroyResource),
      resourceValid{valid},
      managedResource(std::move(resource)),
      aux(std::move(aux)) {
}

template <typename T, typename AuxType>
inline ScopedResource<T, AuxType>::ScopedResource(ScopedResource&& other)
    : destroyResource(other.destroyResource),
      resourceValid(other.resourceValid),
      managedResource(std::move(other.managedResource)) {
    other.resourceValid = false;
}

template <typename T, typename AuxType>
inline ScopedResource<T, AuxType>&
ScopedResource<T, AuxType>::operator=(ScopedResource&& other) {
    releaseResource();
    destroyResource = other.destroyResource;
    resourceValid = other.resourceValid;
    managedResource = std::move(other.managedResource);
    other.resourceValid = false;
}

template <typename T, typename AuxType>
inline void ScopedResource<T, AuxType>::takeOwnership(T&& resource) {
    releaseResource();
    managedResource = std::move(resource);
    resourceValid = true;
}

// factory function implementations
template <typename T, typename... Args, ResourceCreator<T, Args...> CreatorFun>
ScopedResource<T, Nothing> inline makeScoped(
    typename DestroyerFun<T, Nothing>::Type destroyResource, CreatorFun createResource,
    Args&&... args) {
    return ScopedResource<T, Nothing>(createResource(std::forward<Args>(args)...),
                                      destroyResource);
}

template <typename T, typename AuxT, typename... Args,
          ResourceCreator<T, Args...> CreatorFun>
ScopedResource<T, AuxT> inline makeScoped(
    AuxT aux, typename DestroyerFun<T, AuxT>::Type destroyResource,
    CreatorFun createResource, Args&&... args) {
    return ScopedResource<T, AuxT>(createResource(std::forward<Args>(args)...),
                                   destroyResource, std::move(aux));
}
// End ScopedResource Implementations

} // namespace vsc
#endif /* VSC_SCOPED_RESOURCE_H_ */
