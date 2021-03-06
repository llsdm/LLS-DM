#if defined UIV_POLICE
	#endinput
#endif
#define UIV_POLICE

static enum
{
	UIV_POLICE_DIALOGID_WEAPON = 10,
	UIV_POLICE_DIALOGID_SECOND_WEAP,
	UIV_POLICE_DIALOGID_TRUNK,
	UIV_POLICE_DIALOGID_VAN,

	UIV_POLICE_AUTO_SAVE = LLSDM_PARAMS_ON
};

static
	weapon_id[MAX_PLAYERS char],
	weapon_exp[MAX_PLAYERS],
	weapon_old_id[MAX_PLAYERS char],
	weapon_old_ammo[MAX_PLAYERS],
	weapon_old_exp[MAX_PLAYERS],
	weapon_listitem[MAX_PLAYERS char],
	weapon_type[MAX_PLAYERS char],
	weapon_second[MAX_PLAYERS char],
	bool:weapon_pick[MAX_PLAYERS char],
	bool:weapon_backpack[MAX_PLAYERS char];

static const
	weapon_buystat_caliber[][] =
	{
		".45",
		".50",
		"12-й",
		"9x19 мм Парабеллум",
		"7,62 мм",
		"5,56x45 мм НАТО"
	},
	weapon_caliber_id[] =
	{
		1, 1, 2, 3, 3, 3, 4, 4, 5, 6, 4, 5, 5
	},
	weapon_type_id[] =
	{
		0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, 1
	},
	weapon_dialog_button[][] =
	{
		"Взять",
		"Добавить",
		"Заменить"
	};

stock GiveWeapon(playerid, weaponid, exp, vehicleid = 0, listitem = 0)
{
	static const
		str_take[] = "Оружие:\t\t%s\nКалибр:\t\t%s\nТип:\t\t\t%s\nОпыт за убийство:\t%iXP",
		str_replace_1[] = "%s\n\n{FF0000}Внимание!\n{FFFFFF}У Вас уже есть оружие этого типа.\n%s будет заменено на %s.",
		str_replace_2[] = "%s\n\n{FF0000}Внимание!\n{FFFFFF}У Вас уже есть оружие этого типа.\n\nПервый слот:\t%s\nВторой слот:\t%s";

	static
		typeid,
		my_weaponid,
		my_weaponid_exp,
		second_weapon,
		buttonid,
		weapon_name_1[MAX_WEAPON_NAME],
		weapon_name_2[MAX_WEAPON_NAME],
		weapon_name_3[MAX_WEAPON_NAME],
		string[(sizeof str_replace_2 + MAX_WEAPON_NAME*2 - 2*3 - 4)+(sizeof str_take + MAX_WEAPON_NAME*3 - 2*3 - 9)];

	typeid = weapon_type_id[weaponid-22];
	my_weaponid = pInfo[playerid][pWeapons][typeid];
	my_weaponid_exp = pInfo[playerid][pWeaponExp][typeid];
	second_weapon = pInfo[playerid][pWeapons][2];
	weapon_id{playerid} = weaponid;
	weapon_type{playerid} = typeid;
	weapon_exp[playerid] =  exp;
	buttonid = 0;

	if (vehicleid != 0)
	{
		TrunkOrRearDoorsOfVehicle(playerid, vehicleid);
	}

	if (my_weaponid == weaponid || second_weapon == weaponid)
	{
		return ShowPlayerReminder(playerid, !"У Вас уже есть это оружие", UIV_POLICE_DIALOGID_TRUNK);
	}

	weapon_listitem{playerid} = listitem;

	GetWeaponName(weaponid, weapon_name_1, MAX_WEAPON_NAME);

	format(string, sizeof string, str_take,
		weapon_name_1,
		weapon_buystat_caliber[weapon_caliber_id[weaponid-22]-1],
		typeid == 1 ? "Основное" : "Дополнительное",
		exp
	);

	if (my_weaponid != 0)
	{
		GetWeaponName(my_weaponid, weapon_name_2, MAX_WEAPON_NAME);

		weapon_old_ammo[playerid] = pInfo[playerid][pAmmo][typeid];
		weapon_old_id{playerid} = my_weaponid;
		weapon_old_exp[playerid] = my_weaponid_exp;

		if (typeid == 0 || (typeid == 1 && pInfo[playerid][pBackpackLevel] == 0))
		{
			format(string, sizeof string, str_replace_1, 
				string,
				weapon_name_2,
				weapon_name_1
			);

			buttonid = 2;
			weapon_pick{playerid} = true;
		}
		else
		{
			GetWeaponName(second_weapon, weapon_name_3, MAX_WEAPON_NAME);

			format(string, sizeof string, str_replace_2, 
				string,
				weapon_name_2,
				weapon_name_3[0] == EOS ? "{FF0000}пусто" : weapon_name_3
			);

			buttonid = second_weapon == 0 ? 1 : 2;
			weapon_second{playerid} = buttonid-1;
			weapon_pick{playerid} = second_weapon != 0 ? true : false;
		}
	}

	return ShowPlayerDialog(playerid, UIV_POLICE_DIALOGID_WEAPON, DIALOG_STYLE_MSGBOX, !"Найдено:", string, weapon_dialog_button[buttonid], !"Закрыть");
}

