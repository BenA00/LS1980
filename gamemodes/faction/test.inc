#include <YSI_Coding\y_hooks>

// ---------- Constants ----------
#define COLOR_WHITE 0xFFFFFFFF
#define COLOR_BLUE  0x0000FFFF
#define COLOR_RED   0xFF0000FF
#define COLOR_GREEN 0x00FF00FF

// ---------- Faction Enum ----------
enum Faction
{
    FACTION_NONE = 0,
    FACTION_ANGELS,
    FACTION_BARRIO,
    FACTION_ORANGE_GROVE,
    FACTION_WEST_BALLAS,
    FACTION_EAST_BALLAS,
    FACTION_LSPD,
    FACTION_TEST
};

// ---------- Faction Tier Enum ----------
enum FactionTier
{
    FACTION_TIER_NONE = 0,
    FACTION_TIER_1, // Member
    FACTION_TIER_2, // Supervisor/OG
    FACTION_TIER_3  // Leader/Co-Leader
};

#define MAX_FACTION_TIERS (FACTION_TIER_3)
// ---------- Faction Name Mapping ----------
new const FactionNames[][] = {
    "None",
    "Angels of Death MC",
    "Barrio Brown Pride",
    "Orange Grove Families",
    "West Side Ballas",
    "East Side Ballas",
    "LSPD",
    "Test Faction"
};

// -------- Faction colours --------

new const FactionChatColors[] = {
    COLOR_WHITE,        // FACTION_NONE
    0xAA3333FF,         // Angels of Death MC (dark red)
    0xCC9933FF,         // Barrio Brown Pride (bronze)
    0x33CC33FF,         // Orange Grove Families (green)
    0x9933CCFF,         // West Side Ballas (purple)
    0xCC33CCFF,         // East Side Ballas (pink)
    0x3399FFFF,         // LSPD (blue)
    0xCCCCCCFF          // Test Faction (gray)
};

// ---------- Player Data ----------
new Faction:gFaction[MAX_PLAYERS];
new FactionTier:gFactionTier[MAX_PLAYERS];
new gFactionRank[MAX_PLAYERS][32];

// ---------- Utility Functions ----------
stock GetFactionName(Faction:faction, dest[], size)
{
    if (faction < FACTION_NONE || faction >= sizeof(FactionNames))
    {
        format(dest, size, "Unknown");
        return 1;
    }

    format(dest, size, "%s", FactionNames[faction]);
    return 1;
}

// ---------- Helper ----------
stock GetPlayerNameEx(playerid)
{
    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));
    return name;
}

stock bool:IsPlayerInFaction(playerid)
{
    return gFaction[playerid] != FACTION_NONE;
}

stock bool:IsPlayerInPoliceFaction(playerid)
{
    return gFaction[playerid] == FACTION_LSPD;
}

stock bool:IsPlayerFactionTier(playerid, FactionTier:tier)
{
    return gFactionTier[playerid] >= tier;
}

stock GetFactionChatColor(Faction:faction)
{
    if (faction < FACTION_NONE || faction >= sizeof(FactionChatColors))
        return COLOR_WHITE;
    return FactionChatColors[faction];
}

stock SetPlayerFactionRank(playerid, const rank[])
{
    format(gFactionRank[playerid], sizeof(gFactionRank[]), "%s", rank);
    // Optionally trigger save, update UI, etc.
}
// ---------- Commands ----------


// TESTING CMDS ----------------------------
CMD:joinfaction(playerid, params[])
{
    if (IsPlayerInFaction(playerid))
        return SendClientMessage(playerid, COLOR_RED, "You are already in a faction.");

    new factionId;
    if (sscanf(params, "i", factionId))
        return SendClientMessage(playerid, COLOR_WHITE, "Usage: /joinfaction <faction_id>");

    if (factionId < FACTION_ANGELS || factionId >= sizeof(FactionNames))
        return SendClientMessage(playerid, COLOR_RED, "Invalid faction ID.");

    gFaction[playerid] = Faction:factionId;
    gFactionTier[playerid] = FACTION_TIER_1;

    new factionName[32];
    GetFactionName(gFaction[playerid], factionName, sizeof(factionName));

    /*new msg[64];
    format(msg, sizeof(msg), "You have joined the %s faction.", factionName);
    SendClientMessage(playerid, COLOR_GREEN, msg); */
    return 1;
}

CMD:leavefaction(playerid, params[])
{
    if (!IsPlayerInFaction(playerid))
        return SendClientMessage(playerid, COLOR_RED, "You are not in a faction.");

    gFaction[playerid] = FACTION_NONE;
    gFactionTier[playerid] = FACTION_TIER_NONE;

    // SendClientMessage(playerid, COLOR_GREEN, "You have left your faction."); 
    return 1;
}


// --------------------------


CMD:faction(playerid, params[])
{
    new msg[128];
    format(msg, sizeof(msg), "Faction: %s | Tier: %d", GetFactionName(gFaction[playerid]), gFactionTier[playerid]);
    SendClientMessage(playerid, COLOR_WHITE, msg);
    return 1;
}

