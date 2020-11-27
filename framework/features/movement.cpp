#include "features.hpp"

void features::movement::run(ucmd_t* ucmd) {
	if (!g::local
		|| !g::local->alive()
		|| g::local->movetype() == movetypes::movetype_noclip
		|| g::local->movetype() == movetypes::movetype_fly
		|| g::local->movetype() == movetypes::movetype_ladder
		)
		return;

	if (ucmd->m_buttons & IN_JUMP && !(g::local->flags() & FL_ONGROUND) && vars.movement.bhop)
		ucmd->m_buttons &= ~IN_JUMP;
}

void AngleVectors(const vec3_t& angles, vec3_t* forward, vec3_t* right, vec3_t* up) {
	float cp = std::cos(csgo::deg2rad(angles.x)), sp = std::sin(csgo::deg2rad(angles.x));
	float cy = std::cos(csgo::deg2rad(angles.y)), sy = std::sin(csgo::deg2rad(angles.y));
	float cr = std::cos(csgo::deg2rad(angles.z)), sr = std::sin(csgo::deg2rad(angles.z));

	if (forward) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right) {
		right->x = -1.f * sr * sp * cy + -1.f * cr * -sy;
		right->y = -1.f * sr * sp * sy + -1.f * cr * cy;
		right->z = -1.f * sr * cp;
	}

	if (up) {
		up->x = cr * sp * cy + -sr * -sy;
		up->y = cr * sp * sy + -sr * cy;
		up->z = cr * cp;
	}
}

void features::movement::fixmove(ucmd_t* ucmd, vec3_t wish_angles) {
	vec3_t view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	AngleVectors(wish_angles, &view_fwd, &view_right, &view_up);
	AngleVectors(ucmd->m_angs, &cmd_fwd, &cmd_right, &cmd_up);

	const auto v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const auto v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const auto v12 = sqrtf(view_up.z * view_up.z);

	const vec3_t norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const vec3_t norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const vec3_t norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const auto v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
	const auto v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
	const auto v18 = sqrtf(cmd_up.z * cmd_up.z);

	const vec3_t norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
	const vec3_t norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
	const vec3_t norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

	const auto v22 = norm_view_fwd.x * ucmd->m_fmove;
	const auto v26 = norm_view_fwd.y * ucmd->m_fmove;
	const auto v28 = norm_view_fwd.z * ucmd->m_fmove;
	const auto v24 = norm_view_right.x * ucmd->m_smove;
	const auto v23 = norm_view_right.y * ucmd->m_smove;
	const auto v25 = norm_view_right.z * ucmd->m_smove;
	const auto v30 = norm_view_up.x * ucmd->m_umove;
	const auto v27 = norm_view_up.z * ucmd->m_umove;
	const auto v29 = norm_view_up.y * ucmd->m_umove;

	ucmd->m_fmove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
		+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
		+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
	ucmd->m_smove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
		+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
		+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
	ucmd->m_umove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
		+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
		+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

	wish_angles = ucmd->m_angs;

}
