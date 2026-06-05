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
#include "data_UI_refactored.h"
#include "process_UI_refactored.h"
#include "data_config.h"
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

uint8_t string_list_test[10][30] = {
    {"0000 - A           AAAAAAAAAAA"},
    {"0001 -  B          BBBBBBBBBBB"},
    {"0002 -   C         CCCCCCCCCCC"},
    {"0003 -    D        DDDDDDDDDDD"},
    {"0004 -     E       EEEEEEEEEEE"},
    {"0005 -      F      FFFFFFFFFFF"},
    {"0006 -       G     GGGGGGGGGGG"},
    {"0007 -        H    HHHHHHHHHHH"},
    {"0008 -         I   IIIIIIIIIII"},
    {"0009 -          J  JJJJJJJJJJJ"}
};

PROCESS_UI_SCREEN error_screen = {
    .index = 0,
    .next_screen = {30, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 30,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"        ERROR        "},
        {"                     "},
        {" You're not supposed "},
        {"     to be here.     "},
        {"                     "},
        {"                     "},
        {"                     "}},
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
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"       OMNIBUS       "},
        {"                     "},
        {"   Informations de   "},
        {"       transit       "},
        {"                     "},
        {"     ver. 1.0.0      "},
        {"   Samuel Cr\xE9peault  "}},
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
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
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

PROCESS_UI_SCREEN wait_screen = {
    .index = 12,
    .next_screen = {0, 0, 0, 0, 0},
    .next_screen_count = 0,
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
        {"    Validation...    "},
        {"                     "},
        {" Veuillez patienter. "},
        {"                     "},
        {"                     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN main_screen = {
    .index = 20,
    .next_screen = {30, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 30,
 
    .behavior = UI_BEHAVIOR_CUSTOM,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    .screen_on_arrival = arrival_20,
    .screen_on_exit = NULL,
    .screen_on_tick = tick_20,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"                     "},
        {"                     "},
        {"                     "},
        {"                     "},
        {"                     "},
        {"  Ajoutez un arr\xEAt!  "}},
      //{"012345678901234567890"}
    .accent_regions = {
        {{1,5}, {3,5}},
        {{4,5}, {6,5}},
        {{7,5}, {9,5}},
        {{12,5}, {19,5}},
        {{0,0}, {0,0}},
    }    
    };
 
PROCESS_UI_SCREEN menu_screen = {
    .index = 30,
    .next_screen = {40, 60, 83, 70, 80},
    .next_screen_count = 3,
    .cursor_count = 3,
    .previous_screen = 20,
 
    .behavior = UI_BEHAVIOR_MENU_LIST,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"MENU                 "},
        {"  ARRETS ACTIFS      "},
        {"  MODE D'AFFICHAGE   "},
        {"  FORMAT D'HEURE     "},
        {"                     "},
        {"                     "}}
    };
 
