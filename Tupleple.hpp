#pragma once

namespace tupleple
{
	template<class T, class ...R>
	auto cons(const T&lhs, const std::tuple<R...>&rhs)
		->decltype(std::tuple_cat(std::make_tuple(lhs), rhs))
	{
		return std::tuple_cat(
			std::make_tuple(lhs)
			, rhs);
	}
	template<class T, class ...R>
	auto cons_back(const std::tuple<R...>&lhs, const T&rhs)
		->decltype(std::tuple_cat(lhs, std::make_tuple(rhs)))
	{
		return std::tuple_cat(
			lhs
			, std::make_tuple(rhs));
	}
	namespace deteil
	{
		template<bool end, size_t N, class LTuple, class RTuple>
		struct zip_impl2;

		template<size_t N, class LTuple, class RTuple>
		struct zip_impl :zip_impl2<N == std::tuple_size<LTuple>::value, N, LTuple, RTuple>
		{};

		template<size_t N, class LTuple, class RTuple>
		struct zip_impl2<false, N, LTuple, RTuple>
		{
			using Rest = zip_impl<N + 1, LTuple, RTuple>;
			static auto zip(const LTuple&lhs, const RTuple&rhs)
				->decltype(cons(std::make_tuple(std::get<N>(lhs), std::get<N>(rhs)), Rest::zip(lhs, rhs)))
			{
				return cons(std::make_tuple(std::get<N>(lhs), std::get<N>(rhs)), Rest::zip(lhs, rhs));
			}
		};
		template<size_t N, class LTuple, class RTuple>
		struct zip_impl2<true, N, LTuple, RTuple>
		{
			static std::tuple<> zip(const LTuple&lhs, const RTuple&rhs)
			{
				return std::tuple<>();
			}
		};
	}


	template<class LTuple, class RTuple>
	auto zip(const LTuple&lhs, const RTuple&rhs)
		->decltype(deteil::zip_impl<0, LTuple, RTuple>::zip(lhs, rhs))
	{
		return deteil::zip_impl<0, LTuple, RTuple>::zip(lhs, rhs);
	}



	namespace deteil
	{
		template<bool, class T, class Tuple>
		struct cons_if_impl2;

		template< class T, class Tuple>
		struct cons_if_impl2<true, T, Tuple>
		{
			static auto cons_if(const T&t, const Tuple&tuple)
			->decltype(cons(t, tuple))
			{
				return cons(t, tuple);
			}
		};
		template< class T, class Tuple>
		struct cons_if_impl2<false, T, Tuple>
		{
			static Tuple cons_if(const T&t, const Tuple&tuple)
			{
				return tuple;
			}
		};

		template<class T, class Tuple, template<class T> class Pred_class >
		struct cons_if_impl :cons_if_impl2<Pred_class<T>::value, T, Tuple>
		{};
		template< template<class T> class Pred_class, class T, class Tuple>
		auto cons_if(const T&t, const Tuple&tuple)
			->decltype(cons_if_impl<T, Tuple, Pred_class>::cons_if(t, tuple))
		{
			return cons_if_impl<T, Tuple, Pred_class>::cons_if(t, tuple);
		}

		template<bool end, size_t N, class Tuple, template<class T> class Pred_class>
		struct filter_impl2;
		template<size_t N, class Tuple, template<class T> class Pred_class>
		struct filter_impl :filter_impl2<N == std::tuple_size<Tuple>::value, N, Tuple, Pred_class>
		{

		};
		template<size_t N, class Tuple, template<class T> class Pred_class>
		struct filter_impl2<false, N, Tuple, Pred_class>
		{
			using element = typename std::tuple_element<N, Tuple>::type;
			static auto filter(const Tuple&tuple)
				->decltype(cons_if<Pred_class>(std::get<N>(tuple), filter_impl<N + 1, Tuple, Pred_class>::filter(tuple)))
			{
				return cons_if<Pred_class>(std::get<N>(tuple), filter_impl<N + 1, Tuple, Pred_class>::filter(tuple));
			}
		};
		template<size_t N, class Tuple, template<class T> class Pred_class>
		struct filter_impl2<true, N, Tuple, Pred_class>
		{
			static std::tuple<> filter(const Tuple&tuple)
			{
				return  std::tuple<>();
			}
		};
	}

	
	template <template<class T> class Pred_class, class Tuple>
	auto filter(const Tuple&tuple)
		->decltype(deteil::filter_impl<0, Tuple, Pred_class>::filter(tuple))
	{
		return deteil::filter_impl<0, Tuple, Pred_class>::filter(tuple);
	}

