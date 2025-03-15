1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

A remote client usually waits to determine that a particular command's output is fully received. It waits for a specific end-of-message marker RDSH_EOF_CHAR. Which the server adds only after complete output. It keeps reading from the socket until it successfully ascertains the presence of the marker.  Attempts to scan the buffer are made -- the functionality of memchr could come in handy. Protocols might as well have opted for length-prefixed messages in a bid to confirm full transmission.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

Since TCP is a reliable stream protocol that does not preserve message boundaries, the networked shell protocol must define explicit delimiters to mark the beginning and end of a command. This is typically done by appending a null byte to requests and a designated EOF character to responses; without proper framing, the receiver may encounter partial or merged messages, leading to parsing errors.

3. Describe the general differences between stateful and stateless protocols.

Stateful protocol is maintained over multiple requests and therefore the server can keep track of the client's context. In contrast, the server treats every request as independent with the Stateless protocols and therefore requires it to carry all necessary information, so it simplifies server design and scaling but may increase overhead on the client.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

Although UDP is unreliable in terms of delivery, order, and error checking, it is used because of its low overhead and minimal latency. This makes it ideal for real-time applications, such as streaming or online gaming, where speed is crucial and occasional data loss is tolerable. Additionally, UDP supports features like broadcasting and multicasting which are valuable in certain scenarios.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The operating system provides the sockets API to enable network communications. This abstraction offers functions like socket(), bind(), listen(), accept(), connect(), send(), and recv() to simplify the process of establishing and managing network connections regardless of the underlying transport protocol