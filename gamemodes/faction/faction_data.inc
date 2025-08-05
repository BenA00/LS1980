// FACTION ENUM

enum Faction {
    FACTION_NONE = 0,
    FACTION_AOD_OMG,
    FACTION_ORANGE_GROVE,
    FACTION_EAST_SIDE_BALLAS,
    FACTION_WEST_SIDE_BALLAS,
    FACTION_BARRIO_BROWN_PRIDE,
    FACTION_LSPD
}

// faction tiers
enum FactionRank {
    RANK_NONE = 0,
    RANK_TIER_1,
    RANK_TIER_2,
    RANK_TIER_3,
    RANK_TIER_4,
    RANK_LEADER
}


// LSPD SQUAD ENUM

enum Squad {
    SQUAD_NONE = 0,
    SQUAD_SWAT
}

// Fac Stocks
stock IsInFaction(playerid, Faction:fac)
{
    return gFaction[playerid] == fac;
}

stock IsLegalFaction(playerid)
{
    return gFaction[playerid] == FACTION_LSPD;
}

stock GetFactionName(Faction:fac)
{
    switch (fac)
    {
        case FACTION_AOD_OMG: return "Angels of Death OMG";
        case FACTION_ORANGE_GROVE: return "Orange Grove Families";
        case FACTION_EAST_SIDE_BALLAS: return "East Side Ballas";
        case FACTION_WEST_SIDE_BALLAS: return "West Side Ballas";
        case FACTION_BARRIO_BROWN_PRIDE: return "Barrio Brown Pride";
        case FACTION_LSPD: return "LSPD";
    }
    return "Unassigned";
}

stock GetSquadName(Squad:sq)
{
    switch (sq)
    {
        case SQUAD_NONE: return "Patrol";
        case SQUAD_SWAT: return "SWAT";
    }
    return "Unknown";
}

stock IsFactionLeader(playerid)
{
    return gFactionRank[playerid] == RANK_LEADER;
}

stock IsSameFaction(playerid1, playerid2)
{
    return gFaction[playerid1] == gFaction[playerid2];
}


// ----------- FACTION SCRIPT -------------

CMD:factionhelp(playerid, params[])
{
	SendClientMessage(playerid, -1, "Faction Commands:");
	SendClientMessage(playerid, -1, "/factions - Show available factions and tiers");
	SendClientMessage(playerid, -1, "/setsquad [playerid] [squadid] - Assign a squad to a player");
	return 1;
}

CMD:factions(playerid, params[])
{
    new factionCounts[7]; // Assuming 7 factions (0–6), adjust if needed

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!IsPlayerConnected(i)) continue;
        factionCounts[gFaction[i]]++;
    }

    new msg[256];
    format(msg, sizeof(msg),
        "Factions Information\n\n\
Angels of Death OMG: %d\n\
Orange Grove Families: %d\n\
East Side Ballas: %d\n\
West Side Ballas: %d\n\
Barrio Brown Pride: %d\n\
LSPD: %d",
        factionCounts[FACTION_AOD_OMG],
        factionCounts[FACTION_ORANGE_GROVE],
        factionCounts[FACTION_EAST_SIDE_BALLAS],
        factionCounts[FACTION_WEST_SIDE_BALLAS],
        factionCounts[FACTION_BARRIO_BROWN_PRIDE],
        factionCounts[FACTION_LSPD]
    );

    ShowPlayerDialog(playerid, DIALOG_FACTIONS, DIALOG_STYLE_MSGBOX, "Factions Online", msg, "Close", "");
    return 1;
}

// --------- LSPD FACTION CMDS ------------------------

CMD:setsquad(playerid, params[])
{
    new targetid, squadid;
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_MANAGER))
        return SendClientMessage(playerid, -1, "You lack permissionto use this cm");

    if (sscanf(params, "ii", targetid, squadid))
        return SendClientMessage(playerid, -1, "Usage: /setsquad [playerid] [squadid]");

    if (!IsPlayerConnected(targetid))
        return SendClientMessage(playerid, -1, "Target not connected");

    if (gFaction[targetid] != FACTION_LSPD)
        return SendClientMessage(playerid, -1, "Player is not part of LSPD");

    if (Squad < SQUAD_NONE || Squad > SQUAD_SWAT)
        return SendClientMessage(playerid, -1, "Invalid squad ID");

    gSquad[targetid] = Squad:squadid;

    new targetName[MAX_PLAYER_NAME], adminName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, adminName, sizeof(adminName));
    GetPlayerName(targetid, targetName, sizeof(targetName));

    new msg[128];
    format(msg, sizeof(msg), "[ADMIN] %s assigned squad %d to %s.", adminName, squadid, targetName);
    SendClientMessageToAll(-1, msg);
    return 1;
}
