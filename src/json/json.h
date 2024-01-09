#ifndef JSON_H
#define JSON_H

typedef void *json_ptr;
typedef const json_ptr const_json_ptr;
typedef json_ptr *json_ptr2;
typedef json_ptr **json_ptr3;

/**
 * @{ \name Json api
 */

/**
 * \brief Frees json object
 *
 * Frees json object. By default you should free only root json object.
 */
typedef void json_free(json_ptr json);
typedef int json_parse_file(json_ptr2 json, const char *filepath);

typedef int json_get_double(const_json_ptr json, const char *item_name,
                            double *val);
typedef int json_get_int(const_json_ptr json, const char *item_name, int *val);
/**
 * \brief Gets string by name
 *
 * Gets string item by name from json object.
 * \param json Pointer to json object where to search for item.
 * \param item_name Json item name.
 * \param str Result string. It should be freed manually if this function
 * returned 0. \return 0 on success, otherwise non zero value returned.
 */
typedef int json_get_str(const_json_ptr json, const char *item_name,
                         char **str);
typedef int json_get_obj(const_json_ptr json, const char *item_name,
                         json_ptr2 json_item);
typedef int json_get_arr_obj(const_json_ptr json, const char *item_name,
                             json_ptr2 json_arr_obj);
typedef int json_get_arr_item(const_json_ptr json, int index,
                              json_ptr2 json_arr_item);
typedef int json_get_arr_items_pointers(const_json_ptr json,
                                        const char *arr_item_name,
                                        json_ptr3 json_arr_pointers,
                                        int *arr_length);
/**
 * @}
 */

typedef struct {
    json_free *free;
    json_parse_file *parse_file;

    json_get_double *get_double;
    json_get_int *get_int;
    json_get_str *get_str;
    json_get_obj *get_obj;
    json_get_arr_obj *get_arr_obj;
    json_get_arr_item *get_arr_item;
    json_get_arr_items_pointers *get_arr_items_pointers;
} json_interface;

#endif // JSON_H