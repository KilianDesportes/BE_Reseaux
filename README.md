## 'Network Protocol' Project &mdash; INSA Toulouse.

Logical continuation of the project 'Internet Applications and Socket Programming' : [AIPS_tp](https://github.com/KilianDesportes/AIPS_tp "AIPS_tp")

DELIOT Maxence

DESPORTES Kilian


3 MIC
Group C
2018-2019.


Development of a protocol (MICTCP) implementing a "partially reliable" loss recovery mechanism:
* tolerating a % of "acceptable" losses by the application
* but leading to a better "fluidity" of the application in case of losses


Demonstrate the merits of PCM-TCP over TCP when the network is generating losses.
* but leading to a better "fluidity" of the application in case of losses


4 versions are available: 
* v1: Data transfer phase without guarantee of reliability.
* v2: Guarantee of total reliability via a "Stop and Wait" type loss recovery mechanism.
* v3: Guarantee of partial "static" reliability via a "Stop and Wait" type loss recovery mechanism with partial "pre-wired" reliability, i.e. with a static defined % of admissible losses.
* v4: Connection establishment phase + Guarantee of partial reliability via a "Stop and Wait" type loss recovery mechanism whose % of admissible losses is now negotiated during the connection establishment phase.

Generation of the executable via a Makefile: `$ make`.

Test the protocol via several provided executables, *tsock_text* and *tsock_video*.