PROCESS_UI_SCREEN active_stops_screen = {
    .index = 40,
    .next_screen = {43, 51, 0, 0, 0},
    .next_screen_count = 4,
    .cursor_count = 2,
    .previous_screen = 30,
 
    .behavior = UI_BEHAVIOR_MENU_LIST,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    .screen_on_arrival = arrival_40,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"ARRETS ACTIFS        "},
        {"  AJOUTER UN ARRET   "},
        {"  RETIRER ARRETS     "},
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
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {" Espace d'affichage  "},
        {"\t     plein.        "},
        {"                     "},
        {" Veuillez commencer  "},
        {"par retirer un arr\xEAt."},
        {"    > Continuer <    "}},
    .accent_regions = {
        {{5,7}, {15,7}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}},
    }    
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
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"    Aucun arr\xEAt \xE0    "},
        {"     supprimer.      "},
        {"                     "},
        {" Veuillez commencer  "},
        {"par ajouter un arr\xEAt."},
        {"    > Continuer <    "}},
    .accent_regions = {
        {{5,7}, {15,7}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}},
    }    
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
    .screen_on_arrival = NULL,
    .screen_on_exit = exit_43,
    
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
    .screen_on_arrival = NULL,
    .screen_on_exit = exit_44,

    .screen_text = {
        {"                     "},
        {"                     "},
        {"Entrez un num\xE9ro de  "},
        {"ligne.               "},
        {"  \xA9  \xA9  \xA9            "},
        {" [0][0][0]  Valider  "},
        {"  \xAE  \xAE  \xAE            "},
        {"                     "}},

    .accent_regions = {
        {{1,5}, {3,5}},
        {{4,5}, {6,5}},
        {{7,5}, {9,5}},
        {{12,5}, {19,5}},
        {{0,0}, {0,0}},
    }    
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
    .screen_on_arrival = arrival_45,
    .screen_on_exit = exit_45,
    .screen_on_tick = arrival_45,
    
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Ligne ###, vers :    "},
        {"                     "},
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
    .scroll_amount = 0,
    .screen_on_arrival = arrival_46,
    .screen_on_exit = exit_46,
    .screen_on_tick = tick_46,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Arr\xEAts, ligne ### :  "},
        {"          \xA9          "},
        {"#### - ***********   "},
        {"#### - *********** <<"},
        {"#### - ***********   "},
        {"          \xAE          "}},

    .accent_regions = {
        {{9,3}, {11,3}},
        {{9,7}, {11,7}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}}
    }    
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
    .screen_on_arrival = NULL,
    .screen_on_exit = exit_47,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Entrez un num\xE9ro     "},
        {"d'arr\xEAt.             "},
        {"  \xA9  \xA9  \xA9  \xA9         "},
        {" [0][0][0][0] Valider"},
        {"  \xAE  \xAE  \xAE  \xAE         "},
        {"                     "}},

    .accent_regions = {
        {{1,5}, {3,5}},
        {{4,5}, {6,5}},
        {{7,5}, {9,5}},
        {{10,5}, {12,5}},
        {{14,5}, {21,5}},
    }    
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
    .scroll_amount = 0,
    .screen_on_arrival = arrival_48,
    .screen_on_exit = exit_48,

    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Lignes, arr\xEAt #### : "},
        {"          \xA9          "},
        {"         001         "},
        {"         011       <<"},
        {"         283         "},
        {"          \xAE          "}},
    
    .accent_regions = {
        {{9,3}, {11,3}},
        {{9,7}, {11,7}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}}
    }    
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
    .screen_on_arrival = arrival_49,
    .screen_on_exit = NULL,
    
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
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Retirez un arr\xEAt:    "},
        {"  ####               "},
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
    .screen_on_arrival = NULL,
    .screen_on_exit = exit_51,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"   Arr\xEAts retir\xE9s    "},
        {"     avec succ\xE8s.    "},
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
 
    .behavior = UI_BEHAVIOR_TOGGLE,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    .screen_on_arrival = arrival_60,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"MODE D'AFFICHAGE     "},
        {"  TOUJOURS ON        "},
        {"  HEURES PROG.       "},
        {"  REVEIL BOUTON      "},
        {"                     "},
        {"                     "}},
    .accent_regions = {
        {{1,3}, {14,3}},
        {{1,4}, {14,4}},
        {{1,5}, {14,5}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}},
    } 
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
    .num_digits = 4,
    .digit_max  = {3, 10, 6, 10},
    .menu_button_actions = 1,
    .screen_on_arrival = NULL,
    .screen_on_exit = exit_61,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Heure d'activation : "},
        {"(format 24h)         "},
        {" \xA9    \xA9              "},
        {"[##]:[##]     Valider"},
        {" \xAE    \xAE              "},
        {"                     "}},

    .accent_regions = {
        {{1,5}, {1,5}},
        {{2,5}, {2,5}},
        {{6,5}, {6,5}},
        {{7,5}, {7,5}},
        {{14,5}, {21,5}}
    }    
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
    .num_digits = 4,
    .digit_max  = {3, 10, 6, 10},
    .menu_button_actions = 1,
    .screen_on_arrival = NULL,
    .screen_on_exit = exit_62,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Heure de             "},
        {"d\xE9sactivation :      "},
        {" \xA9    \xA9              "},
        {"[##]:[##]     Valider"},
        {" \xAE    \xAE              "},
        {"                     "}},

    .accent_regions = {
        {{1,5}, {1,5}},
        {{2,5}, {2,5}},
        {{6,5}, {6,5}},
        {{7,5}, {7,5}},
        {{14,5}, {21,5}}
    }    
    };
 
PROCESS_UI_SCREEN time_activate_12_1_screen = {
    .index = 63,
    .next_screen = {64, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 60,
 
    .behavior = UI_BEHAVIOR_NUMERIC_ENTRY,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 3,
    .menu_button_actions = 0,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Heure d'activation : "},
        {"                     "},
        {" \xA9    \xA9   \xA9          "},
        {"[##]:[##][AM] Valider"},
        {" \xAE    \xAE   \xAE          "},
        {"                     "}},

    .accent_regions = {
        {{1,5}, {2,5}},
        {{6,5}, {7,5}},
        {{9,5}, {10,5}},
        {{14,5}, {21,5}},
        {{0,0}, {0,0}},
    }    
    };
 
