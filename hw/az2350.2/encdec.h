#ifndef __ENCDEC_H__
#define __ENCDEC_H__

char *PBKDF2(char *P, char *s, int c, int dkLen);
int sanitize(char input[], char *splitLine[], int maxArg);
void strip(char arg[], char strippedArg[]);

#endif
