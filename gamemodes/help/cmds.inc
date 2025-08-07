#include <YSI_Coding\y_hooks>

// test help - temp to focus on what needs tested
CMD:testhelp(playerid, params[])
{
	SendClientMessage(playerid, -1, "Available commands:");
	SendClientMessage(playerid, -1, "/car [vehicle ID] - Spawn a vehicle");
	SendClientMessage(playerid, -1, "/weapon [weapon ID] - Give a weapon");
	SendClientMessage(playerid, -1, "/startrestocking - Start 24/7 Job");
	SendClientMessage(playerid, -1, "/stoprestocking - Stop 24/7 Job");
	SendClientMessage(playerid, -1, "/admins - List online admins");
	return 1;
}

// admin help

CMD:adminhelp(playerid, params[])
{
	SendClientMessage(playerid, -1, "Admin Commands:");   
	SendClientMessage(playerid, -1, "/givecash [playerid] [amount] - Give cash to a player");
	SendClientMessage(playerid, -1, "/giveallcash [amount] - Give cash to all players");
	SendClientMessage(playerid, -1, "/giveallgun [weaponid] [ammo] - Give a weapon to all players");
	SendClientMessage(playerid, -1, "/givegun [playerid] [gunid] [ammo] - Give a gun to a player");
	SendClientMessage(playerid, -1, "/disarm [playerid] - Disarm player");
	SendClientMessage(playerid, -1, "/heal [playerid] [amount] - Heal a player");
	SendClientMessage(playerid, -1, "/sethp [playerid] [amount]");
	SendClientMessage(playerid, -1, "/setap [playerid] [amount] - Set armour for a player");
	SendClientMessage(playerid, -1, "/setskin [playerid] [skinid] - Set skin for a player");
	SendClientMessage(playerid, -1, "/slap [playerid] - Slap player");
	SendClientMessage(playerid, -1, "/kick [playerid] [reason] - kick player");
	SendClientMessage(playerid, -1, "/ajail [playerid] [minutes] [reason] - Jail a player");
	SendClientMessage(playerid, -1, "/unajail [playerid] - Unjail a player");
	SendClientMessage(playerid, -1, "/goto [playerid] - tp to a player");
	SendClientMessage(playerid, -1, "/goback - return to your last position");
	SendClientMessage(playerid, -1, "/setadmin [playerid] [level] - set admin lvl");
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

CMD:getpos(playerid, params[])
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    new msg[128];
    format(msg, sizeof(msg), "Your current position: X: %.2f, Y: %.2f, Z: %.2f", x, y, z);
    SendClientMessage(playerid, -1, msg);
    return 1;
}