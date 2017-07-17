#!/usr/bin/env python
# -*- coding: ascii -*-
"""
JSON-RPC (remote procedure call).

It consists of 3 (independent) parts:
    - proxy/dispatcher
    - data structure / serializer
    - transport

It's intended for JSON-RPC, but since the above 3 parts are independent,
it could be used for other RPCs as well.

Currently, JSON-RPC 2.0(pre) and JSON-RPC 1.0 are implemented

:Version:   2008-08-31-beta
:Status:    experimental

:Example:
    simple Client with JsonRPC2.0 and TCP/IP::

        >>> proxy = ServerProxy( JsonRpc20(), TransportTcpIp(addr=("127.0.0.1",31415)) )
        >>> proxy.echo( "hello world" )
        u'hello world'
        >>> proxy.echo( "bye." )
        u'bye.'

    simple Server with JsonRPC2.0 and TCP/IP with logging to STDOUT::

        >>> server = Server( JsonRpc20(), TransportTcpIp(addr=("127.0.0.1",31415), logfunc=log_stdout) )
        >>> def echo( s ):
        ...   return s
        >>> server.register_function( echo )
        >>> server.serve( 2 )   # serve 2 requests          # doctest: +ELLIPSIS
        listen ('127.0.0.1', 31415)
        ('127.0.0.1', ...) connected
        ('127.0.0.1', ...) <-- {"jsonrpc": "2.0", "method": "echo", "params": ["hello world"], "id": 0}
        ('127.0.0.1', ...) --> {"jsonrpc": "2.0", "result": "hello world", "id": 0}
        ('127.0.0.1', ...) close
        ('127.0.0.1', ...) connected
        ('127.0.0.1', ...) <-- {"jsonrpc": "2.0", "method": "echo", "params": ["bye."], "id": 0}
        ('127.0.0.1', ...) --> {"jsonrpc": "2.0", "result": "bye.", "id": 0}
        ('127.0.0.1', ...) close
        close ('127.0.0.1', 31415)

    Client with JsonRPC2.0 and an abstract Unix Domain Socket::
    
        >>> proxy = ServerProxy( JsonRpc20(), TransportUnixSocket(addr="\\x00.rpcsocket") )
        >>> proxy.hi( message="hello" )         #named parameters
        u'hi there'
        >>> proxy.test()                        #fault
        Traceback (most recent call last):
          ...
        jsonrpc.RPCMethodNotFound: <RPCFault -32601: u'Method not found.' (None)>
        >>> proxy.debug.echo( "hello world" )   #hierarchical procedures
        u'hello world'

    Server with JsonRPC2.0 and abstract Unix Domain Socket with a logfile::
        
        >>> server = Server( JsonRpc20(), TransportUnixSocket(addr="\\x00.rpcsocket", logfunc=log_file("mylog.txt")) )
        >>> def echo( s ):
        ...   return s
        >>> def hi( message ):
        ...   return "hi there"
        >>> server.register_function( hi )
        >>> server.register_function( echo, name="debug.echo" )
        >>> server.serve( 3 )   # serve 3 requests

        "mylog.txt" then contains:
        listen '\\x00.rpcsocket'
        '' connected
        '' --> '{"jsonrpc": "2.0", "method": "hi", "params": {"message": "hello"}, "id": 0}'
        '' <-- '{"jsonrpc": "2.0", "result": "hi there", "id": 0}'
        '' close
        '' connected
        '' --> '{"jsonrpc": "2.0", "method": "test", "id": 0}'
        '' <-- '{"jsonrpc": "2.0", "error": {"code":-32601, "message": "Method not found."}, "id": 0}'
        '' close
        '' connected
        '' --> '{"jsonrpc": "2.0", "method": "debug.echo", "params": ["hello world"], "id": 0}'
        '' <-- '{"jsonrpc": "2.0", "result": "hello world", "id": 0}'
        '' close
        close '\\x00.rpcsocket'

:Note:      all exceptions derived from RPCFault are propagated to the client.
            other exceptions are logged and result in a sent-back "empty" INTERNAL_ERROR.
:Uses:      simplejson, socket, sys,time,codecs
:SeeAlso:   JSON-RPC 2.0 proposal, 1.0 specification
:Warning:
    .. Warning::
        This is **experimental** code!
:Bug:

:Author:    Roland Koebler (rk(at)simple-is-better.org)
:Copyright: 2007-2008 by Roland Koebler (rk(at)simple-is-better.org)
:License:   see __license__
:Changelog:
        - 2008-08-31:     1st release

TODO:
        - server: multithreading rpc-server
        - client: multicall (send several requests)
        - transport: SSL sockets, maybe HTTP, HTTPS
        - types: support for date/time (ISO 8601)
        - errors: maybe customizable error-codes/exceptions
        - mixed 1.0/2.0 server ?
        - system description etc. ?
        - maybe test other json-serializers, like cjson?
"""

__version__ = "2008-08-31-beta"
__author__   = "Roland Koebler <rk(at)simple-is-better.org>"
__license__  = """Copyright (c) 2007-2008 by Roland Koebler (rk(at)simple-is-better.org)

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE."""

