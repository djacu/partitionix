#include <stdio.h>
#include <zfs_prop.h>
#include <json.h>

json_object* zprop_desc_list_to_json(zprop_desc_t* props, int prop_num) {
    json_object* res = json_object_new_object();
    for (int i = 0; i < prop_num; i++) {
        json_object* jprop = json_object_new_object();
        json_object_object_add(res, props[i].pd_name, jprop);

        const char * attr;
        switch (props[i].pd_attr) {
            case PROP_DEFAULT:
                attr = "default";
                break;
            case PROP_READONLY:
                attr = "readonly";
                break;
            case PROP_INHERIT:
                attr = "inherit";
                break;
            case PROP_ONETIME:
                attr = "onetime";
                break;
            case PROP_ONETIME_DEFAULT:
                attr = "onetime_default";
                break;
        }

        json_object_object_add(
            jprop,
            "attr",
            json_object_new_string(attr)
        );

        switch (props[i].pd_proptype) {
        case PROP_TYPE_NUMBER:
            json_object_object_add(
                jprop,
                "type",
                json_object_new_string("number")
            );
            if (props[i].pd_numdefault != -1) {
                json_object_object_add(
                    jprop,
                    "default",
                    json_object_new_int(props[i].pd_numdefault)
                );
            }
            break;

        case PROP_TYPE_STRING:
            json_object_object_add(
                jprop,
                "type",
                json_object_new_string("string")
            );
            if (props[i].pd_strdefault != NULL) {
                json_object_object_add(
                    jprop,
                    "default",
                    json_object_new_string(props[i].pd_strdefault)
                );
            }
            break;

        case PROP_TYPE_INDEX:
            json_object_object_add(
                jprop,
                "type",
                json_object_new_string("index")
            );

            json_object_object_add(
                jprop,
                "default",
                json_object_new_string(props[i].pd_table[props[i].pd_numdefault].pi_name)
            );

            json_object* jtable = json_object_new_array_ext(
                props[i].pd_table_size
            );
            json_object_object_add(jprop, "table", jtable);

            for (int j = 0; j < props[i].pd_table_size; j++) {
                json_object_array_add(
                    jtable,
                    json_object_new_string(
                        props[i].pd_table[j].pi_name
                    )
                );
            }
            break;
        }
    }
    return res;
}

int main() {
    zfs_prop_init();
    zpool_prop_init();
    vdev_prop_init();

    zprop_desc_t* zfs_props = zfs_prop_get_table();

    json_object* json = zprop_desc_list_to_json(zfs_props, ZFS_NUM_PROPS);

    int res = json_object_to_fd(1, json, JSON_C_TO_STRING_PRETTY);
    if (res < 0) {
        printf("error: %s\n", json_util_get_last_err());
    }

    return 0;
}
