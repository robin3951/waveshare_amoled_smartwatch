idf_component_register(
    SRCS
        "ui.c"
        "screens.c"
        "actions.c"
        "vars.c"
        "images.c"
        "styles.c"
        "fonts/fonts.c"
        "fonts/lv_font_montserrat_14_ext.c"
        "fonts/lv_font_montserrat_16_ext.c"
    INCLUDE_DIRS "." "fonts"
    REQUIRES lvgl
)
