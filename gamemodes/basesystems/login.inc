
#include <YSI_Coding\y_hooks>

// Constants
#define MAX_USERNAME_LENGTH 24
#define MAX_PASSWORD_LENGTH 64
#define MAX_CHARACTERS 4
#define MAX_CHARACTER_NAME 24

// Dialog IDs
#define DIALOG_REGISTER 1000
#define DIALOG_LOGIN 1001
#define DIALOG_CHAR_SELECT 1002
#define DIALOG_CHAR_CREATE 1003
#define DIALOG_CHAR_DETAILS 1004
#define DIALOG_SPAWN_SELECT 1005
#define DIALOG_CHAR_RACE 1006
#define DIALOG_CHAR_HEIGHT 1007
#define DIALOG_CHAR_GENDER 1008

// Character data enum
enum E_CHARACTER_DATA {
    bool:cExists,
    cName[MAX_CHARACTER_NAME],
    cModel,
    Float:cHealth,
    cMoney,
    Float:cLastPosX,
    Float:cLastPosY,
    Float:cLastPosZ,
    bool:cCustomSpawn,
    cRace[20],
    Float:cHeight,
    cGender[10]
}

// Player data enum
enum E_PLAYER_DATA {
    pUsername[MAX_USERNAME_LENGTH],
    pUserId,
    bool:pRegistered,
    bool:pLoggedIn,
    pSelectedChar,
    pTempName[MAX_CHARACTER_NAME]
}

// Player data arrays
new PlayerInfo[MAX_PLAYERS][E_PLAYER_DATA];
new PlayerCharacters[MAX_PLAYERS][MAX_CHARACTERS][E_CHARACTER_DATA];

// Database functions
stock GetUserIdFromUsername(const username[]) {
    new query[128], DBResult:result, userId = -1;
    format(query, sizeof(query), "SELECT userId FROM users WHERE username = '%q'", username);
    result = db_query(server_database, query);
    if (db_num_rows(result)) {
        userId = db_get_field_int(result, 0);
    }
    db_free_result(result);
    new debug_msg[128];
    format(debug_msg, sizeof(debug_msg), "[LS80] GetUserIdFromUsername('%s') returned userId %d", username, userId);
    printf(debug_msg);
    return userId;
}

stock bool:GetUserPassword(const username[], password[], len) {
    new query[128], DBResult:result;
    format(query, sizeof(query), "SELECT password FROM users WHERE username = '%q'", username);
    result = db_query(server_database, query);
    if (db_num_rows(result)) {
        db_get_field_assoc(result, "password", password, len);
        db_free_result(result);
        new debug_msg[128];
        format(debug_msg, sizeof(debug_msg), "[LS80] GetUserPassword('%s') found password", username);
        printf(debug_msg);
        return true;
    }
    db_free_result(result);
    new debug_msg[128];
    format(debug_msg, sizeof(debug_msg), "[LS80] GetUserPassword('%s') no password found", username);
    printf(debug_msg);
    return false;
}

stock SaveUser(const username[], const password[]) {
    new query[256];
    format(query, sizeof(query), "INSERT INTO users (username, password) VALUES ('%q', '%q')", username, password);
    new DBResult:result = db_query(server_database, query);
    new save_msg[128];
    format(save_msg, sizeof(save_msg), "[LS80] Saved user '%s' to database.", username);
    printf(save_msg);
    if (!result) {
        format(save_msg, sizeof(save_msg), "[LS80] Error: Failed to save user '%s'.", username);
        printf(save_msg);
    }
    db_free_result(result);
}

