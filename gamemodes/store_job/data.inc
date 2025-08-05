#include <YSI_Coding\y_hooks>

// === CONFIG ===
#define JOB_PICKUP_POS_X -25.3943
#define JOB_PICKUP_POS_Y -90.8605
#define JOB_PICKUP_POS_Z 1003.5469

#define BOX_OBJECT_ID 1271 // Realistic box model

// === INIT ===
public OnFilterScriptInit()
{
    print("\n----------------------------------");
    print("24/7 stock testing");
    return 1;
}

// === VARIABLES ===
new bool:g_PlayerOnJob[MAX_PLAYERS];
new bool:g_JobProgress[MAX_PLAYERS];
new g_PlayerBoxObj[MAX_PLAYERS];
new g_JobTargetShelf[MAX_PLAYERS];

new g_ShelvesFilled[MAX_PLAYERS];
new bool:g_StopRestocking[MAX_PLAYERS];
new g_ShelfOrder[MAX_PLAYERS][4]; // shuffled shelf sequence

new Float:g_ShelfPositions[][4] = {
    {-25.5331,-78.9479,1003.5469},
    {-17.2447,-78.9064,1003.5469},
    {-33.5364,-87.1933,1003.5469},
    {-35.5889,-83.0019,1003.5469}
};

// === JOB START ===
CMD:startrestocking(playerid, params[])
{
    if (g_PlayerOnJob[playerid]) return SendClientMessage(playerid, -1, "You're already on shift.");

    g_PlayerOnJob[playerid] = true;
    g_JobProgress[playerid] = false;
    g_ShelvesFilled[playerid] = 0;
    g_StopRestocking[playerid] = false;

    // Generate shelf shuffle
    for (new i = 0; i < sizeof(g_ShelfPositions); i++)
        g_ShelfOrder[playerid][i] = i;

    for (new i = sizeof(g_ShelfPositions) - 1; i > 0; i--)
    {
        new j = random(i + 1);
        new tmp = g_ShelfOrder[playerid][i];
        g_ShelfOrder[playerid][i] = g_ShelfOrder[playerid][j];
        g_ShelfOrder[playerid][j] = tmp;
    }

    SetPlayerCheckpoint(playerid, JOB_PICKUP_POS_X, JOB_PICKUP_POS_Y, JOB_PICKUP_POS_Z, 1.0);
    SendClientMessage(playerid, -1, "Shelf stocking started. Pick up the box from the tills.");
    return 1;
}

// === JOB STOP ===
CMD:stoprestocking(playerid, params[])
{
    if (!g_PlayerOnJob[playerid]) return SendClientMessage(playerid, -1, "You're not currently restocking.");

    g_StopRestocking[playerid] = true;
    SendClientMessage(playerid, -1, "You've chosen to end your shift early. Return the box to the back room for payout.");
    SetPlayerCheckpoint(playerid, JOB_PICKUP_POS_X, JOB_PICKUP_POS_Y, JOB_PICKUP_POS_Z, 1.0);
    return 1;
}

