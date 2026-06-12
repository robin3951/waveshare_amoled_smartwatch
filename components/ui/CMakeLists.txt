idf_component_register(
    SRCS
        "ui.c"
        "screens.c"
        "actions.c"
        "vars.c"
        "images.c"
        "styles.c"
        "fonts/fonts.c"
    INCLUDE_DIRS "." "fonts"
    REQUIRES lvgl
)
