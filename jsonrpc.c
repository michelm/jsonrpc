#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include <jansson.h>
#include "jsonrpc.h"

json_t *jsonrpc_error_object(int code, const char *message, json_t *data) {
	/* reference to data is stolen */

	json_t *json;

	if (!message) {
		message = "";
	}

	json = json_pack("{s:i,s:s}", "code", code, "message", message);
	if (data) {
		json_object_set_new(json, "data", data);
	}
	return json;
}

json_t *jsonrpc_error_object_predefined(int code, json_t *data) {
	/* reference to data is stolen */

	const char *message = "";

	assert(-32768 <= code && code <= -32000);	// reserved for pre-defined errors

	switch (code) {
		case JSONRPC_PARSE_ERROR:
			message = "Parse Error";
			break;
		case JSONRPC_INVALID_REQUEST:
			message = "Invalid Request";
			break;
		case JSONRPC_METHOD_NOT_FOUND:
			message = "Method not found";
			break;
		case JSONRPC_INVALID_PARAMS:
			message = "Invalid params";
			break;
		case JSONRPC_INTERNAL_ERROR:
			message = "Internal error";
			break;
	}

	return jsonrpc_error_object(code, message, data);
}

json_t *jsonrpc_error_response(json_t *id, json_t *error) {
	/* error reference is stolen */

	if (id) {
		json_incref(id);
	}
	else {
		id = json_null();
	}

	error = error ? error : json_null();

	return json_pack("{s:s,s:o,s:o}", "jsonrpc", "2.0", "id", id, "error", error);
}

json_t *jsonrpc_result_response(json_t *id, json_t *result) {
	/*  result reference is stolen */

	if (id) {
		json_incref(id);
	}
	else {
		id = json_null();
	}

	result = result ? result : json_null();

	return json_pack("{s:s,s:o,s:o}", "jsonrpc", "2.0", "id", id, "result", result);
}

int jsonrpc_validate_response(json_t *request, json_t **id, json_t **result) {
	int rc;
	size_t flags = 0;
	const char *version = NULL;

	rc = json_unpack_ex(request, NULL, flags, "{s:s,s:o,s:o}", "jsonrpc", &version, "result", result, "id", id);
	if (rc == -1) {
		return -1; // not a valid response
	}

	if ( strcmp(version, "2.0") ) {
		return 0; // is a response, but unsupported version
	}

	if ( !json_is_integer(*id) ) {
		return 0; // is a response, but identifier must be number
	}

	return 1; // valid response
}

json_t *jsonrpc_validate_request(json_t *request, const char **method, json_t **params, json_t **id) {
	size_t flags = 0;
	json_error_t error;
	const char *version = NULL;
	int rc;
	json_t *data = NULL;
	int valid_id = 0;

	*method = NULL;
	*params = NULL;
	*id = NULL;

	rc = json_unpack_ex(request, &error, flags, "{s:s,s:s,s?o,s?o}", "jsonrpc", &version, "method", method, "params", params, "id", id);
	if (rc==-1) {
		data = json_string(error.text);
		goto invalid;
	}

	if ( strcmp(version, "2.0") ) {
		data = json_string("\"jsonrpc\" MUST be exactly \"2.0\"");
		goto invalid;
	}

	if (*id) {
		if ( !json_is_string(*id) && !json_is_number(*id) && !json_is_null(*id) ) {
			data = json_string("\"id\" MUST contain a String, Number, or NULL value if included");
			goto invalid;
		}
	}

	/*  Note that we only return json_id in the error response after we have established that it is jsonrpc/2.0 compliant */
	/*  otherwise we would be returning a non-compliant response ourselves! */
	valid_id = 1;

	if (*params) {
		if ( !json_is_array(*params) && !json_is_object(*params) ) {
			data = json_string("\"params\" MUST be Array or Object if included");
			goto invalid;
		}
	}

	return NULL;

invalid:
	if (!valid_id) {
		*id = NULL;
	}
	return jsonrpc_error_response(*id, jsonrpc_error_object_predefined(JSONRPC_INVALID_REQUEST, data));
}

