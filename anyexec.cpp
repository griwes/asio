#include <asio/execution.hpp>

#include <any>
#include <cassert>

struct my_executor
{
    my_executor() = default;
    my_executor(const my_executor &) noexcept = default;

    template<typename F>
    void execute(F && f) const
    {
        std::forward<F>(f)();
    }

    friend bool operator==(const my_executor &, const my_executor &) noexcept
    {
        return true;
    }

    friend bool operator!=(const my_executor &, const my_executor &) noexcept
    {
        return false;
    }
};

struct target_property_t
{
    using polymorphic_query_result_type = std::any;

    static constexpr bool is_requirable = false;
    static constexpr bool is_preferable = false;

    template<typename Ex>
    static constexpr bool is_applicable_property_v = asio::execution::is_executor<Ex>::value;
};

template<typename Executor>
struct is_polymorphic_executor : std::false_type {};

template<typename ...Ts>
struct is_polymorphic_executor<asio::execution::any_executor<Ts...>> : std::true_type {};

template<typename Executor>
inline constexpr bool is_polymorphic_executor_v = is_polymorphic_executor<Executor>::value;

template<typename Executor>
std::any query(Executor ex, target_property_t)
{
    static_assert(!is_polymorphic_executor_v<Executor>,
        "tried to query the target of a polymorphic executor which does not support target_property_t");

    return std::any(ex);
}

int main()
{
    asio::execution::any_executor<> any1 = my_executor{};
    // static asserts:
    // assert(asio::query(any1, target_property_t{}).type() == typeid(my_executor));

    asio::execution::any_executor<target_property_t> any2 = my_executor{};
    assert(asio::query(any2, target_property_t{}).type() == typeid(my_executor));

    return 0;
}
