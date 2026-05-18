/**
 ******************************************************************************
 * @file           : process_UI.c
 * @brief          :
 * @author         : Samuel Crepeault
 *
 ******************************************************************************
 * @details
 *
 *
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "data_UI.h"

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
uint8_t index_to_address_LUT[] = {
    [0] = 0,
    [10] = 1,
    [11] = 2,
    [20] = 3,
    [30] = 4,
    [40] = 5,
    [41] = 6,
    [42] = 7,
    [43] = 8,
    [44] = 9,
    [45] = 10,
    [46] = 11,
    [47] = 12,
    [48] = 13,
    [49] = 14,
    [50] = 15,
    [51] = 16,
    [60] = 17,
    [61] = 18,
    [62] = 19,
    [63] = 20,
    [64] = 21,
    [65] = 22,
    [66] = 23,
    [70] = 24,
    [71] = 25,
    [72] = 26,
    [73] = 27,
    [74] = 28,
    [75] = 29,
    [76] = 30,
    [80] = 31,
    [81] = 32,
    [82] = 33,
    [83] = 34,
};

static const PROCESS_UI_SCREEN* screen_adresses[] = {
    /* Error */
    &error_screen,

    /* Boot */
    &boot_screen,
    &tooltip_screen,

    /* Main */
    &main_screen,

    /* Menu */
    &menu_screen,

    /* Stops */
    &active_stops_screen,
    &too_many_stops_screen,
    &no_stops_screen,
    &add_stop_screen,
    &by_line_1_screen,
    &by_line_2_screen,
    &by_line_3_screen,
    &by_stop_1_screen,
    &by_stop_2_screen,
    &stop_added_screen,
    &remove_stop_screen,
    &stop_removed_screen,

    /* Display settings */
    &display_mode_screen,
    &time_activate_24_1_screen,
    &time_activate_24_2_screen,
    &time_activate_12_1_screen,
    &time_activate_12_2_screen,
    &wake_duration_screen,
    &changes_saved_screen,

    /* WiFi */
    &wifi_connection_screen,
    &wifi_manual_1_screen,
    &wifi_manual_2_screen,
    &wifi_automatic_1_screen,
    &wifi_trying_connection_screen,
    &wifi_success_screen,
    &wifi_failure_screen,

    /* Data sync / Time */
    &data_sync_1_screen,
    &data_sync_2_screen,
    &data_sync_3_screen,
    &time_format_screen
};

const uint8_t string_list_test[10][30] = {
    {"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"},
    {"BBBBBBBBBBBBBBBBBBBBBBBBBBBBBB"},
    {"CCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"},
    {"DDDDDDDDDDDDDDDDDDDDDDDDDDDDDD"},
    {"EEEEEEEEEEEEEEEEEEEEEEEEEEEEEE"},
    {"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"},
    {"GGGGGGGGGGGGGGGGGGGGGGGGGGGGGG"},
    {"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHH"},
    {"IIIIIIIIIIIIIIIIIIIIIIIIIIIIII"},
    {"JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ"}
};

PROCESS_UI_SCREEN error_screen = {
    .index = 0,
    .next_screen = {30, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_CUSTOM,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"        ERROR        "},
        {"                     "},
        {" You're not supposed "},
        {"     to be here.     "},
        {"                     "},
        {"                     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN boot_screen = {
    .index = 10,
    .next_screen = {11, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_WAIT,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"       OMNIBUS       "},
        {"                     "},
        {"   Informations de   "},
        {"       transit       "},
        {"                     "},
        {"     ver. 1.0.0      "},
        {"   Samuel Cr\xE9peault  "}}
    };
 
PROCESS_UI_SCREEN tooltip_screen = {
    .index = 11,
    .next_screen = {20, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_WAIT,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"    Naviguer dans    "},
        {"\t     OMNIBUS       "},
        {"                     "},
        {" Boutons (\xE0 l'avant) "},
        {"                     "},
        {" Cadran (c\xF4t\xE9 droit) "},
        {"pour faire d\xE9filer le"},
        {"curseur/les options. "}}
    };
 
