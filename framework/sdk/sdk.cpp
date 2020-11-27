#include "sdk.hpp"
#include "netvar.hpp"

c_entlist* csgo::i::ent_list = nullptr;
c_matsys* csgo::i::mat_sys = nullptr;
c_mdlinfo* csgo::i::mdl_info = nullptr;
c_mdlrender* csgo::i::mdl_render = nullptr;
c_renderview* csgo::i::render_view = nullptr;
c_client* csgo::i::client = nullptr;
c_surface* csgo::i::surface = nullptr;
c_engine* csgo::i::engine = nullptr;
c_globals* csgo::i::globals = nullptr;
c_phys* csgo::i::phys = nullptr;
c_engine_trace* csgo::i::trace = nullptr;
c_glow_obj_mgr* csgo::i::glow_obj_mgr = nullptr;
c_clientstate* csgo::i::client_state = nullptr;
c_mem_alloc* csgo::i::mem_alloc = nullptr;
c_prediction* csgo::i::pred = nullptr;
c_move_helper* csgo::i::move_helper = nullptr;
c_movement* csgo::i::move = nullptr;
c_panel* csgo::i::panel = nullptr;
mdl_cache_t* csgo::i::mdl_cache = nullptr;
c_input* csgo::i::input = nullptr;
ICVar* csgo::i::cvar = nullptr;
c_game_event_mgr* csgo::i::events = nullptr;
IDirect3DDevice9* csgo::i::dev = nullptr;

c_mdl_cache_critical_section::c_mdl_cache_critical_section() {
	csgo::i::mdl_cache->begin_lock();
}

c_mdl_cache_critical_section::~c_mdl_cache_critical_section() {
	csgo::i::mdl_cache->end_lock();
}

std::deque<vec3_t> csgo::get_multiple_points_for_hitbox(player_t* local, player_t* entity, int iHitbox, int iHeadScale, int iBodyScale) /* stacker */
{
	auto angle_vectors = [] (const vec3_t& angles, vec3_t* forward)
	{
		float sp, sy, cp, cy;

		csgo::sin_cos(csgo::deg2rad(angles[YAW]), &sy, &cy);
		csgo::sin_cos(csgo::deg2rad(angles[PITCH]), &sp, &cp);

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	};

	auto vectortransform_wrapper = [] (const vec3_t& in1, const matrix3x4_t& in2, vec3_t& out)
	{
		auto vectortransform = [] (const float* in1, const matrix3x4_t& in2, float* out)
		{
			auto dotproducts = [] (const float* v1, const float* v2)
			{
				return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
			};
			out[0] = dotproducts(in1, in2[0]) + in2[0][3];
			out[1] = dotproducts(in1, in2[1]) + in2[1][3];
			out[2] = dotproducts(in1, in2[2]) + in2[2][3];
		};
		vectortransform(&in1.x, in2, &out.x);
	};

	studiohdr_t* pstudiomodel = csgo::i::mdl_info->studio_mdl(entity->mdl());
	mstudiohitboxset_t* set = pstudiomodel->hitbox_set(0);
	mstudiobbox_t* hitbox = set->hitbox(iHitbox);

	std::deque<vec3_t> vecarray;

	vec3_t max;
	vec3_t min;
	vectortransform_wrapper(hitbox->m_bbmax, entity->get_bone_matrix(hitbox->m_bone), max);
	vectortransform_wrapper(hitbox->m_bbmin, entity->get_bone_matrix(hitbox->m_bone), min);

	auto center = (min + max) * 0.5f;

	vec3_t currentangles = csgo::calc_angle(center, local->eyes());
	vec3_t forward;
	angle_vectors(currentangles, &forward);

	vec3_t right = forward.cross_product(vec3_t(0, 0, 1));
	vec3_t left = vec3_t(-right.x, -right.y, right.z);

	vec3_t top = vec3_t(0, 0, 1);
	vec3_t bot = vec3_t(0, 0, -1);

	switch (iHitbox) {
	case hitbox_head:
		for (auto i = 0; i < 4; ++i)
			vecarray.emplace_front(center);

		vecarray[1] += top * (hitbox->m_radius * iHeadScale / 100.f);
		vecarray[2] += right * (hitbox->m_radius * iHeadScale / 100.f);
		vecarray[3] += left * (hitbox->m_radius * iHeadScale / 100.f);
		break;

	default:
		for (auto i = 0; i < 4; ++i)
			vecarray.emplace_back(center);

		vecarray[1] += top * (hitbox->m_radius * iBodyScale / 100.f);
		vecarray[2] += right * (hitbox->m_radius * iBodyScale / 100.f);
		vecarray[3] += left * (hitbox->m_radius * iBodyScale / 100.f);
		break;
	}
	return vecarray;
}