#=========================================
#import

import sys

#=========================================
# errors

#----------------------
# error-codes + exceptions

#JSON-RPC 2.0 error-codes
PARSE_ERROR           = -32700
INVALID_REQUEST       = -32600
METHOD_NOT_FOUND      = -32601
INVALID_METHOD_PARAMS = -32602  #invalid number/type of parameters
INTERNAL_ERROR        = -32603  #"all other errors"

#additional error-codes
PROCEDURE_EXCEPTION    = -32000
AUTHENTIFICATION_ERROR = -32001
PERMISSION_DENIED      = -32002
INVALID_PARAM_VALUES   = -32003

#human-readable messages
ERROR_MESSAGE = {
    PARSE_ERROR           : "Parse error.",
    INVALID_REQUEST       : "Invalid Request.",
    METHOD_NOT_FOUND      : "Method not found.",
    INVALID_METHOD_PARAMS : "Invalid parameters.",
    INTERNAL_ERROR        : "Internal error.",

    PROCEDURE_EXCEPTION   : "Procedure exception.",
    AUTHENTIFICATION_ERROR : "Authentification error.",
    PERMISSION_DENIED   : "Permission denied.",
    INVALID_PARAM_VALUES: "Invalid parameter values."
    }
 
#----------------------
# exceptions

class RPCError(Exception):
    """Base class for rpc-errors."""


class RPCTransportError(RPCError):
    """Transport error."""
class RPCTimeoutError(RPCTransportError):
    """Transport/reply timeout."""

class RPCFault(RPCError):
    """RPC error/fault package received.
    
    This exception can also be used as a class, to generate a
    RPC-error/fault message.

    :Variables:
        - error_code:   the RPC error-code
        - error_string: description of the error
        - error_data:   optional additional information
                        (must be json-serializable)
    :TODO: improve __str__
    """
    def __init__(self, error_code, error_message, error_data=None):
        RPCError.__init__(self)
        self.error_code   = error_code
        self.error_message = error_message
        self.error_data   = error_data
    def __str__(self):
        return repr(self)
    def __repr__(self):
        return( "<RPCFault %s: %s (%s)>" % (self.error_code, repr(self.error_message), repr(self.error_data)) )

class RPCParseError(RPCFault):
    """Broken rpc-package. (PARSE_ERROR)"""
    def __init__(self, error_data=None):
        RPCFault.__init__(self, PARSE_ERROR, ERROR_MESSAGE[PARSE_ERROR], error_data)

class RPCInvalidRPC(RPCFault):
    """Invalid rpc-package. (INVALID_REQUEST)"""
    def __init__(self, error_data=None):
        RPCFault.__init__(self, INVALID_REQUEST, ERROR_MESSAGE[INVALID_REQUEST], error_data)

class RPCMethodNotFound(RPCFault):
    """Method not found. (METHOD_NOT_FOUND)"""
    def __init__(self, error_data=None):
        RPCFault.__init__(self, METHOD_NOT_FOUND, ERROR_MESSAGE[METHOD_NOT_FOUND], error_data)

class RPCInvalidMethodParams(RPCFault):
    """Invalid method-parameters. (INVALID_METHOD_PARAMS)"""
    def __init__(self, error_data=None):
        RPCFault.__init__(self, INVALID_METHOD_PARAMS, ERROR_MESSAGE[INVALID_METHOD_PARAMS], error_data)

class RPCInternalError(RPCFault):
    """Internal error. (INTERNAL_ERROR)"""
    def __init__(self, error_data=None):
        RPCFault.__init__(self, INTERNAL_ERROR, ERROR_MESSAGE[INTERNAL_ERROR], error_data)


class RPCProcedureException(RPCFault):
    """Procedure exception. (PROCEDURE_EXCEPTION)"""
    def __init__(self, error_data=None):
        RPCFault.__init__(self, PROCEDURE_EXCEPTION, ERROR_MESSAGE[PROCEDURE_EXCEPTION], error_data)
class RPCAuthentificationError(RPCFault):
    """AUTHENTIFICATION_ERROR"""
    def __init__(self, error_data=None):
        RPCFault.__init__(self, AUTHENTIFICATION_ERROR, ERROR_MESSAGE[AUTHENTIFICATION_ERROR], error_data)
class RPCPermissionDenied(RPCFault):
    """PERMISSION_DENIED"""
    def __init__(self, error_data=None):
        RPCFault.__init__(self, PERMISSION_DENIED, ERROR_MESSAGE[PERMISSION_DENIED], error_data)
class RPCInvalidParamValues(RPCFault):
    """INVALID_PARAM_VALUES"""
    def __init__(self, error_data=None):
        RPCFault.__init__(self, INVALID_PARAM_VALUES, ERROR_MESSAGE[INVALID_PARAM_VALUES], error_data)


#=========================================
# data structure / serializer

try:
    import simplejson
except ImportError, err:
    print "FATAL: json-module 'simplejson' is missing (%s)" % (err)
    sys.exit(1)

