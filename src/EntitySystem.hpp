#pragma once
#include "SortedVector.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <tuple>
#include <vector>

typedef uint32_t ComponentTypeId;
static const ComponentTypeId invalid_component_type = ~0;
struct ComponentType {
	std::string name;

	ComponentType() {}
	ComponentType(const std::string& name)
		: name(name)
	{}
};

typedef uint32_t EntityId;
static const EntityId invalid_entity = ~0;
struct Entity {
	std::string name;

	Entity() {}
	Entity(const std::string& name)
		: name(name)
	{}
};

typedef uint32_t ComponentId;
static const ComponentId invalid_component = ~0;

struct EntityWorld {
	typedef SortedVector<std::tuple<EntityId, ComponentId>> EntityComponentMap;
	typedef SortedVector<std::tuple<ComponentTypeId, ComponentId>> ComponentEntityMap;

	std::vector<ComponentType> component_types;
	std::vector<Entity> entities;
	std::vector<ComponentId> component_ids;

	std::vector<EntityComponentMap> entities_by_component;
	std::vector<ComponentEntityMap> components_by_entity;

	void addComponentType(ComponentTypeId id, const std::string& name) {
		if (component_types.size() < id + 1) {
			component_types.resize(id + 1);
			component_ids.resize(id + 1, invalid_component);
			entities_by_component.resize(id + 1);
		} else {
			assert(component_ids[id] == invalid_component);
		}
		component_types[id] = ComponentType(name);
		component_ids[id] = 0;
		assert(component_types.size() == component_ids.size() && component_types.size() == entities_by_component.size());
	}

	EntityId createEntity(const std::string& name) {
		entities.emplace_back(name);
		components_by_entity.emplace_back();
		return entities.size() - 1;
	}

	ComponentId addComponentToEntity(EntityId entity, ComponentTypeId type) {
		assert(type < component_ids.size() && component_ids[type] != invalid_component);
		ComponentId id = component_ids[type]++;

		components_by_entity[entity].insert(std::make_tuple(type, id));
		entities_by_component[type].insert(std::make_tuple(entity, id));
		return id;
	}

	void removeComponentFromEntity(EntityId entity, ComponentTypeId type) {
		assert(type < component_ids.size() && component_ids[type] != invalid_component);

		components_by_entity[entity].remove(type);
		entities_by_component[type].remove(entity);
	}
};

template <size_t num_types>
struct EntityQueryIter {
	static_assert(num_types >= 1, "Need to query at least one type.");

	EntityWorld* world;
	std::array<EntityWorld::ComponentEntityMap::const_iterator, num_types> iters;
	std::array<EntityWorld::ComponentEntityMap::const_iterator, num_types> end_iters;

	EntityQueryIter()
		: world(nullptr)
	{}

	EntityQueryIter(EntityWorld* world, const std::array<ComponentTypeId, num_types>& types)
		: world(world)
	{
		for (size_t i = 0; i < num_types; ++i) {
			auto& x = world->entities_by_component[types[i]];
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
			ComponentId max_id = std::get<0>(*iters[0]);
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

	std::array<ComponentId, num_types> operator*() const {
		assert(world != nullptr);
		std::array<ComponentId, num_types> ret;
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

template<class T, class... Tail>
static EntityQuery<1 + sizeof...(Tail)> query(EntityWorld& world, T head, Tail... tail) {
	std::array<T, 1 + sizeof...(Tail)> a = {head, tail ...};
	return EntityQuery<1 + sizeof...(Tail)>(&world, a);
}
