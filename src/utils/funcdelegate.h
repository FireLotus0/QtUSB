/**
  ******************************************************************************
  * @file           : funcdelegate.h
  * @author         : lyf
  * @brief          : 函数代理实现
  * @attention      : None
  * @date           : 2026/2/9
  ******************************************************************************
  */

#pragma once

#include "QtUsb/usb_namespace.h"
#include <functional>
#include <utility>
#include <qvector.h>

QT_USB_NAMESPACE_BEGIN

// 在std::bind中用于参数占位
template<std::size_t>
struct ParamPlaceHolder;

#define DECL_PLACE_HOLDER(Index) template<> struct ParamPlaceHolder<Index> { static constexpr auto value = std::placeholders::_##Index; };
DECL_PLACE_HOLDER(1)

DECL_PLACE_HOLDER(2)

DECL_PLACE_HOLDER(3)

DECL_PLACE_HOLDER(4)

DECL_PLACE_HOLDER(5)

DECL_PLACE_HOLDER(6)

// 仿函数判断：是class类型并且重载了operator()
template<typename F, typename... Args>
struct IsFunctor {
    using FuncType = std::decay_t<F>;
    constexpr static bool value = std::is_invocable_v<FuncType, Args...> && std::is_class_v<FuncType>;
};

/**
  * @brief  对可调用对象进行封装
  */
template<typename R, typename... Args>
class FuncWrapper {
public:
    FuncWrapper() = default;

    ~FuncWrapper() = default;

    FuncWrapper(const FuncWrapper &other) : func(other.func) {
    }

    FuncWrapper(FuncWrapper &&other) noexcept : func(std::move(other.func)) {
        other.func = nullptr;
    }

    FuncWrapper &operator=(const FuncWrapper &other) {
        if (this != &other) {
            func = other.func;
        }
        return *this;
    }

    FuncWrapper &operator=(FuncWrapper &&other) noexcept {
        if (this != &other) {
            func = std::move(other.func);
            other.func = nullptr;
        }
        return *this;
    }

    // 从普通函数构造
    template<typename F, std::enable_if_t<std::is_pointer_v<F> && std::is_invocable_v<F, Args...>>* = nullptr>
    explicit FuncWrapper(F fun) {
        func = std::function<R(Args...)>(fun);
    }

    // 从普通函数赋值
    template<typename F, std::enable_if_t<std::is_pointer_v<F> && std::is_invocable_v<F, Args...>>* = nullptr>
    FuncWrapper &operator=(F fun) {
        func = std::function<R(Args...)>(fun);
        return *this;
    }

    // 从成员函数构造
    template<typename F, typename T>
    explicit FuncWrapper(F fun, T *obj) {
        func = std::move(makeMbrFunHelp(fun, obj, std::index_sequence_for<Args...>{}));
    }

    // 从仿函数构造
    template<typename F, std::enable_if_t<IsFunctor<F, Args...>::value>* = nullptr>
    explicit FuncWrapper(const F &fun) {
        func = fun;
    }

    template<typename F, std::enable_if_t<IsFunctor<F, Args...>::value>* = nullptr>
    explicit FuncWrapper(F &&fun) noexcept {
        func = std::forward<F>(fun);
    }

    // 从仿函数赋值
    template<typename F, std::enable_if_t<IsFunctor<F, Args...>::value>* = nullptr>
    FuncWrapper &operator=(const F &fun) {
        func = fun;
        return *this;
    }

    template<typename F, std::enable_if_t<IsFunctor<F, Args...>::value>* = nullptr>
    FuncWrapper &operator=(F &&fun) noexcept {
        func = std::forward<F>(fun);
        return *this;
    }

    // 执行调用
    template<typename...FuncArgs>
    decltype(auto) operator()(FuncArgs &&... funcArgs) {
        assert(func);
        return func(std::forward<FuncArgs>(funcArgs)...);
    }

private:
    template<typename F, typename T, std::size_t... I>
    auto makeMbrFunHelp(F fun, T *obj, std::index_sequence<I...>) {
        return std::bind(fun, obj, ParamPlaceHolder<I + 1>::value...);
    }

private:
    std::function<R(Args...)> func = nullptr;
};


