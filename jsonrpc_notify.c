#include <jsonrpc.h>

/**
    Creates a JSONRPC 2.0 notification embedding given parameters.
    REMARK: steals reference to parameters json object (if any)
 */
json_t* jsonrpc_notification(const char *method, json_t *params) {
	json_t *object = json_object();
	
	if ( json_object_set_new(object, "jsonrpc", json_string("2.0")) ) {
		goto jsonrpc_notification_failed;
	}
	
	if ( json_object_set_new(object, "method", json_string(method)) ) {
		goto jsonrpc_notification_failed;
	}
	
	if ( json_object_set_new(object, "params", params) ) {
		goto jsonrpc_notification_failed;
	}
	
	return object;

jsonrpc_notification_failed:
	json_object_clear(params);
	json_decref(params);
	json_object_clear(object);
	json_decref(object);
	return NULL;
}

/**
    Creates a JSONRPC 2.0 notification embedding given parameters.
    REMARK: steals reference to parameters json object (if any)
 */
json_t* jsonrpc_request(const char *method, json_t *params, uint32_t id) {
	json_t *object = json_object();

	if ( json_object_set_new(object, "jsonrpc", json_string("2.0")) ) {
		goto jsonrpc_notification_failed;
	}

	if ( json_object_set_new(object, "method", json_string(method)) ) {
		goto jsonrpc_notification_failed;
	}

	if ( json_object_set_new(object, "params", params) ) {
		goto jsonrpc_notification_failed;
	}

	if ( json_object_set_new(object, "id", json_integer(id)) ) {
		goto jsonrpc_notification_failed;
	}

	return object;

jsonrpc_notification_failed:
	json_object_clear(params);
	json_decref(params);
	json_object_clear(object);
	json_decref(object);
	return NULL;
}
