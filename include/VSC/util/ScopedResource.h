/** @file
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */
#ifndef VSC_SCOPED_RESOURCE_H_
#define VSC_SCOPED_RESOURCE_H_

#include <utility>

#include "VSC/util/Concept.h"

namespace vsc {

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
template <typename T> class ScopedResource {
public:
    template <typename U> struct DestroyerFun {
        using Type = void (*)(U&);
    };

    template <typename U> struct DestroyerFun<U*> {
        using Type = void (*)(U*);
    };

private:
    DestroyerFun<T>::Type destroyResource;
    bool resourceValid;
    T managedResource;

    void releaseResource() {
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
    ScopedResource(DestroyerFun<T>::Type destroyResource);
    /**
     * Create an initialized managed resource by taking ownership of an existing
     * resource.
     * @param resource resource to take ownership of.
     * @param destroyResource callable that will be called automatically to release
     *                        resource at end-of-life.
     */
    ScopedResource(T&& resource, DestroyerFun<T>::Type destroyResource);
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
    void takeOwnership(T&& resource) {
        releaseResource();
        managedResource = std::move(resource);
    }
    /**
     * This forwards the wrapped type in most circumstances. If the autoconversion
     * selects the wrong type, use the get() method to get the underlying resource.
     *
     * WARNING: This is unsafe for performance reasons. DO NOT ATTEMPT TO ACCESS
     * UN-INITIALIZED RESOURCE.
     */
    operator T&() { return managedResource; }
    T& get() { return managedResource; }
    const T& get() const { return managedResource; }

    // disable all forms of copy
    ScopedResource(const ScopedResource&) = delete;
    ScopedResource& operator=(const ScopedResource&) = delete;

    /**
     * Create a new instance of a managed resource by initializing it from a given
     * custruction function/lambda.
     * @param destroyResource callable that will be called automatically to release
     *                        resource at end-of-life.
     * @param createResource callable that will be called to create the resource.
     * @param args arguments to pass to to the createResource function used to create the
     *             managed resource.
     */
    template <typename U, typename... Args, ResourceCreator<U, Args...> CreatorFun>
    friend ScopedResource<U> makeScopedResource(
        typename ScopedResource<U>::template DestroyerFun<U>::Type destroyResource,
        CreatorFun createResource, Args&&... args);
};

template <typename T, typename... Args, ResourceCreator<T, Args...> CreatorFun>
ScopedResource<T> makeScopedResource(
    typename ScopedResource<T>::template DestroyerFun<T>::Type destroyResource,
    CreatorFun createResource, Args&&... args);

///////////////////////////////////////////////////////////////////////////////
// Template Method Implementations
///////////////////////////////////////////////////////////////////////////////

// Begin ScopedResource Implementations
template <typename T>
inline ScopedResource<T>::ScopedResource(DestroyerFun<T>::Type destroyResource)
    : destroyResource(destroyResource), resourceValid{false} {
}

template <typename T>
inline ScopedResource<T>::ScopedResource(T&& resource,
                                         DestroyerFun<T>::Type destroyResource)
    : destroyResource(destroyResource),
      resourceValid{true},
      managedResource(std::move(resource)) {
}

template <typename T, typename... Args, ResourceCreator<T, Args...> CreatorFun>
ScopedResource<T> inline makeScopedResource(
    typename ScopedResource<T>::template DestroyerFun<T>::Type destroyResource,
    CreatorFun createResource, Args&&... args) {
    return ScopedResource<T>(createResource(std::forward<Args>(args)...),
                             destroyResource);
}

template <typename T>
inline ScopedResource<T>::ScopedResource(ScopedResource&& other)
    : destroyResource(other.destroyResource),
      resourceValid(other.resourceValid),
      managedResource(std::move(other.managedResource)) {
    other.resourceValid = false;
}

template <typename T>
inline ScopedResource<T>& ScopedResource<T>::operator=(ScopedResource&& other) {
    releaseResource();
    destroyResource = other.destroyResource;
    resourceValid = other.resourceValid;
    managedResource = std::move(other.managedResource);
    other.resourceValid = false;
}
// End ScopedResource Implementations

} // namespace vsc
#endif /* VSC_SCOPED_RESOURCE_H_ */