stock SaveCharacter(userId, const name[], slot, const race[], Float:height, const gender[]) {
    new query[512];
    format(query, sizeof(query), "INSERT INTO characters (userId, name, slot, race, height, gender) VALUES (%d, '%q', %d, '%q', %.2f, '%q')",
        userId, name, slot, race, height, gender);
    new DBResult:result = db_query(server_database, query);
    new save_msg[128];
    format(save_msg, sizeof(save_msg), "[LS80] Saved character '%s' in slot %d for userId %d (race: %s, height: %.2fm, gender: %s).",
        name, slot + 1, userId, race, height, gender);
    printf(save_msg);
    if (!result) {
        format(save_msg, sizeof(save_msg), "[LS80] Error: Failed to save character '%s' in slot %d.", name, slot + 1);
        printf(save_msg);
    }
    db_free_result(result);
}

stock bool:CheckCharacterNameUnique(const name[]) {
    new query[128], DBResult:result;
    format(query, sizeof(query), "SELECT name FROM characters WHERE name = '%q'", name);
    result = db_query(server_database, query);
    new bool:isUnique = (db_num_rows(result) == 0);
    db_free_result(result);
    new debug_msg[128];
    format(debug_msg, sizeof(debug_msg), "[LS80] CheckCharacterNameUnique('%s') returned %s", name, isUnique ? "unique" : "not unique");
    printf(debug_msg);
    return isUnique;
}

stock LoadCharacters(userId, playerid) {
    new query[128], DBResult:result;
    format(query, sizeof(query), "SELECT name, slot, race, height, gender FROM characters WHERE userId = %d ORDER BY slot", userId);
    new debug_msg[128];
    format(debug_msg, sizeof(debug_msg), "[LS80] LoadCharacters query: %s", query);
    printf(debug_msg);
    result = db_query(server_database, query);
    // Clear existing character data
    for (new i = 0; i < MAX_CHARACTERS; i++) {
        PlayerCharacters[playerid][i][cExists] = false;
        PlayerCharacters[playerid][i][cName][0] = '\0';
        PlayerCharacters[playerid][i][cRace][0] = '\0';
        PlayerCharacters[playerid][i][cHeight] = 0.0;
        PlayerCharacters[playerid][i][cGender][0] = '\0';
    }
    // Load characters into PlayerCharacters
    new count = 0;
    new rows = db_num_rows(result);
    for (new i = 0; i < rows && count < MAX_CHARACTERS; i++) {
        new slot = db_get_field_int(result, 1);
        if (slot >= 0 && slot < MAX_CHARACTERS) {
            db_get_field_assoc(result, "name", PlayerCharacters[playerid][slot][cName], MAX_CHARACTER_NAME);
            db_get_field_assoc(result, "race", PlayerCharacters[playerid][slot][cRace], 20);
            PlayerCharacters[playerid][slot][cHeight] = db_get_field_float(result, 3);
            db_get_field_assoc(result, "gender", PlayerCharacters[playerid][slot][cGender], 10);
            PlayerCharacters[playerid][slot][cExists] = true;
            format(debug_msg, sizeof(debug_msg), "[LS80] Loaded slot %d: %s (race: %s, height: %.2fm, gender: %s)",
                slot + 1, PlayerCharacters[playerid][slot][cName], PlayerCharacters[playerid][slot][cRace],
                PlayerCharacters[playerid][slot][cHeight], PlayerCharacters[playerid][slot][cGender]);
            printf(debug_msg);
            count++;
        }
        db_next_row(result);
    }
    db_free_result(result);
    format(debug_msg, sizeof(debug_msg), "[LS80] Loaded %d characters for userId %d, player %d", count, userId, playerid);
    printf(debug_msg);
}


