Physical layer simulator. 

``send.c`` - the code that the sender will execute

``recv.c`` - the code that the receiver on the other end of the wire will execute

Overview of the architecture
```
sender                receiver
  |                      |
  |______________________|
        Physical (wire)
```

Note, if you want to use C++, simply change the extension of the `send.c` and `recv.c` to `.cpp` and update the Makefile.

## API
```
/* Sends one character to the other end via the Physical layer */
int send_byte(char c);
/* Receives one character from the other end, if nothing is sent by the other end, returns a random character */
char recv_byte();
```
## Usage
To compile the code
```
make
```

We will run the sender and receiver in parallel using ``run_experiment.sh``.
