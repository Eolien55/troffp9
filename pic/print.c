#include <stdio.h>
#include <math.h>
#include "pic.h"
#include "y.tab.h"

void dotext(obj *, char*);
void dotline(double, double, double, double, int, double, char*);
void dotbox(double, double, double, double, int, double, char*);
void ellipse(double, double, double, double, int, double, char*);
void circle(double, double, double, int, double, char*);
void arc(double, double, double, double, double, double, int, double, char*);
void arrow(double, double, double, double, double, double, double, int, char*);
void line(double, double, double, double, char*);
void box(double, double, double, double, int, double, char*);
void spline(double, double, double, ofloat *, int, double, int, double, char*);
void muline(double, double, double, ofloat *, int, double, char*);
void move(double, double);
void troff(char *);
void dot(void);

int hvflush(void);

void print(void)
{
	obj *p;
	int i, j, k, m;
	int fill, vis, invis;
	double x0, y0, x1, y1, ox, oy, dx, dy, ndx, ndy;

	for (i = 0; i < nobj; i++) {
		p = objlist[i];
		ox = p->o_x;
		oy = p->o_y;
		if (p->o_count >= 1)
			x1 = p->o_val[0];
		if (p->o_count >= 2)
			y1 = p->o_val[1];
		m = p->o_mode;
		fill = p->o_attr & FILLBIT;
		invis = p->o_attr & INVIS;
		vis = !invis;
		switch (p->o_type) {
		case TROFF:
			troff(text[p->o_nt1].t_val);
			break;
		case BOX:
		case BLOCK:
			x0 = ox - x1 / 2;
			y0 = oy - y1 / 2;
			x1 = ox + x1 / 2;
			y1 = oy + y1 / 2;
			if (fill)
				box(x0, y0, x1, y1, 1, p->o_fillval, p->o_bgrgb);
			if (p->o_type == BLOCK)
				;	/* nothing at all */
			else if (invis && !fill)
				;	/* nothing at all */
			else if (p->o_attr & (DOTBIT|DASHBIT))
				dotbox(x0, y0, x1, y1, p->o_attr, p->o_ddval, p->o_fgrgb);
			else if (vis)
				box(x0, y0, x1, y1, 0, 0, p->o_fgrgb);
			move(ox, oy);
			dotext(p, p->o_fgrgb);	/* if there are any text strings */
			if (ishor(m))
				move(isright(m) ? x1 : x0, oy);	/* right side */
			else
				move(ox, isdown(m) ? y0 : y1);	/* bottom */
			break;
		case BLOCKEND:
			break;
		case CIRCLE:
			if (fill)
				circle(ox, oy, x1, 1, p->o_fillval, p->o_bgrgb);
			if (vis)
				circle(ox, oy, x1, 0, 0, p->o_fgrgb);
			move(ox, oy);
			dotext(p, p->o_fgrgb);
			if (ishor(m))
				move(ox + isright(m) ? x1 : -x1, oy);
			else
				move(ox, oy + isup(m) ? x1 : -x1);
			break;
		case ELLIPSE:
			if (fill)
				ellipse(ox, oy, x1, y1, 1, p->o_fillval, p->o_bgrgb);
			if (vis)
				ellipse(ox, oy, x1, y1, 0, 0, p->o_fgrgb);
			move(ox, oy);
			dotext(p, p->o_fgrgb);
			if (ishor(m))
				move(ox + isright(m) ? x1 : -x1, oy);
			else
				move(ox, (oy - isdown(m)) ? y1 : -y1);
			break;
		case ARC:
			if (p->o_attr & HEAD1)
				arrow(x1 - (y1 - oy), y1 + (x1 - ox),
				      x1, y1, p->o_val[4], p->o_val[5], p->o_val[5]/p->o_val[6]/2, p->o_nhead, p->o_fgrgb);
                        if (fill)
				arc(ox, oy, x1, y1, p->o_val[2], p->o_val[3], 1, p->o_fillval, p->o_bgrgb);
                        if (invis && !fill)
                                /* probably wrong when it's cw */
                                move(x1, y1);
                        else if (vis)
				arc(ox, oy, x1, y1, p->o_val[2], p->o_val[3], 0, 0, p->o_fgrgb);
			if (p->o_attr & HEAD2)
				arrow(p->o_val[2] + p->o_val[3] - oy, p->o_val[3] - (p->o_val[2] - ox),
				      p->o_val[2], p->o_val[3], p->o_val[4], p->o_val[5], -p->o_val[5]/p->o_val[6]/2, p->o_nhead, p->o_fgrgb);
			if (p->o_attr & CW_ARC)
				move(x1, y1);	/* because drawn backwards */
			move(ox, oy);
			dotext(p, p->o_fgrgb);
			break;
		case LINE:
		case ARROW:
		case SPLINE:
			if (vis && p->o_attr & HEAD1)
				arrow(ox + p->o_val[5], oy + p->o_val[6], ox, oy, p->o_val[2], p->o_val[3], 0.0, p->o_nhead, p->o_fgrgb);
                        if (invis && !fill)
                                move(x1, y1);
			else if (p->o_type == SPLINE) {
				if (fill)
					spline(ox, oy, p->o_val[4], &p->o_val[5],
						p->o_attr & (DOTBIT|DASHBIT), p->o_ddval, 1, p->o_fillval, p->o_bgrgb);
				if (vis)
					spline(ox, oy, p->o_val[4], &p->o_val[5],
						p->o_attr & (DOTBIT|DASHBIT), p->o_ddval, 0, 0, p->o_fgrgb);
			} else {
				if (fill)
					muline(ox, oy, p->o_val[4], &p->o_val[5], 1, p->o_fillval, p->o_bgrgb);
				dx = ox;
				dy = oy;
				for (k=0, j=5; k < p->o_val[4]; k++, j += 2) {
					ndx = dx + p->o_val[j];
					ndy = dy + p->o_val[j+1];
					if (vis && p->o_attr & (DOTBIT|DASHBIT))
						dotline(dx, dy, ndx, ndy, p->o_attr, p->o_ddval, p->o_fgrgb);
					else if (vis)
						line(dx, dy, ndx, ndy, p->o_fgrgb);
					dx = ndx;
					dy = ndy;
				}
			}
			if (vis && p->o_attr & HEAD2) {
				dx = ox;
				dy = oy;
				for (k = 0, j = 5; k < p->o_val[4] - 1; k++, j += 2) {
					dx += p->o_val[j];
					dy += p->o_val[j+1];
				}
				arrow(dx, dy, x1, y1, p->o_val[2], p->o_val[3], 0.0, p->o_nhead, p->o_fgrgb);
			}
			move((ox + x1)/2, (oy + y1)/2);	/* center */
			dotext(p, p->o_fgrgb);
			break;
		case MOVE:
			move(ox, oy);
			break;
		case TEXT:
			move(ox, oy);
                        if (vis)
				dotext(p, p->o_fgrgb);
			break;
		}
	}
}

