Welcome to HostAPI
==================
The HostAPI library is a simple communication protocol. This protocol runs on 
TCP socket. For now, it's mainly used to run flexidag on CV2x, such as send NN input data and get NN result.

Prerequisites
-------------

1. Board side.
   * CV2x board, e.g. CV2, CV22, or CV25
   * ambalink SDK

2. PC side.
   * Ubuntu 16.04 or newer

Quick Build Instructions
------------------------
## Board Side

Refer to  CnnTestBedUserGuide_v1_8_0.pdf

## PC Side
These steps are to build HostAPI dynamic library.
1. `cd $SRC_PATH/PC_APP/host`
2. `make`

These steps are to build default test APP, UT2.
1. `cd $SRC_PATH/PC_APP/host/app/ut2`
2. `make`

Documents
---------
1. Getting Start with HostAPI: GettingStartedWithHostAPI.pdf
2. HostAPI CV2x Manual: HostAPI_CV2x_Manual.pdf
3. HostAPI PC Manual: HostAPI_PC_Manual.pdf
4. Quick Start Guide - Flexidag Ethernet Data Flow: QuickStartGuide_FlexidagEthernetDataFlow.pdf
