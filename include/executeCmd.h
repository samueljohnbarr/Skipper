#ifndef _EXECUTECMD_H_
#define _EXECUTECMD_H_

void decodeCommand(int * payload);

void handleLidarInfo(int * payload);

void handleChassisCmd(int * payload);

#endif
