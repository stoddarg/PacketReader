# Packet Reader
A CCSDS Packet Reader for LunaH
The program reads in a binary configuration file and a binary CCSDS packet file which are created while running the L2 XC FSW tests. A pass or fail is reported back depending on what is found in the packets when compared with the expected output.

- Version 1.0/Initial Commit - The initial commit of the Packet Reader. This version handles the L1 XC FSW tests.

- Version 2.0 - This version of the program is used to test the output packets from the L2 XC FSW.

- Version 2.1 - Slightly updated executables for the program. The file read in is named differently and now we check the number of packets that were read in.