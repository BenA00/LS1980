#include <YSI_Coding\y_hooks>

// ----------- useful testing cmds
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

CMD:pos(playerid, params[])
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    new msg[64];
    format(msg, sizeof(msg), "Position: %.2f, %.2f, %.2f", x, y, z);
    SendClientMessage(playerid, -1, msg);
    return 1;
}