PROCESS_UI_SCREEN main_screen = {
    .index = 20,
    .next_screen = {30, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 30,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"MAIN                 "},
        {"                     "},
        {"                     "},
        {"                     "},
        {"                     "},
        {"                     "},
        {"                     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN menu_screen = {
    .index = 30,
    .next_screen = {40, 60, 70, 80, 83},
    .next_screen_count = 5,
    .cursor_count = 5,
    .previous_screen = 20,
 
    .behavior = UI_BEHAVIOR_MENU_LIST,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"MENU                 "},
        {"  ARRETS ACTIFS      "},
        {"  MODE D'AFFICHAGE   "},
        {"  CONNEXION WIFI     "},
        {"  SYNCHRONISATION    "},
        {"  FORMAT D'HEURE     "}}
    };
 
PROCESS_UI_SCREEN active_stops_screen = {
    .index = 40,
    .next_screen = {43, 50, 42, 41, 0},
    .next_screen_count = 4,
    .cursor_count = 2,
    .previous_screen = 30,
 
    .behavior = UI_BEHAVIOR_MENU_LIST,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"ARRETS ACTIFS        "},
        {"  AJOUTER            "},
        {"  SUPPRIMER          "},
        {"                     "},
        {"                     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN too_many_stops_screen = {
    .index = 41,
    .next_screen = {40, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 30,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {" Espace d'affichage  "},
        {"\t     plein.        "},
        {"                     "},
        {" Veuillez commencer  "},
        {"par retirer un arr\xEAt."},
        {"    > Continuer <    "}}
    };
 
PROCESS_UI_SCREEN no_stops_screen = {
    .index = 42,
    .next_screen = {40, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 30,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"    Aucun arr\xEAt \xE0    "},
        {"     supprimer.      "},
        {"                     "},
        {" Veuillez commencer  "},
        {"par retirer un arr\xEAt."},
        {"    > Continuer <    "}}
    };
 