#----------------------
#
def dictkeyclean(d):
    """Convert all keys of the dict 'd' to (ascii-)strings.

    :Raises: UnicodeEncodeError
    """
    new_d = {}
    for (k, v) in d.iteritems():
        new_d[str(k)] = v
    return new_d

#----------------------
# JSON-RPC 1.0

class JsonRpc10:
    """JSON-RPC V1.0 data-structure / serializer

    This implementation is quite liberal in what it accepts: It treats
    missing "params" and "id" in Requests and missing "result"/"error" in
    Responses as empty/null.

    :SeeAlso:   JSON-RPC 1.0 specification
    :TODO:      catch simplejson.dumps not-serializable-exceptions
    """
    def __init__(self, dumps=simplejson.dumps, loads=simplejson.loads):
        """init: set serializer to use

        :Parameters:
            - dumps: json-encoder-function
            - loads: json-decoder-function
        :Note: The dumps_* functions of this class already directly create
               the invariant parts of the resulting json-object themselves,
               without using the given json-encoder-function.
        """
        self.dumps = dumps
        self.loads = loads

    def dumps_request( self, method, params=(), id=0 ):
        """serialize JSON-RPC-Request

        :Parameters:
            - method: the method-name (str/unicode)
            - params: the parameters (list/tuple)
            - id:     if id=None, this results in a Notification
        :Returns:   | {"method": "...", "params": ..., "id": ...}
                    | "method", "params" and "id" are always in this order.
        :Raises:    TypeError if method/params is of wrong type or 
                    not JSON-serializable
        """
        if not isinstance(method, (str, unicode)):
            raise TypeError('"method" must be a string (or unicode string).')
        if not isinstance(params, (tuple, list)):
            raise TypeError("params must be a tuple/list.")

        return '{"method": %s, "params": %s, "id": %s}' % \
                (self.dumps(method), self.dumps(params), self.dumps(id))

    def dumps_notification( self, method, params=() ):
        """serialize a JSON-RPC-Notification

        :Parameters: see dumps_request
        :Returns:   | {"method": "...", "params": ..., "id": null}
                    | "method", "params" and "id" are always in this order.
        :Raises:    see dumps_request
        """
        if not isinstance(method, (str, unicode)):
            raise TypeError('"method" must be a string (or unicode string).')
        if not isinstance(params, (tuple, list)):
            raise TypeError("params must be a tuple/list.")

        return '{"method": %s, "params": %s, "id": null}' % \
                (self.dumps(method), self.dumps(params))

    def dumps_response( self, result, id=None ):
        """serialize a JSON-RPC-Response (without error)

        :Returns:   | {"result": ..., "error": null, "id": ...}
                    | "result", "error" and "id" are always in this order.
        :Raises:    TypeError if not JSON-serializable
        """
        return '{"result": %s, "error": null, "id": %s}' % \
                (self.dumps(result), self.dumps(id))

    def dumps_error( self, error, id=None ):
        """serialize a JSON-RPC-Response-error

        Since JSON-RPC 1.0 does not define an error-object, this uses the
        JSON-RPC 2.0 error-object.
      
        :Parameters:
            - error: a RPCFault instance
        :Returns:   | {"result": null, "error": {"code": error_code, "message": error_message, "data": error_data}, "id": ...}
                    | "result", "error" and "id" are always in this order, data is omitted if None.
        :Raises:    ValueError if error is not a RPCFault instance,
                    TypeError if not JSON-serializable
        """
        if not isinstance(error, RPCFault):
            raise ValueError("""error must be a RPCFault-instance.""")
        if error.error_data is None:
            return '{"result": null, "error": {"code":%s, "message": %s}, "id": %s}' % \
                    (self.dumps(error.error_code), self.dumps(error.error_message), self.dumps(id))
        else:
            return '{"result": null, "error": {"code":%s, "message": %s, "data": %s}, "id": %s}' % \
                    (self.dumps(error.error_code), self.dumps(error.error_message), self.dumps(error.error_data), self.dumps(id))

    def loads_request( self, string ):
        """de-serialize a JSON-RPC Request/Notification

        :Returns:   | [method_name, params, id] or [method_name, params]
                    | params is a tuple/list
                    | if id is missing, this is a Notification
        :Raises:    RPCParseError, RPCInvalidRPC, RPCInvalidMethodParams
        """
        try:
            data = self.loads(string)
        except ValueError, err:
            raise RPCParseError("No valid JSON. (%s)" % str(err))
        if not isinstance(data, dict):  raise RPCInvalidRPC("No valid RPC-package.")
        if "method" not in data:        raise RPCInvalidRPC("""Invalid Request, "method" is missing.""")
        if not isinstance(data["method"], (str, unicode)):
            raise RPCInvalidRPC("""Invalid Request, "method" must be a string.""")
        if "id"     not in data:        data["id"]     = None   #be liberal
        if "params" not in data:        data["params"] = ()     #be liberal
        if not isinstance(data["params"], (list, tuple)):
            raise RPCInvalidRPC("""Invalid Request, "params" must be an array.""")
        if len(data) != 3:          raise RPCInvalidRPC("""Invalid Request, additional fields found.""")

        # notification / request
        if data["id"] is None:
            return data["method"], data["params"]               #notification
        else:
            return data["method"], data["params"], data["id"]   #request

    def loads_response( self, string ):
        """de-serialize a JSON-RPC Response/error

        :Returns: | [result, id] for Responses
        :Raises:  | RPCFault+derivates for error-packages/faults, RPCParseError, RPCInvalidRPC
                  | Note that for error-packages which do not match the
                    V2.0-definition, RPCFault(-1, "Error", RECEIVED_ERROR_OBJ)
                    is raised.
        """
        try:
            data = self.loads(string)
        except ValueError, err:
            raise RPCParseError("No valid JSON. (%s)" % str(err))
        if not isinstance(data, dict):  raise RPCInvalidRPC("No valid RPC-package.")
        if "id" not in data:            raise RPCInvalidRPC("""Invalid Response, "id" missing.""")
        if "result" not in data:        data["result"] = None    #be liberal
        if "error"  not in data:        data["error"]  = None    #be liberal
        if len(data) != 3:              raise RPCInvalidRPC("""Invalid Response, additional or missing fields.""")

        #error
        if data["error"] is not None:
            if data["result"] is not None:
                raise RPCInvalidRPC("""Invalid Response, one of "result" or "error" must be null.""")
            #v2.0 error-format
            if( isinstance(data["error"], dict)  and  "code" in data["error"]  and  "message" in data["error"]  and
                (len(data["error"])==2 or ("data" in data["error"] and len(data["error"])==3)) ):
                if "data" not in data["error"]:
                    error_data = None
                else:
                    error_data = data["error"]["data"]

                if   data["error"]["code"] == PARSE_ERROR:
                    raise RPCParseError(error_data)
                elif data["error"]["code"] == INVALID_REQUEST:
                    raise RPCInvalidRPC(error_data)
                elif data["error"]["code"] == METHOD_NOT_FOUND:
                    raise RPCMethodNotFound(error_data)
                elif data["error"]["code"] == INVALID_METHOD_PARAMS:
                    raise RPCInvalidMethodParams(error_data)
                elif data["error"]["code"] == INTERNAL_ERROR:
                    raise RPCInternalError(error_data)
                elif data["error"]["code"] == PROCEDURE_EXCEPTION:
                    raise RPCProcedureException(error_data)
                elif data["error"]["code"] == AUTHENTIFICATION_ERROR:
                    raise RPCAuthentificationError(error_data)
                elif data["error"]["code"] == PERMISSION_DENIED:
                    raise RPCPermissionDenied(error_data)
                elif data["error"]["code"] == INVALID_PARAM_VALUES:
                    raise RPCInvalidParamValues(error_data)
                else:
                    raise RPCFault(data["error"]["code"], data["error"]["message"], error_data)
            #other error-format
            else:
                raise RPCFault(-1, "Error", data["error"])
        #result
        else:
            return data["result"], data["id"]

