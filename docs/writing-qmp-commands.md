# How to write QMP commands using the QAPI framework

This document is a step-by-step guide on how to write new QMP commands using the QAPI framework.

This document doesn't discuss QMP protocol level details, nor does it dive into the QAPI framework implementation.

For an in-depth introduction to the QAPI framework, please refer to docs/qapi-code-gen.txt. For documentation about the QMP protocol,
start with docs/qmp-intro.txt.

## Overview

Generally speaking, the following steps should be taken in order to write a new QMP command.

1. Write the command's and type(s) specification in the QAPI schema file (qapi-schema.json in the root source directory)

2. Write the QMP command itself, which is a regular C function. Preferably, the command should be exported by some QEMU subsystem. But it can also be added to the qmp.c file

3. At this point the command can be tested under the QMP protocol

The following sections will demonstrate each of the steps above. We will start very simple and get more complex as we progress.

## Writing a command that doesn't return data

That's the most simple QMP command that can be written. Usually, this kind of command carries some meaningful action in QEMU but here it will just print "Hello, world" to the standard output.

Our command will be called "hello-world". It takes no arguments, nor does it return any data.

The first step is to add the following line to the bottom of the qapi-schema.json file:
```
{ 'command': 'hello-world' }
```
The "command" keyword defines a new QMP command. It's an JSON object. All schema entries are JSON objects. The line above will instruct the QAPI to generate any prototypes and the necessary code to marshal and unmarshal protocol data.

The next step is to write the "hello-world" implementation. As explained earlier, it's preferable for commands to live in QEMU subsystems. But "hello-world" doesn't pertain to any, so we put its implementation in qmp.c:
```
void qmp_hello_world(Error **errp)
{
    printf("Hello, world!\n");
}
```
There are a few things to be noticed:

1. QMP command implementation functions must be prefixed with "qmp_"

2. qmp_hello_world() returns void, this is in accordance with the fact that the command doesn't return any data

3. It takes an "Error **" argument. This is required. Later we will see how to return errors and take additional arguments. The Error argument should not be touched if the command doesn't return errors

4. We won't add the function's prototype. That's automatically done by the QAPI

5. Printing to the terminal is discouraged for QMP commands, we do it here because it's the easiest way to demonstrate a QMP command

You're done. Now build qemu, run it as suggested in the "Testing" section, and then type the following QMP command:
```
{ "execute": "hello-world" }
```
Then check the terminal running qemu and look for the "Hello, world" string. If you don't see it then something went wrong.

### Arguments

Let's add an argument called "message" to our "hello-world" command. The new argument will contain the string to be printed to stdout. It's an optional argument, if it's not present we print our default "Hello, World" string.

The first change we have to do is to modify the command specification in the schema file to the following:
```
{ 'command': 'hello-world', 'data': { '*message': 'str' } }
```
Notice the new 'data' member in the schema. It's an JSON object whose each element is an argument to the command in question. Also notice the asterisk, it's used to mark the argument optional (that means that you shouldn't use it for mandatory arguments). Finally, 'str' is the argument's type, which stands for "string". The QAPI also supports integers, booleans, enumerations and user defined types.

Now, let's update our C implementation in qmp.c:
```
void qmp_hello_world(bool has_message, const char *message, Error **errp)
{
    if (has_message) {
        printf("%s\n", message);
    } else {
        printf("Hello, world\n");
    }
}
```
There are two important details to be noticed:

1. All optional arguments are accompanied by a 'has_' boolean, which is set if the optional argument is present or false otherwise
2. The C implementation signature must follow the schema's argument ordering, which is defined by the "data" member

Time to test our new version of the "hello-world" command. Build qemu, run it as described in the "Testing" section and then send two commands:
```
{ "execute": "hello-world" }
{
    "return": {
    }
}

{ "execute": "hello-world", "arguments": { "message": "We love qemu" } }
{
    "return": {
    }
}
```
You should see "Hello, world" and "we love qemu" in the terminal running qemu, if you don't see these strings, then something went wrong.

### Command Documentation

There's only one step missing to make "hello-world"'s implementation complete, and that's its documentation in the schema file.

This is very important. No QMP command will be accepted in QEMU without proper documentation.

There are many examples of such documentation in the schema file already, but here goes "hello-world"'s new entry for the qapi-schema.json file:
```
##
# @hello-world
#
# Print a client provided string to the standard output stream.
#
# @message: #optional string to be printed
#
# Returns: Nothing on success.
#
# Notes: if @message is not provided, the "Hello, world" string will
#        be printed instead
#
# Since: <next qemu stable release, eg. 1.0>
##
{ 'command': 'hello-world', 'data': { '*message': 'str' } }
```
Please, note that the "Returns" clause is optional if a command doesn't return any data nor any errors.