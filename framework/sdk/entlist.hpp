#pragma once
#include "../utils/vfunc.hpp"
#include "../utils/padding.hpp"

class c_entlist {
public:
	template < typename t >
	t get(int i) {
		using getcliententity_fn = t(__thiscall*)(void*, int);
		return vfunc< getcliententity_fn >(this, 3)(this, i);
	}

	template < typename t >
	t get_by_handle(uintptr_t h) {
		using getcliententityfromhandle_fn = t(__thiscall*)(void*, uintptr_t);
		return vfunc< getcliententityfromhandle_fn >(this, 4)(this, h);
	}
	void* get_client_entity_handle(uintptr_t handle) {
		using original_fn = void* (__thiscall*)(c_entlist*, uintptr_t);
		return (*(original_fn**) this)[4](this, handle);
	}

	void* get_client_entity(int index) {
		using original_fn = void* (__thiscall*)(c_entlist*, int);
		return (*(original_fn**) this)[3](this, index);
	}
};
