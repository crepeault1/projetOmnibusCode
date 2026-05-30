/**
  ******************************************************************************
  * @file           : data_config.c
  * @brief          : Holds data for program operation
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
#include "data_config.h"

/* Defines -------------------------------------------------------------------*/

/* Variables -----------------------------------------------------------------*/
DATA_CONFIG_USER_SETUP data_config_user_setup = {


};

DATA_CONFIG_BUS_DATA data_config_bus_data_dummy = {
  .number_of_lines = 10,
  .number_of_stops = 10,
  .timetable_stop_list = {
    8116,
    1560,
    2664,
    1191,
    8002,
    1195,
    1125,
    8201,
    6644,
    1204
  },
  .timetable_stop_names = {
    {"Am\xE9r.-Fran\xE7" "a"},
    {"C. Parlement"},
    {"D'Youville  "},
    {"Ste-Genev./1"},
    {"Univ. du Qu\xE9"},
    {"Place-J.-Car"},
    {"Pr.\xC9" "douard/1"},
    {"des Embarcat"},
    {"Pte-Li\xE8vres/"},
    {"4e Rue      "},
  },
  .line_list = {
  },

  .directions = {
    {"Colline Parlement."},
    {"Terminus Les Saul."}
  },

  .minutes_until_passage = {
    {12, 34, 51},
    {22, 1245, 52},
    {12, 34, 2030},
    {845, 1045, 53},
    {13, 35, 57},
    {14, 36, 58},
  }
};
/* Functions -----------------------------------------------------------------*/
void data_config_init(void)
{
}