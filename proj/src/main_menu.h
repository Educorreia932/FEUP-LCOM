#pragma once

#include <lcom/lcf.h>

/** @file */

/** @defgroup ui UI */
/** @addtogroup ui
  *
  * @brief	All the (Graphical) User Interfaces developed
  *
  * @{
  */

/**
 * @brief The complete Main Menu object, handles all the transitions between gamemodes alongside the GameManager
 * 
 */
typedef struct MainMenu MainMenu_t;

/**
 * @brief Creates a new Main Menu object
 * 
 * @return MainMenu_t* Pointer to the fully built Main Menu on success\n
 * NULL otherwise
 */
MainMenu_t* new_main_menu();

/**
 * @brief Frees a created Main Menu
 * 
 * @param menu Pointer to the Main Menu to be freed
 */
void free_main_menu(MainMenu_t* menu);

/**
 * @brief Updates the needed elements of the main menu
 * @description Must be called every frame
 * 
 * @param menu Pointer to the Main Menu to be updated
 */
void update_main_menu(MainMenu_t *menu);

/**
 * @brief Renders the needed elements of the main menu
 * @description Must be called every frame
 * 
 * @param menu Pointer to the Main Menu to be rendered
 */
void render_main_menu(MainMenu_t *menu);

/** @{ */
