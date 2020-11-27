#include <intrin.h>
#include "hooks.hpp"
#include "globals.hpp"
#include "minhook/minhook.h"
#include "renderer/d3d9.hpp"

/* features */
#include "features/features.hpp"
#include "utils/draw.hpp"
#include "menu/menu.hpp"
#include "menu/variables.hpp"
settings vars;

bool __fastcall fire_event_hk(REG);
void __fastcall override_view_hk(REG, viewsetup_t* setup);
void __fastcall drawmodelexecute_hk(REG, void* ctx, void* state, const mdlrender_info_t& info, matrix3x4_t* bone_to_world);
bool __fastcall createmove_hk(REG, float sampletime, ucmd_t* ucmd);
void __fastcall framestagenotify_hk(REG, int stage);
long __fastcall endscene_hk(REG, IDirect3DDevice9* device);
long __fastcall reset_hk(REG, IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* presentation_params);
void __fastcall lockcursor_hk(REG);
void __fastcall doextraboneprocessing_hk(REG, int a2, int a3, int a4, int a5, int a6, int a7);
vec3_t* __fastcall get_eye_angles_hk(REG);
bool __fastcall get_bool_hk(REG);
long __stdcall wndproc(HWND hwnd, std::uint32_t msg, std::uintptr_t wparam, std::uint32_t lparam);
bool __fastcall setupbones_hk(REG, matrix3x4_t* out, int max_bones, int mask, float curtime);
void __fastcall paint_traverse_hk(REG, int ipanel, bool force_repaint, bool allow_force);
void __fastcall sceneend_hk(REG);

decltype(&sceneend_hk) sceneend = nullptr;
decltype(&fire_event_hk) fire_event = nullptr;
decltype(&override_view_hk) override_view = nullptr;
decltype(&get_bool_hk) get_bool = nullptr;
decltype(&createmove_hk) createmove = nullptr;
decltype(&framestagenotify_hk) framestagenotify = nullptr;
decltype(&endscene_hk) endscene = nullptr;
decltype(&reset_hk) reset = nullptr;
decltype(&lockcursor_hk) lockcursor = nullptr;
decltype(&drawmodelexecute_hk) drawmodelexecute = nullptr;
decltype(&doextraboneprocessing_hk) doextraboneprocessing = nullptr;
decltype(&get_eye_angles_hk) get_eye_angles = nullptr;
decltype(&setupbones_hk) setupbones = nullptr;
decltype(&paint_traverse_hk) paint_traverse = nullptr;

/* event listener */
class c_event_handler : c_event_listener {
public:
	c_event_handler() {
		csgo::i::events->add_listener(this, "weapon_fire", false);
		csgo::i::events->add_listener(this, "player_say", false);
		csgo::i::events->add_listener(this, "player_hurt", false);
		csgo::i::events->add_listener(this, "bullet_impact", false);
	}

	virtual ~c_event_handler() {
		csgo::i::events->remove_listener(this);
	}

	virtual void fire_game_event(event_t* event) {
		if (!event || !g::local)
			return;

		if (!strcmp(event->get_name(), "player_hurt"))
			features::visuals::hitmarker_event(event);
	}

	int get_event_debug_id() override {
		return 42;
	}
};

std::unique_ptr< c_event_handler > event_handler;

/* fix event delays */
bool __fastcall fire_event_hk(REG) {
	auto local = csgo::i::ent_list->get< player_t* >(csgo::i::engine->get_local_player());

	if (!local->valid())
		return fire_event(REG_OUT);

	struct event_t {
	public:
		PAD(4);
		float delay;
		PAD(48);
		event_t* next;
	};

	auto ei = *(event_t**) (std::uintptr_t(csgo::i::client_state) + 0x4E6C);

	event_t* next = nullptr;

	if (!ei)
		return fire_event(REG_OUT);

	do {
		next = ei->next;
		ei->delay = 0.f;
		ei = next;
	} while (next);

	return fire_event(REG_OUT);
}

