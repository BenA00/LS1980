#include <YSI_Coding\y_hooks>

// ADMIN RANKS
enum AdminLevel {
    ADMIN_LVL_NONE = 0,
    ADMIN_LVL_TRIAL,
    ADMIN_LVL_JUNIOR,
    ADMIN_LVL_GENERAL,
    ADMIN_LVL_SENIOR,
    ADMIN_LVL_MANAGER,
    ADMIN_LVL_DEVELOPER
}

new AdminLevel:gAdminLevel[MAX_PLAYERS];
new bool:gOnDuty[MAX_PLAYERS];
new Float:gLastPos[MAX_PLAYERS][3]; // X, Y, Z

bool:IsAdminLevelAtLeast(playerid, level)
{
    return gAdminLevel[playerid] >= level;
}

// ------ AJAIL DATA --------


// ajail vars
new bool:gIsJailed[MAX_PLAYERS];
new gJailTime[MAX_PLAYERS];
new gJailTimer[MAX_PLAYERS];
// ajail constants
#define AJAIL_X 264.0
#define AJAIL_Y 77.0
#define AJAIL_Z 1001.0
#define AJAIL_INTERIOR 6
#define AJAIL_VWORLD 0
// ajail stock
stock JailPlayer(playerid, seconds)
{
    gIsJailed[playerid] = true;
    gJailTime[playerid] = seconds;
    SetPlayerPos(playerid, AJAIL_X, AJAIL_Y, AJAIL_Z);
    SetPlayerInterior(playerid, AJAIL_INTERIOR);
    SetPlayerVirtualWorld(playerid, AJAIL_VWORLD);
    gJailTimer[playerid] = SetTimerEx("JailTick", 1000, true, "i", playerid);
}
// ajail countdown
forward JailTick(playerid);
public JailTick(playerid)
{
    if (!gIsJailed[playerid]) return 0;

    gJailTime[playerid]--;
    if (gJailTime[playerid] <= 0)
    {
        UnjailPlayer(playerid);
    }
    return 1;
}

//unjail function
stock UnjailPlayer(playerid)
{
    gIsJailed[playerid] = false;
    KillTimer(gJailTimer[playerid]);
    TogglePlayerControllable(playerid, true);
    SetPlayerPos(playerid, 1481.0, -1720.0, 13.5); // pershing for now
    SetPlayerInterior(playerid, 0);
    SetPlayerVirtualWorld(playerid, 0);
	TogglePlayerControllable(playerid, true);
    SendClientMessage(playerid, -1, "You have been released from admin jail.");
	
}

// ADMIN CMDS


CMD:aduty(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_TRIAL))
        return SendClientMessage(playerid, -1, "You lack permission.");

    gOnDuty[playerid] = !gOnDuty[playerid];

    if (gOnDuty[playerid])
    {
        SetPlayerHealth(playerid, 999999.0); // or 1000.0 if you prefer
        SetPlayerSkin(playerid, 217);
        SendClientMessage(playerid, -1, "You are now on admin duty.");
    }
    else
    {
        SetPlayerHealth(playerid, 100.0);
        SendClientMessage(playerid, -1, "You are now off admin duty.");
    }

    return 1;
}

CMD:admins(playerid, params[])
{
    SendClientMessage(playerid, -1, "=== Admins Online ===");

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!IsPlayerConnected(i)) continue;
        if (gAdminLevel[i] == ADMIN_LVL_NONE) continue;

        new name[MAX_PLAYER_NAME];
        GetPlayerName(i, name, sizeof(name));

        new msg[144], dutyStatus[8];
        format(dutyStatus, sizeof(dutyStatus), gOnDuty[i] ? "OnDuty" : "OffDuty");
        format(msg, sizeof(msg), "[ID:%d] %s | Level: %d | %s", i, name, gAdminLevel[i], dutyStatus);
        SendClientMessage(playerid, -1, msg);
    }

    return 1;
}

