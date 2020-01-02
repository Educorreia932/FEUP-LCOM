#include "main_menu.h"
#include "sprite.h"
#include "ui_elements.h"
#include "game_manager.h"

struct MainMenu {
    Sprite_t *background;
    Button_t *camp_single, *camp_coop_p2, *camp_coop_p1, *arcade_single, *arcade_versus;
    // Button_t *camp_single_h, *camp_coop_h, *arcade_single_h, *arcade_coop_h;
};

static void menu_start_campaign_single() {
    GameManager_t* gm = get_game_manager();
    gm->gamemode = GM_LEVEL;
    printf("To start level\n");
    gm_start_level();
    printf("Started level\n");
}

static void menu_start_campaign_coop_p1() {
    GameManager_t* gm = get_game_manager();
    gm->gamemode = GM_LEVEL_UART;
    gm_start_level();
}

static void menu_start_campaign_coop_p2() {
    GameManager_t* gm = get_game_manager();
    gm->gamemode = GM_SWITCHBOARD_UART;
    gm_start_switchboard();
}

static void menu_start_arcade_single() {
    GameManager_t* gm = get_game_manager();
    gm->gamemode = GM_ARCADE;
    gm_start_arcade();
}

static void menu_start_arcade_versus() {
    GameManager_t* gm = get_game_manager();
    gm->gamemode = GM_ARCADE_UART;
    gm_start_arcade();
}

MainMenu_t* new_main_menu() {
    MainMenu_t* menu = (MainMenu_t*) malloc(sizeof(MainMenu_t));
    if (menu == NULL) {
        printf("new_main_menu: Failed to allocate memory for the main menu\n");
        return NULL;
    }

    menu->background = new_sprite(0, 0, 1, "main_menu/title_screen.bmp");
    if (menu->background == NULL) {
        printf("new_main_menu: Failed to create the background Sprite object\n");
        free(menu);
        return NULL;
    }

    menu->camp_single = new_button_auto_size("main_menu/single_button.bmp", menu_start_campaign_single, vec2d(75, 490));
    menu->camp_coop_p1 = new_button_auto_size("main_menu/coop_p1_button.bmp", menu_start_campaign_coop_p1, vec2d(281, 490));
    menu->camp_coop_p2 = new_button_auto_size("main_menu/coop_p2_button.bmp", menu_start_campaign_coop_p2, vec2d(279, 563));
    menu->arcade_single = new_button_auto_size("main_menu/single_button.bmp", menu_start_arcade_single, vec2d(545, 490));
    menu->arcade_versus = new_button_auto_size("main_menu/versus_button.bmp", menu_start_arcade_versus, vec2d(751, 490)); 

    return menu;

}

void free_main_menu(MainMenu_t *menu) {
    if (menu == NULL) {
        printf("free_main_menu: Cannot free a NULL ptr\n");
        return;
    }

    free_sprite(menu->background);

    free_button(menu->camp_single);
    free_button(menu->camp_coop_p1);
    free_button(menu->camp_coop_p2);
    free_button(menu->arcade_single);
    free_button(menu->arcade_versus);
}

void update_main_menu(MainMenu_t *menu) {

    update_button(menu->camp_single);
    update_button(menu->camp_coop_p1);
    update_button(menu->camp_coop_p2);
    update_button(menu->arcade_single);
    update_button(menu->arcade_versus);

}

void render_main_menu(MainMenu_t *menu) {
    
    draw_sprite_floats(menu->background, 0, 0, COLOR_NO_MULTIPLY, SPRITE_NORMAL);

    render_button(menu->camp_single);
    render_button(menu->camp_coop_p1);
    render_button(menu->camp_coop_p2);
    render_button(menu->arcade_single);
    render_button(menu->arcade_versus);

}