// === CHECKPOINTS ===
public OnPlayerEnterCheckpoint(playerid)
{
    new Float:tx, Float:ty, Float:tz;
    if (!g_PlayerOnJob[playerid]) return;

    // Pickup box
    if (!g_JobProgress[playerid] && IsPlayerInRangeOfPoint(playerid, 1.5, JOB_PICKUP_POS_X, JOB_PICKUP_POS_Y, JOB_PICKUP_POS_Z))
    {
        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);

        ApplyAnimation(playerid, "CARRY", "lift_up", 4.1, 1, 1, 1, 1, 1);
        g_PlayerBoxObj[playerid] = CreateObject(BOX_OBJECT_ID, x, y, z + 1.0, 0.0, 0.0, 0.0);
        AttachObjectToPlayer(g_PlayerBoxObj[playerid], playerid, 0.0, 0.15, -0.20, 270.0, 0.0, 0.0);

        g_JobTargetShelf[playerid] = g_ShelfOrder[playerid][g_ShelvesFilled[playerid]];
        tx = g_ShelfPositions[g_JobTargetShelf[playerid]][0];
        ty = g_ShelfPositions[g_JobTargetShelf[playerid]][1];
        tz = g_ShelfPositions[g_JobTargetShelf[playerid]][2];
        SetPlayerCheckpoint(playerid, tx, ty, tz, 1.0);

        SendClientMessage(playerid, -1, "You've picked up the box. Take it to the shelf.");
        return;
    }

    // Stock shelf
    tx = g_ShelfPositions[g_JobTargetShelf[playerid]][0];
    ty = g_ShelfPositions[g_JobTargetShelf[playerid]][1];
    tz = g_ShelfPositions[g_JobTargetShelf[playerid]][2];

    if (!g_JobProgress[playerid] && IsPlayerInRangeOfPoint(playerid, 1.5, tx, ty, tz))
    {
        DestroyObject(g_PlayerBoxObj[playerid]);
        g_PlayerBoxObj[playerid] = 0;

        ApplyAnimation(playerid, "BOMBER", "BOM_Plant_Loop", 4.1, 1, 1, 1, 1, 0);
        SendClientMessage(playerid, -1, "Stocking shelf...");

        SetTimerEx("FinishStacking", 20000, false, "d", playerid);
        DisablePlayerCheckpoint(playerid);
        return;
    }

    // Return box after stocking
    if (g_JobProgress[playerid] && IsPlayerInRangeOfPoint(playerid, 1.5, JOB_PICKUP_POS_X, JOB_PICKUP_POS_Y, JOB_PICKUP_POS_Z))
    {
        if (g_StopRestocking[playerid] || g_ShelvesFilled[playerid] >= sizeof(g_ShelfPositions))
        {
            new payout = g_ShelvesFilled[playerid] * 5;
            GivePlayerMoney(playerid, payout);

            new szMessage[128];
            format(szMessage, sizeof szMessage, "The store owner hands you $%d for stocking %d shelves.", payout, g_ShelvesFilled[playerid]);
            SendClientMessage(playerid, -1, szMessage);

            g_PlayerOnJob[playerid] = false;
            g_JobProgress[playerid] = false;
            g_ShelvesFilled[playerid] = 0;
            return;
        }

        g_JobProgress[playerid] = false;
        g_JobTargetShelf[playerid] = g_ShelfOrder[playerid][g_ShelvesFilled[playerid]];

        tx = g_ShelfPositions[g_JobTargetShelf[playerid]][0];
        ty = g_ShelfPositions[g_JobTargetShelf[playerid]][1];
        tz = g_ShelfPositions[g_JobTargetShelf[playerid]][2];
        SetPlayerCheckpoint(playerid, tx, ty, tz, 1.0);

        new Float:x, Float:y, Float:z;
        GetPlayerPos(playerid, x, y, z);
        g_PlayerBoxObj[playerid] = CreateObject(BOX_OBJECT_ID, x, y, z + 1.0, 0.0, 0.0, 0.0);
        AttachObjectToPlayer(g_PlayerBoxObj[playerid], playerid, 0.0, 0.15, -0.20, 270.0, 0.0, 0.0);

        SendClientMessage(playerid, -1, "Take the next box to the next shelf.");
        return;
    }
}

// === FINISH STOCKING ===
forward FinishStacking(playerid);
public FinishStacking(playerid)
{
    ClearAnimations(playerid);
    g_JobProgress[playerid] = true;
    g_ShelvesFilled[playerid]++;

    SendClientMessage(playerid, -1, "Shelf stocked. Return the box to the tills.");
    SetPlayerCheckpoint(playerid, JOB_PICKUP_POS_X, JOB_PICKUP_POS_Y, JOB_PICKUP_POS_Z, 1.0);
}

// === CLEANUP ===
public OnPlayerDisconnect(playerid, reason)
{
    if (g_PlayerBoxObj[playerid] != 0)
    {
        DestroyObject(g_PlayerBoxObj[playerid]);
        g_PlayerBoxObj[playerid] = 0;
    }

    g_PlayerOnJob[playerid] = false;
    g_JobProgress[playerid] = false;
    g_ShelvesFilled[playerid] = 0;
    g_StopRestocking[playerid] = false;
}

public OnFilterScriptExit()
{
    return 1;
}