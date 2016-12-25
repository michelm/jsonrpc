jsonrpc.{ch} implements the parsing and validation of the jsonrpc/2.0 request before handing it over to 
user-defined methods.

jsonrpc_server.c is an example of usage using ZeroMQ as a transport.
It has been tested against ZeroMQ 3.2.2 and jansson 2.3.1


Since significant functionality is provided by jansson library, the same licence is chosen to be used by this code.

REMARKS
=======
Cloned from https://github.com/pijyoi/jsonrpc
Changes:
- Added method for creating notifications
- Added stdout logging to test scripts
- Added client support; i.e. receive and process JSONRPC responses
- Use static and/or shared libraries
- Added MinGW support

