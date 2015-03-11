
#include <jansson.h>

/**
    Creates a JSONRPC 2.0 notification embedding given parameters.
    REMARK: steals reference to parameters json object (if any)
 */
json_t* jsonrpc_notification(const char *method, json_t *params)
{
	json_t* notification;
	json_t* value = NULL;

	if (!method) {
		json_decref(params);
		return NULL;
	}

    if (!params) {
        params = json_array();
    }

    if (!json_is_array(params) && !json_is_object(params)) {
		value = params;
        params = json_array();
        json_array_append_new(params, value);
    }

	notification = json_pack("{s:s,s:s,s:o}", "jsonrpc", "2.0", "method", method, "params", params);
	if (!notification) {
		json_decref(value);
        json_decref(params);
	}
	return notification;
}

