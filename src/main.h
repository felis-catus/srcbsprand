#ifndef MAIN_H
#define MAIN_H

extern BOOL bVerbose;
extern char szGameDir[MAX_PATH * 2];
extern char szModName[256];
extern BOOL bBSPTexturesOnly;
extern BOOL bIgnoreStoryNPCs;
extern BOOL bDumpEntList;
extern BOOL bDumpTexList;
extern BOOL bDumpStrTable;

extern clock_t clockStart;

int Main_Execute( int argc, char *argv[] );
BOOL Main_Init();
void Main_Cleanup();

BOOL Main_IsVerbose();
const char *Main_GetGameDir();
const char *Main_GetModName();
BOOL Main_BSPTexturesOnly();
BOOL Main_IgnoreStoryNPCs();
BOOL Main_ShouldDumpEntList();
BOOL Main_ShouldDumpTexList();
BOOL Main_ShouldDumpStringTable();

#endif
