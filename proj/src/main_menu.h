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
 * @note That pointer to pointer is here for a reason:\n
 * As these buttons delete this Menu's instance, we need to know if it has been deleted, hence the pointer to pointer
 * @param menu Pointer to pointer to the Main Menu to be updated
 */
void update_main_menu(MainMenu_t **menu);

/**
 * @brief Renders the needed elements of the main menu
 * @description Must be called every frame
 * 
 * @param menu Pointer to the Main Menu to be rendered
 */
void render_main_menu(MainMenu_t *menu);

/**
 * @brief Returns wether the menu is in a submenu (showing a tutorial)
 * 
 * @param menu Pointer to the menu to check
 * @return True when the Menu is in a submenu (showing a tutorial)\n
 * False otherwise
 */
bool main_menu_in_submenu(MainMenu_t *menu);

/**
 * @brief Exits a menu's submenu (the tutorials)
 * 
 * @param menu Pointer to the Menu
 */
void main_menu_exit_submenu(MainMenu_t *menu);

/** @{ */
