# Chord
Implementation of Chord DHT

To compile, go to Debug folder and run make.

To start a ring, run '''Chord <port>''' where <port> is the port you want to listen on.

To join this ring from another node, run ''' Chord <port> <entry_ip> <entry_port> ''' where <port> is the port that this node should listen on, <entry_ip> is the ip address of a node you want to use to enter the ring, and <entry_port> is the port that the node is listening on.

To run a query, simply run ''' Chord query <entry_ip> <entry_port> ''' where the <entry_ip> and <entry_port> are the details of a node in the ring that you will communicate through.
You can then search for anything by following the prompts.


To Do:
- Still need to add finger table. I worked on this project all week after break, but ran out of time to integrate it. I believe I have the majority of the logic sorted and many of the helper methods written. I could do it if I had another day or two.
 
