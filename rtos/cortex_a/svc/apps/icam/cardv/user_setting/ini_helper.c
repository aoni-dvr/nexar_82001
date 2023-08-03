#include "ini_helper.h"

static int ini_readline(void *pFile, char buf[], int buf_size);
static int ini_section_item_parse(const char *section_name, const char *key, const char *value);

static ini_item_callback_func ini_item_cb = NULL;
static int section_num = 0;
static ini_section_s *section_setting = NULL;
int ini_register_setting(int num, ini_section_s *section)
{
    section_num = num;
    section_setting = section;

    return 0;
}

int ini_register_callback(ini_item_callback_func func)
{
    ini_item_cb = func;
    return 0;
}

int ini_load(const char *path)
{
    char line[256] = {0};
    void *pFile = NULL;
    char section_name[32] = {0};
    char key[32] = {0};
    char value[256] = {0};
    unsigned int i = 0;

    pFile = fopen(path, "r");
    if (pFile == NULL) {
        debug_line("%s load [%s] failed", __func__, path);
        return -1;
    }
    while (1) {
        memset(line, 0, sizeof(line));
        if (ini_readline(pFile, line, sizeof(line)) < 0) {
            break;
        }
        if (strlen(line) <= 0) {
            continue;
        }
        //debug_line("%s", line);
        if (line[0] == '[' && line[strlen(line) - 1] == ']' && strlen(line) > 2) {
            memset(section_name, 0, sizeof(section_name));
            memcpy(section_name, line + 1, strlen(line) - 2);
            continue;
        }
        // parse key value
        memset(key, 0, sizeof(key));
        memset(value, 0, sizeof(value));
        for (i = 0; i < strlen(line); i++) {
            if (line[i] == '=') {
                break;
            }
            key[i] = line[i];
        }
        if (i >= strlen(line)) {
            continue;
        }
        if ((strlen(line) - (i + 1)) > 0) {
            memcpy(value, line + i + 1, strlen(line) - (i + 1));
        }
        //debug_line("section_name: %s, key=%s, value=%s\n", section_name, key, value);
        ini_section_item_parse(section_name, key, value);
    }
    fclose(pFile);

    return 0;
}

static int ini_section_item_parse(const char *section_name, const char *key, const char *value)
{
    int i = 0;
    ini_section_s *section = NULL;
    char new_value[256] = {0};

    memset(new_value, 0, sizeof(new_value));
    for (i = 0; i < section_num; i++) {
        section = &section_setting[i];
        if (strcmp(section_name, section->name) == 0) {
            ini_section_item_s *section_items = section->items;
            int j = 0;
            for (j = 0; j < section->items_num; j++) {
                if (strcmp(key, section_items[j].key) == 0) {                    
                    int found = 0;
                    //convert str to actual value
                    if (section_items[j].use_value_str == 0) {
                        int k = 0;
                        for (k = 0; k < (int)(sizeof(section_items[j].pairs) / sizeof(value_pair_s)); k++) {
                            if (strcmp(value, section_items[j].pairs[k].value_str) == 0) {
                                snprintf(new_value, sizeof(new_value) - 1, "%d", section_items[j].pairs[k].value_int);
                                found = 1;
                                break;
                            }
                        }
                    } else {
                        snprintf(new_value, sizeof(new_value) - 1, "%s", value);
                        found = 1;
                    }
                    if (found) {
                        if (ini_item_cb) {
                            (*ini_item_cb)(i, j, new_value);
                        }
                    }
                    break;
                }
            }
            break;
        }
    }

    if (i >= section_num) {
        return -1;
    }

    return 0;
}

static int ini_readline(void *pFile, char buf[], int buf_size)
{
    unsigned char ch;
    int index = 0;

    // Normal state
    do {
        if (fread(&ch, 1, 1, pFile) == 0) { // read 1 byte
            return -1;
        }
        if ((ch == '\n') || (ch == '\r') ) {
            break;  // go to end-of-line status
        }
        if (index >= (buf_size - 1)) {
            return -1;
        }
        *buf = ch;
        buf++;
        index += 1;
    } while (1);
    // End of line state
    do {
        if (fread(&ch, 1, 1, pFile) == 0) { // read 1 byte
            break;
        }
        if ( (ch == '\n') || (ch == '\r') ) {
            /* do nothing */
        } else {
            // Reach to next line, roll back 1 byte
            fseek(pFile, -1, SEEK_CUR);
            break;
        }
    } while (1);
    *buf = '\0';

    return 0;
}

int ini_add_line(void *pFile, const char *line)
{
    if (pFile == NULL) {
        debug_line("%s invalid param", __func__);
        return -1;
    }
    if (line != NULL && line[0] != '\0') {
        fwrite(line, strlen(line), 1, pFile);
    }
    fwrite("\n", 1, 1, pFile);
    return 0;
}

int ini_save_section_name(void *pFile, int section_index)
{
    char line[256] = {0};
    ini_section_s *section = NULL;

    if (pFile == NULL || section_index < 0 || section_index >= section_num) {        
        debug_line("%s invalid param", __func__);
        return -1;
    }
    section = &section_setting[section_index];
    memset(line, 0, sizeof(line));
    snprintf(line, sizeof(line) - 1, "[%s]\n", section->name);
    //debug_line("%s", line);
    fwrite(line, strlen(line), 1, pFile);

    return 0;
}

int ini_save_section_item_int(void *pFile, int section_index, int item_index, int value)
{
    char line[256] = {0};
    char value_str[32] = {0};
    ini_section_s *section = NULL;

    if (pFile == NULL || section_index < 0 || section_index >= section_num || item_index < 0) {
        return -1;
    }
    section = &section_setting[section_index];
    memset(value_str, 0, sizeof(value_str));
    if (section->items[item_index].use_value_str) {
        snprintf(value_str, sizeof(value_str) - 1, "%d", value);
    } else {
        int i = 0;
        int pairs_num = sizeof(section->items[item_index].pairs) / sizeof(value_pair_s);
        for (i = 0; i < pairs_num; i++) {
            if (value == section->items[item_index].pairs[i].value_int) {                
                snprintf(value_str, sizeof(value_str) - 1, "%s", section->items[item_index].pairs[i].value_str);
                break;
            }
        }
    }
    memset(line, 0, sizeof(line));
    snprintf(line, sizeof(line) - 1, "%s=%s\n", section->items[item_index].key, value_str);
    //debug_line("%s", line);
    fwrite(line, strlen(line), 1, pFile);

    return 0;
}

int ini_save_section_item_str(void *pFile, int section_index, int item_index, const char *value)
{
    char line[256] = {0};
    ini_section_s *section = NULL;

    if (pFile == NULL || section_index < 0 || section_index >= section_num || item_index < 0 || value == NULL) {
        return -1;
    }
    section = &section_setting[section_index];
    memset(line, 0, sizeof(line));
    snprintf(line, sizeof(line) - 1, "%s=%s\n", section->items[item_index].key, value);
    //debug_line("%s", line);
    fwrite(line, strlen(line), 1, pFile);

    return 0;
}

int ini_save_section_item_double(void *pFile, int section_index, int item_index, double value)
{
    char line[256] = {0};
    ini_section_s *section = NULL;

    if (pFile == NULL || section_index < 0 || section_index >= section_num || item_index < 0) {
        return -1;
    }
    section = &section_setting[section_index];
    memset(line, 0, sizeof(line));
    snprintf(line, sizeof(line) - 1, "%s=%.1f\n", section->items[item_index].key, value);
    //debug_line("%s", line);
    fwrite(line, strlen(line), 1, pFile);

    return 0;
}