void __fastcall paint_traverse_hk(REG, int ipanel, bool force_repaint, bool allow_force) {
	static unsigned int vguiPanelID = NULL;
	static auto override_post_processing_disable = pattern::search("client.dll", "80 3D ? ? ? ? ? 53 56 57 0F 85").add(2).deref().get< bool* >();

	paint_traverse(REG_OUT, ipanel, force_repaint, allow_force);

	if (!vguiPanelID && !strcmp("MatSystemTopPanel", csgo::i::panel->get_name(ipanel)))
		vguiPanelID = ipanel;

	if (vguiPanelID == ipanel)
	{
		features::visuals::run();
	}
}

void __fastcall sceneend_hk(REG) {
	sceneend(REG_OUT);

	for (auto i = 0; i < csgo::i::glow_obj_mgr->m_size; i++) {
		if (csgo::i::glow_obj_mgr->m_glow_object_definitions[i].is_unused() || !csgo::i::glow_obj_mgr->m_glow_object_definitions[i].get_entity())
			continue;

		auto& glow_object = csgo::i::glow_obj_mgr->m_glow_object_definitions[i];

		auto entity = reinterpret_cast<player_t*>(glow_object.m_ent);
		player_t* local = csgo::i::ent_list->get<player_t*>(csgo::i::engine->get_local_player());
		bool player = entity->team() != local->team();

		if (!entity)
			continue;

		if (!local)
			continue;

		if (entity->client_class()->m_class_id == 40 && player && vars.players.glow)
		{
			glow_object.set(static_cast<float>(vars.players.glow_clr.r) / 255.0f, static_cast<float>(vars.players.glow_clr.g) / 255.0f, static_cast<float>(vars.players.glow_clr.b) / 255.0f, static_cast<float>(vars.players.glow_opacity) / 255.0f);
		}
	}
}

void __fastcall override_view_hk(REG, viewsetup_t* setup) {
	auto get_ideal_dist = [&] (float ideal_distance) {
		vec3_t inverse;
		csgo::i::engine->get_viewangles(inverse);

		inverse.x *= -1.0f, inverse.y += 180.0f;

		vec3_t direction = csgo::angle_vec(inverse);

		ray_t ray;
		trace_t trace;
		trace_filter_t filter(g::local);

		csgo::util_traceline(g::local->eyes(), g::local->eyes() + (direction * ideal_distance), 0x600400B, g::local, &trace);

		return (ideal_distance * trace.m_fraction) - 10.0f;
	};


	if (g::local)
	{
		if (vars.visuals.camera_fov)
			setup->fov = vars.visuals.camera_fov_range;
	}

	override_view(REG_OUT, setup);
}