template<typename R, typename... Args>
class FuncDelegate {
    using ElemType = FuncWrapper<R, Args...>;

    template<typename... Params>
    static constexpr auto PARAMS_ADD_ENABLE = std::is_constructible_v<ElemType, Params...> && !(std::is_same_v<std::decay_t<Params>, ElemType> && ...);

    template<typename... Params>
    static constexpr auto DELEGATE_ADD_ENABLE = PARAMS_ADD_ENABLE<Params...> && !(std::is_same_v<std::decay_t<Params>, FuncDelegate> && ...);

public:
    FuncDelegate() = default;

    ~FuncDelegate() = default;

    FuncDelegate(const FuncDelegate &other) : functors(other.functors) {
    }

    FuncDelegate(FuncDelegate &&other) noexcept : functors(std::move(other.functors)) {
    }

    FuncDelegate &operator=(const FuncDelegate &other) {
        if (this != &other) {
            functors = other.functors;
        }
        return *this;
    }

    FuncDelegate &operator=(FuncDelegate &&other) noexcept {
        if (this != &other) {
            functors = std::move(other.functors);
        }
        return *this;
    }

    void add(const FuncWrapper<R, Args...> &func) {
        functors.push_back(func);
    }

    void add(FuncWrapper<R, Args...> &&func) {
        functors.push_back(std::move(func));
    }

    template<typename... Params>
    std::enable_if_t<PARAMS_ADD_ENABLE<Params...>> add(Params &&... params) {
        functors.push_back(std::move(ElemType(std::forward<Params>(params)...)));
    }

    void removeAt(std::size_t index) {
        functors.remove(index);
    }

    template<typename...FuncArgs>
    void operator()(FuncArgs &&... funcArgs) {
        for (auto &func: functors) {
            func(std::forward<FuncArgs>(funcArgs)...);
        }
    }

    FuncDelegate &operator+=(const FuncWrapper<R, Args...> &func) {
        add(func);
        return *this;
    }

    template<typename... Params>
    std::enable_if_t<DELEGATE_ADD_ENABLE<Params...>, FuncDelegate &> operator+=(Params &&... params) {
        add(std::forward<Params>(params)...);
        return *this;
    }

    FuncDelegate &operator+=(const FuncDelegate &other) {
        functors += other.functors;
        return *this;
    }

    FuncDelegate &operator+=(FuncDelegate &&other) noexcept {
        for (auto &func: functors) {
            functors.push_back(std::move(func));
        }
        return *this;
    }

private:
    QVector<ElemType> functors;
};


#if 0
template<typename F, typename T>
decltype(auto) makeFunctor(F &&func, T *obj) {
    return makeFunctorImpl(std::forward<F>(func), obj, std::index_sequence_for<Args...>{});
}

template<typename F, typename T, std::size_t... I>
decltype(auto) makeFunctorImpl(F &&func, T *obj, std::index_sequence<I...>) {
    using RT = decltype(std::invoke(func, obj, std::declval<Args>()...));
    return std::function<RT(Args...)>(
        std::bind(func, obj, ParamPlaceHolder < I + 1 > ::value...)
    );
}

#endif

/**
 * @brief USB数据传输和错误处理回调函数集合
 * @note: 这些函数会在传输数据的线程中调用，实际的函数都是一些信号函数
 */
struct EventDelegate {
    FuncDelegate<void, const QByteArray &> readFinishedDelegate;    // 调用UsbDevice对象的readFinished信号
    FuncDelegate<void> writeFinishedDelegate;                       // 调用UsbDevice对象的writeFinished信号
    FuncDelegate<void, int, const QString &> errorOccurredDelegate; // 调用UsbDevice对象的errorOccurred信号
    FuncDelegate<void, TransferDirection, int> transferFinishDelegate;                // 调用IoCommand的transferFinished信号，进行内部处理
};

QT_USB_NAMESPACE_END