hook OnPlayerConnect(playerid) {
    PlayerInfo[playerid][pUsername][0] = '\0';
    PlayerInfo[playerid][pUserId] = -1;
    PlayerInfo[playerid][pRegistered] = false;
    PlayerInfo[playerid][pLoggedIn] = false;
    PlayerInfo[playerid][pSelectedChar] = -1;
    PlayerInfo[playerid][pTempName][0] = '\0';

    new name[MAX_USERNAME_LENGTH];
    GetPlayerName(playerid, name, sizeof(name));
    format(PlayerInfo[playerid][pUsername], MAX_USERNAME_LENGTH, "%s", name);

    new debug_msg[128];
    format(debug_msg, sizeof(debug_msg), "[LS80] Player %d (%s) connected", playerid, name);
    printf(debug_msg);

    // Check database for username
    new userId = GetUserIdFromUsername(name);
    if (userId != -1) {
        PlayerInfo[playerid][pUserId] = userId;
        PlayerInfo[playerid][pRegistered] = true;
        new title[32], caption[64], button1[16], button2[16];
        format(title, sizeof(title), "Login");
        format(caption, sizeof(caption), "Enter your password:");
        format(button1, sizeof(button1), "Login");
        format(button2, sizeof(button2), "Quit");
        format(debug_msg, sizeof(debug_msg), "[LS80] Showing login dialog for player %d", playerid);
        printf(debug_msg);
        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, title, caption, button1, button2);
    } else {
        new title[32], caption[64], button1[16], button2[16];
        format(title, sizeof(title), "Register");
        format(caption, sizeof(caption), "Enter a password (6+ characters, cannot be blank):");
        format(button1, sizeof(button1), "Register");
        format(button2, sizeof(button2), "Quit");
        format(debug_msg, sizeof(debug_msg), "[LS80] Showing register dialog for player %d", playerid);
        printf(debug_msg);
        ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, title, caption, button1, button2);
    }
    return 1;
}

// Block spawning until registered, logged in, and character selected.
hook OnPlayerRequestSpawn(playerid) {
    if (!PlayerInfo[playerid][pRegistered] || !PlayerInfo[playerid][pLoggedIn] || PlayerInfo[playerid][pSelectedChar] == -1) {
        new error_msg[128];
        format(error_msg, sizeof(error_msg), "You must register, log in, and select a character to spawn!");
        SendClientMessage(playerid, -1, error_msg);
        return 0;
    }
    return 1;
}