#----------------------
# JSON-RPC 2.0

class JsonRpc20:
    """JSON-RPC V2.0 data-structure / serializer

    :SeeAlso:   JSON-RPC 2.0 specification
    :TODO:      catch simplejson.dumps not-serializable-exceptions
    """
    def __init__(self, dumps=simplejson.dumps, loads=simplejson.loads):
        """init: set serializer to use

        :Parameters:
            - dumps: json-encoder-function
            - loads: json-decoder-function
        :Note: The dumps_* functions of this class already directly create
               the invariant parts of the resulting json-object themselves,
               without using the given json-encoder-function.
        """
        self.dumps = dumps
        self.loads = loads

    def dumps_request( self, method, params=(), id=0 ):
        """serialize JSON-RPC-Request

        :Parameters:
            - method: the method-name (str/unicode)
            - params: the parameters (list/tuple/dict)
            - id:     the id (should not be None)
        :Returns:   | {"jsonrpc": "2.0", "method": "...", "params": ..., "id": ...}
                    | "jsonrpc", "method", "params" and "id" are always in this order.
                    | "params" is omitted if empty
        :Raises:    TypeError if method/params is of wrong type or 
                    not JSON-serializable
        """
        if not isinstance(method, (str, unicode)):
            raise TypeError('"method" must be a string (or unicode string).')
        if not isinstance(params, (tuple, list, dict)):
            raise TypeError("params must be a tuple/list/dict or None.")

        if params:
            return '{"jsonrpc": "2.0", "method": %s, "params": %s, "id": %s}' % \
                    (self.dumps(method), self.dumps(params), self.dumps(id))
        else:
            return '{"jsonrpc": "2.0", "method": %s, "id": %s}' % \
                    (self.dumps(method), self.dumps(id))

    def dumps_notification( self, method, params=() ):
        """serialize a JSON-RPC-Notification

        :Parameters: see dumps_request
        :Returns:   | {"jsonrpc": "2.0", "method": "...", "params": ...}
                    | "jsonrpc", "method" and "params" are always in this order.
        :Raises:    see dumps_request
        """
        if not isinstance(method, (str, unicode)):
            raise TypeError('"method" must be a string (or unicode string).')
        if not isinstance(params, (tuple, list, dict)):
            raise TypeError("params must be a tuple/list/dict or None.")

        if params:
            return '{"jsonrpc": "2.0", "method": %s, "params": %s}' % \
                    (self.dumps(method), self.dumps(params))
        else:
            return '{"jsonrpc": "2.0", "method": %s}' % \
                    (self.dumps(method))

    def dumps_response( self, result, id=None ):
        """serialize a JSON-RPC-Response (without error)

        :Returns:   | {"jsonrpc": "2.0", "result": ..., "id": ...}
                    | "jsonrpc", "result", and "id" are always in this order.
        :Raises:    TypeError if not JSON-serializable
        """
        return '{"jsonrpc": "2.0", "result": %s, "id": %s}' % \
                (self.dumps(result), self.dumps(id))

    def dumps_error( self, error, id=None ):
        """serialize a JSON-RPC-Response-error
      
        :Parameters:
            - error: a RPCFault instance
        :Returns:   | {"jsonrpc": "2.0", "error": {"code": error_code, "message": error_message, "data": error_data}, "id": ...}
                    | "jsonrpc", "result", "error" and "id" are always in this order, data is omitted if None.
        :Raises:    ValueError if error is not a RPCFault instance,
                    TypeError if not JSON-serializable
        """
        if not isinstance(error, RPCFault):
            raise ValueError("""error must be a RPCFault-instance.""")
        if error.error_data is None:
            return '{"jsonrpc": "2.0", "error": {"code":%s, "message": %s}, "id": %s}' % \
                    (self.dumps(error.error_code), self.dumps(error.error_message), self.dumps(id))
        else:
            return '{"jsonrpc": "2.0", "error": {"code":%s, "message": %s, "data": %s}, "id": %s}' % \
                    (self.dumps(error.error_code), self.dumps(error.error_message), self.dumps(error.error_data), self.dumps(id))

    def loads_request( self, string ):
        """de-serialize a JSON-RPC Request/Notification

        :Returns:   | [method_name, params, id] or [method_name, params]
                    | params is a tuple/list or dict (with only str-keys)
                    | if id is missing, this is a Notification
        :Raises:    RPCParseError, RPCInvalidRPC, RPCInvalidMethodParams
        """
        try:
            data = self.loads(string)
        except ValueError, err:
            raise RPCParseError("No valid JSON. (%s)" % str(err))
        if not isinstance(data, dict):  raise RPCInvalidRPC("No valid RPC-package.")
        if "jsonrpc" not in data:       raise RPCInvalidRPC("""Invalid Response, "jsonrpc" missing.""")
        if not isinstance(data["jsonrpc"], (str, unicode)):
            raise RPCInvalidRPC("""Invalid Response, "jsonrpc" must be a string.""")
        if data["jsonrpc"] != "2.0":    raise RPCInvalidRPC("""Invalid jsonrpc version.""")
        if "method" not in data:        raise RPCInvalidRPC("""Invalid Request, "method" is missing.""")
        if not isinstance(data["method"], (str, unicode)):
            raise RPCInvalidRPC("""Invalid Request, "method" must be a string.""")
        if "params" not in data:        data["params"] = ()
        #convert params-keys from unicode to str
        elif isinstance(data["params"], dict):
            try:
                data["params"] = dictkeyclean(data["params"])
            except UnicodeEncodeError:
                raise RPCInvalidMethodParams("Parameter-names must be in ascii.")
        elif not isinstance(data["params"], (list, tuple)):
            raise RPCInvalidRPC("""Invalid Request, "params" must be an array or object.""")
        if not( len(data)==3 or ("id" in data and len(data)==4) ):
            raise RPCInvalidRPC("""Invalid Request, additional fields found.""")

        # notification / request
        if "id" not in data:
            return data["method"], data["params"]               #notification
        else:
            return data["method"], data["params"], data["id"]   #request

    def loads_response( self, string ):
        """de-serialize a JSON-RPC Response/error

        :Returns: | [result, id] for Responses
        :Raises:  | RPCFault+derivates for error-packages/faults, RPCParseError, RPCInvalidRPC
        """
        try:
            data = self.loads(string)
        except ValueError, err:
            raise RPCParseError("No valid JSON. (%s)" % str(err))
        if not isinstance(data, dict):  raise RPCInvalidRPC("No valid RPC-package.")
        if "jsonrpc" not in data:       raise RPCInvalidRPC("""Invalid Response, "jsonrpc" missing.""")
        if not isinstance(data["jsonrpc"], (str, unicode)):
            raise RPCInvalidRPC("""Invalid Response, "jsonrpc" must be a string.""")
        if data["jsonrpc"] != "2.0":    raise RPCInvalidRPC("""Invalid jsonrpc version.""")
        if "id" not in data:            raise RPCInvalidRPC("""Invalid Response, "id" missing.""")
        if "result" not in data:        data["result"] = None
        if "error"  not in data:        data["error"]  = None
        if len(data) != 4:              raise RPCInvalidRPC("""Invalid Response, additional or missing fields.""")

        #error
        if data["error"] is not None:
            if data["result"] is not None:
                raise RPCInvalidRPC("""Invalid Response, only "result" OR "error" allowed.""")
            if not isinstance(data["error"], dict): raise RPCInvalidRPC("Invalid Response, invalid error-object.")
            if "code" not in data["error"]  or  "message" not in data["error"]:
                raise RPCInvalidRPC("Invalid Response, invalid error-object.")
            if "data" not in data["error"]:  data["error"]["data"] = None
            if len(data["error"]) != 3:
                raise RPCInvalidRPC("Invalid Response, invalid error-object.")

            error_data = data["error"]["data"]
            if   data["error"]["code"] == PARSE_ERROR:
                raise RPCParseError(error_data)
            elif data["error"]["code"] == INVALID_REQUEST:
                raise RPCInvalidRPC(error_data)
            elif data["error"]["code"] == METHOD_NOT_FOUND:
                raise RPCMethodNotFound(error_data)
            elif data["error"]["code"] == INVALID_METHOD_PARAMS:
                raise RPCInvalidMethodParams(error_data)
            elif data["error"]["code"] == INTERNAL_ERROR:
                raise RPCInternalError(error_data)
            elif data["error"]["code"] == PROCEDURE_EXCEPTION:
                raise RPCProcedureException(error_data)
            elif data["error"]["code"] == AUTHENTIFICATION_ERROR:
                raise RPCAuthentificationError(error_data)
            elif data["error"]["code"] == PERMISSION_DENIED:
                raise RPCPermissionDenied(error_data)
            elif data["error"]["code"] == INVALID_PARAM_VALUES:
                raise RPCInvalidParamValues(error_data)
            else:
                raise RPCFault(data["error"]["code"], data["error"]["message"], error_data)
        #result
        else:
            return data["result"], data["id"]