PROCESS_UI_SCREEN add_stop_screen = {
    .index = 43,
    .next_screen = {44, 47, 0, 0, 0},
    .next_screen_count = 2,
    .cursor_count = 2,
    .previous_screen = 40,
 
    .behavior = UI_BEHAVIOR_MENU_LIST,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"AJOUT D'ARRET        "},
        {"  PAR LIGNE          "},
        {"  PAR NUM. D'ARRET   "},
        {"                     "},
        {"                     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN by_line_1_screen = {
    .index = 44,
    .next_screen = {45, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 43,
 
    .behavior = UI_BEHAVIOR_NUMERIC_ENTRY,
    .select_button_action = 0,
    .max_select_actions = 4,
    .num_digits = 3,
    .menu_button_actions = 4,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Entrez un num\xE9ro de  "},
        {"ligne.               "},
        {"  \xA9  \xA9  \xA9            "},
        {" [#][#][#]  Valider  "},
        {"  \xAE  \xAE  \xAE            "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN by_line_2_screen = {
    .index = 45,
    .next_screen = {46, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 2,
    .previous_screen = 44,
 
    .behavior = UI_BEHAVIOR_MENU_LIST,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Ligne ###, vers :    "},
        {"1                    "},
        {"2                    "},
        {"                     "},
        {"                     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN by_line_3_screen = {
    .index = 46,
    .next_screen = {49, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 45,
 
    .behavior = UI_BEHAVIOR_SCROLL,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Arr\xEAts, ligne ### :  "},
        {"          \xA9          "},
        {"#### - ***********   "},
        {"#### - *********** <<"},
        {"#### - ***********   "},
        {"          \xAE          "}}
    };
 
PROCESS_UI_SCREEN by_stop_1_screen = {
    .index = 47,
    .next_screen = {48, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 43,
 
    .behavior = UI_BEHAVIOR_NUMERIC_ENTRY,
    .select_button_action = 0,
    .max_select_actions = 5,
    .num_digits = 4,
    .menu_button_actions = 5,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Entrez un num\xE9ro     "},
        {"d'arr\xEAt.             "},
        {"  \xA9  \xA9  \xA9  \xA9         "},
        {" [#][#][#][#] Valider"},
        {"  \xAE  \xAE  \xAE  \xAE         "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN by_stop_2_screen = {
    .index = 48,
    .next_screen = {49, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 47,
 
    .behavior = UI_BEHAVIOR_SCROLL,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Lignes, arr\xEAt 1102 : "},
        {"          \xA9          "},
        {"         001         "},
        {"         011       <<"},
        {"         283         "},
        {"          \xAE          "}}
    };
 
PROCESS_UI_SCREEN stop_added_screen = {
    .index = 49,
    .next_screen = {40, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 40,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"  Arr\xEAt ajout\xE9 avec  "},
        {"       succ\xE8s.       "},
        {"                     "},
        {"                     "},
        {"     > Valider <     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN remove_stop_screen = {
    .index = 50,
    .next_screen = {51, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 4,
    .previous_screen = 40,
 
    .behavior = UI_BEHAVIOR_MENU_LIST,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Retirez un arr\xEAt:    "},
        {"  ####             <<"},
        {"  ####               "},
        {"  ####               "},
        {"  ####               "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN stop_removed_screen = {
    .index = 51,
    .next_screen = {40, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 40,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"  Arr\xEAt retir\xE9 avec  "},
        {"       succ\xE8s.       "},
        {"                     "},
        {"                     "},
        {"     > Valider <     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN display_mode_screen = {
    .index = 60,
    .next_screen = {60, 61, 65, 0, 0},
    .next_screen_count = 3,
    .cursor_count = 3,
    .previous_screen = 30,
 
    .behavior = UI_BEHAVIOR_MENU_LIST,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"MODE D'AFFICHAGE     "},
        {"[X]  TOUJOURS ON   <<"},
        {"[ ]  HEURES PROG.    "},
        {"[ ]  REVEIL BOUTON   "},
        {"                     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN time_activate_24_1_screen = {
    .index = 61,
    .next_screen = {62, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 60,
 
    .behavior = UI_BEHAVIOR_NUMERIC_ENTRY,
    .select_button_action = 0,
    .max_select_actions = 3,
    .num_digits = 2,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Heure d'activation : "},
        {"                     "},
        {" \xA9    \xA9              "},
        {"[##]:[##]     Valider"},
        {" \xAE    \xAE              "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN time_activate_24_2_screen = {
    .index = 62,
    .next_screen = {66, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 61,
 
    .behavior = UI_BEHAVIOR_NUMERIC_ENTRY,
    .select_button_action = 0,
    .max_select_actions = 3,
    .num_digits = 2,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Heure de             "},
        {"d\xE9sactivation :      "},
        {" \xA9    \xA9              "},
        {"[##]:[##]     Valider"},
        {" \xAE    \xAE              "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN time_activate_12_1_screen = {
    .index = 63,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_NUMERIC_ENTRY,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 2,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Heure d'activation : "},
        {"                     "},
        {" \xA9    \xA9   \xA9          "},
        {"[##]:[##][AM] Valider"},
        {" \xAE    \xAE   \xAE          "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN time_activate_12_2_screen = {
    .index = 64,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_NUMERIC_ENTRY,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 2,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Heure de             "},
        {"d\xE9sactivation :      "},
        {" \xA9    \xA9              "},
        {"[##]:[##][AM] Valider"},
        {" \xAE    \xAE              "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN wake_duration_screen = {
    .index = 65,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_NUMERIC_ENTRY,
    .select_button_action = 0,
    .max_select_actions = 2,
    .num_digits = 1,
    .menu_button_actions = 2,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Dur\xE9""e de r\xE9veil apr\xE8s"},
        {"appui du bouton :    "},
        {" \xA9                   "},
        {"[##] min.     Valider"},
        {" \xAE                   "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN changes_saved_screen = {
    .index = 66,
    .next_screen = {60, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 60,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"    Modifications    "},
        {"    enregistr\xE9""es     "},
        {"    avec succ\xE8s.     "},
        {"                     "},
        {"     > Valider <     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN wifi_connection_screen = {
    .index = 70,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 2,
    .cursor_count = 2,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_MENU_LIST,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"CONNEXION WIFI       "},
        {"  MANUELLE         <<"},
        {"  AUTOMATIQUE        "},
        {"                     "},
        {"                     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN wifi_manual_1_screen = {
    .index = 71,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_SCROLL,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"R\xE9seaux disponibles: "},
        {"         ^           "},
        {"  ***************    "},
        {"  ***************  <<"},
        {"  ***************    "},
        {"         v           "}}
    };
 
PROCESS_UI_SCREEN wifi_manual_2_screen = {
    .index = 72,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_TEXT_ENTRY,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Mot de passe :       "},
        {"                     "},
        {"  _________________  "},
        {"                     "},
        {"         v           "},
        {"< OK aAbBcCdDeEfFgG >"}}
    };
 
PROCESS_UI_SCREEN wifi_automatic_1_screen = {
    .index = 73,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Sur votre routeur,   "},
        {"appuyez sur le bouton"},
        {"WPS. Vous avez deux  "},
        {"minutes pour ensuite "},
        {"appuyer sur SELECT   "},
        {"sur votre Omnibus.   "}}
    };
 
PROCESS_UI_SCREEN wifi_trying_connection_screen = {
    .index = 74,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_WAIT,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"                     "},
        {" Connexion en cours. "},
        {"                     "},
        {" Veuillez patienter. "},
        {"                     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN wifi_success_screen = {
    .index = 75,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Wi-Fi :              "},
        {"Connexion r\xE9ussie au "},
        {"r\xE9seau *************."},
        {"                     "},
        {"                     "},
        {"    > Continuer <    "}}
    };
 
PROCESS_UI_SCREEN wifi_failure_screen = {
    .index = 76,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Wi-Fi :              "},
        {"Connexion \xE9""chou\xE9""e.   "},
        {"                     "},
        {"V\xE9rifiez le MdP du   "},
        {"r\xE9seau.              "},
        {"    > Continuer <    "}}
    };
 
PROCESS_UI_SCREEN data_sync_1_screen = {
    .index = 80,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 2,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_MENU_LIST,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Voulez-vous lancer   "},
        {"une r\xE9""cup\xE9ration des "},
        {"donn\xE9""es?             "},
        {"                     "},
        {"  OUI              <<"},
        {"  NON                "}}
    };
 
PROCESS_UI_SCREEN data_sync_2_screen = {
    .index = 81,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_WAIT,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Veuillez patienter.  "},
        {"                     "},
        {"Cette op\xE9ration peut "},
        {"prendre jusqu'\xE0 deux "},
        {"minutes.             "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN data_sync_3_screen = {
    .index = 82,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"   Synchronisation   "},
        {"      termin\xE9""e.      "},
        {"                     "},
        {"                     "},
        {"                     "},
        {"    > Continuer <    "}}
    };
 
PROCESS_UI_SCREEN time_format_screen = {
    .index = 83,
    .next_screen = {1, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 2,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_TOGGLE,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"FORMAT DE L'HEURE    "},
        {"[ ]  12H           <<"},
        {"[X]  24H             "},
        {"                     "},
        {"                     "},
        {"                     "}}
    };


/* Functions -----------------------------------------------------------------*/

//==============================================================================
void data_UI_init(void)
//
//==============================================================================
{
}

//==============================================================================
PROCESS_UI_SCREEN* data_UI_get_screen_data(uint8_t screen_index)
//
//==============================================================================
{
    return screen_adresses[index_to_address_LUT[screen_index]];
}