bool csgo::render::screen_transform(vec3_t& screen, vec3_t& origin) {
	static auto view_matrix = pattern::search("client.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9").add(3).deref().add(176).get< std::uintptr_t >();

	const auto& world_matrix = *(matrix3x4_t*) view_matrix;

	screen.x = world_matrix[0][0] * origin.x + world_matrix[0][1] * origin.y + world_matrix[0][2] * origin.z + world_matrix[0][3];
	screen.y = world_matrix[1][0] * origin.x + world_matrix[1][1] * origin.y + world_matrix[1][2] * origin.z + world_matrix[1][3];
	screen.z = 0.0f;

	const auto w = world_matrix[3][0] * origin.x + world_matrix[3][1] * origin.y + world_matrix[3][2] * origin.z + world_matrix[3][3];

	if (w < 0.001f) {
		screen.x *= 100000.0f;
		screen.y *= 100000.0f;

		return true;
	}

	screen.x *= 1.0f / w;
	screen.y *= 1.0f / w;

	return false;
}

mstudiobbox_t* csgo::get_hitbox(player_t* entity, int hitbox_index)
{
	if (entity->dormant() || entity->health() <= 0)
		return NULL;

	const auto pModel = entity->mdl();
	if (!pModel)
		return NULL;

	auto pStudioHdr = csgo::i::mdl_info->studio_mdl(pModel);
	if (!pStudioHdr)
		return NULL;

	auto pSet = pStudioHdr->hitbox_set(0);
	if (!pSet)
		return NULL;

	if (hitbox_index >= pSet->m_num_hitboxes || hitbox_index < 0)
		return NULL;

	return pSet->hitbox(hitbox_index);
}

vec3_t csgo::get_hitbox_position(player_t* entity, int hitbox_id)
{
	auto hitbox = csgo::get_hitbox(entity, hitbox_id);
	if (!hitbox)
		return vec3_t(0, 0, 0);

	auto bone_matrix = entity->get_bone_matrix(hitbox->m_bone);

	vec3_t bbmin, bbmax;
	VEC_TRANSFORM(hitbox->m_bbmin, bone_matrix, bbmin);
	VEC_TRANSFORM(hitbox->m_bbmax, bone_matrix, bbmax);

	return (bbmin + bbmax) * 0.5f;
}

bool csgo::render::world_to_screen(vec3_t& screen, vec3_t& origin) {
	if (!screen_transform(screen, origin)) {
		int width, height;
		csgo::i::engine->get_screen_size(width, height); // TODO: use the one we calculate in sceneend instead

		screen.x = (width * 0.5f) + (screen.x * width) * 0.5f;
		screen.y = (height * 0.5f) - (screen.y * height) * 0.5f;

		return true;
	}

	return false;
}

void csgo::util::trace_line(const vec3_t& start, const vec3_t& end, std::uint32_t mask, const entity_t* ignore, trace_t* ptr) {
	trace_filter_t filter((void*) ignore);

	ray_t ray;
	ray.init(start, end);

	csgo::i::trace->trace_ray(ray, mask, &filter, ptr);

	/*
	using fn = void( __fastcall* )( const vec3_t&, const vec3_t&, std::uint32_t, const entity_t*, std::uint32_t, trace_t* );
	static auto util_traceline = pattern::search( STR( "client.dll" ), STR( "55 8B EC 83 E4 F0 83 EC 7C 56 52" ) ).get< fn >( );
	util_traceline( start, end, mask, ignore, 0, ptr );
	*/
}

