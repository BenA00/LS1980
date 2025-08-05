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