#pragma once
#include "entity.hpp"
#include "../utils/padding.hpp"

// Generated using ReClass 2016

class weapon_info_t;

class weapon_info_t
{
public:
	std::uint8_t pad_0x0000[0x4]; //0x0000
	char* m_weapon_name; //0x0004 
	std::uint8_t pad_0x0008[0xC]; //0x0008
	std::uint32_t m_max_clip; //0x0014 
	std::uint8_t pad_0x0018[0x68]; //0x0018
	char* m_ammo_name; //0x0080 
	char* m_ammo_name_2; //0x0084 
	char* m_hud_name; //0x0088 
	char* m_weapon_id; //0x008C 
	std::uint8_t pad_0x0090[0x3C]; //0x0090
	std::uint32_t m_type; //0x00CC 
	std::uint32_t m_price; //0x00D0 
	std::uint32_t m_reward; //0x00D4 
	std::uint8_t pad_0x00D8[0x14]; //0x00D8
	std::uint8_t m_full_auto; //0x00EC 
	std::uint8_t pad_0x00ED[0x3]; //0x00ED
	std::uint32_t m_dmg; //0x00F0 
	float m_armor_ratio; //0x00F4 
	std::uint32_t m_bullets; //0x00F8 
	float m_penetration; //0x00FC 
	std::uint8_t pad_0x0100[0x8]; //0x0100
	float m_range; //0x0108 
	float m_range_modifier; //0x010C 
	std::uint8_t pad_0x0110[0x20]; //0x0110
	float m_max_speed; //0x0130 
	float m_max_speed_alt; //0x0134 
	std::uint8_t pad_0x0138[0x108]; //0x0138
}; //Size=0x0240