void csgo::util::clip_trace_to_players(const vec3_t& start, const vec3_t& end, std::uint32_t mask, trace_filter_t* filter, trace_t* trace_ptr) {
	static auto util_cliptracetoplayers = pattern::search("client.dll", "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC D8 ? ? ? 0F 57 C9").get< std::uint32_t >();

	if (!util_cliptracetoplayers)
		return;

	/* changed from ptr to trace_ptr cuz assmebly */
	__asm {
		mov	eax, filter
		lea ecx, trace_ptr
		push ecx
		push eax
		push mask
		lea edx, end
		lea ecx, start
		call util_cliptracetoplayers
		add esp, 0xC
	}

	/*
	using fn = void( __fastcall* )( const vec3_t&, const vec3_t&, std::uint32_t, trace_filter_t*, trace_t* );
	static auto util_cliptracetoplayers = pattern::search( STR( "client.dll" ), STR( "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC D8 ? ? ? 0F 57 C9" ) ).get< fn >( );
	util_cliptracetoplayers( start, end, mask, filter, ptr );
	*/
}

void csgo::for_each_player(std::function< void(player_t*) > fn) {
	for (auto i = 1; i <= i::globals->m_max_clients; i++) {
		auto entity = i::ent_list->get< player_t* >(i);

		if (!entity->valid())
			continue;

		fn(entity);
	}
}

float csgo::normalize(float ang) {
	return std::remainderf(ang, 360.0f);
}

float csgo::normalize_yaw(float f)
{
	while (f < -180.0f)
		f += 360.0f;

	while (f > 180.0f)
		f -= 360.0f;

	return f;
}


void csgo::clamp(vec3_t& ang) {
	auto flt_valid = [] (float val) {
		return std::isfinite(val) && !std::isnan(val);
	};

	for (auto i = 0; i < 3; i++)
		if (!flt_valid(ang[i]))
			return;

	ang.x = std::clamp(normalize(ang.x), -89.0f, 89.0f);
	ang.y = std::clamp(normalize(ang.y), -180.0f, 180.0f);
	ang.z = 0.0f;
}


float csgo::rad2deg(float rad) {
	float result = rad * (180.0f / pi);
	return result;
}

float csgo::deg2rad(float deg) {
	float result = deg * (pi / 180.0f);
	return result;
}

void csgo::sin_cos(float radians, float* sine, float* cosine) {
	*sine = std::sinf(radians);
	*cosine = std::cosf(radians);
}

vec3_t csgo::calc_angle(const vec3_t& from, const vec3_t& to) {
	return csgo::vec_angle(to - from);
}

vec3_t csgo::vec_angle(vec3_t vec) {
	vec3_t ret;

	if (vec.x == 0.0f && vec.y == 0.0f) {
		ret.x = (vec.z > 0.0f) ? 270.0f : 90.0f;
		ret.y = 0.0f;
	}
	else {
		ret.x = rad2deg(std::atan2f(-vec.z, vec.length_2d()));
		ret.y = rad2deg(std::atan2f(vec.y, vec.x));

		if (ret.y < 0.0f)
			ret.y += 360.0f;

		if (ret.x < 0.0f)
			ret.x += 360.0f;
	}

	ret.z = 0.0f;

	return ret;
}

#define M_PI 3.14159265358979323846f
void csgo::vector_angles(const vec3_t& forward, vec3_t& angles)
{
	vec3_t view;

	if (!forward[0] && !forward[1])
	{
		view[0] = 0.0f;
		view[1] = 0.0f;
	}
	else
	{
		view[1] = atan2(forward[1], forward[0]) * 180.0f / M_PI;

		if (view[1] < 0.0f)
			view[1] += 360.0f;

		view[2] = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		view[0] = atan2(forward[2], view[2]) * 180.0f / M_PI;
	}

	angles[0] = -view[0];
	angles[1] = view[1];
	angles[2] = 0.f;
}

vec3_t csgo::angle_vec(vec3_t angle) {
	vec3_t ret;

	float sp, sy, cp, cy;

	sin_cos(deg2rad(angle.y), &sy, &cy);
	sin_cos(deg2rad(angle.x), &sp, &cp);

	ret.x = cp * cy;
	ret.y = cp * sy;
	ret.z = -sp;

	return ret;
}

void csgo::util_traceline(const vec3_t& start, const vec3_t& end, unsigned int mask, const void* ignore, trace_t* tr) {
	using fn = void(__fastcall*)(const vec3_t&, const vec3_t&, std::uint32_t, const void*, std::uint32_t, trace_t*);
	static auto utl = pattern::search("client.dll", "55 8B EC 83 E4 F0 83 EC 7C 56 52").get< fn >();
	utl(start, end, mask, ignore, 0, tr);
}