void dotline(double x0, double y0, double x1, double y1, int ddtype, double ddval, char *col) /* dotted line */
{
	static double prevval = 0.05;	/* 20 per inch by default */
	int i, numdots;
	double a, b, dx, dy;

	if (ddval == 0)
		ddval = prevval;
	prevval = ddval;
	/* don't save dot/dash value */
	dx = x1 - x0;
	dy = y1 - y0;
	if (ddtype & DOTBIT) {
		numdots = sqrt(dx*dx + dy*dy) / prevval + 0.5;
		if (numdots > 0)
			for (i = 0; i <= numdots; i++) {
				a = (double) i / (double) numdots;
				move(x0 + (a * dx), y0 + (a * dy));
				dot();
			}
	} else if (ddtype & DASHBIT) {
		double d, dashsize, spacesize;
		d = sqrt(dx*dx + dy*dy);
		if (d <= 2 * prevval) {
			line(x0, y0, x1, y1, col);
			return;
		}
		numdots = d / (2 * prevval) + 1;	/* ceiling */
		dashsize = prevval;
		spacesize = (d - numdots * dashsize) / (numdots - 1);
		for (i = 0; i < numdots-1; i++) {
			a = i * (dashsize + spacesize) / d;
			b = a + dashsize / d;
			line(x0 + (a*dx), y0 + (a*dy), x0 + (b*dx), y0 + (b*dy), col);
			a = b;
			b = a + spacesize / d;
			move(x0 + (a*dx), y0 + (a*dy));
		}
		line(x0 + (b * dx), y0 + (b * dy), x1, y1, col);
	}
	prevval = 0.05;
}

void dotbox(double x0, double y0, double x1, double y1, int ddtype, double ddval, char *col)	/* dotted or dashed box */
{
	dotline(x0, y0, x1, y0, ddtype, ddval, col);
	dotline(x1, y0, x1, y1, ddtype, ddval, col);
	dotline(x1, y1, x0, y1, ddtype, ddval, col);
	dotline(x0, y1, x0, y0, ddtype, ddval, col);
}

void dotext(obj *p, char *col)	/* print text strings of p in proper vertical spacing */
{
	int i, nhalf;
	double angle = 0.0, ox, oy, dx, dy;
	void label(char *, int, int);

	nhalf = p->o_nt2 - p->o_nt1 - 1;
	ox = p->o_x;
	oy = p->o_y;

	for (i = p->o_nt1; i < p->o_nt2; i++) {
		if(text[i].t_type & ALIGNED) {
			switch (p->o_type) {
			case ARC:
				break;
			case LINE:
			case ARROW:
			case SPLINE:
				dx = p->o_val[0] - ox;
				dy = p->o_val[1] - oy;
				angle = 90-atan2(dx,dy)*180/M_PI;
				break;
			}
		} else
			angle = 0.0;
		hvflush();
		if (angle != 0.0)
			printf("\\X'rotate %.3f'", angle);
		if (col != NULL)
			printf("\\m[%s]", col);
		label(text[i].t_val, text[i].t_type, nhalf);
		if (col != NULL)
			printf("\\m[]");
		if (angle != 0.0)
			printf("\\X'rotate 0'\\c\n");
		nhalf -= 2;
	}
}
