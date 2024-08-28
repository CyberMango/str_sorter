design:

client, server and IPC are the main components.

server:
- requests handler (ui)
    - both receive and send.
    - its own thread. always running. 
    - first single threaded.
        - Later handles each request in its own thread.
    - depends on the IPC library and the Logic.
- data storer (db)
    - sorts the currently received data by UIDs
    - sends the data to the requesting components.
    - stores everything in RAM at first.
    - doesnt fail on asking too much data, simply sends what it has.
    - only creates new clients, doesnt delete them even on empty data.
    - First single threaded.
        - Later handles each request in a different thread.
        - each client data has its own lock.
    - depends on both the UI and the Logic.
- Logic (business)
    - handles sorting the data.
    - probably so small in this one that can be coupled inside of the DB at first.
*future idea: have the big server class handle all their inter connections so the modules
are completely independent.

client:
* Its a library, not some independent tool.
- requests sender
    - Single threaded.
    - Needs a creation and destruction.
    - depends on the IPC.

IPC:
- At first will use sockets in a single use. Send and close connection.
    - Later will keep sockets until closed.
- api to make it not socket dependent:
    - start
        - starts listening to connections. clients can at least try to connect.
    - wait for requests.
        - Requests in a single form for both send and receive. UID, type, data_len, data.

