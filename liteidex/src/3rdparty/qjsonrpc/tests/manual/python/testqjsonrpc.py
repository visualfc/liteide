#!/usr/bin/python

import jsonrpc
import socket

rpc = jsonrpc.ServerProxy(jsonrpc.JsonRpc20(), jsonrpc.TransportSocket(addr="/tmp/testservice", sock_type=socket.AF_UNIX))
print rpc.agent.testMethod()
print rpc.agent.testMethodWithParams("one", False, 10)
print rpc.agent.testMethodWithParamsAndReturnValue("matt")