PROCESS_UI_SCREEN time_activate_12_2_screen = {
    .index = 64,
    .next_screen = {66, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 63,
 
    .behavior = UI_BEHAVIOR_NUMERIC_ENTRY,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 2,
    .menu_button_actions = 0,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Heure de             "},
        {"d\xE9sactivation :      "},
        {" \xA9    \xA9              "},
        {"[##]:[##][AM] Valider"},
        {" \xAE    \xAE              "},
        {"                     "}},

    .accent_regions = {
        {{1,5}, {2,5}},
        {{6,5}, {7,5}},
        {{9,5}, {10,5}},
        {{14,5}, {21,5}},
        {{0,0}, {0,0}},
    }    
    };
 
PROCESS_UI_SCREEN wake_duration_screen = {
    .index = 65,
    .next_screen = {66, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 60,
 
    .behavior = UI_BEHAVIOR_NUMERIC_ENTRY,
    .select_button_action = 0,
    .max_select_actions = 3,
    .num_digits = 2,
    .menu_button_actions = 3,
    .screen_on_arrival = NULL,
    .screen_on_exit = exit_65,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Dur\xE9""e de r\xE9veil apr\xE8s"},
        {"appui du bouton :    "},
        {" \xA9                   "},
        {"[00] min.     Valider"},
        {" \xAE                   "},
        {"                     "}},

    .accent_regions = {
        {{1,5}, {1,5}},
        {{2,5}, {2,5}},
        {{14,5}, {21,5}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}},
    }    
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
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
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
    .next_screen = {71, 73, 0, 0, 0},
    .next_screen_count = 2,
    .cursor_count = 2,
    .previous_screen = 30,
 
    .behavior = UI_BEHAVIOR_MENU_LIST,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
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
    .next_screen = {72, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 70,
 
    .behavior = UI_BEHAVIOR_SCROLL,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"R\xE9seaux disponibles: "},
        {"         \xA9           "},
        {"  ***************    "},
        {"  ***************  <<"},
        {"  ***************    "},
        {"         \xAE           "}}
    };
 
PROCESS_UI_SCREEN wifi_manual_2_screen = {
    .index = 72,
    .next_screen = {74, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 71,
 
    .behavior = UI_BEHAVIOR_TEXT_ENTRY,
    .select_button_action = 0,
    .max_select_actions = 30,
    .num_digits = 0,
    .menu_button_actions = 30,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"Mot de passe :       "},
        {"                     "},
        {"  _________________  "},
        {"                     "},
        {"         \xAE           "},
        {"< OK aAbBcCdDeEfFgG >"}}
    };
 
PROCESS_UI_SCREEN wifi_automatic_1_screen = {
    .index = 73,
    .next_screen = {74, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 70,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
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
    .next_screen = {75, 76, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_WAIT,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
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
    .next_screen = {70, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 70,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
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
    .next_screen = {70, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 70,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
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
    .next_screen = {81, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 2,
    .previous_screen = 30,
 
    .behavior = UI_BEHAVIOR_MENU_LIST,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
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
    .next_screen = {82, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 0,
 
    .behavior = UI_BEHAVIOR_WAIT,
    .select_button_action = 0,
    .max_select_actions = 0,
    .num_digits = 0,
    .menu_button_actions = 0,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
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
    .next_screen = {80, 0, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 0,
    .previous_screen = 80,
 
    .behavior = UI_BEHAVIOR_NAVIGATE,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    .screen_on_arrival = NULL,
    .screen_on_exit = NULL,
    
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
    .next_screen = {83, 83, 0, 0, 0},
    .next_screen_count = 1,
    .cursor_count = 2,
    .previous_screen = 30,
 
    .behavior = UI_BEHAVIOR_TOGGLE,
    .select_button_action = 0,
    .max_select_actions = 1,
    .num_digits = 0,
    .menu_button_actions = 1,
    .screen_on_arrival = arrival_83,
    .screen_on_exit = NULL,
    
    .screen_text = {
        {"                     "},
        {"                     "},
        {"FORMAT DE L'HEURE    "},
        {"  12H                "},
        {"  24H                "},
        {"                     "},
        {"                     "},
        {"                     "}},
    .accent_regions = {
        {{1,3}, {4,3}},
        {{1,4}, {4,4}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}},
        {{0,0}, {0,0}},
    }    
    };


/* Functions -----------------------------------------------------------------*/

//==============================================================================
void data_UI_init(void)
//
//==============================================================================
{
    by_line_3_screen.scroll_amount = data_config_bus_data_dummy.number_of_stops;
    by_stop_2_screen.scroll_amount = data_config_bus_data_dummy.number_of_lines;
}

//==============================================================================
PROCESS_UI_SCREEN* data_UI_get_screen_data(uint8_t screen_index)
//
//==============================================================================
{
    return screen_adresses[index_to_address_LUT[screen_index]];
}