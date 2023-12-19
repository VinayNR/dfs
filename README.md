# A Distributed File System Implementation in C++

## The distributed file system consists of the client and the server as primary components. The client is allowed to add a file, and query for a file from the filesystem to begin with. The server consists of 3 components

## Server Components

### Authorization Server
#### The client first establishes a TCP connection to the Auth server to obtain a JWT token. Subsequent requests from the client consist of JWT token issued to it, and the rest of the components validate the authenticity of the token along with the bearer and issuer of the token.

### Metadata Server
#### The metadata server is the primary server of the backend. It is responsible for receiving the client requests and directing the client to reach out to individual data servers for the file operations. The metadata server runs a discovery endpoint for the other data servers to discover the cluster and publish it's data endpoint. The discovery server upon receiving a JOIN request from a data server, adds it to a KeyRing (implemented as a Binary Search Tree) and forms the cluster. The metadata server is now able to route client requests to the data server.

#### Keepalive 
##### Each data server sends a heartbeat on a UDP socket to the discovery server to notify that it is alive and healthy. This is part of the discover protocol which enables the nodes to be part of the cluster. Upon a missing heartbeat in two successive intervals, the metadata server evicts the data server from the KeyRing and issues an eviction policy as a failover mechanism. All blocks of data stored on the node are replicated across other nodes to ensure the replication factor

### Data Server
#### The data server is responsible for storing the files physically on their local filesystem. Each data server is collectively responsible for providing the file namespace for each client that connects to the system. The files are stored in a flat hierarchy identified by a file UUID, which is maintained unique across clients. Fo every block of data (~anywhere between 100KB to 10MB, which also depends on the number of blocks for any given file), there is a primary block node that owns the data. This node is responsible for replicating this block across other nodes using a P2P protocol.

### Client
#### The distributed client is allowed to issue GET and PUT requests to the filesystem. Each authorized client is allowed to issue these requests upon successful authentication with the server. The client first issues a PUT request to the metadata server, which replies back with the locations of blocks for the file. Subsequently, the client parallely issues a PUT request to the data servers to store the copies of each block in their file system. Similarly, for each GET request, the client asks the location of blocks for the file from the metadata server. After this the client is able to independently request the file blocks and assemble itself to receive the file from the respective data servers
