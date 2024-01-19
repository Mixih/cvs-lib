/** @file
 * SPDX-FileCopyrightText:  (C) 2024 Max Hahn
 * SPDX-License-Identifier: BSD-3-Clause OR CERN-OHL-S-2.0
 */
#ifndef VSC_CONCEPT_H_
#define VSC_CONCEPT_H_

#include <concepts>
#include <functional>
#include <type_traits>

namespace vsc {

/**
 * Constraint for resource creator functions
 *
 * They must be callable and return an instance of the desired type T.
 */
template <typename CreatorFun, typename T, typename... Args>
concept ResourceCreator = requires(CreatorFun f, Args&&... args) {
    { std::invoke(f, std::forward<Args>(args)...) } -> std::same_as<T>;
};

/**
 * Clock edge handler callable constraint
 *
 * A valid function must have the signature void handler(VerilatedModel &m)
 */
template <typename ClkEdgeFun, typename VerilatedModel>
concept ClkEdgeHandler = requires(ClkEdgeFun f, VerilatedModel vm) {
    { f } -> std::convertible_to<std::function<void(VerilatedModel&)>>;
};

/**
 * Verilated testbench constraint
 *
 * The testbench must have the specified methods and fields, as well as be a subclass of
 * the base verilated model class with the correct methods.
 */
template <typename TopModule>
concept VerilatedToplevel = requires(TopModule m) {
    { m.clk } -> std::convertible_to<int>;
    { m.rst } -> std::convertible_to<int>;
    m.eval_step();
    m.end_eval_step();
};

} // namespace vsc

#endif /* VSC_CONCEPT_H_ */
