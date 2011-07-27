#include <stdio.h>
#include <string.h>
#include "ustr.h"
#include "common.h"
#include "tr2ps.h"

char devname[20] = { 'u', 't', 'f', '\0' };
int resolution;
int minx, miny;

struct sjt {
	char *str;
	void (*func)(void *);
};

/*
 * Interpret device control commands, ignoring any we don't recognize. The
 * "x X ..." commands are a device dependent collection generated by troff's
 * \X'...' request.
 *
 */
void devcntl(struct ustr *ustr)
{
	char cmd[50], buf[256], str[MAXTOKENSIZE];
	int n;
	ustr_str(ustr, cmd, 50);
	if (debug)
		fprintf(ferr, "devcntl(cmd=%s)\n", cmd);
	switch (cmd[0]) {
	case 'f':		/* mount font in a position */
		ustr_int(ustr, &n);
		ustr_str(ustr, str, 100);
		mountfont(n, str);
		break;

	case 'i':			/* initialize */
		initialize();
		break;

	case 'p':			/* pause */
		break;

	case 'r':			/* resolution assumed when prepared */
		ustr_int(ustr, &resolution);
		ustr_int(ustr, &minx);
		ustr_int(ustr, &miny);
		break;

	case 's':			/* stop */
	case 't':			/* trailer */
		/* flushtext(); */
		break;

	case 'H':			/* char height */
		ustr_int(ustr, &n);
		t_charht(n);
		break;

	case 'S':			/* slant */
		ustr_int(ustr, &n);
		t_slant(n);
		break;

	case 'T':			/* device name */
		ustr_str(ustr, devname, 16);
		if (debug)
			fprintf(ferr, "devname=%s\n", devname);
		break;

	case 'E':			/* input encoding - not in troff yet */
		ustr_str(ustr, str, 100);
		break;

	case 'X':			/* copy through - from troff */
		if (ustr_str(ustr, str, MAXTOKENSIZE - 1) <= 0)
			error(FATAL, "incomplete devcntl line\n");
		if (ustr_line(ustr, buf, sizeof(buf)) <= 0)
			error(FATAL, "incomplete devcntl line\n");
		ustr_back(ustr);

		if (strncmp(str, "PI", sizeof("PI") - 1) == 0 ||
				strncmp(str, "PictureInclusion", strlen("PictureInclusion")) == 0) {
			picture(ustr, str);
		} else if (strncmp(str, "InlinePicture", strlen("InlinePicture")) == 0) {
			error(FATAL, "InlinePicture not implemented yet.\n");
/*			inlinepic(inp, buf);			*/
		} else if (strncmp(str, "BeginPath", strlen("BeginPath")) == 0) {
			beginpath(buf, FALSE);
		} else if (strncmp(str, "DrawPath", strlen("DrawPath")) == 0) {
			drawpath(buf, FALSE);
		} else if (strncmp(str, "BeginObject", strlen("BeginObject")) == 0) {
			beginpath(buf, TRUE);
		} else if (strncmp(str, "EndObject", strlen("EndObject")) == 0) {
			drawpath(buf, TRUE);
		} else if (strncmp(str, "NewBaseline", strlen("NewBaseline")) == 0) {
			error(FATAL, "NewBaseline not implemented yet.\n");
/*			newbaseline(buf);			*/
		} else if (strncmp(str, "DrawText", strlen("DrawText") - 1) == 0) {
			error(FATAL, "DrawText not implemented yet.\n");
/*			drawtext(buf);				*/
		} else if (strncmp(str, "SetText", strlen("SetText")) == 0) {
			error(FATAL, "SetText not implemented yet.\n");
/*			settext(buf);				*/
		} else if (strncmp(str, "SetColor", strlen("SetColor")) == 0) {
			error(FATAL, "SetColor not implemented yet.\n");
/*			newcolor(buf);				*/
/*			setcolor();					*/
		} else if (strncmp(str, "INFO", strlen("INFO")) == 0) {
			error(FATAL, "INFO not implemented yet.\n");
		} else if (strncmp(str, "PS", strlen("PS")) == 0 ||
				strncmp(str, "PostScript", strlen("PostScript")) == 0) {
			if (pageon()) {
				endstring();
				fprintf(fout, "%s\n", buf);
			}
		} else if (strncmp(str, "ExportPS", strlen("ExportPS")) == 0) {	/* dangerous!! */
			error(FATAL, "ExportPS not implemented yet.\n");
		}
		break;
	}
	ustr_eol(ustr);
	inputlineno++;
}
