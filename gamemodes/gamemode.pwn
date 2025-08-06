// LS 1980 1st Testing GM

#include <a_samp>
#include <sscanf2>
#include "samp_bcrypt.inc"
#include "sqlitei.inc"
#include "streamer.inc"
#include "sscanf2.inc"

#define CGEN_MEMORY 60000
#define YSI_NO_HEAP_MALLOC

DEFINE_HOOK_REPLACEMENT(Dyn, Dynamic);

#include "YSI_Coding\y_malloc.inc"
#include "YSI_Coding\y_hooks.inc"
#include "YSI_Coding\y_timers.inc"
#include "YSI_Data\y_iterate.inc"
#include "YSI_Visual\y_commands.inc"


#include "basesystems/database.inc"
#include "admin/data.inc"
#include "basesystems/login.inc"
#include "help/cmds"
#include "store_job/data.inc"

// colors

#define COLOR_WHITE         0xFFFFFFFF
#define COLOR_RED           0xFF0000FF
#define COLOR_GREEN         0x00FF00FF
#define COLOR_BLUE          0x0000FFFF
#define COLOR_LIGHTGRAY     0xD3D3D3FF

// Custom usage
#define COLOR_PM_SENT       0xADFF2FFF
#define COLOR_PM_RECEIVED   0x00BFFFEE
#define COLOR_USAGE         0xFFCC00FF
#define COLOR_SUCCESS       0x00FF00FF
#define COLOR_ERROR         0xFF0000FF


// Global variables
new FactionRank:gFactionRank[MAX_PLAYERS];
new Faction:gFaction[MAX_PLAYERS];
new Squad:gSquad[MAX_PLAYERS];

#define DIALOG_FACTIONS 9001

main()
{
	print("\n----------------------------------");
	print(" Welcome to LS 1980 Testing!");
	print("----------------------------------\n");
}

public OnGameModeInit()
{
	// Don't use these lines if it's a filterscript
	SetGameModeText("LS 1980 Testing");
	OpenDatabase();
	AddPlayerClass(0,1828.5991,-1849.2347,13.5781,192.4898,0,0,0,0,0,0);
	AddStaticVehicleEx(463, 1356.9719,-1753.5742,13.3624,239.2360, 1, 1, 60000);
	AddStaticVehicleEx(463, 1357.3567,-1751.4238,13.3719,239.2360, 1, 1, 60000);
	AddStaticVehicleEx(463, 1357.4979,-1748.8281,13.3833,239.2360, 1, 1, 60000);
	AddStaticVehicleEx(596, 2486.6838,-1557.3418,24.0538,359.8841, 1, 1, 60000);
	AddStaticVehicleEx(596, 2483.6987,-1557.7448,24.0350,359.1791, 1, 1, 60000);
	AddStaticVehicleEx(596, 2480.4043,-1557.8333,24.0204,6.4641, 1, 1, 60000);
	new bikerone = CreateActor(100, 1359.2006,-1757.2776,13.5078,85.0978);
	new bikertwo = CreateActor(100, 1357.3555,-1757.1243,13.5078,270.9612);
	SetActorInvulnerable (bikerone, true);
	SetActorInvulnerable (bikertwo, true);
	return 1;
}

public OnGameModeExit()
{
	CloseDatabase();
	return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
	SetPlayerPos(playerid, 1828.5991,-1849.2347,13.5781);
	SetPlayerCameraPos(playerid, 1828.5991,-1849.2347,13.5781);
	SetPlayerCameraLookAt(playerid, 2098.0891,-1782.1296,13.5628);
	return 1;
}

public OnPlayerConnect(playerid)
{
	new pname[MAX_PLAYER_NAME];
    GetPlayerName(playerid, pname, sizeof(pname));

    if (strcmp(pname, "Ben", true) == 0)
    {
        gAdminLevel[playerid] = ADMIN_LVL_DEVELOPER;
        SendClientMessage(playerid, -1, "Hi, Ben! Your developer account has been set.");
    }

    return 1;
}