// Handle dialogs
hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {
    new debug_msg[128];
    format(debug_msg, sizeof(debug_msg), "[LS80] Dialog response: player %d, dialogid %d, response %d, listitem %d", playerid, dialogid, response, listitem);
    printf(debug_msg);

    switch (dialogid) {
        case DIALOG_REGISTER: {
            if (!response) {
                Kick(playerid);
                return 1;
            }

            // Check for blank or invalid password
            if (strlen(inputtext) == 0) {
                new title[32], caption[64], button1[16], button2[16];
                format(title, sizeof(title), "Register");
                format(caption, sizeof(caption), "Password cannot be blank! Enter a password (6+ characters):");
                format(button1, sizeof(button1), "Register");
                format(button2, sizeof(button2), "Quit");
                ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, title, caption, button1, button2);
                return 1;
            }
            if (strlen(inputtext) < 6 || strlen(inputtext) >= MAX_PASSWORD_LENGTH) {
                new title[32], caption[64], button1[16], button2[16];
                format(title, sizeof(title), "Register");
                format(caption, sizeof(caption), "Password must be 6-%d characters!", MAX_PASSWORD_LENGTH);
                format(button1, sizeof(button1), "Register");
                format(button2, sizeof(button2), "Quit");
                ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_PASSWORD, title, caption, button1, button2);
                return 1;
            }

            // Hashing
            bcrypt_hash(playerid, "OnPasswordHashed", inputtext, 12);
            return 1;
        }
        case DIALOG_LOGIN: {
            if (!response) {
                Kick(playerid);
                return 1;
            }

            if (strlen(inputtext) == 0) {
                new title[32], caption[64], button1[16], button2[16];
                format(title, sizeof(title), "Login");
                format(caption, sizeof(caption), "Password cannot be blank! Enter your password:");
                format(button1, sizeof(button1), "Login");
                format(button2, sizeof(button2), "Quit");
                ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, title, caption, button1, button2);
                return 1;
            }

            new stored_hash[MAX_PASSWORD_LENGTH];
            if (GetUserPassword(PlayerInfo[playerid][pUsername], stored_hash, sizeof(stored_hash))) {
                bcrypt_verify(playerid, "OnPasswordVerified", inputtext, stored_hash);
            } else {
                new error_msg[128];
                format(error_msg, sizeof(error_msg), "Error: Could not retrieve password. Please contact an admin.");
                SendClientMessage(playerid, -1, error_msg);
                Kick(playerid);
            }
            return 1;
        }
        case DIALOG_CHAR_SELECT: {
            if (!response) {
                // Redisplay dialog if player cancels
                new title[32], caption[256], button1[16], button2[16];
                new display_names[MAX_CHARACTERS][MAX_CHARACTER_NAME];
                for (new i = 0; i < MAX_CHARACTERS; i++) {
                    if (PlayerCharacters[playerid][i][cExists]) {
                        format(display_names[i], MAX_CHARACTER_NAME, "Slot %d: %s", i + 1, PlayerCharacters[playerid][i][cName]);
                    } else {
                        format(display_names[i], MAX_CHARACTER_NAME, "Slot %d: Empty", i + 1);
                    }
                    format(debug_msg, sizeof(debug_msg), "[LS80] Dialog slot %d: %s", i + 1, display_names[i]);
                    printf(debug_msg);
                }
                format(title, sizeof(title), "Character Selection");
                format(caption, sizeof(caption), "%s\n%s\n%s\n%s",
                    display_names[0], display_names[1], display_names[2], display_names[3]);
                format(button1, sizeof(button1), "Select");
                format(button2, sizeof(button2), "Cancel");
                format(debug_msg, sizeof(debug_msg), "[LS80] Redisplaying char select dialog for player %d: %s", playerid, caption);
                printf(debug_msg);
                ShowPlayerDialog(playerid, DIALOG_CHAR_SELECT, DIALOG_STYLE_LIST, title, caption, button1, button2);
                return 1;
            }

            // slotz select
            PlayerInfo[playerid][pSelectedChar] = listitem;
            if (!PlayerCharacters[playerid][listitem][cExists]) {
                // Empty slot, show creation dialog
                new title[32], caption[64], button1[16], button2[16];
                format(title, sizeof(title), "Create Character");
                format(caption, sizeof(caption), "Enter Firstname_Lastname (e.g., John_Doe):");
                format(button1, sizeof(button1), "Create");
                format(button2, sizeof(button2), "Cancel");
                format(debug_msg, sizeof(debug_msg), "[LS80] Showing char create dialog for player %d, slot %d", playerid, listitem + 1);
                printf(debug_msg);
                ShowPlayerDialog(playerid, DIALOG_CHAR_CREATE, DIALOG_STYLE_INPUT, title, caption, button1, button2);
            } else {
                // Select existing character
                new select_msg[128];
                format(select_msg, sizeof(select_msg), "Selected character: %s (slot %d).", PlayerCharacters[playerid][listitem][cName], listitem + 1);
                SendClientMessage(playerid, -1, select_msg);
                SetPlayerName(playerid, PlayerCharacters[playerid][listitem][cName]);
            }
            return 1;
        }
        case DIALOG_CHAR_CREATE: {
            if (!response) {
                // Redisplay character selection
                new title[32], caption[256], button1[16], button2[16];
                new display_names[MAX_CHARACTERS][MAX_CHARACTER_NAME];
                for (new i = 0; i < MAX_CHARACTERS; i++) {
                    if (PlayerCharacters[playerid][i][cExists]) {
                        format(display_names[i], MAX_CHARACTER_NAME, "Slot %d: %s", i + 1, PlayerCharacters[playerid][i][cName]);
                    } else {
                        format(display_names[i], MAX_CHARACTER_NAME, "Slot %d: Empty", i + 1);
                    }
                    format(debug_msg, sizeof(debug_msg), "[LS80] Dialog slot %d: %s", i + 1, display_names[i]);
                    printf(debug_msg);
                }
                format(title, sizeof(title), "Character Selection");
                format(caption, sizeof(caption), "%s\n%s\n%s\n%s",
                    display_names[0], display_names[1], display_names[2], display_names[3]);
                format(button1, sizeof(button1), "Select");
                format(button2, sizeof(button2), "Cancel");
                format(debug_msg, sizeof(debug_msg), "[LS80] Redisplaying char select dialog after cancel for player %d: %s", playerid, caption);
                printf(debug_msg);
                ShowPlayerDialog(playerid, DIALOG_CHAR_SELECT, DIALOG_STYLE_LIST, title, caption, button1, button2);
                return 1;
            }

            // Validate fn_ln format (Firstname_Lastname)
            new underscore_pos = strfind(inputtext, "_");
            if (underscore_pos == -1 || underscore_pos == 0 || underscore_pos == strlen(inputtext) - 1 || strlen(inputtext) > MAX_CHARACTER_NAME || strlen(inputtext) < 3) {
                new title[32], caption[64], button1[16], button2[16];
                format(title, sizeof(title), "Create Character");
                format(caption, sizeof(caption), "Invalid format! Use Firstname_Lastname (e.g., John_Doe):");
                format(button1, sizeof(button1), "Create");
                format(button2, sizeof(button2), "Cancel");
                format(debug_msg, sizeof(debug_msg), "[LS80] Invalid char name '%s' for player %d", inputtext, playerid);
                printf(debug_msg);
                ShowPlayerDialog(playerid, DIALOG_CHAR_CREATE, DIALOG_STYLE_INPUT, title, caption, button1, button2);
                return 1;
            }

            // Check if character name is unique
            if (!CheckCharacterNameUnique(inputtext)) {
                new title[32], caption[64], button1[16], button2[16];
                format(title, sizeof(title), "Create Character");
                format(caption, sizeof(caption), "Name '%s' already exists! Choose a unique Firstname_Lastname:", inputtext);
                format(button1, sizeof(button1), "Create");
                format(button2, sizeof(button2), "Cancel");
                format(debug_msg, sizeof(debug_msg), "[LS80] Duplicate char name '%s' for player %d", inputtext, playerid);
                printf(debug_msg);
                ShowPlayerDialog(playerid, DIALOG_CHAR_CREATE, DIALOG_STYLE_INPUT, title, caption, button1, button2);
                return 1;
            }

            
            format(PlayerInfo[playerid][pTempName], MAX_CHARACTER_NAME, "%s", inputtext);
            new title[32], caption[64], button1[16], button2[16];
            format(title, sizeof(title), "Character Race");
            format(caption, sizeof(caption), "Enter race (Black/White/Hispanic/Asian):");
            format(button1, sizeof(button1), "Next");
            format(button2, sizeof(button2), "Cancel");
            format(debug_msg, sizeof(debug_msg), "[LS80] Showing race dialog for player %d, temp name '%s'", playerid, inputtext);
            printf(debug_msg);
            ShowPlayerDialog(playerid, DIALOG_CHAR_RACE, DIALOG_STYLE_INPUT, title, caption, button1, button2);
            return 1;
        }
        case DIALOG_CHAR_RACE: {
            if (!response) {
                // Redisplay character selection
                new title[32], caption[256], button1[16], button2[16];
                new display_names[MAX_CHARACTERS][MAX_CHARACTER_NAME];
                for (new i = 0; i < MAX_CHARACTERS; i++) {
                    if (PlayerCharacters[playerid][i][cExists]) {
                        format(display_names[i], MAX_CHARACTER_NAME, "Slot %d: %s", i + 1, PlayerCharacters[playerid][i][cName]);
                    } else {
                        format(display_names[i], MAX_CHARACTER_NAME, "Slot %d: Empty", i + 1);
                    }
                    format(debug_msg, sizeof(debug_msg), "[LS80] Dialog slot %d: %s", i + 1, display_names[i]);
                    printf(debug_msg);
                }
                format(title, sizeof(title), "Character Selection");
                format(caption, sizeof(caption), "%s\n%s\n%s\n%s",
                    display_names[0], display_names[1], display_names[2], display_names[3]);
                format(button1, sizeof(button1), "Select");
                format(button2, sizeof(button2), "Cancel");
                format(debug_msg, sizeof(debug_msg), "[LS80] Redisplaying char select dialog after race cancel for player %d", playerid);
                printf(debug_msg);
                ShowPlayerDialog(playerid, DIALOG_CHAR_SELECT, DIALOG_STYLE_LIST, title, caption, button1, button2);
                return 1;
            }

            
            new race[20];
            format(race, sizeof(race), "%s", inputtext);
            strlower(race);
            if (strcmp(race, "black") != 0 && strcmp(race, "white") != 0 && strcmp(race, "hispanic") != 0 && strcmp(race, "asian") != 0) {
                new title[32], caption[64], button1[16], button2[16];
                format(title, sizeof(title), "Character Race");
                format(caption, sizeof(caption), "Invalid race! Enter Black/White/Hispanic/Asian:");
                format(button1, sizeof(button1), "Next");
                format(button2, sizeof(button2), "Cancel");
                format(debug_msg, sizeof(debug_msg), "[LS80] Invalid race '%s' for player %d", inputtext, playerid);
                printf(debug_msg);
                ShowPlayerDialog(playerid, DIALOG_CHAR_RACE, DIALOG_STYLE_INPUT, title, caption, button1, button2);
                return 1;
            }

          
            race[0] = toupper(race[0]);
            format(PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cRace], 20, "%s", race);
            // Proceed to height
            new title[32], caption[64], button1[16], button2[16];
            format(title, sizeof(title), "Character Height");
            format(caption, sizeof(caption), "Enter height (1.50-2.00 meters):");
            format(button1, sizeof(button1), "Next");
            format(button2, sizeof(button2), "Cancel");
            format(debug_msg, sizeof(debug_msg), "[LS80] Showing height dialog for player %d, race '%s'", playerid, race);
            printf(debug_msg);
            ShowPlayerDialog(playerid, DIALOG_CHAR_HEIGHT, DIALOG_STYLE_INPUT, title, caption, button1, button2);
            return 1;
        }
        case DIALOG_CHAR_HEIGHT: {
            if (!response) {
                // Redisplay race dialog
                new title[32], caption[64], button1[16], button2[16];
                format(title, sizeof(title), "Character Race");
                format(caption, sizeof(caption), "Enter race (Black/White/Hispanic/Asian):");
                format(button1, sizeof(button1), "Next");
                format(button2, sizeof(button2), "Cancel");
                format(debug_msg, sizeof(debug_msg), "[LS80] Redisplaying race dialog after height cancel for player %d", playerid);
                printf(debug_msg);
                ShowPlayerDialog(playerid, DIALOG_CHAR_RACE, DIALOG_STYLE_INPUT, title, caption, button1, button2);
                return 1;
            }

            // Validate height
            new Float:height;
            if (sscanf(inputtext, "f", height) || height < 1.50 || height > 2.00) {
                new title[32], caption[64], button1[16], button2[16];
                format(title, sizeof(title), "Character Height");
                format(caption, sizeof(caption), "Invalid height! Enter 1.50-2.00 meters:");
                format(button1, sizeof(button1), "Next");
                format(button2, sizeof(button2), "Cancel");
                format(debug_msg, sizeof(debug_msg), "[LS80] Invalid height '%s' for player %d", inputtext, playerid);
                printf(debug_msg);
                ShowPlayerDialog(playerid, DIALOG_CHAR_HEIGHT, DIALOG_STYLE_INPUT, title, caption, button1, button2);
                return 1;
            }

            // Store height and proceed to gender
            PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cHeight] = height;
            new title[32], caption[64], button1[16], button2[16];
            format(title, sizeof(title), "Character Gender");
            format(caption, sizeof(caption), "Enter gender (Male/Female):");
            format(button1, sizeof(button1), "Create");
            format(button2, sizeof(button2), "Cancel");
            format(debug_msg, sizeof(debug_msg), "[LS80] Showing gender dialog for player %d, height %.2fm", playerid, height);
            printf(debug_msg);
            ShowPlayerDialog(playerid, DIALOG_CHAR_GENDER, DIALOG_STYLE_INPUT, title, caption, button1, button2);
            return 1;
        }
        case DIALOG_CHAR_GENDER: {
            if (!response) {
                // Redisplay height dialog
                new title[32], caption[64], button1[16], button2[16];
                format(title, sizeof(title), "Character Height");
                format(caption, sizeof(caption), "Enter height (1.50-2.00 meters):");
                format(button1, sizeof(button1), "Next");
                format(button2, sizeof(button2), "Cancel");
                format(debug_msg, sizeof(debug_msg), "[LS80] Redisplaying height dialog after gender cancel for player %d", playerid);
                printf(debug_msg);
                ShowPlayerDialog(playerid, DIALOG_CHAR_HEIGHT, DIALOG_STYLE_INPUT, title, caption, button1, button2);
                return 1;
            }

            // Validate gender
            new gender[10];
            format(gender, sizeof(gender), "%s", inputtext);
            strlower(gender);
            if (strcmp(gender, "male") != 0 && strcmp(gender, "female") != 0) {
                new title[32], caption[64], button1[16], button2[16];
                format(title, sizeof(title), "Character Gender");
                format(caption, sizeof(caption), "Invalid gender! Enter Male/Female:");
                format(button1, sizeof(button1), "Create");
                format(button2, sizeof(button2), "Cancel");
                format(debug_msg, sizeof(debug_msg), "[LS80] Invalid gender '%s' for player %d", inputtext, playerid);
                printf(debug_msg);
                ShowPlayerDialog(playerid, DIALOG_CHAR_GENDER, DIALOG_STYLE_INPUT, title, caption, button1, button2);
                return 1;
            }

            // Capitalize gender
            gender[0] = toupper(gender[0]);
            format(PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cGender], 10, "%s", gender);
            // Save character and rename player
            SaveCharacter(
                PlayerInfo[playerid][pUserId],
                PlayerInfo[playerid][pTempName],
                PlayerInfo[playerid][pSelectedChar],
                PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cRace],
                PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cHeight],
                PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cGender]
            );
            format(PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cName], MAX_CHARACTER_NAME, "%s", PlayerInfo[playerid][pTempName]);
            PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cExists] = true;
            new success_msg[128];
            format(success_msg, sizeof(success_msg), "Created character: %s in slot %d (race: %s, height: %.2fm, gender: %s).",
                PlayerInfo[playerid][pTempName], PlayerInfo[playerid][pSelectedChar] + 1,
                PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cRace],
                PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cHeight],
                PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cGender]);
            SendClientMessage(playerid, -1, success_msg);
            format(debug_msg, sizeof(debug_msg), "[LS80] Created character '%s' in slot %d for player %d (race: %s, height: %.2fm, gender: %s)",
                PlayerInfo[playerid][pTempName], PlayerInfo[playerid][pSelectedChar] + 1, playerid,
                PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cRace],
                PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cHeight],
                PlayerCharacters[playerid][PlayerInfo[playerid][pSelectedChar]][cGender]);
            printf(debug_msg);
            SetPlayerName(playerid, PlayerInfo[playerid][pTempName]);
            PlayerInfo[playerid][pTempName][0] = '\0';
            return 1;
        }
    }
    return 0;
}