	class find_default_type{};
	namespace deteil
	{
		template<bool end, size_t N, class Tuple, template<class T>class Pred_class>
		struct find_if_impl2;

		template<size_t N, class Tuple, template<class T>class Pred_class>
		struct find_if_impl :find_if_impl2<N == std::tuple_size<Tuple>::value, N, Tuple, Pred_class>
		{};

		template<bool r, size_t N, class Tuple, template<class T>class Pred_class>
		struct find_if_impl3;

		template<size_t N, class Tuple, template<class T>class Pred_class>
		struct find_if_impl3<false, N, Tuple, Pred_class>
		{
			using Rest = find_if_impl<N + 1, Tuple, Pred_class>;
			static auto find_if(const Tuple&tuple)
				->decltype(Rest::find_if(tuple))
			{
				return Rest::find_if(tuple);
			}
		};

		template<size_t N, class Tuple, template<class T>class Pred_class>
		struct find_if_impl3<true, N, Tuple, Pred_class>
		{
			static typename std::tuple_element<N, Tuple>::type find_if(const Tuple&tuple)
			{
				auto n = N;

				//std::string ss=typeid(Pred_class).name()
				auto r = Pred_class<typename std::tuple_element<N, Tuple>::type>::value;
				std::string s=typeid(typename std::tuple_element<N, Tuple>::type).name();
				return std::get<N>(tuple);
			}
		};

		template<size_t N, class Tuple, template<class T>class Pred_class>
		struct find_if_impl2<false, N, Tuple, Pred_class>
			:find_if_impl3<Pred_class<typename std::tuple_element<N, Tuple>::type>::value, N, Tuple, Pred_class>
		{};
		template<size_t N, class Tuple, template<class T>class Pred_class>
		struct find_if_impl2<true, N, Tuple, Pred_class>
		{
			static find_default_type find_if(const Tuple&)
			{
				return find_default_type();
			}
		};
	}
	
	template<template<class T>class Pred_class, class Tuple>
	auto find_if(const Tuple&tuple)
		->decltype(deteil::find_if_impl<0, Tuple, Pred_class>::find_if(tuple))
	{
		return deteil::find_if_impl<0, Tuple, Pred_class>::find_if(tuple );
	}

	namespace deteil
	{
		template<class T>
		struct map_impl
		{
			template<class Tuple>
			struct Key
			{
				static const bool value = std::is_same<typename std::tuple_element<0, Tuple>::type, T>::value;
			};

			//template<class Tuple>
			//using key=std::is_same<typename std::tuple_element<0, Tuple>::type, T>;
			//VSで上を使うとバグった。クソッたれめ
			template<class Tuple>
			static auto map(const Tuple&tuple)
				->typename std::tuple_element<1, typename std::tuple_element<0, decltype(filter<Key>(tuple))>::type>::type
			{
				auto t = filter<Key>(tuple);
				static_assert(std::tuple_size<decltype(t)>::value != 0, "tupleple::map 見つかりません");
				static_assert(std::tuple_size<decltype(t)>::value == 1, "tupleple::map 多過ぎます");
				return std::get<1>(std::get<0>(t));
			}
		};
	}

	template<class T, class Tuple>
	auto map(const Tuple&tuple)
		->decltype(deteil::map_impl<T>::map(tuple))
	{
		return deteil::map_impl<T>::map(tuple);
	}


}