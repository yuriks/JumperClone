#pragma once

/** Equivalent to C++14's std::integer_sequence. Taken from http://stackoverflow.com/a/6454211/42029 */

template<size_t...> struct index_tuple {};

template<size_t I, typename IndexTuple, typename... Types>
struct make_indexes_impl;

template<size_t I, size_t... Indexes, typename T, typename ... Types>
struct make_indexes_impl<I, index_tuple<Indexes...>, T, Types...> {
	typedef typename make_indexes_impl<I + 1, index_tuple<Indexes..., I>, Types...>::type type;
};

template<size_t I, size_t... Indexes>
struct make_indexes_impl<I, index_tuple<Indexes...> > {
	typedef index_tuple<Indexes...> type;
};

template<typename ... Types>
struct make_indexes : make_indexes_impl<0, index_tuple<>, Types...> {};