#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "themeSwitcher.h"
#include "logger.h"
#include "cJSON.h"

#define CONFIG_FILE "/home/Michael/.theme_daemon_config.json"
#define XRESOURCES_FILE "/home/Michael/.Xresources"
#define NUM_COLORS 16
#define THEME_COUNT 3
#define MODE_COUNT 4

typedef struct {
  char* name;
  char* barColorActive;
  char* barColorInactive;
  char* colors[NUM_COLORS];
} Theme;


Theme themes[THEME_COUNT] = {
  {
    .name = "pink",
    .barColorActive = "#eb348c",
    .barColorInactive = "#9c0868",
    .colors = {
      "#33001A", // Dark cherry
      "#BF40BF", // Bright fuchsia
      "#FF004D", // Vivid pink-red
      "#A80073", // Deep magenta
      "#FF3399", // Hot magenta-pink
      "#8000FF", // Electric purple
      "#FF66FF", // Pastel pink-purple
      "#FFD6FF", // Light lilac
      "#660033", // Deep wine
      "#FF0066", // Neon rose
      "#BF0080", // Dark magenta
      "#FF99CC", // Soft pink
      "#9933CC", // Rich violet
      "#FF33CC", // Bright magenta-pink
      "#CC00FF", // Fluorescent purple
      "#FFCCE5"  // Blush pink
    }
  },
  {
    .name = "blue",
    .barColorActive = "#348ceb", // Bright sky blue
    .barColorInactive = "#08689c", // Deep sea blue
    .colors = {
      "#00A3FF", // Fluorescent cyan-blue
      "#0033FF", // Vivid royal blue
      "#001933", // Midnight navy
      "#004DA8", // Deep azure
      "#3399FF", // Bright sky blue
      "#0055FF", // Electric blue
      "#4070BF", // Bold cobalt
      "#66CCFF", // Light aqua blue
      "#D6EEFF", // Pale ice blue
      "#003366", // Navy blue
      "#0066FF", // Neon blue
      "#0073BF", // Ocean blue
      "#99CCFF", // Soft pastel blue
      "#3366CC", // Rich sapphire
      "#3399CC", // Sky teal
      "#CCEFFF"  // Very light cyan
    }
  },
  {
    .name = "green",
    .barColorActive = "#34eb46", // Bright lime green
    .barColorInactive = "#086c1a", // Deep forest green
    .colors = {
      "#00CC00", // Electric green
      "#336633", // Rich jade green
      "#001900", // Deep emerald green
      "#00FF00", // Vivid green
      "#00A800", // Bright grass green
      "#33FF33", // Light neon green
      "#40BF40", // Soft moss green
      "#66FF66", // Pastel mint green
      "#D6FFD6", // Pale spring green
      "#003300", // Dark evergreen
      "#00FF33", // Bright spring green
      "#00BF00", // Fresh leaf green
      "#99FF99", // Soft light green
      "#339933", // Bright olive green
      "#00FF66", // Fluorescent lime-green
      "#CCFFCC"  // Very pale mint
    }
  },
};

void generate_xresources(const Theme *theme) {
  FILE *file = fopen(XRESOURCES_FILE, "w");
  if (!file) {
    Logger_error("Failed to open .Xresources file");
    exit(EXIT_FAILURE);
  }

  fprintf(file, "*barColorActive: %s\n\n", theme->barColorActive);
  fprintf(file, "*barColorInactive: %s\n\n", theme->barColorInactive);

  for (int i = 0; i < NUM_COLORS; i++) {
    fprintf(file, "*color%d: %s\n", i, theme->colors[i]);
  }

  fclose(file);
  printf(".Xresources file generated at %s\n", XRESOURCES_FILE);
}

char* hex_to_cli( char* hex ) {
  hex++;
  return  hex;
}

int update_razer_cli(int mode) {
  if( mode > MODE_COUNT - 1 ) {
    printf("Invalid Mode Provided");
    return -1;
  }

  ThemeJSON current_theme;

  if( load_current_theme( &current_theme ) != 0 ) {
    Logger_log("Failed to load theme");
  }

  Theme theme = themes[current_theme.themeIndex];

  char effect[128];
  bool found = false;

  if( mode == 0 ) {
    found = true;
    char* color = hex_to_cli(theme.colors[0]);
    sprintf(effect, "ripple -c %s", color);
  } else if( mode == 1 ) {
    found = true;
    char* color1 = hex_to_cli(theme.colors[0]);
    char* color2 = hex_to_cli(theme.colors[1]);

    sprintf(effect, "starlight_dual,3 -c %s %s", color1, color2);
  } else if( mode == 2 ) {
    found = true;
    char* color = hex_to_cli(theme.colors[0]);
    sprintf(effect, "reactive -c %s", color);
  } else if( mode == 3 ) {
    found = true;
    sprintf(effect, "multicolor,x");
  }
  
  if( !found ) {
    return -1;
  }

  char command[256];
  sprintf(command, "razer-cli -e %s", effect);

  if( system(command) != 0 ) {
    printf("Failed to change theme on Razer-CLI");
  }

  return 0;
}

