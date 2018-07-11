# Wireless Communication Protocol
This protocol uses two wireless UART transmitters attached to usart2 on both VEX robots

## Handshake 
To initiate a communication line between the two parties, a handshake
between the two must occur.  A handshake must occur for EVERY transmission
to establish who is talking and who is listening.

**Transmit Request**
- To request to transmit to the other party, the transmitting party must send
   the REQUEST(0xA5) byte to the receiving party
- The transmitting party must wait to receive one of two responses
  - CONFIRM: Other party is ready to receive transmission
  - DENY: Other party is busy; try again later
  - If the transmission is denied, the transmitting party must save
     the command, and attempt transmission at another time
  - If the transmitting party receives any other byte for a handshake
    response, it results in a FAILED state, as this indicates the parties
    are out of sync.

**Receive Handshake**
- A party must always be looking for a transmission to them - utilizing a
  separate thread to constantly check for a REQUEST byte.
- Once a request byte is received, the receiving party must always CONFIRM
   unless the party is undergoing any of the following:
  - Undergoing user control
    - parties are under command of humans first and foremost
  - Currently transmitting
  - Executing a previously received command
- If the party is undergoing any of these conditions, a DENY will be sent in
   response to the handshake
  - It is up to the transmitting party to send at another time (For now).
- Once the receiving party done processing the request, an OVER byte will be
   transmitted to the transmitting party

## Data Packet Structure 
Once a handshake is received and confirmed, the transmitting party
will send this data packet to the receiving party:

### *| Start Byte | Command | PayloadSize | Payload | Checksum |*

- **Start Byte**: Denotes the start of the transmission (0xC3)
- **Command**: Specifies what type of action the receiving party should do (1 byte)
- **PayloadSize**: Specifies in bytes how long the payload is (1 byte)
- **Payload**: Specifies either details for the command, or simply data or sensor
          information from the transmitting party (PayloadSize bytes)
- **Checksum**: An XOR result of the Payload, PayloadSize, Command, and Start Byte
           (1 byte)
