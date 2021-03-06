#if defined PLAYER_SPAWN
	#endinput
#endif
#define PLAYER_SPAWN

static enum
{
	#if LLSDM_RESPAWN_WITH_SAVED == 1
		PLAYER_SPAWN_TIMER_SPEED = 1000,
		PLAYER_SPAWN_ANIM_KILL_TIMER = 1231,
	#endif
	PLAYER_SPAWN_MAX_AIR_OBJECTS = 4
};

static enum Float:
{
	PLAYER_SPAWN_DISTANCE_AREA = 1500.0,
	PLAYER_SPAWN_AIR_DISTANCE = 4500.0,
	PLAYER_SPAWN_AIR_FLIGHT = 400.0,
	PLAYER_SPAWN_AIR_SPEED = 200.0
};

static enum
	spawn_info
	{
		Float:spawn_pos_x,
		Float:spawn_pos_y,
		Float:spawn_f_pos_x,
		Float:spawn_f_pos_y,
		Float:spawn_angle
	};

static
	SpawnInfo[MAX_PLAYERS][spawn_info],
	#if LLSDM_RESPAWN_WITH_SAVED == 1
		spawn_timer[MAX_PLAYERS],
	#endif
	bool:spawn_player[MAX_PLAYERS char],
	spawn_objects[MAX_PLAYERS][PLAYER_SPAWN_MAX_AIR_OBJECTS] = {{-1, ...}, ...},
	spawn_area[MAX_PLAYERS];