public OnPlayerSpawn(playerid)
{
	SetPlayerSkin(playerid, 152);
	return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
	return 1;
}

public OnVehicleSpawn(vehicleid)
{
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])

{
	return 1;
}

public OnVehicleDeath(vehicleid, killerid)
{
	return 1;
}

public OnPlayerText(playerid, text[])
{
	return 1;
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	return 1;
}

public OnPlayerExitVehicle(playerid, vehicleid)
{
	return 1;
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	return 1;
}


public OnPlayerLeaveCheckpoint(playerid)
{
	return 1;
}

public OnPlayerEnterRaceCheckpoint(playerid)
{
	return 1;
}

public OnPlayerLeaveRaceCheckpoint(playerid)
{
	return 1;
}

public OnRconCommand(cmd[])
{
	return 1;
}

public OnPlayerRequestSpawn(playerid)
{
	return 1;
}

public OnObjectMoved(objectid)
{
	return 1;
}

public OnPlayerObjectMoved(playerid, objectid)
{
	return 1;
}

public OnPlayerPickUpPickup(playerid, pickupid)
{
	return 1;
}

public OnVehicleMod(playerid, vehicleid, componentid)
{
	return 1;
}

public OnVehiclePaintjob(playerid, vehicleid, paintjobid)
{
	return 1;
}

public OnVehicleRespray(playerid, vehicleid, color1, color2)
{
	return 1;
}

public OnPlayerSelectedMenuRow(playerid, row)
{
	return 1;
}

public OnPlayerExitedMenu(playerid)
{
	return 1;
}

public OnPlayerInteriorChange(playerid, newinteriorid, oldinteriorid)
{
	return 1;
}


public OnRconLoginAttempt(ip[], password[], success)
{
	return 1;
}

public OnPlayerUpdate(playerid)
{
	return 1;
}

public OnPlayerStreamIn(playerid, forplayerid)
{
	return 1;
}

public OnPlayerStreamOut(playerid, forplayerid)
{
	return 1;
}

public OnVehicleStreamIn(vehicleid, forplayerid)
{
	return 1;
}

public OnVehicleStreamOut(vehicleid, forplayerid)
{
	return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	return 1;
}

public OnPlayerClickPlayer(playerid, clickedplayerid, source)
{
	return 1;
}


// useful testing cmds
CMD:car(playerid, params[])
{
    // Check if params were provided
    if (!strlen(params))
    {
        SendClientMessage(playerid, -1, "Usage: /car [vehicle ID]");
        return 1;
    }

    // Convert string to integer manually
    new vehicleid = strval(params);

    if (vehicleid < 400 || vehicleid > 611)
    {
        SendClientMessage(playerid, -1, "Invalid vehicle model ID. Use 400 to 611.");
        return 1;
    }

    // Get player position and angle
    new Float:x, Float:y, Float:z, Float:a;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    // Create vehicle slightly offset from player
    new spawned = CreateVehicle(vehicleid, x + 2.0, y, z, a, -1, -1, 60000);
    PutPlayerInVehicle(playerid, spawned, 0);

    SendClientMessage(playerid, -1, "Vehicle spawned successfully.");
    return 1;
}

CMD:weapon(playerid, params[])
{
	// Check if params were provided
	if (!strlen(params))
	{
		SendClientMessage(playerid, -1, "Usage: /weapon [weapon ID]");
		return 1;
	}

	// Convert string to integer manually
	new weaponid = strval(params);

	// Validate weapon ID range
	if (weaponid < 0 || weaponid > 46)
	{
		SendClientMessage(playerid, -1, "Invalid weapon ID. Use 0 to 46.");
		return 1;
	}

	// Give the weapon to the player
	GivePlayerWeapon(playerid, weaponid, 1000);
	
	SendClientMessage(playerid, -1, "Weapon given successfully.");
	return 1;
}

