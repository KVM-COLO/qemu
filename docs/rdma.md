## Introduction
RDMA helps make your migration more deterministic under heavy load because of the significantly lower latency and higher throughput over TCP/IP. This is because the RDMA I/O architecture reduces the number of interrupts and data copies by bypassing the host networking stack. In particular, a TCP-based migration, under certain types of memory-bound workloads, may take a more unpredicatable amount of time to complete the migration if the amount of memory tracked during each live migration iteration round cannot keep pace with the rate of dirty memory produced by the workload.

## BEFORE RUNNING
Experimental: Decide if you want dynamic page registration. For example, if you have an 8GB RAM virtual machine, but only 1GB
is in active use, then enabling this feature will cause all 8GB to be pinned and resident in memory.

## RDMA Protocol Description

Migration with RDMA is separated into two parts:

1. The transmission of the pages using RDMA Write operations

2. Everything else (a control channel is introduced)<br>
   Each message has a header portion and a data portion. The 'type' field has different command values:<br>
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;1. Register request           (dynamic chunk registration)<br>
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;2. QEMU File                  (for sending non-live device state)<br>
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;3. Ready                      (control-channel is available)<br>
   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;4. ...

"Everything else" is transmitted using a formal protocol now, consisting of infiniband SEND messages.

qemu_rdma_exchange_send():

1. Block on the CQ event channel waiting for a READY command from the receiver to tell us that the receiver is *ready* for us to transmit some new bytes.

2. Optionally: if we are expecting a response from the command (that we have not yet transmitted), let's post an RQ work request to receive that data a few moments later.

3. When the READY arrives, librdmacm will unblock us and we immediately post a RQ work request to replace the one we just used up.

4. Now, we can actually post the work request to SEND the requested command type of the header we were asked for.

5. Optionally, if we are expecting a response (as before), we block again and wait for that response using the additional work request we previously posted. (This is used to carry 'Register result' commands back to the sender which hold the rkey need to perform RDMA. Note that the virtual address corresponding to this rkey was already exchanged at the beginning of the connection (described below).

## Migration of VM's ram

At the beginning of the migration, (migration-rdma.c), the sender and the receiver populate the list of RAMBlocks to be registered with each other into a structure. Then, using the aforementioned protocol, they exchange a description of these blocks with each other, to be used later during the iteration of main memory. This description includes a list of all the RAMBlocks, their offsets and lengths, virtual
addresses and possibly includes pre-registered RDMA keys in case dynamic page registration was disabled on the server-side, otherwise not.

Main memory is not migrated with the aforementioned protocol, but is instead migrated with normal RDMA Write operations.

Pages are migrated in "chunks" (hard-coded to 1 Megabyte right now).

When a chunk is full (or a flush() occurs), the memory backed by the chunk is registered with librdmacm is pinned in memory on
both sides using the aforementioned protocol. After pinning, an RDMA Write is generated and transmitted for the entire chunk.

Chunks are also transmitted in batches: This means that we do not request that the hardware signal the completion queue for the completion of *every* chunk. The current batch size is about 64 chunks (corresponding to 64 MB of memory). Only the last chunk in a batch must be signaled. This helps keep everything as asynchronous as possible and helps keep the hardware busy performing RDMA operations.