public OnPlayerSpawn(playerid)
{
	new
		obj0, obj1, obj2, obj3,
		Float:x, Float:y, Float:z, Float:a,
		Float:f_x, Float:f_y;

	InvisiblePlayerIcon(playerid);

	player_authorized{playerid} = true;

	if (pInfo[playerid][pLife] != 0)
	{
		GivePlayerWeapon(playerid, pInfo[playerid][pWeapons][0], pInfo[playerid][pAmmo][0]);
		GivePlayerWeapon(playerid, pInfo[playerid][pWeapons][1], pInfo[playerid][pAmmo][1]);
		SetPlayerArmedWeapon(playerid, 0);
		SetPlayerSkin(playerid, pInfo[playerid][pSkin]);
		SetPlayerHealth(playerid, pInfo[playerid][pHealth]);
		
		if (pInfo[playerid][pBackpackLevel] != 0)
		{
			SetPlayerAttachedObject(playerid, AMMU_SLOT_BACKPACK, 371, 1, 0.016000, -0.115997, -0.020997, 0.000000, 86.999992, 0.000000, 1.351999, 1.102998,1.280998, 0xFF101010, 0xFF101010);
		}

		/**/
		//SetPlayerAttachedObject(playerid, AMMU_SLOT_PROTOCOL, 1316, 1, -0.037,-0.303, -0.02, 90.0, 0.0, 0.0,   0.025, 0.025, 0.0, 0xFFFFFFFF, 0xFFFFFFFF);
		/**/

		AttachKitToPlayer(playerid);
		
		return 1;
	}

	if (!spawn_player{playerid})
	{
		x = SpawnInfo[playerid][spawn_pos_x];
		y = SpawnInfo[playerid][spawn_pos_y];
		z = PLAYER_SPAWN_AIR_FLIGHT;
		a = SpawnInfo[playerid][spawn_angle];
		f_x = SpawnInfo[playerid][spawn_f_pos_x];
		f_y = SpawnInfo[playerid][spawn_f_pos_y];

		SetPlayerFacingAngle(playerid, a);
		SetCameraBehindPlayer(playerid);
		TogglePlayerSpectating(playerid, true);

		obj0 = CreateObject(1681, x, y, z, 0.0, 0.0, a, 1000.0);
		obj1 = CreateObject(19477, 0.0, 0.0, 0.0, 90.0, 0.0, 0.0);
		obj2 = CreateObject(19482, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		obj3 = CreateObject(19482, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

		SetObjectMaterialText(obj2, "F", 0, 130, "Arial", 200, 0, 0xFFFFFF00, 0, 1);
		SetObjectMaterialText(obj3, "c", 0, 130, "Webdings", 200, 0, -1, 0, 1);

		AttachObjectToObject(obj1, obj0, 0.0, -35.0, 6.0, 0.0, 0.0, 0.0, 1); 
		AttachObjectToObject(obj2, obj0, 0.0, 0.0, 3.0, 0.0, 0.0, -90.0, 1); 
		AttachObjectToObject(obj3, obj0, 0.0, 0.0, 3.0, 0.0, 0.0, -90.0, 1); 

		AttachCameraToObject(playerid, obj1);

		MoveObject(obj0, f_x, f_y, z, PLAYER_SPAWN_AIR_SPEED, 0.0, 0.0, a);

		spawn_objects[playerid][0] = obj0;
		spawn_objects[playerid][1] = obj1;
		spawn_objects[playerid][2] = obj2;
		spawn_objects[playerid][3] = obj3;
	}
	else
	{
		SetCameraBehindPlayer(playerid);
	}

	spawn_player{playerid} = !spawn_player{playerid};

	#if defined player_spawn_OnPlayerSpawn
		player_spawn_OnPlayerSpawn(playerid);
	#endif

	return 1;
}

#if defined _ALS_OnPlayerSpawn
	#undef OnPlayerSpawn
#else
	#define _ALS_OnPlayerSpawn
#endif
#define OnPlayerSpawn player_spawn_OnPlayerSpawn
#if defined player_spawn_OnPlayerSpawn
	forward player_spawn_OnPlayerSpawn(playerid);
#endif

public OnPlayerEnterDynamicArea(playerid, areaid)
{
	if (areaid == spawn_area[playerid])
	{
		static
			Float:x,
			Float:y,
			Float:z;

		DestroyDynamicArea(areaid);
		spawn_area[playerid] = 0;

		CancelEdit(playerid);
		GetPlayerPos(playerid, x, y, z);
		SetSpawnInfo(playerid, 0, pInfo[playerid][pSkin], x, y, z-5.0, floatrandom(360), WEAPON_PARACHUTE, 1, 0, 0, 0, 0);
		TogglePlayerSpectating(playerid, false);

		for (new i = 0, j = _:spawn_info; i < j; i++)
		{
			SpawnInfo[playerid][spawn_info:i] = 0;
		}

		return 1;
	}
	
	#if defined player_spawn_OnPlayerArea
		player_spawn_OnPlayerArea(playerid, areaid);
	#endif

	return 1;
}

#if defined _ALS_OnPlayerEnterDynamicArea
	#undef OnPlayerEnterDynamicArea
#else
	#define _ALS_OnPlayerEnterDynamicArea
#endif
#define OnPlayerEnterDynamicArea player_spawn_OnPlayerArea
#if defined player_spawn_OnPlayerArea
	forward player_spawn_OnPlayerArea(playerid, areaid);
#endif

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if (spawn_player{playerid})
	{
		if (HOLDING(KEY_SECONDARY_ATTACK))
		{
			static
				Float:x,
				Float:y,
				Float:z;

			DestroyDynamicArea(spawn_area[playerid]);
			spawn_area[playerid] = 0;

			CancelEdit(playerid);
			GetPlayerPos(playerid, x, y, z);
			SetSpawnInfo(playerid, 0, pInfo[playerid][pSkin], x, y, z-5.0, floatrandom(360), WEAPON_PARACHUTE, 1, 0, 0, 0, 0);
			TogglePlayerSpectating(playerid, false);
			ToggleDynAreaSpectateMode(spawn_area[playerid], false);

			for (new i = 0, j = _:spawn_info; i < j; i++)
			{
				SpawnInfo[playerid][spawn_info:i] = 0;
			}

			#if LLSDM_RESPAWN_WITH_SAVED == 1
				SetTimerEx(!"PlayerLife", PLAYER_SPAWN_TIMER_SPEED, true, "i", playerid);
			#endif

			return 1;
		}
	}

	#if defined player_spawn_OnPlayerNewKey
		player_spawn_OnPlayerNewKey(playerid, newkeys, oldkeys);
	#endif

	return 1;
}

#if defined _ALS_OnPlayerKeyStateChange
	#undef	OnPlayerKeyStateChange
#else
	#define	_ALS_OnPlayerKeyStateChange
#endif
#define OnPlayerKeyStateChange	player_spawn_OnPlayerNewKey
#if defined player_spawn_OnPlayerNewKey
	forward player_spawn_OnPlayerNewKey(playerid, newkeys, oldkeys);
#endif


public OnObjectMoved(objectid)
{
	foreach (Player, i)
	{
		new
			objectair = spawn_objects[i][0];

		if (objectid == objectair)
		{
			for (new j = 0; j < PLAYER_SPAWN_MAX_AIR_OBJECTS; j++)
			{
				DestroyObject(spawn_objects[i][j]);
				spawn_objects[i][j] = -1;
			}

			break;
		}
	}

	#if defined player_spawn_OnObjectMoved
		player_spawn_OnObjectMoved(objectid);
	#endif

	return 1;
}

#if defined _ALS_OnObjectMoved
	#undef OnObjectMoved
#else
	#define _ALS_OnObjectMoved
#endif
#define OnObjectMoved player_spawn_OnObjectMoved
#if defined player_spawn_OnObjectMoved
	forward player_spawn_OnObjectMoved(objectid);
#endif

public OnPlayerDeath(playerid, killerid, reason)
{
	#if LLSDM_RESPAWN_WITH_SAVED == 1
		if (spawn_timer[playerid] != -1)
		{
			KillTimer(spawn_timer[playerid]);
			spawn_timer[playerid] = -1;
		}
	#endif

	spawn_player{playerid} = false;

	DestroyDynamicArea(spawn_area[playerid]);
	spawn_area[playerid] = 0;

	for (new i = 0; i < PLAYER_SPAWN_MAX_AIR_OBJECTS; i++)
	{
		DestroyObject(spawn_objects[playerid][i]);
		spawn_objects[playerid][i] = -1;
	}

	#if LLSDM_RESPAWN_WITH_SAVED == 0
		pInfo[playerid][pLife] = 0;
		CreateSpawnInfo(playerid);
	#else
		new
			Float:pos_x, Float:pos_y, Float:pos_z,
			Float:angle;

		if (pInfo[playerid][pLife] == 0)
		{
			CreateSpawnInfo(playerid);
		}
		else
		{
			GetPlayerPos(playerid, pos_x, pos_y, pos_z);
			GetPlayerFacingAngle(playerid, angle);

			SetSpawnInfo(playerid, 0, pInfo[playerid][pSkin], pos_x, pos_y, pos_z, angle, 0, 0, 0, 0, 0, 0);
		}
	#endif

	#if defined player_spawn_OnPlayerDeath
		player_spawn_OnPlayerDeath(playerid, killerid, reason);
	#endif

	return 1;
}

#if defined _ALS_OnPlayerDeath
	#undef OnPlayerDeath
#else
	#define _ALS_OnPlayerDeath
#endif
#define OnPlayerDeath player_spawn_OnPlayerDeath
#if defined player_spawn_OnPlayerDeath
	forward player_spawn_OnPlayerDeath(playerid, killerid, reason);
#endif

public OnPlayerDisconnect(playerid, reason)
{
	#if LLSDM_RESPAWN_WITH_SAVED == 1
		if (spawn_timer[playerid] != -1)
		{
			KillTimer(spawn_timer[playerid]);
			spawn_timer[playerid] = -1;
		}
	#endif

	spawn_player{playerid} = false;

	DestroyDynamicArea(spawn_area[playerid]);
	spawn_area[playerid] = 0;

	for (new i = 0; i < PLAYER_SPAWN_MAX_AIR_OBJECTS; i++)
	{
		DestroyObject(spawn_objects[playerid][i]);
		spawn_objects[playerid][i] = -1;
	}

	for (new i = 0, j = _:spawn_info; i < j; i++)
	{
		SpawnInfo[playerid][spawn_info:i] = 0;
	}

	#if defined player_spawn_OnPlayerDisconnect
		player_spawn_OnPlayerDisconnect(playerid, reason);
	#endif

	return 1;
}

#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect player_spawn_OnPlayerDisconnect
#if defined player_spawn_OnPlayerDisconnect
	forward player_spawn_OnPlayerDisconnect(playerid, reason);
#endif

stock CreateSpawnInfo(playerid)
{
	static
		Float:pos_x,
		Float:pos_y,
		Float:f_pos_x,
		Float:f_pos_y,
		Float:angle;

	GetDirectionOfAircraft(angle, pos_x, pos_y, f_pos_x, f_pos_y, 180.0);
	SetSpawnInfo(playerid, 0, pInfo[playerid][pSkin], pos_x, pos_y, PLAYER_SPAWN_AIR_FLIGHT-1.0, angle, 0, 0, 0, 0, 0, 0);
	spawn_area[playerid] = CreateDynamicRectangle(f_pos_x-PLAYER_SPAWN_DISTANCE_AREA, f_pos_y-PLAYER_SPAWN_DISTANCE_AREA, f_pos_x+PLAYER_SPAWN_DISTANCE_AREA, f_pos_y+PLAYER_SPAWN_DISTANCE_AREA, .playerid = playerid);

	SpawnInfo[playerid][spawn_pos_x] = pos_x;
	SpawnInfo[playerid][spawn_pos_y] = pos_y;
	SpawnInfo[playerid][spawn_f_pos_x] = f_pos_x;
	SpawnInfo[playerid][spawn_f_pos_y] = f_pos_y;
	SpawnInfo[playerid][spawn_angle] = angle;

	return 1;
}

stock GetDirectionOfAircraft(&Float:angle, &Float:x, &Float:y, &Float:fpoint_x, &Float:fpoint_y, Float:fault = 0.0)
{
	new
		Float:distance = PLAYER_SPAWN_AIR_DISTANCE;

	angle = floatrandom(360);
	x = distance * -floatsin(angle, degrees);
	y = distance * floatcos(angle, degrees);

	distance *= 2.0;
	angle += fault + floatrandom(90) - 60.0;
	fpoint_x = x + distance * -floatsin(angle, degrees);
	fpoint_y = y + distance * floatcos(angle, degrees);
	distance = PLAYER_SPAWN_AIR_DISTANCE;

	return 1;
}

#if LLSDM_RESPAWN_WITH_SAVED == 1
	forward PlayerLife(playerid);
	public PlayerLife(playerid)
	{
		if (GetPlayerAnimationIndex(playerid) == PLAYER_SPAWN_ANIM_KILL_TIMER)
		{
			KillTimer(spawn_timer[playerid]);
			spawn_timer[playerid] = -1;
			pInfo[playerid][pLife] = 1;
			SaveLife(playerid);
		}

		return 1;
	}
#endif