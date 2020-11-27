#pragma once
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <functional>
#include "../utils/utils.hpp"

// sdk classes
#include "engine.hpp"
#include "surface.hpp"
#include "client.hpp"
#include "netvar.hpp"
#include "entlist.hpp"
#include "mat_system.hpp"
#include "mdl_info.hpp"
#include "mdl_render.hpp"
#include "render_view.hpp"
#include "ucmd.hpp"
#include "player.hpp"
#include "globals.hpp"
#include "trace.hpp"
#include "physics.hpp"
#include "mem_alloc.hpp"
#include "mdl_cache.hpp"
#include "prediction.hpp"
#include "input.hpp"
#include "events.hpp"
#include "glow.hpp"
#include "cvar.hpp"
#include "panel.hpp"

enum
{
	PITCH = 0,	// up / down
	YAW,		// left / right
	ROLL		// fall over
};

struct viewsetup_t
{
	int			x, x_old;
	int			y, y_old;
	int			width, width_old;
	int			height, height_old;
	bool		m_bOrtho;
	float		m_OrthoLeft;
	float		m_OrthoTop;
	float		m_OrthoRight;
	float		m_OrthoBottom;
	bool		m_bCustomViewMatrix;
	matrix3x4_t	m_matCustomViewMatrix;
	char		pad_0x68[0x48];
	float		fov;
	float		fovViewmodel;
	vec3_t		origin;
	vec3_t		angles;
	float		zNear;
	float		zFar;
	float		zNearViewmodel;
	float		zFarViewmodel;
	float		m_flAspectRatio;
	float		m_flNearBlurDepth;
	float		m_flNearFocusDepth;
	float		m_flFarFocusDepth;
	float		m_flFarBlurDepth;
	float		m_flNearBlurRadius;
	float		m_flFarBlurRadius;
	int			m_nDoFQuality;
	int			m_nMotionBlurMode;
	float		m_flShutterTime;
	vec3_t		m_vShutterOpenPosition;
	vec3_t		m_shutterOpenAngles;
	vec3_t		m_vShutterClosePosition;
	vec3_t		m_shutterCloseAngles;
	float		m_flOffCenterTop;
	float		m_flOffCenterBottom;
	float		m_flOffCenterLeft;
	float		m_flOffCenterRight;
	int			m_EdgeBlur;
};

namespace csgo {
	constexpr auto pi = 3.14159265358979f;

	namespace i {
		extern c_surface* surface;
		extern c_engine* engine;
		extern c_client* client;
		extern c_entlist* ent_list;
		extern c_panel* panel;
		extern c_matsys* mat_sys;
		extern c_mdlinfo* mdl_info;
		extern c_mdlrender* mdl_render;
		extern c_renderview* render_view;
		extern c_globals* globals;
		extern c_phys* phys;
		extern c_engine_trace* trace;
		extern c_glow_obj_mgr* glow_obj_mgr;
		extern c_clientstate* client_state;
		extern c_mem_alloc* mem_alloc;
		extern c_prediction* pred;
		extern c_move_helper* move_helper;
		extern c_movement* move;
		extern mdl_cache_t* mdl_cache;
		extern c_input* input;
		extern ICVar* cvar;
		extern c_game_event_mgr* events;
		extern IDirect3DDevice9* dev;
	}

	namespace render {
		bool screen_transform(vec3_t& screen, vec3_t& origin);
		bool world_to_screen(vec3_t& screen, vec3_t& origin);
	}

	namespace util {
		void trace_line(const vec3_t& start, const vec3_t& end, std::uint32_t mask, const entity_t* ignore, trace_t* ptr);
		void clip_trace_to_players(const vec3_t& start, const vec3_t& end, std::uint32_t mask, trace_filter_t* filter, trace_t* trace_ptr);
	}

	constexpr int time2ticks(float t) {
		return static_cast<int>(t / i::globals->m_ipt + 0.5f);
	}

	constexpr float ticks2time(int t) {
		return static_cast<float>(t) * i::globals->m_ipt;
	}

	std::deque<vec3_t> get_multiple_points_for_hitbox(player_t* local, player_t* entity, int iHitbox, int iHeadScale, int iBodyScale);

	mstudiobbox_t* get_hitbox(player_t* entity, int hitbox_index);

	vec3_t get_hitbox_position(player_t* entity, int hitbox_id);

	void for_each_player(std::function< void(player_t*) > fn);
	float normalize(float ang);
	float normalize_yaw(float f);
	void clamp(vec3_t& ang);
	float rad2deg(float rad);
	float deg2rad(float deg);
	void sin_cos(float radians, float* sine, float* cosine);
	vec3_t calc_angle(const vec3_t& from, const vec3_t& to);
	vec3_t vec_angle(vec3_t vec);
	void vector_angles(const vec3_t& forward, vec3_t& angles);
	vec3_t angle_vec(vec3_t angle);
	void util_tracehull(const vec3_t& start, const vec3_t& end, const vec3_t& mins, const vec3_t& maxs, unsigned int mask, const void* ignore, trace_t* tr);
	void util_traceline(const vec3_t& start, const vec3_t& end, unsigned int mask, const void* ignore, trace_t* tr);
	void rotate_movement(ucmd_t* ucmd);

#define DOT_PROD( a, b ) ( a.x * b.x + a.y * b.y + a.z * b.z )

#define VEC_TRANSFORM( in1, in2, out )																	\
out.x = DOT_PROD( in1, vec3_t( in2 [ 0 ] [ 0 ], in2 [ 0 ] [ 1 ], in2 [ 0 ] [ 2 ] ) ) + in2 [ 0 ] [ 3 ];	\
out.y = DOT_PROD( in1, vec3_t( in2 [ 1 ] [ 0 ], in2 [ 1 ] [ 1 ], in2 [ 1 ] [ 2 ] ) ) + in2 [ 1 ] [ 3 ];	\
out.z = DOT_PROD( in1, vec3_t( in2 [ 2 ] [ 0 ], in2 [ 2 ] [ 1 ], in2 [ 2 ] [ 2 ] ) ) + in2 [ 2 ] [ 3 ]

	template<typename t>
	void clampv(t& n, const t& lower, const t& upper);

	template < typename t >
	t create_interface(const char* module, const char* iname);

	bool init();
}
