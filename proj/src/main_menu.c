#include "main_menu.h"
#include "sprite.h"
#include "ui_elements.h"
#include "game_manager.h"

struct MainMenu {
    Sprite_t *background;
    Button_t *camp_single, *camp_coop_p2, *camp_coop_p1, *arcade_single, *arcade_versus;

    Sprite_t *tutorial_screen;
    bool sub_menu;
    Button_t *controls_h, *camp_h, *arcade_single_h, *arcade_versus_h;

    bool is_over;
};

static void menu_show_buttons(MainMenu_t *main_menu) {
    button_show(main_menu->camp_single);
    button_show(main_menu->camp_coop_p2);
    button_show(main_menu->camp_coop_p1);
    button_show(main_menu->arcade_single);
    button_show(main_menu->arcade_versus);

    button_show(main_menu->controls_h);
    button_show(main_menu->camp_h);
    button_show(main_menu->arcade_single_h);
    button_show(main_menu->arcade_versus_h);
}

static void menu_hide_buttons (MainMenu_t *main_menu) {
    button_hide(main_menu->camp_single);
    button_hide(main_menu->camp_coop_p2);
    button_hide(main_menu->camp_coop_p1);
    button_hide(main_menu->arcade_single);
    button_hide(main_menu->arcade_versus);
    
    button_hide(main_menu->controls_h);
    button_hide(main_menu->camp_h);
    button_hide(main_menu->arcade_single_h);
    button_hide(main_menu->arcade_versus_h);
}

static void open_tutorial_controls() {
    MainMenu_t* menu = get_game_manager()->main_menu;
    menu->sub_menu = true;

    menu_hide_buttons(menu);

    if (menu->tutorial_screen != NULL)
        free_sprite(menu->tutorial_screen);
    
    menu->tutorial_screen = new_sprite(0, 0, 1, "main_menu/tutorial_1.bmp");
}

static void open_tutorial_campaign() {
    MainMenu_t* menu = get_game_manager()->main_menu;
    menu->sub_menu = true;

    menu_hide_buttons(menu);

    if (menu->tutorial_screen != NULL)
        free_sprite(menu->tutorial_screen);
    
    menu->tutorial_screen = new_sprite(0, 0, 1, "main_menu/tutorial_2.bmp");
}

static void open_tutorial_arcade_single() {
    MainMenu_t* menu = get_game_manager()->main_menu;
    menu->sub_menu = true;

    menu_hide_buttons(menu);

    if (menu->tutorial_screen != NULL)
        free_sprite(menu->tutorial_screen);
    
    menu->tutorial_screen = new_sprite(0, 0, 1, "main_menu/tutorial_3.bmp");
}

static void open_tutorial_arcade_versus() {
    MainMenu_t* menu = get_game_manager()->main_menu;
    menu->sub_menu = true;

    menu_hide_buttons(menu);

    if (menu->tutorial_screen != NULL)
        free_sprite(menu->tutorial_screen);
    
    menu->tutorial_screen = new_sprite(0, 0, 1, "main_menu/tutorial_4.bmp");
}

static void menu_start_campaign_single() {
    GameManager_t* gm = get_game_manager();
    gm->gamemode = GM_LEVEL;
    gm_start_level();
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
    MainMenu_t* menu = (MainMenu_t*) calloc(1, sizeof(MainMenu_t));
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

    menu->sub_menu = false;

    menu->controls_h = new_button_auto_size("main_menu/controls_button.bmp", open_tutorial_controls, vec2d(420, 680));
    menu->camp_h = new_button_auto_size("main_menu/help_button.bmp", open_tutorial_campaign, vec2d(254, 562));
    menu->arcade_single_h = new_button_auto_size("main_menu/help_button.bmp", open_tutorial_arcade_single, vec2d(724, 560));
    menu->arcade_versus_h = new_button_auto_size("main_menu/help_button.bmp", open_tutorial_arcade_versus, vec2d(936, 560));

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

    free_button(menu->controls_h);
    free_button(menu->camp_h);
    free_button(menu->arcade_single_h);
    free_button(menu->arcade_versus_h);

    if (menu->tutorial_screen != NULL) {
        free_sprite(menu->tutorial_screen);
        menu->tutorial_screen = NULL;
    }

    free(menu);
}

void update_main_menu(MainMenu_t **menu) {

    if (*menu == NULL)
        return;
    update_button((*menu)->camp_single);
    if (*menu == NULL)
        return;
    update_button((*menu)->camp_coop_p1);
    if (*menu == NULL)
        return;
    update_button((*menu)->camp_coop_p2);
    if (*menu == NULL)
        return;
    update_button((*menu)->arcade_single);
    if (*menu == NULL)
        return;
    update_button((*menu)->arcade_versus);

    if (*menu == NULL)
        return;
    update_button((*menu)->controls_h);
    if (*menu == NULL)
        return;
    update_button((*menu)->camp_h);
    if (*menu == NULL)
        return;
    update_button((*menu)->arcade_single_h);
    if (*menu == NULL)
        return;
    update_button((*menu)->arcade_versus_h);

}

void render_main_menu(MainMenu_t *menu) {
    
    if (menu == NULL)
        return;

    if (!menu->sub_menu) {
        draw_sprite_floats(menu->background, 0, 0, COLOR_NO_MULTIPLY, SPRITE_NORMAL);

        render_button(menu->camp_single);
        render_button(menu->camp_coop_p1);
        render_button(menu->camp_coop_p2);
        render_button(menu->arcade_single);
        render_button(menu->arcade_versus);

        render_button(menu->controls_h);
        render_button(menu->camp_h);
        render_button(menu->arcade_single_h);
        render_button(menu->arcade_versus_h);
    }
    else {
        if (menu->tutorial_screen != NULL)
            draw_sprite_floats(menu->tutorial_screen, 0, 0, COLOR_NO_MULTIPLY, SPRITE_NORMAL);
    }
}

inline bool main_menu_in_submenu(MainMenu_t *menu) {
    return menu->sub_menu;
}

void main_menu_exit_submenu(MainMenu_t *menu) {
    menu->sub_menu = false;

    menu_show_buttons(menu);

    if (menu->tutorial_screen != NULL) {
        free_sprite(menu->tutorial_screen);
        menu->tutorial_screen = NULL;
    }
    
}
