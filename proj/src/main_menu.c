#include "main_menu.h"
#include "sprite.h"
#include "ui_elements.h"
#include "game_manager.h"

struct MainMenu {
    Sprite_t *background;
    Button_t *camp_single, *camp_coop, *arcade_single, *arcade_versus;
    // Button_t *camp_single_h, *camp_coop_h, *arcade_single_h, *arcade_coop_h;
};

void do_nothing() {

}

MainMenu_t* new_main_menu() {
    MainMenu_t* menu = (MainMenu_t*) malloc(sizeof(MainMenu_t));
    if (menu == NULL) {
        printf("new_main_menu: Failed to allocate memory for the main menu\n");
        return NULL;
    }

    menu->background = new_sprite(0, 0, 1, "");
    if (menu->background == NULL) {
        printf("new_main_menu: Failed to create the background Sprite object\n");
        free(menu);
        return NULL;
    }

    // menu->camp_single = new_button("", func, rect(200, 200, 200, 200));
    // menu->camp_coop = new_button("", func, rect());
    // menu->arcade_single = new_button("", func, rect());
    // menu->arcade_versus;

    return menu;

}

void free_main_menu(MainMenu_t *menu) {

    if (menu == NULL) {
        printf("free_main_menu: Cannot free a NULL ptr\n");
        return;
    }

    free_sprite(menu->background);

    free_button(menu->camp_single);
    free_button(menu->camp_coop);
    free_button(menu->arcade_single);
    free_button(menu->arcade_versus);

}

void render_main_menu(MainMenu_t *menu) {
    
    draw_sprite_floats(menu->background, 0, 0, COLOR_NO_MULTIPLY, SPRITE_NORMAL);

    render_button(menu->camp_single);
    render_button(menu->camp_coop);
    render_button(menu->arcade_single);
    render_button(menu->arcade_versus);

}