#=========================================
# transports

#----------------------
# transport-logging

import codecs
import time

def log_dummy( message ):
    """dummy-logger: do nothing"""
    pass
def log_stdout( message ):
    """print message to STDOUT"""
    print message

def log_file( filename ):
    """return a logfunc which logs to a file (in utf-8)"""
    def logfile( message ):
        f = codecs.open( filename, 'a', encoding='utf-8' )
        f.write( message+"\n" )
        f.close()
    return logfile

def log_filedate( filename ):
    """return a logfunc which logs date+message to a file (in utf-8)"""
    def logfile( message ):
        f = codecs.open( filename, 'a', encoding='utf-8' )
        f.write( time.strftime("%Y-%m-%d %H:%M:%S ")+message+"\n" )
        f.close()
    return logfile

#----------------------

class Transport:
    """generic Transport-interface.
    
    This class, and especially its methods and docstrings,
    define the Transport-Interface.
    """
    def __init__(self):
        pass

    def send( self, data ):
        """send all data. must be implemented by derived classes."""
        raise NotImplementedError
    def recv( self ):
        """receive data. must be implemented by derived classes."""
        raise NotImplementedError

    def sendrecv( self, string ):
        """send + receive data"""
        self.send( string )
        return self.recv()
    def serve( self, handler, n=None ):
        """serve (forever or for n communicaions).
        
        - receive data
        - call result = handler(data)
        - send back result if not None

        The serving can be stopped by SIGINT.

        :TODO:
            - how to stop?
              maybe use a .run-file, and stop server if file removed?
            - maybe make n_current accessible? (e.g. for logging)
        """
        n_current = 0
        while 1:
            if n is not None  and  n_current >= n:
                break
            data = self.recv()
            result = handler(data)
            if result is not None:
                self.send( result )
            n_current += 1