void __fastcall drawmodelexecute_hk(REG, void* ctx, void* state, const mdlrender_info_t& info, matrix3x4_t* bone_to_world) {

	player_t* player = csgo::i::ent_list->get<player_t*>(info.m_entity_index);
	player_t* local = csgo::i::ent_list->get<player_t*>(csgo::i::engine->get_local_player());
	std::string m_mdl_name = csgo::i::mdl_info->mdl_name(info.m_model);

	static material_t* material = nullptr;
	static material_t* debugdrawflat = csgo::i::mat_sys->findmat("debug/debugdrawflat", nullptr);
	static material_t* debugambientcube = csgo::i::mat_sys->findmat("debug/debugambientcube", nullptr);

	debugambientcube->increment_reference_count();
	debugdrawflat->increment_reference_count();

	if (csgo::i::mdl_render->is_forced_mat_override()) // glow fix (not glow for chams)
	{
		drawmodelexecute(REG_OUT, ctx, state, info, bone_to_world);
		return;
	}

	switch (vars.players.mdl)
	{
	case 0: /* regular */
		material = debugambientcube;
		break;
	case 1: /* flat */
		material = debugdrawflat;
		break;
	}

	if (player && vars.players.model_override)
	{
		if (player->valid())
		{
			bool m_is_player = m_mdl_name.find("models/player") != std::string::npos;
			bool enemy = player->team() != local->team();

			if (enemy)
			{
				if (m_is_player)
				{
					if (vars.players.model_xqz)
					{
						material->alpha_modulate(vars.players.model_xqz_alpha);
						material->color_modulate(vars.players.model_xqz_clr.r, vars.players.model_xqz_clr.g, vars.players.model_xqz_clr.b);
						material->set_material_var_flag(MATERIAL_VAR_IGNOREZ, true);
						csgo::i::mdl_render->force_mat(material);
						drawmodelexecute(REG_OUT, ctx, state, info, bone_to_world);
					}
					if (vars.players.model_visible)
					{
						material->alpha_modulate(vars.players.model_vis_alpha);
						material->color_modulate(vars.players.model_vis_clr.r, vars.players.model_vis_clr.g, vars.players.model_vis_clr.b);
						material->set_material_var_flag(MATERIAL_VAR_IGNOREZ, false);
						csgo::i::mdl_render->force_mat(material);
						drawmodelexecute(REG_OUT, ctx, state, info, bone_to_world);
					}
				}
			}
		}
	}

	drawmodelexecute(REG_OUT, ctx, state, info, bone_to_world);
	csgo::i::mdl_render->force_mat(nullptr);
}

bool __fastcall createmove_hk(REG, float sampletime, ucmd_t* ucmd) {
	if (!ucmd || !ucmd->m_cmdnum)
		return false;
	g::send_packet = true;
	auto ret = createmove(REG_OUT, sampletime, ucmd);

	g::ucmd = ucmd;
	vec3_t	m_strafe_angles;
	const auto backup_angle = ucmd->m_angs;
	const auto backup_sidemove = ucmd->m_smove;
	const auto backup_forwardmove = ucmd->m_fmove;
	const auto backup_flags = g::local->flags();

	m_strafe_angles = ucmd->m_angs;

	features::movement::fixmove(ucmd, m_strafe_angles);
	features::movement::run(ucmd);


	csgo::clamp(ucmd->m_angs);
	csgo::rotate_movement(ucmd);

	// mov ebp into m_pFramePointer
	unsigned int* m_pFramePointer;
	__asm mov m_pFramePointer, ebp;

	// set sendpacket to desired value
	*(bool*) (*m_pFramePointer - 0x1C) = g::send_packet;

	//*(bool*) (*(uintptr_t*) (uintptr_t(_AddressOfReturnAddress()) - 4) - 28) = g::send_packet;

	return false;
}

void __fastcall framestagenotify_hk(REG, int stage) {
	g::local = (!csgo::i::engine->is_connected() || !csgo::i::engine->is_in_game()) ? nullptr : csgo::i::ent_list->get< player_t* >(csgo::i::engine->get_local_player());


	if (vars.misc.rpc)
		discord_rpc::init();
	else
		discord_rpc::disable();

	framestagenotify(REG_OUT, stage);
}

IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
DWORD dwOld_D3DRS_COLORWRITEENABLE;
DWORD colorwrite, srgbwrite;

void SaveState(IDirect3DDevice9* device)
{
	D3DVIEWPORT9 d3d_viewport;
	device->GetViewport(&d3d_viewport);

	device->CreateStateBlock(D3DSBT_ALL, &pixel_state);
	pixel_state->Capture();

	device->SetVertexShader(nullptr);
	device->SetPixelShader(nullptr);
	device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	device->SetRenderState(D3DRS_FOGENABLE, FALSE);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	device->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

	device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
}

void RestoreState(IDirect3DDevice9* pDevice) // not restoring everything. Because its not needed.
{
	pixel_state->Apply();
	pixel_state->Release();
}