CMD:givecash(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_TRIAL))
        return SendClientMessage(playerid, -1, "You lack permission.");

    new targetid, amount;
    if (sscanf(params, "ii", targetid, amount))
        return SendClientMessage(playerid, -1, "Usage: /givecash [playerid] [amount]");

    GivePlayerMoney(targetid, amount);
    return 1;
}

CMD:giveallcash(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_JUNIOR))
        return SendClientMessage(playerid, -1, "You lack permission.");

    new amount;
    if (sscanf(params, "i", amount))
        return SendClientMessage(playerid, -1, "Usage: /giveallcash [amount]");

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!IsPlayerConnected(i) || i == playerid) continue;
        GivePlayerMoney(i, amount);
    }

    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));

    new msg[128];
    format(msg, sizeof(msg), "[ADMIN] %s gave $%d to all players online.", name, amount);
    SendClientMessageToAll(-1, msg);

    return 1;
}

CMD:givegun(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_GENERAL))
        return SendClientMessage(playerid, -1, "You lack permission.");

    new targetid, gunid, ammo;
    if (sscanf(params, "iii", targetid, gunid, ammo))
        return SendClientMessage(playerid, -1, "Usage: /givegun [playerid] [gunid] [ammo]");

    GivePlayerWeapon(targetid, gunid, ammo);
    return 1;
}

CMD:giveallgun(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_JUNIOR))
        return SendClientMessage(playerid, -1, "You lack permission.");

    new weaponid, ammo;
    if (sscanf(params, "ii", weaponid, ammo))
        return SendClientMessage(playerid, -1, "Usage: /giveallgun [weaponid] [ammo]");

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!IsPlayerConnected(i) || i == playerid) continue;
        GivePlayerWeapon(i, weaponid, ammo);
    }

    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));

    new msg[128];
    format(msg, sizeof(msg), "[ADMIN] %s gave weapon ID %d (%d ammo) to all players online.", name, weaponid, ammo);
    SendClientMessageToAll(-1, msg);
    return 1;
}

CMD:disarm(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_JUNIOR))
        return SendClientMessage(playerid, -1, "You lack permission.");

    new targetid;
    if (sscanf(params, "i", targetid))
        return SendClientMessage(playerid, -1, "Usage: /disarm [playerid]");

    ResetPlayerWeapons(targetid);
    SendClientMessage(targetid, -1, "You've been disarmed by an admin.");
    return 1;
}

CMD:slap(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_TRIAL))
        return SendClientMessage(playerid, -1, "You lack permission.");

    new targetid;
    if (sscanf(params, "i", targetid))
        return SendClientMessage(playerid, -1, "Usage: /slap [playerid]");

    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    SetPlayerPos(targetid, x, y, z + 5.0);
    SendClientMessage(targetid, -1, "You've been slapped by an admin.");
    return 1;
}

CMD:kick(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_MANAGER))
        return SendClientMessage(playerid, -1, "You lack permission.");

    new targetid, reason[128];
    if (sscanf(params, "is", targetid, reason))
        return SendClientMessage(playerid, -1, "Usage: /kick [playerid] [reason]");

    new msg[144];
    format(msg, sizeof(msg), "You have been kicked by an admin: %s", reason);
    SendClientMessage(targetid, -1, msg);
    Kick(targetid);
    return 1;
}

CMD:goto(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_TRIAL))
        return SendClientMessage(playerid, -1, "You dont have permission to use this");
    new targetid;
    if (sscanf(params, "i", targetid))
        return SendClientMessage(playerid, -1, "Usage: /goto [playerid]");

    if (!IsPlayerConnected(targetid))
        return SendClientMessage(playerid, -1, "Player not connected");

    GetPlayerPos(playerid, gLastPos[playerid][0], gLastPos[playerid][1], gLastPos[playerid][2]);

    new Float:x, Float:y, Float:z;
    GetPlayerPos(targetid, x, y, z);
    SetPlayerPos(playerid, x + 1.0, y, z);

    SendClientMessage(playerid, -1, "You teleported to your target");
    return 1;
}