class TransportSTDINOUT(Transport):
    """receive from STDIN, send to STDOUT.

    Useful e.g. for debugging.
    """
    def send(self, string):
        """write data to STDOUT with '***SEND:' prefix """
        print "***SEND:"
        print string
    def recv(self):
        """read data from STDIN"""
        print "***RECV (please enter, ^D ends.):"
        return sys.stdin.read()


import socket, select
class TransportSocket(Transport):
    """Transport via socket.
   
    :SeeAlso:   python-module socket
    :TODO:
        - documentation
        - improve this (e.g. make sure that connections are closed, socket-files are deleted etc.)
        - exception-handling? (socket.error)
    """
    def __init__( self, addr, limit=4096, sock_type=socket.AF_INET, sock_prot=socket.SOCK_STREAM, timeout=1.0, logfunc=log_dummy ):
        """
        :Parameters:
            - addr: socket-address
            - timeout: timeout in seconds
            - logfunc: function for logging, logfunc(message)
        :Raises: socket.timeout after timeout
        """
        self.limit  = limit
        self.addr   = addr
        self.s_type = sock_type
        self.s_prot = sock_prot
        self.s      = None
        self.timeout = timeout
        self.log    = logfunc
    def connect( self ):
        self.close()
        self.log( "connect to %s" % repr(self.addr) )
        self.s = socket.socket( self.s_type, self.s_prot )
        self.s.settimeout( self.timeout )
        self.s.connect( self.addr )
    def close( self ):
        if self.s is not None:
            self.log( "close %s" % repr(self.addr) )
            self.s.close()
            self.s = None
    def __repr__(self):
        return "<TransportSocket, %s>" % repr(self.addr)
    
    def send( self, string ):
        if self.s is None:
            self.connect()
        self.log( "--> "+repr(string) )
        self.s.sendall( string )
    def recv( self ):
        if self.s is None:
            self.connect()
        data = self.s.recv( self.limit )
        while( select.select((self.s,), (), (), 0.1)[0] ):  #TODO: this select is probably not necessary, because server closes this socket
            d = self.s.recv( self.limit )
            if len(d) == 0:
                break
            data += d
        self.log( "<-- "+repr(data) )
        return data

    def sendrecv( self, string ):
        """send data + receive data + close"""
        try:
            self.send( string )
            return self.recv()
        finally:
            self.close()
    def serve(self, handler, n=None):
        """open socket, wait for incoming connections and handle them.
        
        :Parameters:
            - n: serve n requests, None=forever
        """
        self.close()
        self.s = socket.socket( self.s_type, self.s_prot )
        try:
            self.log( "listen %s" % repr(self.addr) )
            self.s.bind( self.addr )
            self.s.listen(1)
            n_current = 0
            while 1:
                if n is not None  and  n_current >= n:
                    break
                conn, addr = self.s.accept()
                self.log( "%s connected" % repr(addr) )
                data = conn.recv(self.limit)
                self.log( "%s --> %s" % (repr(addr), repr(data)) )
                result = handler(data)
                if data is not None:
                    self.log( "%s <-- %s" % (repr(addr), repr(result)) )
                    conn.send( result )
                self.log( "%s close" % repr(addr) )
                conn.close()
                n_current += 1
        finally:
            self.close()