void apply_xresources() {
  char command[64];
  sprintf(command, "xrdb %s\n", XRESOURCES_FILE);
  if( system(command) != 0 ) {
    printf("Failed to apply Xresources!");
  } else {
    printf(".Xresources applied with xrdb\n");
  }
}

// Load the current theme from the config file
int load_current_theme(ThemeJSON* buffer) {
  FILE *file = fopen(CONFIG_FILE, "r");

  if (!file) {
    perror("Failed to open config file for reading");
    return 1;
  }

  // Find length of file
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);

  // Reset file location
  fseek(file, 0, SEEK_SET);

  // Read file contents to json_str and add terminator
  char *json_str = malloc(file_size + 1);
  fread(json_str, 1, file_size, file);
  fclose(file);
  json_str[file_size] = '\0';

  // Parse JSON
  cJSON *config = cJSON_Parse(json_str);
  free(json_str);

  if (!config) {
    fprintf(stderr, "Failed to parse config file\n");
    return 1;
  }

  // Get the current theme
  cJSON *theme = cJSON_GetObjectItemCaseSensitive(config, "current_theme");
  cJSON *mode = cJSON_GetObjectItemCaseSensitive(config, "current_mode");
  int theme_index = -1;
  int mode_index = -1;
  if (cJSON_IsNumber(theme) && cJSON_IsNumber(mode)) {
    theme_index = theme->valueint;
    mode_index = mode->valueint;
  }
  cJSON_Delete(config);

  if( theme_index == -1 || mode_index == -1 ) {
    printf("Theme data in invalid format\n");
    return 1;
  }

  buffer->themeIndex = theme_index;
  buffer->modeIndex = mode_index;

  return 0;
}


int changeTheme(char* name) {
  Theme selected;
  int found = 0;

  ThemeJSON theme;
  load_current_theme(&theme);

  for(int i = 0; i < THEME_COUNT; i++) {
    Theme current = themes[i];
    if( strcmp(current.name, name) == 0 ) {
      selected = current;
      found = 1;
      break;
    }
  }

  if( !found ) {
    return 1;
  }

  // Generate the .Xresources file
  generate_xresources(&selected);
  apply_xresources();

  if( update_razer_cli(theme.modeIndex) != 0 ) {
    Logger_error("Failed to update Razer CLI");
    return 1;
  }

  return 0;
}

// Save the current theme to the config file
void save_current_theme(const ThemeJSON info) {
  // Create JSON object
  cJSON *config = cJSON_CreateObject();
  cJSON_AddNumberToObject(config, "current_theme", info.themeIndex);
  cJSON_AddNumberToObject(config, "current_mode", info.modeIndex);

  // Write JSON to file
  FILE *file = fopen(CONFIG_FILE, "w");
  if (!file) {
    perror("Failed to open config file for writing");
    cJSON_Delete(config);
    return;
  }

  char *json_str = cJSON_Print(config);
  fprintf(file, "%s\n", json_str);
  fclose(file);

  // Clean up
  cJSON_free(json_str);
  cJSON_Delete(config);

  char buff[64];
  sprintf(buff, "Current theme saved as:\nTheme = %d\nMode = %d\n", info.themeIndex, info.modeIndex);
  Logger_log(buff);
}


int set_theme_by_index(int index) {
  Theme theme = themes[index];
  ThemeJSON theme_info;

  if( load_current_theme( &theme_info ) != 0 ) {
    return 1;
  }

  theme_info.themeIndex = index;

  printf("Switching to theme: %s\n", theme.name);
  save_current_theme(theme_info);
  changeTheme(theme.name);

  return 0;
}

void next_theme() {
  ThemeJSON theme_info;
  if( load_current_theme(&theme_info ) != 0 ) {
    return;
  }

  theme_info.themeIndex = (theme_info.themeIndex + 1) % THEME_COUNT;
  set_theme_by_index(theme_info.themeIndex);
}

void previous_theme() {
  ThemeJSON theme_info;

  if( load_current_theme( &theme_info ) != 0 ) {
    return;
  }

  // Change to the next theme
  int prev_index = (theme_info.themeIndex - 1) % THEME_COUNT;
  if( prev_index < 0 ) {
    prev_index = THEME_COUNT - 1;
  }

  set_theme_by_index(prev_index);
}

void set_mode_by_index(int index) {
  ThemeJSON theme_info;
  if( load_current_theme( &theme_info ) != 0 ) {
    return;
  }
  Theme mode = themes[theme_info.themeIndex];

  theme_info.modeIndex = index;

  printf("Switching to theme: %s\n", mode.name);
  save_current_theme(theme_info);
  changeTheme(mode.name);
}

void next_mode() {
  ThemeJSON theme_info; 

  if( load_current_theme( &theme_info ) != 0 ) {
    return;
  }

  // Change to the next theme
  theme_info.modeIndex = (theme_info.modeIndex + 1) % MODE_COUNT;
  set_mode_by_index(theme_info.modeIndex);
}

void previous_mode() {
  ThemeJSON theme_info;

  if( load_current_theme( &theme_info ) ) {
    return;
  }

  // Change to the next theme
  int prev_index = (theme_info.modeIndex - 1) % MODE_COUNT;
  if( prev_index < 0 ) {
    prev_index = MODE_COUNT - 1;
  }

  set_mode_by_index(prev_index);
}
