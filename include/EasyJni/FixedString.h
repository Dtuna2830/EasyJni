#pragma once
#include <string_view>
#include <algorithm>

namespace EasyJni
{

template <size_t N>
struct FixedString
{
	char data[N];
	consteval FixedString(const char (&s)[N])
	{
		std::copy_n(s, N, data);
	}
	consteval FixedString() = default;
	constexpr std::string_view view() const
	{
		return std::string_view(data, N - 1);
	}
	constexpr operator std::string_view() const
	{
		return view();
	}
	template <size_t NN>
	consteval auto operator+(const FixedString<NN> &o) const
	{
		FixedString<N + NN - 1> result;
		std::copy_n(data, N - 1, result.data);
		std::copy_n(o.data, NN, result.data + N - 1);
		return result;
	}
	template <size_t Pos, size_t Count>
	consteval auto substr() const
	{
		static_assert(Pos + Count < N, "substr out of range");
		FixedString<Count + 1> result;
		std::copy_n(data + Pos, Count, result.data);
		result.data[Count] = '\0';
		return result;
	}
};
template <size_t N>
FixedString(const char (&)[N]) -> FixedString<N>;

} // namespace EasyJni