long __fastcall endscene_hk(REG, IDirect3DDevice9* device) {

	static auto ret = _ReturnAddress();

	if (ret != _ReturnAddress())
		return endscene(REG_OUT, device);
	SaveState(device);

	setup::draw();

	features::visuals::watermark();

	RestoreState(device);

	return endscene(REG_OUT, device);
}

long __fastcall reset_hk(REG, IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* presentation_params) {
	reinterpret_cast<ID3DXFont*>(fonts::def)->Release();
	reinterpret_cast<ID3DXFont*>(menu::fonts::fm)->Release();
	reinterpret_cast<ID3DXFont*>(menu::fonts::logo)->Release();

	auto hr = reset(REG_OUT, device, presentation_params);

	if (SUCCEEDED(hr))
	{
		render::create_font((void**) &fonts::def, L"Tahoma", 32, true);
		menu::fonts::init();
	}

	return hr;
}

void __fastcall lockcursor_hk(REG) {
	if (menu::is_opened()) {
		csgo::i::surface->unlock_cursor();
		return;
	}

	lockcursor(REG_OUT);
}

void __fastcall doextraboneprocessing_hk(REG, int a2, int a3, int a4, int a5, int a6, int a7) {
	return;
}

bool __fastcall setupbones_hk(REG, matrix3x4_t* out, int max_bones, int mask, float curtime) {
	static auto c_csplayer__setupbones_ret_addr = pattern::search("client.dll", "E8 ? ? ? ? 5E 5D C2 10 00 CC 55").add(5).get< void* >();
	return setupbones(REG_OUT, out, max_bones, mask, curtime);;
}

vec3_t* __fastcall get_eye_angles_hk(REG) {
	//auto local = csgo::i::ent_list->get< player_t* >(csgo::i::engine->get_local_player());
	//
	//if (ecx != local)
	//	return get_eye_angles(REG_OUT);
	//
	//static auto ret_to_thirdperson_pitch = pattern::search("client.dll", "8B CE F3 0F 10 00 8B 06 F3 0F 11 45 ? FF 90 ? ? ? ? F3 0F 10 55 ?").get< std::uintptr_t >();
	//static auto ret_to_thirdperson_yaw = pattern::search("client.dll", "F3 0F 10 55 ? 51 8B 8E ? ? ? ?").get< std::uintptr_t >();
	//
	//if (std::uintptr_t(_ReturnAddress()) == ret_to_thirdperson_pitch
	//	|| std::uintptr_t(_ReturnAddress()) == ret_to_thirdperson_yaw)
	//	return g::ucmd ? &g::ucmd->m_angs : &local->angles();
	//
	return get_eye_angles(REG_OUT);
}

bool __fastcall get_bool_hk(REG) {
	static auto cam_think = pattern::search("client.dll", "85 C0 75 30 38 86").get< std::uintptr_t >();

	if (!ecx)
		return false;

	auto local = csgo::i::ent_list->get< player_t* >(csgo::i::engine->get_local_player());

	if (std::uintptr_t(_ReturnAddress()) == cam_think)
		return true;

	return get_bool(REG_OUT);
}

long __stdcall wndproc(HWND hwnd, std::uint32_t msg, std::uintptr_t wparam, std::uint32_t lparam) {
	auto skip_mouse_input_processing = false;

	if (menu::is_opened())
		return true;

	return CallWindowProcA(g_wndproc, hwnd, msg, wparam, lparam);
}

unsigned long font_count = 0;