enum e_weapon_type {
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

enum item_definition_indexes {
	WEAPON_DEAGLE = 1,
	WEAPON_ELITE = 2,
	WEAPON_FIVESEVEN = 3,
	WEAPON_GLOCK = 4,
	WEAPON_AK47 = 7,
	WEAPON_AUG = 8,
	WEAPON_AWP = 9,
	WEAPON_FAMAS = 10,
	WEAPON_G3SG1 = 11,
	WEAPON_GALILAR = 13,
	WEAPON_M249 = 14,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10 = 17,
	WEAPON_P90 = 19,
	WEAPON_MP5_SD = 23,
	WEAPON_UMP45 = 24,
	WEAPON_XM1014 = 25,
	WEAPON_BIZON = 26,
	WEAPON_MAG7 = 27,
	WEAPON_NEGEV = 28,
	WEAPON_SAWEDOFF = 29,
	WEAPON_TEC9 = 30,
	WEAPON_TASER = 31,
	WEAPON_HKP2000 = 32,
	WEAPON_MP7 = 33,
	WEAPON_MP9 = 34,
	WEAPON_NOVA = 35,
	WEAPON_P250 = 36,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556 = 39,
	WEAPON_SSG08 = 40,
	WEAPON_KNIFE = 42,
	WEAPON_FLASHBANG = 43,
	WEAPON_HEGRENADE = 44,
	WEAPON_SMOKEGRENADE = 45,
	WEAPON_MOLOTOV = 46,
	WEAPON_DECOY = 47,
	WEAPON_INCGRENADE = 48,
	WEAPON_C4 = 49,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER = 60,
	WEAPON_USP_SILENCER = 61,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER = 64,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_CSS = 503,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516,
	WEAPON_KNIFE_CORD = 517,
	WEAPON_KNIFE_CANIS = 518,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE = 520,
	WEAPON_KNIFE_OUTDOOR = 521,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER = 523,
	WEAPON_KNIFE_SKELETON = 525,
	GLOVE_STUDDED_BLOODHOUND = 5027,
	GLOVE_T_SIDE = 5028,
	GLOVE_CT_SIDE = 5029,
	GLOVE_SPORTY = 5030,
	GLOVE_SLICK = 5031,
	GLOVE_LEATHER_WRAP = 5032,
	GLOVE_MOTORCYCLE = 5033,
	GLOVE_SPECIALIST = 5034,
	GLOVE_HYDRA = 5035
};

class base_view_model : public entity_t {
public:
	NETVAR(int, model_index, "DT_BaseViewModel", "m_nModelIndex");
	NETVAR(int, view_model_index, "DT_BaseViewModel", "m_nViewModelIndex");
	NETVAR(int, m_hweapon, "DT_BaseViewModel", "m_hWeapon");
	NETVAR(int, m_howner, "DT_BaseViewModel", "m_hOwner");
};

class weapon_t : public entity_t {
public:
	NETVAR(short, item_definition_index, "DT_BaseAttributableItem->m_iItemDefinitionIndex");
	NETVAR(float, next_primary_attack, "DT_BaseCombatWeapon->m_flNextPrimaryAttack");
	NETVAR(float, next_secondary_attack, "DT_BaseCombatWeapon->m_flNextSecondaryAttack");
	NETVAR(int, ammo, "DT_BaseCombatWeapon->m_iClip1");
	NETVAR(int, ammo2, "DT_BaseCombatWeapon->m_iClip2");
	NETVAR(float, postpone_fire_time, "DT_BaseCombatWeapon->m_flPostponeFireReadyTime");
	NETVAR(std::uint64_t, world_model_handle, "DT_BaseCombatWeapon->m_hWeaponWorldModel");
	NETVAR(float, throw_time, "DT_BaseCSGrenade->m_fThrowTime");
	NETVAR(bool, pin_pulled, "DT_BaseCSGrenade->m_bPinPulled");
	NETVAR(std::uint64_t, original_owner_xuid, "DT_BaseAttributableItem->m_OriginalOwnerXuidLow");
	NETVAR(std::uint32_t, original_owner_xuid_low, "DT_BaseAttributableItem->m_OriginalOwnerXuidLow");
	NETVAR(std::uint32_t, original_owner_xuid_high, "DT_BaseAttributableItem->m_OriginalOwnerXuidHigh");
	NETVAR(std::uint32_t, fallback_stattrak, "DT_BaseAttributableItem->m_nFallbackStatTrak");
	NETVAR(std::uint32_t, fallback_paintkit, "DT_BaseAttributableItem->m_nFallbackPaintKit");
	NETVAR(std::uint32_t, fallback_seed, "DT_BaseAttributableItem->m_nFallbackSeed");
	NETVAR(float, fallback_wear, "DT_BaseAttributableItem->m_flFallbackWear");
	NETVAR(std::uint32_t, fallback_quality, "DT_BaseAttributableItem->m_iEntityQuality");
	NETVAR(std::uint32_t, item_id_low, "DT_BaseAttributableItem->m_iItemIDLow");
	NETVAR(std::uint32_t, item_id_high, "DT_BaseAttributableItem->m_iItemIDHigh");
	NETVAR(std::uint32_t, account, "DT_BaseAttributableItem->m_iAccountID");
	NETVAR(const char*, name, "DT_BaseAttributableItem->m_szCustomName");
	NETVAR(float, last_shot_time, "DT_BaseCombatWeapon->m_fLastShotTime");

	weapon_info_t* data();

	void update_accuracy() {
		using fn = void(__thiscall*)(void*);
		vfunc< fn >(this, 478)(this);
	}

	float inaccuracy();
	float spread();
	float max_speed();

	bool is_grenade() {
		return data()->m_type == WEAPONTYPE_GRENADE;
	}
	bool is_c4() {
		return data()->m_type == WEAPONTYPE_C4;
	}

	bool is_taser() {
		return data()->m_type == WEAPON_TASER;
	}

	bool is_knife() {
		return data()->m_type == WEAPONTYPE_KNIFE;
	}

};
