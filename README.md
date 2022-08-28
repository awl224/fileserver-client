# Fileserver Project Overview

This is my file server project that has three main components to it, which are all found in the same repository. It is a project I worked on throughout my systems software course at Lehigh and updated since to fix some bugs. The project was completed within 6 months of starting to learn the C and C++ languages. It includes a full serialization protocol, use of UNIX sockets for server and clients, and a hashmap implementation for file objects. Along with the project itself, I learned how to use a version control system, GIT, and organizing the architecute for a project of this size.

### 1) The serialization protocol for the project:
Although not all functions are used for the fileserver, there are serializations of booleans, 8, 32, and 64-bit unsigned and singed integers. Along with this, there are protocols for strings, arrays, and a map. This serialization protocol is used in both the server and client to communicate with eachother.

### 2) The client to the file server:
This client is modified for sustaining connection to the server, when running the executable, you must include the address the server is set up on along with sending a file. After this, you may send or request files to/from server until the user terminates the program.

### 3) The file server:
This server stores files in a hashmap implementation for file items. Once running on an address, this server will run until terminated.

## Example Use
Follow the link to a video example of the program being used: https://drive.google.com/file/d/1qUK2x3AH9bhLByPR8mXpLGFodSLHbPjn/view?usp=sharing