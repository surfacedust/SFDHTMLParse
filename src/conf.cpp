#include "conf.h"

int conf_init(const char *fpath, 
        const char *fname, struct collection_item **config)
{
    int ret = 0;
    struct collection_item *error_set = NULL;

    if (!fpath || !fname){
        FATAL("File path or name is NULL");
        return -1;
    }

    ret = config_for_app(fname, NULL, fpath,
                config, INI_STOP_ON_NONE, &error_set);
    if (ret){
        FATAL("read configuration failed");
        free_ini_config(*config);
        return -1;
    }

    free_ini_config_errors(error_set);
    return 0;
}

int conf_free(struct collection_item *config)
{
    if (config){
        return -1;
    }
    free_ini_config(config);
    return 0;
}

int conf_int(struct collection_item *config,
        const char* section, const char *name, 
        int def, int *value)
{
    int ret = 0;
    struct collection_item *item = NULL;

    if (!config || !section || !name){
        FATAL("either input is NULL");
        return -1;
    }

    ret = get_config_item(section, name, config, &item);
    *value = get_int_config_value(item, 1, def, &ret);
    if (ret){
        WARN("[%s].%s is set failed, using default: %u", section, name, def);
    } else {
        INFO("[%s].%s is set: %u", section, name, *value);
    }
    return 0;
}

int conf_uint(struct collection_item *config,
        const char* section, const char *name, 
        uint32_t def, uint32_t *value)
{
    int ret = 0;
    struct collection_item *item = NULL;

    if (!config || !section || !name){
        FATAL("either input is NULL");
        return -1;
    }

    ret = get_config_item(section, name, config, &item);
    if (ret || !item){
        FATAL("get section[%s] failed", section);
        return -1;
    }
    *value = get_unsigned_config_value(item, 1, def, &ret);
    if (ret){
        WARN("[%s].%s is set failed, using default: %u", section, name, def);
    } else {
        INFO("[%s].%s is set: %u", section, name, *value);
    }
    return 0;
}

int conf_double(struct collection_item *config,
        const char* section, const char *name, 
        double def, double *value)
{
    int ret = 0;
    struct collection_item *item = NULL;

    if (!config || !section || !name){
        FATAL("either input is NULL");
        return -1;
    }

    ret = get_config_item(section, name, config, &item);
    if (ret || !item){
        FATAL("get section[%s] failed", section);
        return -1;
    }
    *value = get_double_config_value(item, 1, def, &ret);
    if (ret){
        WARN("[%s].%s is set failed, using default: %lf", section, name, def);
    } else {
        INFO("[%s].%s is set: %lf", section, name, *value);
    }
    return 0;
}

int conf_string(struct collection_item *config,
        const char* section, const char *name, 
        const char *def, char *s, uint32_t len)
{
    int ret = 0;
    const char *str = NULL;
    struct collection_item *item = NULL;

    if (!config || !section || !name || !def
            || !s || !len){
        FATAL("either input is NULL");
        return -1;
    }

    ret = get_config_item(section, name, config, &item); 
    if (ret || !item){
        FATAL("get name[%s] in section[%s] failed, use default[%s]", name, section, def);
        //use default value
        strncpy(s, def, sizeof(s));
        return -1;
    }

    str = get_const_string_config_value(item, &ret);
    if (ret){
        FATAL("[%s].%s is set failed, use default[%s]", section, name, def);
        strncpy(s, def, sizeof(s));
        return -1;
    }
    if (len < strlen(str) + 1){
        FATAL("[%s].%s is out of range, use default[%s]", section, name, def);
        strncpy(s, def, sizeof(s));
        return -1;
    }
    memset(s, 0, len);
    strncpy(s, str, len);

    if (0 == strncmp(s, "", sizeof(s))) {
        FATAL("[%s].%s is set NULL, use default[%s]", section, name, def);
        strncpy(s, def, sizeof(s));
        return -1;
    }

    INFO("[%s].%s is set: %s", section, name, s);
    return 0;
}

int conf_long_array(struct collection_item *config,
        const char* section, const char *name, 
        long **value, int *size)
{
    int ret = 0;
    struct collection_item *item = NULL;

    if (!config || !section || !name || !size){
        FATAL("either input is NULL");
        return -1;
    }

    ret = get_config_item(section, name, config, &item);
    *value = get_long_config_array(item, size, &ret);
    if (ret){
        WARN("[%s].%s is set failed", section, name);
    } else {
        INFO("[%s].%s is set", section, name);
    }
    return 0;
}

int conf_string_array(struct collection_item *config,
        const char* section, const char *name, 
        char ***value, int *size)
{
    int ret = 0;
    struct collection_item *item = NULL;

    if (!config || !section || !name || !size){
        FATAL("either input is NULL");
        return -1;
    }

    ret = get_config_item(section, name, config, &item);
    *value = get_string_config_array(item, ",", size, &ret);
    if (ret){
        WARN("[%s].%s is set failed", section, name);
    } else {
        INFO("[%s].%s is set", section, name);
    }
    return 0;
}
