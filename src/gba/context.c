signed int leafContextCreate(leafContext * g) {
	g->vwide = 240; g->vtall = 160;
	videoBuffer = (unsigned char *)malloc(g->vwide*g->vtall);
	if(videoBuffer == NULL) {
		return -1;
	}
	g->rgbPalette = (paletteEntry *)malloc(sizeof(paletteEntry)*256);
	if(g->rgbPalette == NULL) {
		return -2;
	}
#if defined(OPENAL)
	alGetError(); /*Do dummy call to reset error stack*/
#endif /* OPENAL */
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
	return 0;
}