forward OnPasswordHashed(playerid);
public OnPasswordHashed(playerid) {
    new hash[MAX_PASSWORD_LENGTH];
    bcrypt_get_hash(hash, sizeof(hash));
    SaveUser(PlayerInfo[playerid][pUsername], hash);
    PlayerInfo[playerid][pRegistered] = true;
    PlayerInfo[playerid][pUserId] = GetUserIdFromUsername(PlayerInfo[playerid][pUsername]);
    new success_msg[128];
    format(success_msg, sizeof(success_msg), "[LS80] Registration successful! Please log in.");
    printf(success_msg);
    SendClientMessage(playerid, -1, success_msg);
    new title[32], caption[64], button1[16], button2[16];
    format(title, sizeof(title), "Login");
    format(caption, sizeof(caption), "Enter your password:");
    format(button1, sizeof(button1), "Login");
    format(button2, sizeof(button2), "Quit");
    new debug_msg[128];
    format(debug_msg, sizeof(debug_msg), "[LS80] Showing login dialog after registration for player %d", playerid);
    printf(debug_msg);
    ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, title, caption, button1, button2);
    return 1;
}

forward OnPasswordVerified(playerid, bool:success);
public OnPasswordVerified(playerid, bool:success) {
    new debug_msg[128];
    format(debug_msg, sizeof(debug_msg), "[LS80] Password verification for player %d: %s", playerid, success ? "success" : "failed");
    printf(debug_msg);
    if (success) {
        PlayerInfo[playerid][pLoggedIn] = true;
        new success_msg[128];
        format(success_msg, sizeof(success_msg), "Login successful! Please select a character.");
        SendClientMessage(playerid, -1, success_msg);
        format(debug_msg, sizeof(debug_msg), "[LS80] Loading characters for player %d, userId %d", playerid, PlayerInfo[playerid][pUserId]);
        printf(debug_msg);
        // Load characters
        LoadCharacters(PlayerInfo[playerid][pUserId], playerid);
        // Prepare dialog
        new title[32], caption[256], button1[16], button2[16];
        new display_names[MAX_CHARACTERS][MAX_CHARACTER_NAME];
        for (new i = 0; i < MAX_CHARACTERS; i++) {
            if (PlayerCharacters[playerid][i][cExists]) {
                format(display_names[i], MAX_CHARACTER_NAME, "Slot %d: %s", i + 1, PlayerCharacters[playerid][i][cName]);
            } else {
                format(display_names[i], MAX_CHARACTER_NAME, "Slot %d: Empty", i + 1);
            }
            format(debug_msg, sizeof(debug_msg), "[LS80] Dialog slot %d: %s", i + 1, display_names[i]);
            printf(debug_msg);
        }
        format(title, sizeof(title), "Character Selection");
        format(caption, sizeof(caption), "%s\n%s\n%s\n%s",
            display_names[0], display_names[1], display_names[2], display_names[3]);
        format(button1, sizeof(button1), "Select");
        format(button2, sizeof(button2), "Cancel");
        format(debug_msg, sizeof(debug_msg), "[LS80] Showing character selection dialog for player %d: %s", playerid, caption);
        printf(debug_msg);
        ShowPlayerDialog(playerid, DIALOG_CHAR_SELECT, DIALOG_STYLE_LIST, title, caption, button1, button2);
    } else {
        new title[32], caption[64], button1[16], button2[16];
        format(title, sizeof(title), "Login");
        format(caption, sizeof(caption), "Incorrect password! Try again:");
        format(button1, sizeof(button1), "Login");
        format(button2, sizeof(button2), "Quit");
        format(debug_msg, sizeof(debug_msg), "[LS80] Showing login dialog (failed) for player %d", playerid);
        printf(debug_msg);
        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_PASSWORD, title, caption, button1, button2);
    }
    return 1;
}


stock strlower(str[]) {
    for (new i = 0; str[i] != '\0'; i++) {
        str[i] = tolower(str[i]);
    }
}