bool hooks::init() {
	draw::init();
	menu::fonts::init();
	render::create_font((void**) &fonts::def, L"Tahoma", 32, true);

	const auto sv_cheats = csgo::i::cvar->FindVar("sv_cheats");

	const auto clientmodeshared_createmove = pattern::search("client.dll", "55 8B EC 8B 0D ? ? ? ? 85 C9 75 06 B0").get< decltype(&createmove_hk) >();
	const auto chlclient_framestagenotify = pattern::search("client.dll", "55 8B EC 8B 0D ? ? ? ? 8B 01 8B 80 74 01 00 00 FF D0 A2").get< decltype(&framestagenotify_hk) >();
	const auto idirect3ddevice9_endscene = vfunc< decltype(&endscene_hk) >(csgo::i::dev, 42);
	const auto idirect3ddevice9_reset = vfunc< decltype(&reset_hk) >(csgo::i::dev, 16);
	const auto vguimatsurface_lockcursor = pattern::search("vguimatsurface.dll", "80 3D ? ? ? ? 00 8B 91 A4 02 00 00 8B 0D ? ? ? ? C6 05 ? ? ? ? 01").get< decltype(&lockcursor_hk) >();
	const auto modelrender_drawmodelexecute = vfunc< decltype(&drawmodelexecute_hk) >(csgo::i::mdl_render, 21);
	const auto c_csplayer_doextraboneprocessing = pattern::search("client.dll", "55 8B EC 83 E4 F8 81 EC FC 00 00 00 53 56 8B F1 57").get< void* >();
	const auto c_csplayer_get_eye_angles = pattern::search("client.dll", "56 8B F1 85 F6 74 32").get< void* >();
	const auto convar_getbool = vfunc< decltype(&get_bool_hk) >(sv_cheats, 13);
	const auto overrideview = pattern::search("client.dll", "55 8B EC 83 E4 F8 83 EC 58 56 57 8B 3D ? ? ? ? 85 FF").get< void* >();
	const auto engine_fire_event = vfunc< void* >(csgo::i::engine, 59);
	const auto c_baseanimating_setupbones = pattern::search("client.dll", "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B F9").get< void* >();
	const auto paint_traverse_hook = vfunc< decltype(&paint_traverse_hk) >(csgo::i::panel, 41);
	const auto ivrenderview_sceneend = vfunc< decltype(&sceneend_hk) >(csgo::i::render_view, 9);

	MH_Initialize();

	/* hook all the functions you need! */
	MH_CreateHook(ivrenderview_sceneend, sceneend_hk, (void**) &sceneend);
	MH_CreateHook(engine_fire_event, fire_event_hk, (void**) &fire_event);
	MH_CreateHook(clientmodeshared_createmove, createmove_hk, (void**) &createmove);
	MH_CreateHook(chlclient_framestagenotify, framestagenotify_hk, (void**) &framestagenotify);
	MH_CreateHook(idirect3ddevice9_endscene, endscene_hk, (void**) &endscene);
	MH_CreateHook(idirect3ddevice9_reset, reset_hk, (void**) &reset);
	MH_CreateHook(vguimatsurface_lockcursor, lockcursor_hk, (void**) &lockcursor);
	MH_CreateHook(modelrender_drawmodelexecute, drawmodelexecute_hk, (void**) &drawmodelexecute);
	MH_CreateHook(c_csplayer_doextraboneprocessing, doextraboneprocessing_hk, (void**) &doextraboneprocessing);
	MH_CreateHook(c_csplayer_get_eye_angles, get_eye_angles_hk, (void**) &get_eye_angles);
	MH_CreateHook(convar_getbool, get_bool_hk, (void**) &get_bool);
	MH_CreateHook(overrideview, override_view_hk, (void**) &override_view);
	MH_CreateHook(c_baseanimating_setupbones, setupbones_hk, (void**) &setupbones);
	MH_CreateHook(paint_traverse_hook, paint_traverse_hk, (void**) &paint_traverse);

	MH_EnableHook(MH_ALL_HOOKS);

	/* hook wndproc handler so we can intercept input. */
	g_wndproc = (WNDPROC) SetWindowLongA(FindWindowA("Valve001", nullptr), GWLP_WNDPROC, (long) wndproc);

	/* event handler */
	event_handler = std::make_unique< c_event_handler >();

	return true;
}
