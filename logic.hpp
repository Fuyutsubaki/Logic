#pragma once 
#include<vector>
#include<tuple>
#include<boost\range\adaptor\filtered.hpp>
#include<boost\range\adaptor\transformed.hpp>
#include<type_traits>
#include"Tupleple.hpp"

namespace logic{
	template<size_t No>
	struct ID
	{
		static const size_t value = No;
		int debug = No;
	};

	namespace deteil{
		//ここの実装大丈夫かしら
		struct AllTimeMatch
		{
			template<class T>
			friend bool operator==(const T&, AllTimeMatch)
			{
				return true;
			}
			template<class T>
			bool operator==(const T&)const
			{
				return true;
			}
		};

		template<class T>
		const T&Undefine2AllTimeMatch_cast(const T&x){ return x; }

		template<size_t No>
		AllTimeMatch Undefine2AllTimeMatch_cast(ID<No>){ return AllTimeMatch(); }

		class NoID{};
		template<class T>
		NoID IDorNoID(const T&){ return NoID(); }
		template<size_t No>
		ID<No> IDorNoID(ID<No> x)
		{
			return x;
		}
		template<class T>
		struct is_ID
		{
			static const bool value = false;
		};
		template<size_t No>
		struct is_ID<ID<No>>
		{
			static const bool value = true;
		};
		template<class Tuple>
		using Key_is_ID = is_ID<typename std::tuple_element<0, Tuple>::type>;


	}

	template<class ID>
	struct result_getter
	{
		result_getter(ID id)
			:id_(id)
		{}
		template<class ...R>
		friend auto operator|(const std::tuple<R...>&tuple, result_getter)
			->decltype(tupleple::map<ID>(tuple))
		{
			return tupleple::map<ID>(tuple);
		}
	private:
		ID id_;
	};

	struct get_forward
	{
		template<class ID>
		result_getter<ID> operator()(ID id)const
		{
			return result_getter<ID>(id);
		}
	};

	static const get_forward get;

	template<class ...T>
	class rule
	{
		using data_type = std::tuple<T...>;
		std::vector<data_type> range_;

	private:
		template<class ...Args>
		struct filter_functor
		{
		public:
			filter_functor(const Args&...args)
				:data_(args...)
			{}
			bool operator()(const data_type&data)const
			{
				return data == data_;
			}
		private:
			std::tuple<Args...> data_;
		};

		template<class ...Args>
		auto filter_impl(const Args&...args)
			->decltype(
			range_ | boost::adaptors::filtered(filter_functor<Args...>(args...))
			)
			const
		{
			return range_ | boost::adaptors::filtered(filter_functor<Args...>(args...));
			//λ式を使いたかった。使うとこの関数の戻り値がわからない。戻り値型予測ぎぶみー
		}
	public:
		template<class ...Args>
		auto filter(const Args&...args)
			->decltype(
			filter_impl(deteil::Undefine2AllTimeMatch_cast(args)...)
			)
		{
			return filter_impl(deteil::Undefine2AllTimeMatch_cast(args)...);
		}

		template<class ...Args>
		struct tuple2result_functor
		{
		private:
			std::tuple<Args...> keys;
		public:
			tuple2result_functor(const Args&...args)
				:keys(args...)
			{}

			auto operator()(const data_type&vals)const
				->decltype(tupleple::filter<deteil::Key_is_ID>(tupleple::zip(keys, vals)))
			{
				return tupleple::filter<deteil::Key_is_ID>(tupleple::zip(keys, vals));
			}
			using result_type = decltype(std::declval<tuple2result_functor>()(std::declval<data_type>()));
		};
		template<class ...Args>
		static tuple2result_functor<Args...> make_tuple2result_functor(const Args&...args)
		{
			return tuple2result_functor<Args...>(args...);
		}

		template<class ...Args>
		auto operator()(const Args&...args)
			->decltype(filter(args...) | boost::adaptors::transformed(make_tuple2result_functor(deteil::IDorNoID(args)...)))
		{
			return filter(args...) |
				boost::adaptors::transformed(make_tuple2result_functor(deteil::IDorNoID(args)...));
		}


		void add(const T&...args)
		{
			range_.emplace_back(args...);
		}
	};
}