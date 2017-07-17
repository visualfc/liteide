[![Build Status](https://drone.io/bitbucket.org/devonit/qjsonrpc/status.png)](https://drone.io/bitbucket.org/devonit/qjsonrpc/latest)

Overview
=======

QJsonRpc is a Qt implementation of the JSON-RPC protocol.
It integrates nicely with Qt, leveraging Qt's meta object system in order
to provide services over the JSON-RPC protocol. QJsonRpc is licensed under
the LGPLv2.1.

- [JSON](http://www.json.org/) is a lightweight data interchange format.
- [JSON-RPC](http://jsonrpc.org/) is lightweight remote procedure call protocol similar to XML-RPC.

Help / Questions / Suggestions
============
[qjsonrpc-development](http://groups.google.com/group/qjsonrpc-development)

Requirements
============

- Qt 4.7 or greater

Features
========

- Support for JSON-RPC 2.0
- Easily create services using the Qt meta object system
- QtScript integration

Building
========

    mkdir build
    cd build
    qmake .. && make install

You can pass the following arguments to qmake:

    PREFIX=<prefix>                 to change the install prefix
                                    default:
                                        unix:  /usr
                                        other: $$[QT_INSTALL_PREFIX]
    QJSONRPC_LIBRARY_TYPE=staticlib to build a static version of the library
    -config private-headers         build with qt private headers (to reduce allocations)