CMD:gotopos(playerid, params[])
{
    // Check admin level
    if (!IsAdminLevelAtLeast(playerid) < ADMIN_LVL_TRIAL)
        return SendClientMessage(playerid, -1, "You must be at least Trial Admin to use this command.");

    new Float:x, Float:y, Float:z;

    if (sscanf(params, "fff", x, y, z))
        return SendClientMessage(playerid, -1, "Usage: /gotopos <X> <Y> <Z>");

    SetPlayerPos(playerid, x, y, z);
    SendClientMessage(playerid, -1, "Teleported to the specified coordinates.");
    return 1;
}

CMD:goback(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_TRIAL))
        return SendClientMessage(playerid, -1, "You lack permission.");

    // Optional: validate saved position is valid (e.g., non-zero)
    if (gLastPos[playerid][0] == 0.0 && gLastPos[playerid][1] == 0.0 && gLastPos[playerid][2] == 0.0)
        return SendClientMessage(playerid, -1, "No previous position stored.");

    SetPlayerPos(playerid, gLastPos[playerid][0], gLastPos[playerid][1], gLastPos[playerid][2]);
    SendClientMessage(playerid, -1, "Returned to your previous position.");
    return 1;
}

CMD:heal(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_TRIAL))
        return SendClientMessage(playerid, -1, "You lack permission.");

    new targetid, amount;
    if (sscanf(params, "ii", targetid, amount))
        return SendClientMessage(playerid, -1, "Usage: /heal [playerid] [amount]");

    if (!IsPlayerConnected(targetid))
        return SendClientMessage(playerid, -1, "Player not connected.");

    if (amount < 1 || amount > 100)
        return SendClientMessage(playerid, -1, "Health amount must be between 1 and 100.");

    SetPlayerHealth(targetid, float(amount));

    new adminName[MAX_PLAYER_NAME], targetName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, adminName, sizeof(adminName));
    GetPlayerName(targetid, targetName, sizeof(targetName));

    new msg[128];
    format(msg, sizeof(msg), "[ADMIN] %s healed %s to %d health.", adminName, targetName, amount);
    SendClientMessageToAll(-1, msg);

    return 1;
}

CMD:setap(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_TRIAL))
        return SendClientMessage(playerid, -1, "You lack permission.");

    new targetid, amount;
    if (sscanf(params, "ii", targetid, amount))
        return SendClientMessage(playerid, -1, "Usage: /setap [playerid] [amount]");

    if (!IsPlayerConnected(targetid))
        return SendClientMessage(playerid, -1, "Player not connected.");

    if (amount < 0 || amount > 100)
        return SendClientMessage(playerid, -1, "Armour must be between 0 and 100.");

    SetPlayerArmour(targetid, float(amount));

    new adminName[MAX_PLAYER_NAME], targetName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, adminName, sizeof(adminName));
    GetPlayerName(targetid, targetName, sizeof(targetName));

    new msg[128];
    format(msg, sizeof(msg), "[ADMIN] %s set %s's armour to %d.", adminName, targetName, amount);
    SendClientMessageToAll(-1, msg);
    return 1;
}

CMD:sethp(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_TRIAL))
        return SendClientMessage(playerid, -1, "You lack permission.");

    new targetid, amount;
    if (sscanf(params, "ii", targetid, amount))
        return SendClientMessage(playerid, -1, "Usage: /sethp [playerid] [amount]");

    if (!IsPlayerConnected(targetid))
        return SendClientMessage(playerid, -1, "Player not connected.");

    if (amount < 1 || amount > 100)
        return SendClientMessage(playerid, -1, "Health must be between 1 and 100.");

    SetPlayerHealth(targetid, float(amount));

    new adminName[MAX_PLAYER_NAME], targetName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, adminName, sizeof(adminName));
    GetPlayerName(targetid, targetName, sizeof(targetName));

    new msg[128];
    format(msg, sizeof(msg), "[ADMIN] %s set %s's health to %d.", adminName, targetName, amount);
    SendClientMessageToAll(-1, msg);
    return 1;
}

