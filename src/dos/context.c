#include "context.h"

signed int leafContextCreate(leafContext * g) {
#if defined(__DJGPP__)
	if(__djgpp_nearptr_enable() == 0) {
		fprintf(stderr,"Cannot enable nearptr\n");
		exit(-1);
	}
#endif /* __DJGPP__ */
	g->vwide = 320; g->vtall = 200;
	videoBuffer = (unsigned char *)malloc(g->vwide*g->vtall);
	if(videoBuffer == NULL) {
		return -1;
	}
	leafCurrentCtx = g;
	return 0;
}

signed int leafContextDestroy(leafContext * g) {
	if(videoBuffer != NULL) {
		free(videoBuffer);
	}
	if(g->name != NULL) {
		free(g->name);
	}
#if defined(__DJGPP__)
	__djgpp_nearptr_disable();
#endif
	return 0;
}
