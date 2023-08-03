#ifndef __CMD_PARSER_H__
#define __CMD_PARSER_H__

int cmd_do_ctrl_handler(int client, const char *data);
int cmd_do_get_handler(int client, const char *path, const char *query_str);
int parse_key(const char *data, const char *key, char *value, unsigned int value_size);
int cmd_get_release(void);

#define SD_MOUNT_PATH "/mnt/extsd/"
//#define SD_MOUNT_PATH ""

#endif//__CMD_PARSER_H__

