
#pragma once
#include <type_traits>
#include <utility>

namespace palace
{
	struct has_operator
	{
	private:

		template<class T, class EqualTo>
		struct has_operator_equal_impl
		{
			template<class U, class V>
			static auto test(U*) -> decltype(std::declval<U>() == std::declval<V>());
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};

		template<class T, class EqualTo>
		struct has_operator_superior_impl
		{
			template<class U, class V>
			static auto test(U*) -> decltype(std::declval<U>() > std::declval<V>());
			template<typename, typename>
			static auto test(...)->std::false_type;

			using type = typename std::is_same<bool, decltype(test<T, EqualTo>(0))>::type;
		};

	public:

		template<class T, class EqualTo = T>
		struct op_equal : has_operator_equal_impl<T, EqualTo>::type {};

		template<class T, class EqualTo = T>
		struct op_superior : has_operator_equal_impl<T, EqualTo>::type {};
	};

	
}