stock GiveWeapons(playerid, vehicleid)
{
	new
		i = 0,
		str[MAX_WEAPON_NAME],
		string[MAX_WEAPON_NAME*(UIV_MAX_WEAPONS+1)];

	for (i = 0; i < UIV_MAX_WEAPONS; i++)
	{
		if (uiv[vehicleid][uiv_weapons][i] == 0)
		{
			break;
		}

		GetWeaponName(uiv[vehicleid][uiv_weapons][i], str, MAX_WEAPON_NAME);

		if (i != 0)
		{
			strcat(string, "\n");
		}
		strcat(string, str);
	}

	if (uiv[vehicleid][uiv_backpacks] != 0)
	{
		weapon_listitem{playerid} = i;
		weapon_backpack{playerid} = true;
		if (i != 0)
		{
			strcat(string, "\n");
		}
		format(string, sizeof string, "%sРюкзаки {FF00FF}%i шт.", string, uiv[vehicleid][uiv_backpacks]);
	}

	TrunkOrRearDoorsOfVehicle(playerid, vehicleid);

	return ShowPlayerDialog(playerid, UIV_POLICE_DIALOGID_VAN, DIALOG_STYLE_LIST, !"Полицейский фургон", string, !"Взять", !"Закрыть");
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	if (dialogid == UIV_POLICE_DIALOGID_WEAPON)
	{
		if (response == 0)
			return TrunkOrRearDoorsOfVehicle(playerid);
		
		static
			weaponid,
			typeid,
			second,
			weapon_name_1[MAX_WEAPON_NAME],
			weapon_name_2[MAX_WEAPON_NAME],
			string[MAX_WEAPON_NAME*2];

		weaponid = weapon_id{playerid};
		typeid = weapon_type_id[weaponid-22];
		second = weapon_second{playerid};

		if (typeid == 0 || (typeid == 1 && pInfo[playerid][pBackpackLevel] == 0))
		{
			return TakeWeapon(playerid, weapon_listitem{playerid});
		}
		else
		{	
			if (second == 0)
			{
				return TakeWeapon(playerid, weapon_listitem{playerid}, true);
			}
			else
			{
				GetWeaponName(pInfo[playerid][pWeapons][1], weapon_name_1, MAX_WEAPON_NAME);
				GetWeaponName(pInfo[playerid][pWeapons][2], weapon_name_2, MAX_WEAPON_NAME);

				format(string, sizeof string, "%s\n%s", 
					weapon_name_1,
					weapon_name_2
				);

				return ShowPlayerDialog(playerid, UIV_POLICE_DIALOGID_SECOND_WEAP, DIALOG_STYLE_LIST, "Замена", string, !"Зменить", !"Отмена");
			}
		}
	}
	else if (dialogid == UIV_POLICE_DIALOGID_SECOND_WEAP)
	{
		if (response == 0)
		{
			return TrunkOrRearDoorsOfVehicle(playerid);
		}

		new
			bool:second = listitem == 0 ? false : true;
			
		return TakeWeapon(playerid, weapon_listitem{playerid}, second);
	}
	else if (dialogid == UIV_POLICE_DIALOGID_TRUNK)
	{
		return TrunkOrRearDoorsOfVehicle(playerid);
	}
	else if (dialogid == UIV_POLICE_DIALOGID_VAN)
	{
		if (response == 0)
		{
			weapon_backpack{playerid} = false;
			return TrunkOrRearDoorsOfVehicle(playerid);
		}

		new
			vehicleid = uiv_vehicleid[playerid];

		if (weapon_backpack{playerid} && listitem == weapon_listitem{playerid})
		{
			weapon_backpack{playerid} = false;
			TrunkOrRearDoorsOfVehicle(playerid);
			if (pInfo[playerid][pBackpackLevel] != 0)
				return ShowPlayerReminder(playerid, !"У Вас уже есть рюкзак");
			pInfo[playerid][pBackpackLevel] = 1;
			pInfo[playerid][pBackpackSize] = BACKPACK_LVL_SIZE;
			uiv[vehicleid][uiv_backpacks]--;
			SetPlayerAttachedObject(playerid, AMMU_SLOT_BACKPACK, 371, 1, 0.016000, -0.115997, -0.020997, 0.000000, 86.999992, 0.000000, 1.351999, 1.102998,1.280998, 0xFF101010, 0xFF101010);
			pInfo[playerid][pBackpackLevel] = 1;
			AttachKitToPlayer(playerid);
			AttachBagToPlayer(playerid);
			TakeWeaponFromVanVehicle(vehicleid);

			#if PLAYER_BACKPACK_AUTO_SAVE == LLSDM_PARAMS_ON
				SaveBackpack(playerid);
				SaveBackpackLevel(playerid);
				SaveWeapon(playerid);
			#endif

			return 1;
		}

		return GiveWeapon(playerid, uiv[vehicleid][uiv_weapons][listitem], uiv[vehicleid][uiv_weapons_exp][listitem], .listitem = listitem);
	}
	
	#if defined uiv_police_OnDialogResponse
		uiv_police_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
	#endif

	return 0;
}

