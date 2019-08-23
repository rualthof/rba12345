## Case study

#### Files

* **Makefile**: allows intalling dependencies, compiling and cleaning files after executions.
* **client.c**: creates a client able to send messages to the server and receive the messages from the server.
* **server.c**: creates a pipe for receiving messages from the clients, broadcasts the messages to all clients and to the logger.
  * my_fifo: created by the server in order to receive messags from the clientes.
* **logger.c**: receives the messages from the server and writes it to the file log.txt
  * log.txt: created by the logger to store all the messages exchanged between clients.
 * **zhelpers.h** : obtained from [Here](https://github.com/booksbyus/zguide/blob/master/examples/C/zhelpers.h), has some utilities related to the zmq library.

#### Make Commands
Installing the dependencies:
```bash
make install
```
Compiling:
```bash
make compile
```

This will create the binaries server, client and logger.

Cleaning up:
```bash
make clean
```

#### Solution

The software implements the sollution as follows:

![Alt text](diagram.jpg?raw=true "Solution")
