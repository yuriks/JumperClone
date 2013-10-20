#pragma once
#include "EntitySystem.hpp"
#include <array>
#include <cassert>
#include "index_tuple.hpp"

template <size_t num_types>
struct EntityQueryIter {
	static_assert(num_types >= 1, "Need to query at least one type.");

	EntityWorld* world;
	std::array<EntityWorld::EntityComponentMap::const_iterator, num_types> iters;
	std::array<EntityWorld::EntityComponentMap::const_iterator, num_types> end_iters;

	EntityQueryIter()
		: world(nullptr)
	{}

	EntityQueryIter(EntityWorld* world, const std::array<ComponentTypeId, num_types>& types)
		: world(world)
	{
		for (size_t i = 0; i < num_types; ++i) {
			auto& x = world->components_by_component_type[types[i]];
			iters[i] = x.data.cbegin();
			end_iters[i] = x.data.cend();
			if (iters[i] == end_iters[i]) {
				invalidate();
				return;
			}
		}

		skip_non_matching();
	}

	bool operator==(const EntityQueryIter& o) {
		if (world == o.world) {
			if (world != nullptr) {
				return iters == o.iters;
			}
			return true;
		}
		return false;
	}

	bool operator!=(const EntityQueryIter& o) {
		return !(*this == o);
	}

	void invalidate() {
		world = nullptr;
	}

	void skip_non_matching() {
		assert(world != nullptr);

		bool advanced;
		do {
			EntityId max_id = std::get<0>(*iters[0]);
			for (size_t i = 1; i < num_types; ++i) {
				max_id = std::max(max_id, std::get<0>(*iters[i]));
			}

			advanced = false;
			for (size_t i = 0; i < num_types; ++i) {
				while (std::get<0>(*iters[i]) < max_id) {
					advanced = true;
					++iters[i];
					if (iters[i] == end_iters[i]) {
						invalidate();
						return;
					}
				}
			}
		} while (advanced);
	}

	EntityQueryIter& operator++() {
		assert(world != nullptr);

		for (size_t i = 0; i < num_types; ++i) {
			if (++iters[i] == end_iters[i]) {
				invalidate();
				break;
			}
		}

		return *this;
	}

	std::array<ComponentHandle, num_types> operator*() const {
		assert(world != nullptr);
		std::array<ComponentHandle, num_types> ret;
		for (size_t i = 0; i < num_types; ++i) {
			ret[i] = std::get<1>(*iters[i]);
		}
		return ret;
	}
};

/** Helper to allow using range-for on a query. */
template <int num_types>
struct EntityQuery {
	static_assert(num_types >= 1, "Need to query at least one type.");

	EntityWorld* world;
	std::array<ComponentTypeId, num_types> types;

	EntityQuery(EntityWorld* world, const std::array<ComponentTypeId, num_types>& types)
		: world(world), types(types)
	{}

	EntityQueryIter<num_types> begin() {
		return EntityQueryIter<num_types>(world, types);
	}

	EntityQueryIter<num_types> end() {
		return EntityQueryIter<num_types>();
	}
};

template <typename T, typename... Tail>
EntityQuery<1 + sizeof...(Tail)> query(EntityWorld& world, T head, Tail... tail) {
	std::array<T, 1 + sizeof...(Tail)> a = {head, tail ...};
	return EntityQuery<1 + sizeof...(Tail)>(&world, a);
}

template <typename Fn, typename Tup, size_t... i>
void query_for_each_impl(const Tup& pools, const Fn& fn, const std::array<ComponentHandle, sizeof...(i)>& handles, index_tuple<i...>) {
	fn(*(std::get<i>(pools)[std::get<i>(handles)])...);
}

template <typename Fn, typename... Comp>
void query_for_each(EntityWorld& world, const std::tuple<yks::ObjectPool<Comp>&...>& pools, const Fn& fn) {
	for (auto handles : query(world, Comp::component_id...)) {
		query_for_each_impl(pools, fn, handles, make_indexes<Comp...>::type());
	}
}
