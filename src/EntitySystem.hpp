#pragma once
#include "Handle.hpp"
#include "SortedVector.hpp"
#include "memory/ObjectPool.hpp"
#include <cstdint>
#include <string>
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

typedef yks::Handle ComponentHandle;

typedef yks::Handle EntityId;
struct Entity {
	std::string name;
	SortedVector<std::tuple<ComponentTypeId, ComponentHandle>> components;

	Entity() {}
	Entity(const std::string& name)
		: name(name)
	{}
};

struct EntityWorld {
	typedef SortedVector<std::tuple<EntityId, ComponentHandle>> EntityComponentMap;
	
	std::vector<ComponentType> component_types;
	yks::ObjectPool<Entity> entities;
	std::vector<EntityComponentMap> components_by_component_type;

	bool typeExists(ComponentTypeId type);

	void addComponentType(ComponentTypeId id, const std::string& name);
	EntityId createEntity(const std::string& name);
	void addComponentToEntity(EntityId entity, ComponentTypeId type, ComponentHandle handle);
	void removeComponentFromEntity(EntityId entity, ComponentTypeId type);

	template <typename C, typename... Args>
	yks::Handle addComponentToEntity(yks::ObjectPool<C>& pool, EntityId entity, Args&&... params) {
		yks::Handle h = pool.emplace(std::forward<Args>(params)...);
		addComponentToEntity(entity, C::component_id, h);
		return h;
	}
};