void csgo::util_tracehull(const vec3_t& start, const vec3_t& end, const vec3_t& mins, const vec3_t& maxs, unsigned int mask, const void* ignore, trace_t* tr) {
	using fn = void(__fastcall*)(const vec3_t&, const vec3_t&, const vec3_t&, const vec3_t&, unsigned int, const void*, std::uint32_t, trace_t*);
	static auto utl = pattern::search("client.dll", "E8 ? ? ? ? 8B 07 83 C4 20").resolve_rip().get< fn >();
	utl(start, end, mins, maxs, mask, ignore, 0, tr);
}

template < typename t >
t csgo::create_interface(const char* module, const char* iname) {
	using createinterface_fn = void* (__cdecl*)(const char*, int);
	const auto createinterface_export = GetProcAddress(GetModuleHandleA(module), "CreateInterface");
	const auto fn = (createinterface_fn) createinterface_export;

	return reinterpret_cast<t>(fn(iname, 0));
}

void csgo::rotate_movement(ucmd_t* ucmd) {
	vec3_t ang;
	csgo::i::engine->get_viewangles(ang);

	const auto vec_move = vec3_t(ucmd->m_fmove, ucmd->m_smove, ucmd->m_umove);
	const auto ang_move = csgo::vec_angle(vec_move);
	const auto speed = vec_move.length_2d();
	const auto rotation = deg2rad(ucmd->m_angs.y - ang.y + ang_move.y);

	ucmd->m_fmove = std::cosf(rotation) * speed;
	ucmd->m_smove = std::sinf(rotation) * speed;
}

bool csgo::init() {
	i::globals = pattern::search("client.dll", "A1 ? ? ? ? F3 0F 10 8F ? ? ? ? F3 0F 10 05 ? ? ? ? ? ? ? ? ? 0F 2F C1 0F 86").add(1).deref().deref().get< c_globals* >();
	i::ent_list = create_interface< c_entlist* >("client.dll", "VClientEntityList003");
	i::mat_sys = create_interface< c_matsys* >("materialsystem.dll", "VMaterialSystem080");
	i::mdl_info = create_interface< c_mdlinfo* >("engine.dll", "VModelInfoClient004");
	i::mdl_render = create_interface< c_mdlrender* >("engine.dll", "VEngineModel016");
	i::render_view = create_interface< c_renderview* >("engine.dll", "VEngineRenderView014");
	i::client = create_interface< c_client* >("client.dll", "VClient018");
	i::panel = create_interface< c_panel* >("vgui2.dll", "VGUI_Panel009");
	i::surface = create_interface< c_surface* >("vguimatsurface.dll", "VGUI_Surface031");
	i::engine = create_interface< c_engine* >("engine.dll", "VEngineClient014");
	i::phys = create_interface< c_phys* >("vphysics.dll", "VPhysicsSurfaceProps001");
	i::trace = create_interface< c_engine_trace* >("engine.dll", "EngineTraceClient004");
	i::pred = create_interface< c_prediction* >("client.dll", "VClientPrediction001");
	i::move = create_interface< c_movement* >("client.dll", "GameMovement001");
	i::mdl_cache = create_interface< mdl_cache_t* >("client.dll", "MDLCache004");
	i::events = create_interface< c_game_event_mgr* >("engine.dll", "GAMEEVENTSMANAGER002");
	i::input = pattern::search("client.dll", "B9 ? ? ? ? FF 60 60").add(1).deref().get< c_input* >();
	i::cvar = create_interface< ICVar* >("vstdlib.dll", "VEngineCvar007");
	i::move_helper = **reinterpret_cast<c_move_helper***>(pattern::search("client.dll", "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01").add(2).get< std::uintptr_t >());
	i::client_state = **reinterpret_cast<c_clientstate***>(reinterpret_cast<std::uintptr_t>(vfunc< void* >(i::engine, 12)) + 16);
	i::mem_alloc = *(c_mem_alloc**) GetProcAddress(GetModuleHandleA("tier0.dll"), "g_pMemAlloc");
	i::dev = pattern::search("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C").add(1).deref().deref().get< IDirect3DDevice9* >();
	i::glow_obj_mgr = pattern::search("client.dll", "0F 11 05 ? ? ? ? 83 C8 01").add(3).deref().get< c_glow_obj_mgr* >();

	return true;
}