CMD:f(playerid, params[])
{
    if (!IsPlayerInFaction(playerid))
        return SendClientMessage(playerid, COLOR_RED, "You are not in a faction.");

    if (isnull(params))
        return SendClientMessage(playerid, COLOR_WHITE, "Usage: /factionchat <message>");

    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));

    new msg[144];
    format(msg, sizeof(msg), "[Faction Chat] %s: %s", name, params);

    new color = GetFactionChatColor(gFaction[playerid]);

    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && gFaction[i] == gFaction[playerid])
        {
            SendClientMessage(i, color, msg);
        }
    }
    return 1;
}

CMD:settier(playerid, params[])
{
    if (!IsAdminLevelAtLeast(playerid, ADMIN_LVL_DEVELOPER)) 
        return SendClientMessage(playerid, COLOR_RED, "You don't have permission to use this command.");

    new targetid, tier;
    if (sscanf(params, "ui", targetid, tier))
        return SendClientMessage(playerid, COLOR_BLUE, "Usage: /settier [playerid] [tier]");

    if (!IsPlayerConnected(targetid))
        return SendClientMessage(playerid, COLOR_RED, "Invalid player ID.");

    if (tier < FACTION_TIER_NONE || tier > MAX_FACTION_TIERS)
        return SendClientMessage(playerid, COLOR_RED, "Invalid tier value.");

    gFactionTier[targetid] = FactionTier:tier;

    new name[MAX_PLAYER_NAME], factionName[32];
    GetPlayerName(targetid, name, sizeof(name));
    GetFactionName(gFaction[targetid], factionName, sizeof(factionName));

    new msg[128];
    format(msg, sizeof(msg), "You have set %s's tier in %s to %d.", name, factionName, tier);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    format(msg, sizeof(msg), "Your faction tier has been set to %d by an admin.", tier);
    SendClientMessage(targetid, COLOR_BLUE, msg);

    return 1;
}

CMD:setrank(playerid, params[])
{
    if (!IsPlayerFactionTier(playerid, FACTION_TIER_2))
        return SendClientMessage(playerid, COLOR_RED, "You don't have permission to use this command.");

    new targetid, rankText[32];
    if (sscanf(params, "us[32]", targetid, rankText))
        return SendClientMessage(playerid, COLOR_BLUE, "Usage: /setrank [playerid] [rank name]");

    if (!IsPlayerConnected(targetid))
        return SendClientMessage(playerid, COLOR_RED, "Invalid player ID.");

    SetPlayerFactionRank(targetid, rankText); // Modular setter

    new name[MAX_PLAYER_NAME], factionName[32];
    GetPlayerName(targetid, name, sizeof(name));
    GetFactionName(gFaction[targetid], factionName, sizeof(factionName));

    new msg[128];
    format(msg, sizeof(msg), "You have set %s's rank in %s to \"%s\".", name, factionName, rankText);
    SendClientMessage(playerid, COLOR_GREEN, msg);

    format(msg, sizeof(msg), "Your faction rank has been set to \"%s\" by an admin.", rankText);
    SendClientMessage(targetid, COLOR_BLUE, msg);

    return 1;
}

CMD:factionon(playerid, params[])
{
    if (!IsPlayerInFaction(playerid))
        return SendClientMessage(playerid, COLOR_RED, "You're not in a faction.");

    new Faction:faction = gFaction[playerid];
    new factionName[32];
    GetFactionName(faction, factionName, sizeof(factionName));

    new header[64];
    format(header, sizeof(header), "Online members of %s:", factionName);
    SendClientMessage(playerid, COLOR_BLUE, header);

    new count = 0;
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (!IsPlayerConnected(i)) continue;
        if (gFaction[i] != faction) continue;

        new name[MAX_PLAYER_NAME];
        GetPlayerName(i, name, sizeof(name));

        new tierName[16];
        switch (gFactionTier[i])
        {
            case FACTION_TIER_1: tierName = "Member";
            case FACTION_TIER_2: tierName = "Supervisor";
            case FACTION_TIER_3: tierName = "Leader";
            default: tierName = "None";
        }

        new msg[128];
        format(msg, sizeof(msg), "%s (%s) - Tier: %s", name, gFactionRank[i], tierName);
        SendClientMessage(playerid, GetFactionChatColor(faction), msg);
        count++;
    }

    if (count == 0)
        SendClientMessage(playerid, COLOR_WHITE, "No other members of your faction are online.");

    return 1;
}

CMD:factioncmd(playerid, params[])
{
    if (!IsPlayerFactionTier(playerid, FACTION_TIER_2))
        return SendClientMessage(playerid, COLOR_RED, "You must be Tier 2 or higher to use this command.");

    SendClientMessage(playerid, COLOR_GREEN, "You have access to faction command.");
    return 1;
}