CMD:setskin(playerid, params[])
{
    new targetid, skinid;

    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_TRIAL))
        return SendClientMessage(playerid, -1, "You lack permission to set skins.");

    if (sscanf(params, "ii", targetid, skinid))
        return SendClientMessage(playerid, -1, "Usage: /setskin <playerid> <skinid>");

    if (!IsPlayerConnected(targetid))
        return SendClientMessage(playerid, -1, "Target player is not connected.");

    if (skinid < 0 || skinid > 311) // SA-MP has ~312 default skins
        return SendClientMessage(playerid, -1, "Invalid skin ID.");

    SetPlayerSkin(targetid, skinid);
 	new name[24], targetName[24];
    GetPlayerName(playerid, name, sizeof(name));
    GetPlayerName(targetid, targetName, sizeof(targetName));

    new msg[128];
    format(msg, sizeof(msg), "Admin %s has changed %s's skin to ID %d.", name, targetName, skinid);
    SendClientMessageToAll(-1, msg);

    return 1;
}

// ------- AJAIL CMDS --------

CMD:ajail(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_GENERAL))
        return SendClientMessage(playerid, -1, "You lack permission.");

    new targetid, minutes;
	new reason[32];
    if (sscanf(params, "iis", targetid, minutes, reason))
        return SendClientMessage(playerid, -1, "Usage: /ajail [playerid] [minutes] [reason]");

    if (!IsPlayerConnected(targetid))
        return SendClientMessage(playerid, -1, "Player not found.");

    JailPlayer(targetid, minutes * 60);

    new adminName[MAX_PLAYER_NAME], targetName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, adminName, sizeof(adminName));
    GetPlayerName(targetid, targetName, sizeof(targetName));

    new msg[144];
    format(msg, sizeof(msg), "[ADMIN] %s jailed %s for %d minute(s) Reason: %s", adminName, targetName, minutes, reason);
    SendClientMessageToAll(-1, msg);

    return 1;
}

CMD:unajail(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_GENERAL))
        return SendClientMessage(playerid, -1, "You lack permission.");

    new targetid;
    if (sscanf(params, "i", targetid))
        return SendClientMessage(playerid, -1, "Usage: /unajail [playerid]");

    if (!IsPlayerConnected(targetid))
        return SendClientMessage(playerid, -1, "Player not found.");

    if (!gIsJailed[targetid])
        return SendClientMessage(playerid, -1, "Player is not jailed.");

    UnjailPlayer(targetid);

    new adminName[MAX_PLAYER_NAME], targetName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, adminName, sizeof(adminName));
    GetPlayerName(targetid, targetName, sizeof(targetName));

    new msg[128];
    format(msg, sizeof(msg), "[ADMIN] %s released %s from admin jail.", adminName, targetName);
    SendClientMessageToAll(-1, msg);

    return 1;
}

CMD:setadmin(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_MANAGER))
        return SendClientMessage(playerid, -1, "Only Managers or higher can set admin levels.");

    new targetid, level;
    if (sscanf(params, "ii", targetid, level))
        return SendClientMessage(playerid, -1, "Usage: /setadmin [playerid] [level 1-6]");

    if (!IsPlayerConnected(targetid))
        return SendClientMessage(playerid, -1, "Player not connected.");

    if (AdminLevel < ADMIN_LVL_TRIAL || AdminLevel > ADMIN_LVL_DEVELOPER)
        return SendClientMessage(playerid, -1, "Invalid level. Must be 1 (TRIAL) to 6 (DEVELOPER).");

    gAdminLevel[targetid] = AdminLevel:level;

    new name[MAX_PLAYER_NAME], targetName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));
    GetPlayerName(targetid, targetName, sizeof(targetName));

    new msg[144];
    format(msg, sizeof(msg), "%s set %s's admin level to %d.", name, targetName, level);
    SendClientMessageToAll(-1, msg);
    return 1;
}