#if defined _ALS_OnDialogResponse
	#undef OnDialogResponse
#else
	#define _ALS_OnDialogResponse
#endif
#define OnDialogResponse uiv_police_OnDialogResponse
#if defined uiv_police_OnDialogResponse
	forward uiv_police_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);
#endif

static stock TakeWeapon(playerid, listitem, bool:second = false)
{
	new
		weapon_type_index = weapon_type{playerid},
		modelid = GetVehicleModel(uiv_vehicleid[playerid]);

	if (weapon_pick{playerid})
	{
		switch (modelid)
		{
			case POLICE_VAN:
			{
				UpdateWeaponInVanVehicle(uiv_vehicleid[playerid], listitem, 
					!second ? weapon_old_id{playerid} : pInfo[playerid][pWeapons][2],
					!second ? weapon_old_exp[playerid] : pInfo[playerid][pWeaponExp][2]
				);
			}	
			case POLICE_LS..POLICE_JEEP: 
			{
				UpdateWeaponInPatrolVehicle(uiv_vehicleid[playerid], 
					!second ? weapon_old_id{playerid} : pInfo[playerid][pWeapons][2],
					!second ? weapon_old_exp[playerid] : pInfo[playerid][pWeaponExp][2]
				);
			}
		}

		TrunkOrRearDoorsOfVehicle(playerid);
	}
	else
	{
		switch (modelid)
		{
			case POLICE_VAN: TakeWeaponFromVanVehicle(uiv_vehicleid[playerid], listitem);
			case POLICE_LS..POLICE_JEEP: TakeWeaponFromPatrolVehicle(uiv_vehicleid[playerid]);
		}
	}

	if (!second)
	{
		pInfo[playerid][pWeapons][weapon_type_index] = weapon_id{playerid};
		pInfo[playerid][pWeaponExp][weapon_type_index] = weapon_exp[playerid];
	}
	else
	{
		if (pInfo[playerid][pWeapons][1] != 0)
		{
			pInfo[playerid][pWeapons][2] = weapon_id{playerid};
			pInfo[playerid][pWeaponExp][2] = weapon_exp[playerid];

		}
		else
		{
			pInfo[playerid][pWeapons][weapon_type_index] = weapon_id{playerid};
			pInfo[playerid][pWeaponExp][weapon_type_index] = weapon_exp[playerid];
		}
	}

	if (weapon_pick{playerid} && !second)
	{
		PickUpPlayerAmmo(playerid, weapon_old_id{playerid}, weapon_old_ammo[playerid]);
		ResetPlayerWeapons(playerid);

		for (new i = 0; i < MAX_WEAPONS; i++)
		{
			if (pInfo[playerid][pAmmo][i] > 0)
			{
				GivePlayerWeapon(playerid, pInfo[playerid][pWeapons][i], pInfo[playerid][pAmmo][i]);
			}
		}
	}

	weapon_id{playerid} =
	weapon_exp[playerid] =
	weapon_old_id{playerid} =
	weapon_old_ammo[playerid] =
	weapon_old_exp[playerid] = 
	weapon_second{playerid} =
	weapon_type{playerid} =
	weapon_listitem{playerid} =  0;
	weapon_backpack{playerid} =
	weapon_pick{playerid} = false;

	#if UIV_POLICE_AUTO_SAVE == LLSDM_PARAMS_ON
		SaveWeapon(playerid);
	#endif

	return 1;
}

