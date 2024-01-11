#ifndef CJSON_H
#define CJSON_H

#include "json.h"

/**
 * \brief Returns cJSON implementation of json interface
 *
 * \return reference to statically allocated json_interface. Do not free it.
 */
json_interface *cJSON_json_impl();

#endif // CJSON_H