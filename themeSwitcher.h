typedef struct {
  int themeIndex;
  int modeIndex;
} ThemeJSON;

int changeTheme(char* name);
void next_theme();
void previous_theme();
void next_mode();
void previous_mode();
int load_current_theme(ThemeJSON* buffer);