static stock UpdateWeaponInPatrolVehicle(vehicleid, weaponid, exp)
{
	uiv[vehicleid][uiv_weapon] = weaponid;
	uiv[vehicleid][uiv_weapons_exp] = exp;
	return 1;
}

static stock UpdateWeaponInVanVehicle(vehicleid, listitem, weaponid, exp)
{
	uiv[vehicleid][uiv_weapons][listitem] = weaponid;
	uiv[vehicleid][uiv_weapons_exp][listitem] = exp;
	return 1;
}

static stock TakeWeaponFromPatrolVehicle(vehicleid)
{
	uiv[vehicleid][uiv_loot] =
	uiv[vehicleid][uiv_drop] = false;
	uiv[vehicleid][uiv_weapon] =
	uiv[vehicleid][uiv_weapons_exp] = 0;

	HideUIVehicle(vehicleid);

	return 1;
}

static stock TakeWeaponFromVanVehicle(vehicleid, listitem = -1)
{
	new
		index_i,
		result;

	if (listitem != -1)
	{
		uiv[vehicleid][uiv_weapons][listitem] = 0;
	}
	uiv[vehicleid][uiv_loot] = false;
	
	for (new i = 0; i < UIV_MAX_WEAPONS; i++)
	{
		if (uiv[vehicleid][uiv_weapons][i] == 0)
		{
			index_i = i == UIV_MAX_WEAPONS - 1 ? 0 : 1;
			uiv[vehicleid][uiv_weapons][i] = uiv[vehicleid][uiv_weapons][i+index_i];
			uiv[vehicleid][uiv_weapons][i+index_i] = 0;
			uiv[vehicleid][uiv_weapons_exp][i] = uiv[vehicleid][uiv_weapons_exp][i+index_i];
			uiv[vehicleid][uiv_weapons_exp][i+index_i] = 0;
		}

		result += uiv[vehicleid][uiv_weapons][i];
	}

	result += uiv[vehicleid][uiv_backpacks];
		
	if (result == 0)
	{
		uiv[vehicleid][uiv_drop] = false;
		HideUIVehicle(vehicleid);
	}

	return 1;
}

stock GetWeaponIndex(weaponid)
{
	return weapon_type_id[weaponid-22];
}

public OnPlayerDeath(playerid, killerid, reason)
{
	weapon_id{playerid} =
	weapon_exp[playerid] =
	weapon_old_id{playerid} =
	weapon_old_ammo[playerid] =
	weapon_old_exp[playerid] = 
	weapon_second{playerid} =
	weapon_type{playerid} =
	weapon_listitem{playerid} =  0;
	weapon_backpack{playerid} =
	weapon_pick{playerid} = false;

	#if defined uiv_police_OnPlayerDeath
		uiv_police_OnPlayerDeath(playerid, killerid, reason);
	#endif

	return 1;
}

#if defined _ALS_OnPlayerDeath
	#undef OnPlayerDeath
#else
	#define _ALS_OnPlayerDeath
#endif
#define OnPlayerDeath uiv_police_OnPlayerDeath
#if defined uiv_police_OnPlayerDeath
	forward uiv_police_OnPlayerDeath(playerid, killerid, reason);
#endif


public OnPlayerDisconnect(playerid, reason)
{
	weapon_id{playerid} =
	weapon_exp[playerid] =
	weapon_old_id{playerid} =
	weapon_old_ammo[playerid] =
	weapon_old_exp[playerid] = 
	weapon_second{playerid} =
	weapon_type{playerid} =
	weapon_listitem{playerid} =  0;
	weapon_backpack{playerid} =
	weapon_pick{playerid} = false;

	#if defined uiv_police_OnPlayerDis
		uiv_police_OnPlayerDis(playerid, reason);
	#endif

	return 1;
}

#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect uiv_police_OnPlayerDis
#if defined uiv_police_OnPlayerDis
	forward uiv_police_OnPlayerDis(playerid, reason);
#endif