if hasattr(socket, 'AF_UNIX'):
    
    class TransportUnixSocket(TransportSocket):
        """Transport via Unix Domain Socket.
        """
        def __init__(self, addr=None, limit=4096, timeout=1.0, logfunc=log_dummy):
            """
            :Parameters:
                - addr: "socket_file"
            :Note: | The socket-file is not deleted.
                   | If the socket-file begins with \x00, abstract sockets are used,
                     and no socket-file is created.
            :SeeAlso:   TransportSocket
            """
            TransportSocket.__init__( self, addr, limit, socket.AF_UNIX, socket.SOCK_STREAM, timeout, logfunc )

class TransportTcpIp(TransportSocket):
    """Transport via TCP/IP.
    """
    def __init__(self, addr=None, limit=4096, timeout=1.0, logfunc=log_dummy):
        """
        :Parameters:
            - addr: ("host",port)
        :SeeAlso:   TransportSocket
        """
        TransportSocket.__init__( self, addr, limit, socket.AF_INET, socket.SOCK_STREAM, timeout, logfunc )


#=========================================
# client side: server proxy

class ServerProxy:
    """RPC-client: server proxy

    A logical connection to a RPC server.

    It works with different data/serializers and different transports.

    Notifications and id-handling/multicall are not yet implemented.

    :Example:
        see module-docstring

    :TODO: verbose/logging?
    """
    def __init__( self, data_serializer, transport ):
        """
        :Parameters:
            - data_serializer: a data_structure+serializer-instance
            - transport: a Transport instance
        """
        #TODO: check parameters
        self.__data_serializer = data_serializer
        if not isinstance(transport, Transport):
            raise ValueError('invalid "transport" (must be a Transport-instance)"')
        self.__transport = transport

    def __str__(self):
        return repr(self)
    def __repr__(self):
        return "<ServerProxy for %s, with serializer %s>" % (self.__transport, self.__data_serializer)

    def __req( self, methodname, args=None, kwargs=None, id=0 ):
        # JSON-RPC 1.0: only positional parameters
        if len(kwargs) > 0 and isinstance(self.data_serializer, JsonRpc10):
            raise ValueError("Only positional parameters allowed in JSON-RPC 1.0")
        # JSON-RPC 2.0: only args OR kwargs allowed!
        if len(args) > 0 and len(kwargs) > 0:
            raise ValueError("Only positional or named parameters are allowed!")
        if len(kwargs) == 0:
            req_str  = self.__data_serializer.dumps_request( methodname, args, id )
        else:
            req_str  = self.__data_serializer.dumps_request( methodname, kwargs, id )

        try:
            resp_str = self.__transport.sendrecv( req_str )
        except Exception,err:
            raise RPCTransportError(err)
        resp = self.__data_serializer.loads_response( resp_str )
        return resp[0]

    def __getattr__(self, name):
        # magic method dispatcher
        #  note: to call a remote object with an non-standard name, use
        #  result getattr(my_server_proxy, "strange-python-name")(args)
        return _method(self.__req, name)

