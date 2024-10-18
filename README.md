Exercise:
The goal of this exercise is to create a server and client software on local Linux machine.
The server should be written in C\C++. And should do the following:
1. The server should receive string from the client. Sort the string (currently without
limitation on space or efficiency) and return the sorted string.
2. The client can send multiple requests before receiving any data.
3. The client request to receive specific amount of data. The data which is sent is the
beginning of the sorted data which have been received.
4. All the received data should not be sent more than once
5. The data in the server should be aggregated by Linux user id.
All the API and the protocols can be chosen by you.
Example for the client interaction:
```
send(“hello”, 5)
recv(5) -> “ehllo”
send(“hello”, 5)
recv(2) -> eh
send(“world”, 5)
recv(8) -> “dllloorw”
```

Please give the following points some consideration during design and implementation:
Implementation Directives:
- The language used to implement this task should be C++. In any event where an exception is needed, please reach out to the interviewer.
- The server must support multiple client connections concurrently.
- The IPC portion of the task needs to be decoupled from the rest of the system. This means that if we choose to substitute the IPC method, the rest of the system will remain (relatively) untouched.
- No libraries should be used to implement the solution [apart from the c++ standard one].
- System architecture (example: what subcomponents does the system have?)
- Components design (example: what entities are involved in making a specific components?)
- Correctness
- Robustness (example: solid and consistent error handling)
- Usability (example: clear error messages)
- What are the low-level Linux facilities that need to be employed here?
- What language features are most useful in achieving the goals mentioned?

*Efficiency and performance considerations are not important at this time
