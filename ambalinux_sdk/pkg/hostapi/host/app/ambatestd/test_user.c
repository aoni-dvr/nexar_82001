#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "ambatestd.h"

#define LOCK_DATABASE   pthread_mutex_lock(&db->lock)
#define UNLOCK_DATABASE pthread_mutex_unlock(&db->lock)

typedef struct {
        pthread_mutex_t lock;
        int             num_users;
        atu_stat_t      stat[0];
} user_db;

static user_db *db;

int user_init(int max_num_users)
{
        int size = sizeof(user_db) + max_num_users * sizeof(atu_stat_t);
        db = calloc(1, size);
        pthread_mutex_init(&db->lock, NULL);
        return AT_ERR_OK;
}

int user_shutdown(void)
{
        free(db);
        return 0;
}

int user_get_id(char *username)
{
        int i;

        LOCK_DATABASE;
        for (i = 0; i < db->num_users; i++) {
                if (!strcmp(db->stat[i].username, username))
                        break;
        }
        
        if (i == db->num_users) {
                strcpy(db->stat[i].username, username);
                db->num_users++;
        }
        UNLOCK_DATABASE;

        return i;
}

int user_add_client(int uid)
{
        db->stat[uid].num_clients++;
        return AT_ERR_OK;
}

int user_add_jobtime(int uid, uint32_t ms)
{
        db->stat[uid].num_jobs++;
        db->stat[uid].cputime += ms;
        return AT_ERR_OK;
}

int user_fill_stat(atu_stat_t *stat, int max_num)
{
        if (max_num > db->num_users) max_num = db->num_users;
        memcpy(stat, db->stat, max_num * sizeof(atu_stat_t));
        return max_num;
}