json_t *jsonrpc_validate_params(json_t *params, const char *spec) {
	json_t *data = NULL;

	if ( !strlen(spec) ) {	/* empty string means no arguments */
		if (!params) {
			/*  no params field: OK */
		}
		else if ( json_is_array(params) && json_array_size(params)==0) {
			/*  an empty Array: OK */
		}
		else {
			data = json_string("method takes no arguments");
		}
	}
	else if (!params) {	/* non-empty string but no params field */
		data = json_string("method takes arguments but params field missing");
	}
	else { /* non-empty string and have params field */
		size_t flags = JSON_VALIDATE_ONLY;
		json_error_t error;
		int rc = json_unpack_ex(params, &error, flags, spec);
		if (rc==-1) {
			data = json_string(error.text);
		}
	}

	return data ? jsonrpc_error_object_predefined(JSONRPC_INVALID_PARAMS, data) : NULL;
}

json_t *jsonrpc_handle_request_single(json_t *request, jsonrpc_method_t methods[], void *userdata, jsonrpc_response_f response_handler) {
	int rc;
	json_t *response;
	const char *name;
	json_t *params, *id;
	json_t *result;
	bool is_notification;
	jsonrpc_method_t *method;

	rc = jsonrpc_validate_response(request, &id, &result);
	if (rc >= 0) {
		if (rc == 1 && result && id) {
			response_handler(result, json_integer_value(id), userdata);
		}
		return NULL;
	}

	response = jsonrpc_validate_request(request, &name, &params, &id);
	if (response) {
		return response;
	}

	is_notification = (NULL==id ? true : false);

	for (method=methods; method->name!=NULL; method++) {
		if ( 0==strcmp(method->name, name) ) {
			break;
		}
	}

	if (method->name==NULL) {
		response = jsonrpc_error_response(id, jsonrpc_error_object_predefined(JSONRPC_METHOD_NOT_FOUND, NULL));
		goto done;
	}

	if (method->params_spec) {
		json_t *error = jsonrpc_validate_params(params, method->params_spec);
		if (error) {
			response = jsonrpc_error_response(id, error);
			goto done;
		}
	}

	response = NULL;
	result = NULL;
	rc = method->func(params, &result, userdata);

	if (is_notification) {
		json_decref(result);
		result = NULL;
	}
	else {
		if (rc==0) {
			response = jsonrpc_result_response(id, result);
		}
		else {
			if (!result) {
				/* method did not set a jsonrpc_error_object, create a generic error */
				result = jsonrpc_error_object_predefined(JSONRPC_INTERNAL_ERROR, NULL);
			}
			response = jsonrpc_error_response(id, result);
		}
	}

done:
	if (is_notification && response) {
		json_decref(response);
		response = NULL;
	}
	return response;
}

char *jsonrpc_handler(const char *msg, const size_t len, jsonrpc_method_t methods[], size_t flags, void *userdata, jsonrpc_response_f response_handler) {
	size_t k, size;
	json_t *request=NULL, *response=NULL, *req, *rep;
	json_error_t error;
	char *output = NULL;

	request = json_loadb(msg, len, 0, &error);
	if (!request) {
		response = jsonrpc_error_response(NULL, jsonrpc_error_object_predefined(JSONRPC_PARSE_ERROR, NULL));
	}
	else if json_is_array(request) {
		size = json_array_size(request);
		if (size==0) {
			response = jsonrpc_error_response(NULL, jsonrpc_error_object_predefined(JSONRPC_INVALID_REQUEST, NULL));
		}
		else {
			response = NULL;
			for (k=0; k < size; k++) {
				req = json_array_get(request, k);
				rep = jsonrpc_handle_request_single(req, methods, userdata, response_handler);
				if (rep) {
					if (!response) {
						response = json_array();
					}
					json_array_append_new(response, rep);
				}
			}
		}
	}
	else {
		response = jsonrpc_handle_request_single(request, methods, userdata, response_handler);
	}

	if (response) {
		output = json_dumps(response, flags);
		json_decref(response);
	}

	if (request) {
		json_decref(request);
	}
	return output;
}