# request dispatcher
class _method:
    """some "magic" to bind an RPC method to an RPC server.

    Supports "nested" methods (e.g. examples.getStateName).

    :Raises: AttributeError for method-names/attributes beginning with '_'.
    """
    def __init__(self, req, name):
        if name[0] == "_":  #prevent rpc-calls for proxy._*-functions
            raise AttributeError("invalid attribute '%s'" % name)
        self.__req  = req
        self.__name = name
    def __getattr__(self, name):
        if name[0] == "_":  #prevent rpc-calls for proxy._*-functions
            raise AttributeError("invalid attribute '%s'" % name)
        return _method(self.__req, "%s.%s" % (self.__name, name))
    def __call__(self, *args, **kwargs):
        return self.__req(self.__name, args, kwargs)

#=========================================
# server side: Server

class Server:
    """RPC-server.

    It works with different data/serializers and 
    with different transports.

    :Example:
        see module-docstring

    :TODO:
        - mixed JSON-RPC 1.0/2.0 server?
        - logging/loglevels?
    """
    def __init__( self, data_serializer, transport, logfile=None ):
        """
        :Parameters:
            - data_serializer: a data_structure+serializer-instance
            - transport: a Transport instance
            - logfile: file to log ("unexpected") errors to
        """
        #TODO: check parameters
        self.__data_serializer = data_serializer
        if not isinstance(transport, Transport):
            raise ValueError('invalid "transport" (must be a Transport-instance)"')
        self.__transport = transport
        self.logfile = logfile
        if self.logfile is not None:    #create logfile (or raise exception)
            f = codecs.open( self.logfile, 'a', encoding='utf-8' )
            f.close()

        self.funcs = {}

    def __repr__(self):
        return "<Server for %s, with serializer %s>" % (self.__transport, self.__data_serializer)

    def log(self, message):
        """write a message to the logfile (in utf-8)"""
        if self.logfile is not None:
            f = codecs.open( self.logfile, 'a', encoding='utf-8' )
            f.write( time.strftime("%Y-%m-%d %H:%M:%S ")+message+"\n" )
            f.close()

    def register_instance(self, myinst, name=None):
        """Add all functions of a class-instance to the RPC-services.
        
        All entries of the instance which do not begin with '_' are added.

        :Parameters:
            - myinst: class-instance containing the functions
            - name:   | hierarchical prefix.
                      | If omitted, the functions are added directly.
                      | If given, the functions are added as "name.function".
        :TODO:
            - only add functions and omit attributes?
            - improve hierarchy?
        """
        for e in dir(myinst):
            if e[0][0] != "_":
                if name is None:
                    self.register_function( getattr(myinst, e) )
                else:
                    self.register_function( getattr(myinst, e), name="%s.%s" % (name, e) )
    def register_function(self, function, name=None):
        """Add a function to the RPC-services.
        
        :Parameters:
            - function: function to add
            - name:     RPC-name for the function. If omitted/None, the original
                        name of the function is used.
        """
        if name is None:
            self.funcs[function.__name__] = function
        else:
            self.funcs[name] = function
    
    def handle(self, rpcstr):
        """Handle a RPC-Request.

        :Parameters:
            - rpcstr: the received rpc-string
        :Returns: the data to send back or None if nothing should be sent back
        :Raises:  RPCFault (and maybe others)
        """
        #TODO: id
        notification = False
        try:
            req = self.__data_serializer.loads_request( rpcstr )
            if len(req) == 2:       #notification
                method, params = req
                notification = True
            else:                   #request
                method, params, id = req
        except RPCFault, err:
            return self.__data_serializer.dumps_error( err, id=None )
        except Exception, err:
            self.log( "%d (%s): %s" % (INTERNAL_ERROR, ERROR_MESSAGE[INTERNAL_ERROR], str(err)) )
            return self.__data_serializer.dumps_error( RPCFault(INTERNAL_ERROR, ERROR_MESSAGE[INTERNAL_ERROR]), id=None )

        if method not in self.funcs:
            if notification:
                return None
            return self.__data_serializer.dumps_error( RPCFault(METHOD_NOT_FOUND, ERROR_MESSAGE[METHOD_NOT_FOUND]), id )

        try:
            if isinstance(params, dict):
                result = self.funcs[method]( **params )
            else:
                result = self.funcs[method]( *params )
        except RPCFault, err:
            if notification:
                return None
            return self.__data_serializer.dumps_error( err, id=None )
        except Exception, err:
            if notification:
                return None
            self.log( "%d (%s): %s" % (INTERNAL_ERROR, ERROR_MESSAGE[INTERNAL_ERROR], str(err)) )
            return self.__data_serializer.dumps_error( RPCFault(INTERNAL_ERROR, ERROR_MESSAGE[INTERNAL_ERROR]), id )

        if notification:
            return None
        try:
            return self.__data_serializer.dumps_response( result, id )
        except Exception, err:
            self.log( "%d (%s): %s" % (INTERNAL_ERROR, ERROR_MESSAGE[INTERNAL_ERROR], str(err)) )
            return self.__data_serializer.dumps_error( RPCFault(INTERNAL_ERROR, ERROR_MESSAGE[INTERNAL_ERROR]), id )

    def serve(self, n=None):
        """serve (forever or for n communicaions).
        
        :See: Transport
        """
        self.__transport.serve( self.handle, n )

#=========================================

