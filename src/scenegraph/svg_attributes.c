/*
 *					GPAC Multimedia Framework
 *
 *			Authors: Cyril Concolato - Jean le Feuvre - Jean-Claude Moissinac
 *				Copyright (c) 2005-200X ENST
 *					All rights reserved
 *
 *  This file is part of GPAC / SVG Loader module
 *
 *  GPAC is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  GPAC is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *   
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *
 */

#include <gpac/base_coding.h>
#include <gpac/scenegraph_svg.h>
#include <gpac/internal/scenegraph_dev.h>

void *svg_create_value_from_attributetype(u8 attribute_type, u8 transform_type);
GF_Err svg_parse_attribute(SVGElement *elt, GF_FieldInfo *info, char *attribute_content, u8 anim_value_type, u8 transform_type);


/*
	    list of supported events in Tiny 1.2 as of 2005/09/10:
		repeat is somehow a special case ...
*/
u32 svg_dom_event_by_name(const char *name)
{
	if (!strcmp(name, "focusin"))	return SVG_DOM_EVT_FOCUSIN;
	if (!strcmp(name, "focusout"))	return SVG_DOM_EVT_FOCUSOUT;
	if (!strcmp(name, "activate"))	return SVG_DOM_EVT_ACTIVATE;
	if (!strcmp(name, "click"))		return SVG_DOM_EVT_CLICK;
	if (!strcmp(name, "mouseup"))	return SVG_DOM_EVT_MOUSEUP;
	if (!strcmp(name, "mousedown")) return SVG_DOM_EVT_MOUSEDOWN;
	if (!strcmp(name, "mouseover")) return SVG_DOM_EVT_MOUSEOVER;
	if (!strcmp(name, "mouseout"))	return SVG_DOM_EVT_MOUSEOUT;
	if (!strcmp(name, "mousemove")) return SVG_DOM_EVT_MOUSEMOVE;
	if (!strcmp(name, "load"))		return SVG_DOM_EVT_LOAD;
	if (!strcmp(name, "unload"))	return SVG_DOM_EVT_UNLOAD;
	if (!strcmp(name, "error"))		return SVG_DOM_EVT_ERROR;
	if (!strcmp(name, "resize"))	return SVG_DOM_EVT_RESIZE;
	if (!strcmp(name, "scroll"))	return SVG_DOM_EVT_SCROLL;
	if (!strcmp(name, "zoom"))		return SVG_DOM_EVT_ZOOM;
	if (!strcmp(name, "begin"))		return SVG_DOM_EVT_BEGIN;
	if (!strcmp(name, "end"))		return SVG_DOM_EVT_END;
	if (!strncmp(name, "repeat", 6))	return SVG_DOM_EVT_REPEAT;
	if (!strcmp(name, "keyup"))		return SVG_DOM_EVT_KEYUP;
	if (!strcmp(name, "keydown"))	return SVG_DOM_EVT_KEYDOWN;
	if (!strcmp(name, "keypress"))	return SVG_DOM_EVT_KEYPRESS;
	return SVG_DOM_EVT_UNKNOWN;
}

const char *gf_dom_event_name(u32 type)
{
	switch (type) {
	case SVG_DOM_EVT_FOCUSIN: return "focusin";
	case SVG_DOM_EVT_FOCUSOUT: return "focusout";
	case SVG_DOM_EVT_ACTIVATE: return "activate";
	case SVG_DOM_EVT_CLICK: return "click";
	case SVG_DOM_EVT_MOUSEUP: return "mouseup";
	case SVG_DOM_EVT_MOUSEDOWN: return "mousedown";
	case SVG_DOM_EVT_MOUSEOVER: return "mouseover";
	case SVG_DOM_EVT_MOUSEOUT: return "mouseout";
	case SVG_DOM_EVT_MOUSEMOVE: return "mousemove";
	case SVG_DOM_EVT_LOAD: return "load";
	case SVG_DOM_EVT_UNLOAD: return "unload";
	case SVG_DOM_EVT_ERROR: return "error";
	case SVG_DOM_EVT_RESIZE: return "resize";
	case SVG_DOM_EVT_SCROLL: return "scroll";
	case SVG_DOM_EVT_ZOOM: return "zoom";
	case SVG_DOM_EVT_BEGIN: return "begin";
	case SVG_DOM_EVT_END: return "end";
	case SVG_DOM_EVT_REPEAT: return "repeat";
	case SVG_DOM_EVT_KEYUP: return "keyup";
	case SVG_DOM_EVT_KEYDOWN: return "keydown";
	case SVG_DOM_EVT_KEYPRESS: return "keypress";
	default: return "unknwon";
	}
}

/* Basic SVG datatype parsing functions */
static const struct predef_col { const char *name; u8 r; u8 g; u8 b; } predefined_colors[] = 
{
	{"aliceblue",240, 248, 255},
	{"antiquewhite",250, 235, 215},
	{"aqua", 0, 255, 255},
	{"aquamarine",127, 255, 212},
	{"azure",240, 255, 255},
	{"beige",245, 245, 220},
	{"bisque",255, 228, 196},
	{"black", 0, 0, 0},
	{"blanchedalmond",255, 235, 205},
	{"blue", 0, 0, 255},
	{"blueviolet",138, 43, 226},
	{"brown",165, 42, 42},
	{"burlywood",222, 184, 135},
	{"cadetblue", 95, 158, 160},
	{"chartreuse",127, 255, 0},
	{"chocolate",210, 105, 30},
	{"coral",255, 127, 80},
	{"lightpink",255, 182, 193},
	{"lightsalmon",255, 160, 122},
	{"lightseagreen", 32, 178, 170},
	{"lightskyblue",135, 206, 250},
	{"lightslategray",119, 136, 153},
	{"lightslategrey",119, 136, 153},
	{"lightsteelblue",176, 196, 222},
	{"lightyellow",255, 255, 224},
	{"lime", 0, 255, 0},
	{"limegreen", 50, 205, 50},
	{"linen",250, 240, 230},
	{"magenta",255, 0, 255},
	{"maroon",128, 0, 0},
	{"mediumaquamarine",102, 205, 170},
	{"mediumblue", 0, 0, 205},
	{"mediumorchid",186, 85, 211},
	{"cornflowerblue",100, 149, 237},
	{"cornsilk",255, 248, 220},
	{"crimson",220, 20, 60},
	{"cyan", 0, 255, 255},
	{"darkblue", 0, 0, 139},
	{"darkcyan", 0, 139, 139},
	{"darkgoldenrod",184, 134, 11},
	{"darkgray",169, 169, 169},
	{"darkgreen", 0, 100, 0},
	{"darkgrey",169, 169, 169},
	{"darkkhaki",189, 183, 107},
	{"darkmagenta",139, 0, 139},
	{"darkolivegreen", 85, 107, 47},
	{"darkorange",255, 140, 0},
	{"darkorchid",153, 50, 204},
	{"darkred",139, 0, 0},
	{"darksalmon",233, 150, 122},
	{"darkseagreen",143, 188, 143},
	{"darkslateblue", 72, 61, 139},
	{"darkslategray", 47, 79, 79},
	{"darkslategrey", 47, 79, 79},
	{"darkturquoise", 0, 206, 209},
	{"darkviolet",148, 0, 211},
	{"deeppink",255, 20, 147},
	{"deepskyblue", 0, 191, 255},
	{"dimgray",105, 105, 105},
	{"dimgrey",105, 105, 105},
	{"dodgerblue", 30, 144, 255},
	{"firebrick",178, 34, 34},
	{"floralwhite",255, 250, 240},
	{"forestgreen", 34, 139, 34},
	{"fuchsia",255, 0, 255},
	{"gainsboro",220, 220, 220},
	{"ghostwhite",248, 248, 255},
	{"gold",255, 215, 0},
	{"goldenrod",218, 165, 32},
	{"gray",128, 128, 128},
	{"grey",128, 128, 128},
	{"green", 0, 128, 0},
	{"greenyellow",173, 255, 47},
	{"honeydew",240, 255, 240},
	{"hotpink",255, 105, 180},
	{"indianred",205, 92, 92},
	{"indigo", 75, 0, 130},
	{"ivory",255, 255, 240},
	{"khaki",240, 230, 140},
	{"lavender",230, 230, 25},
	{"lavenderblush",255, 240, 245},
	{"mediumpurple",147, 112, 219},
	{"mediumseagreen", 60, 179, 113},
	{"mediumslateblue",123, 104, 238},
	{"mediumspringgreen", 0, 250, 154},
	{"mediumturquoise", 72, 209, 204},
	{"mediumvioletred",199, 21, 133},
	{"midnightblue", 25, 25, 112},
	{"mintcream",245, 255, 250},
	{"mistyrose",255, 228, 225},
	{"moccasin",255, 228, 181},
	{"navajowhite",255, 222, 173},
	{"navy", 0, 0, 128},
	{"oldlace",253, 245, 230},
	{"olive",128, 128, 0},
	{"olivedrab",107, 142, 35},
	{"orange",255, 165, 0},
	{"orangered",255, 69, 0},
	{"orchid",218, 112, 214},
	{"palegoldenrod",238, 232, 170},
	{"palegreen",152, 251, 152},
	{"paleturquoise",175, 238, 238},
	{"palevioletred",219, 112, 147},
	{"papayawhip",255, 239, 213},
	{"peachpuff",255, 218, 185},
	{"peru",205, 133, 63},
	{"pink",255, 192, 203},
	{"plum",221, 160, 221},
	{"powderblue",176, 224, 230},
	{"purple",128, 0, 128},
	{"red",255, 0, 0},
	{"rosybrown",188, 143, 143},
	{"royalblue", 65, 105, 225},
	{"saddlebrown",139, 69, 19},
	{"salmon",250, 128, 114},
	{"sandybrown",244, 164, 96},
	{"seagreen", 46, 139, 87},
	{"seashell",255, 245, 238},
	{"sienna",160, 82, 45},
	{"silver",192, 192, 192},
	{"skyblue",135, 206, 235},
	{"slateblue",106, 90, 205},
	{"slategray",112, 128, 144},
	{"slategrey",112, 128, 144},
	{"snow",255, 250, 250},
	{"springgreen", 0, 255, 127},
	{"steelblue", 70, 130, 180},
	{"tan",210, 180, 140},
	{"teal", 0, 128, 128},
	{"lawngreen",124, 252, 0},
	{"lemonchiffon",255, 250, 205},
	{"lightblue",173, 216, 230},
	{"lightcoral",240, 128, 128},
	{"lightcyan",224, 255, 255},
	{"lightgoldenrodyellow",250, 250, 210},
	{"lightgray",211, 211, 211},
	{"lightgreen",144, 238, 144},
	{"lightgrey",211, 211, 211},
	{"thistle",216, 191, 216},
	{"tomato",255, 99, 71},
	{"turquoise", 64, 224, 208},
	{"violet",238, 130, 238},
	{"wheat",245, 222, 179},
	{"white",255, 255, 255},
	{"whitesmoke",245, 245, 245},
	{"yellow",255, 255, 0},
	{"yellowgreen",154, 205, 50}

};

/* parses an color from a named color HTML or CSS 2 */
static void svg_parse_named_color(SVG_Color *col, char *attribute_content)
{
	u32 i, count;
	count = sizeof(predefined_colors) / sizeof(struct predef_col);
	for (i=0; i<count; i++) {
		if (!strcmp(attribute_content, predefined_colors[i].name)) {
			col->red = INT2FIX(predefined_colors[i].r) / 255;
			col->green = INT2FIX(predefined_colors[i].g) / 255;
			col->blue = INT2FIX(predefined_colors[i].b) / 255;
			return;
		}
	}
}

/* Reads an SVG Color 
   either #RRGGBB, #RGB, rgb(r,g,b) in [0,255] , colorname, or 'r g b' in [0,1]
   ignores any space, comma, semi-column before and any space after
   TODO: 
	transform the char into char and duplicate the input, instead of modifying it
*/
void svg_parse_color(SVG_Color *col, char *attribute_content)
{
	char *str = attribute_content;
	while (str[strlen(attribute_content)-1] == ' ') str[strlen(attribute_content)-1] = 0;
	while (*str != 0 && (*str == ' ' || *str == ',' || *str == ';')) str++;

	if (!strcmp(str, "currentColor")) {
		col->type = SVG_COLOR_CURRENTCOLOR;
		return;
	} else if (!strcmp(str, "inherit")) {
		col->type = SVG_COLOR_INHERIT;
		return;
	} else if (str[0]=='#') {
		u32 val;
		sscanf(str+1, "%x", &val);
		if (strlen(str) == 7) {
			col->red = INT2FIX((val>>16) & 0xFF) / 255;
			col->green = INT2FIX((val>>8) & 0xFF) / 255;
			col->blue = INT2FIX(val & 0xFF) / 255;
		} else {
			col->red = INT2FIX((val>>8) & 0xF) / 15;
			col->green = INT2FIX((val>>4) & 0xF) / 15;
			col->blue = INT2FIX(val & 0xF) / 15;
		}
	} else if (strstr(str, "rgb(") || strstr(str, "RGB(")) {
		Float _val;
		u8 is_percentage= 0;
		if (strstr(str, "%")) is_percentage = 1;
		str = strstr(str, "(");
		str++;
		sscanf(str, "%f", &_val); col->red = FLT2FIX(_val);
		str = strstr(str, ",");
		str++;
		sscanf(str, "%f", &_val); col->green = FLT2FIX(_val);
		str = strstr(str, ",");
		str++;
		sscanf(str, "%f", &_val); col->blue = FLT2FIX(_val);
		if (is_percentage) {
			col->red /= 100;
			col->green /= 100;
			col->blue /= 100;
		} else {
			col->red /= 255;
			col->green /= 255;
			col->blue /= 255;
		}
	} else if ((str[0] >= 'a' && str[0] <= 'z')
		|| (str[0] >= 'A' && str[0] <= 'Z')) {
		svg_parse_named_color(col, str);
	} else {
		Float _r, _g, _b;
		sscanf(str, "%f %f %f", &_r, &_g, &_b);
		col->red = FLT2FIX(_r);
		col->green = FLT2FIX(_g);
		col->blue = FLT2FIX(_b);
	}
	col->type = SVG_COLOR_RGBCOLOR;

}

/* 
	Reads a float in scientific notation 
		trims any space, comma, semi-column before or after
		reads an optional + or -
		then reads a digit between 0 and 9
		optionally followed by an '.' and digits between 0 and 9
		optionally followed by e or E and digits between 0 and 9
	Returns the number of char read in d 
*/
static u32 svg_parse_float(char *d, Fixed *f, Bool is_angle) 
{
	Float _val;
	u32 i = 0;
	while (d[i] != 0 && (d[i] == ' ' || d[i] == ',' || d[i] == ';')) i++;
	if (!d[i]) goto end;
	if (d[i] == '+' || d[i] == '-') i++;
	while (d[i] >= '0' && d[i] <= '9' && d[i] != 0) i++;
	if (!d[i]) goto end;
	if (d[i] == '.') {
		i++;
		while (d[i] >= '0' && d[i] <= '9' && d[i] != 0) i++;
		if (!d[i]) goto end;
	}
	if (d[i] == 'e' || d[i] == 'E') {
		i++;
		if (d[i] == '+' || d[i] == '-') i++;
		while (d[i] >= '0' && d[i] <= '9' && d[i] != 0) i++;
	}
end:
	sscanf(d,"%f", &_val);
	if (is_angle) {
		_val/=180;
		(*f) = gf_mulfix(FLT2FIX(_val), GF_PI);
	} else {
		(*f) = FLT2FIX(_val);
	}
	while (d[i] != 0 && (d[i] == ' ' || d[i] == ',' || d[i] == ';')) i++;
	return i;
}

/*
   Parse an Offset Value, i.e +/- Clock Value
*/
static void svg_parse_clock_value(char *d, Double *clock_value) 
{
	char *tmp;
	s32 sign = 1;
	if (d[0] == '+') d++;
	if (d[0] == '-') { sign = -1; d++; }
	while (*d == ' ') d++;

	if ((tmp = strchr(d, ':'))) {
		/* Full or Partial Clock value */
		tmp++;
		if ((tmp = strchr(tmp, ':'))) {
			/* Full Clock value : hh:mm:ss(.frac) */
			u32 hours;
			u32 minutes;
			Float seconds;
			sscanf(d, "%d:%d:%f", &hours, &minutes, &seconds);
			*clock_value = hours*3600 + minutes*60 + seconds;
		} else {
			/* Partial Clock value : mm:ss(.frac) */
			s32 minutes;
			Float seconds;
			sscanf(d, "%d:%f", &minutes, &seconds);
			*clock_value = minutes*60 + seconds;
		}
	} else if ((tmp = strstr(d, "h"))) {
		Float f;
		sscanf(d, "%fh", &f);
		*clock_value = 3600*f;
	} else if (strstr(d, "min")) {
		Float f;
		sscanf(d, "%fmin", &f);
		*clock_value = 60*f;
	} else if ((tmp = strstr(d, "ms"))) {
		Float f;
		sscanf(d, "%fms", &f);
		*clock_value = f/1000;
	} else if (strchr(d, 's')) {
		Float f;
		sscanf(d, "%fs", &f);
		*clock_value = f;
	} else {
		*clock_value = atof(d);
	}
	*clock_value *= sign;
}
/* Parses one SVG time value:
	  'indefinite', 
	  'name.begin', 'name.end', 
	  wallclock,
	  accessKey,
	  events, 
	  clock value.
 */
void smil_parse_time(SVGElement *e, SMIL_Time *v, char *d) 
{
	u32 len;
	char *tmp;
	
	/* Offset Values */
	if ((d[0] >= '0' && d[0] <= '9') || d[0] == '+' || d[0] == '-'){
		v->type = SMIL_TIME_CLOCK;
		svg_parse_clock_value(d, &(v->clock));
		return;
	} 
	
	/* Indefinite Values */
	else if (!strcmp(d, "indefinite")) {
		v->type = SMIL_TIME_INDEFINITE;
		return;
	} 

	/* Wallclock Values */
	else if ((tmp = strstr(d, "wallclock("))) {
		u32 year, month, day;
		u32 hours, minutes;
		u32 nhours, nminutes;
		Float seconds;
		char *tmp1, *tmp2;

		v->type = SMIL_TIME_WALLCLOCK;
		tmp += 10;
		if (tmp1 = strchr(tmp, 'T')) {
			/* From tmp to wallStartTime, we parse a date */
			sscanf(tmp, "%d-%d-%dT", &year, &month, &day);
			tmp1++;
			tmp = tmp1;
		} 	
		if (tmp1 = strchr(tmp, ':')) {
			if (tmp2 = strchr(tmp1, ':')) {
				/* HHMMSS */
				sscanf(tmp, "%d:%d:%f", &hours, &minutes, &seconds);		
			} else {
				/* HHMM */
				sscanf(tmp, "%d:%d", &hours, &minutes);		
			}
		}
		if (strchr(tmp, 'Z')) {
			return;
		} else {
			if (tmp1 = strchr(tmp, '+')) {
				sscanf(tmp1, "%d:%d", &nhours, &nminutes);		
			} else if (tmp1 = strchr(tmp, '-')) {
				sscanf(tmp1, "%d:%d", &nhours, &nminutes);		
			}
		}
		return;
	} 

	/* AccessKey Values */
	else if ((tmp = strstr(d, "accessKey("))) {
		v->type = SMIL_TIME_EVENT;
		v->event = 17;
		tmp+=10;
		v->parameter = *tmp;
		tmp = strchr(d, ')');
		tmp++;
		while (*tmp == ' ') tmp++;
		if (*tmp != 0) svg_parse_clock_value(tmp, &(v->clock));
		return;
	} 

	else {
		char token[500];
		v->type = SMIL_TIME_EVENT;
		if ((tmp = strchr(d, '+')) || (tmp = strchr(d, '-'))) {
			len = tmp - d;
			while (d[len-1] == ' ' && len > 0) len--;
			memcpy(token, d, len);
			token[len] = 0;
			svg_parse_clock_value(tmp, &(v->clock));
		} else {
			strcpy(token, d);
			len = strlen(d);
		}
		if (!strchr(token, '.')) {
			/* animation event name only */
			v->event = svg_dom_event_by_name(token);
			v->element = (GF_Node *)e;
		} else {
			u32 i;
			for (i = 0; i < len; i++) {
				if (token[i] == '\\' && (i+1 < len) && token[i+1] == '.') i++;
				else if (token[i] == '.') {
					/* 0 -> i: element_name
					   i -> len - 1: animation event name */
					GF_SAFEALLOC(v->element_id, i+1);
					memcpy(v->element_id, token, i);
//					gf_list_add(parser->unresolved_timing_elements, v);
					v->event = svg_dom_event_by_name(token+i+1);
				}
			}
		}

		if (v->event == 3) { //repeat
			tmp = strchr(token, '(');
			if (tmp) {
				tmp++;
				v->parameter = atoi(tmp);
			} else {
				v->parameter = 1;
			}
		}
	}
}

/* Parses an SVG transform attribute and collapses all in the given matrix */
void svg_parse_transform(SVG_Matrix *mat, char *attribute_content) 
{
	SVG_Matrix tmp;

	char *str;
	u32 i;
	
	gf_mx2d_init(*mat);
	
	str = attribute_content;
	i = 0;
	while (str[i] != 0) {
		while (str[i] == ' ') i++;
		if (strstr(str+i, "scale")==str+i) {
			Fixed sx, sy;
			i += 5;
			while(str[i] != 0 && str[i] == ' ') i++;
			if (str[i] == '(') {
				i++;
				i+=svg_parse_float(&(str[i]), &sx, 0);
				if (str[i] == ')') {
					sy = sx;
				} else {
					i+=svg_parse_float(&(str[i]), &sy, 0);
				}
				i++;
			}
			gf_mx2d_init(tmp);
			gf_mx2d_add_scale(&tmp, sx, sy);
			gf_mx2d_add_matrix(&tmp, mat);
			gf_mx2d_copy(*mat, tmp);
			while(str[i] != 0 && str[i] == ' ') i++;
		} else if (strstr(str+i, "translate")==str+i) {
			Fixed tx, ty;
			i += 9;
			while(str[i] != 0 && str[i] == ' ') i++;
			if (str[i] == '(') {
				i++;
				i+=svg_parse_float(&(str[i]), &tx, 0);
				if (str[i] == ')') {
					ty = 0;
				} else {
					i+=svg_parse_float(&(str[i]), &ty, 0);
				}
				i++;
			}
			gf_mx2d_init(tmp);
			gf_mx2d_add_translation(&tmp, tx, ty);
			gf_mx2d_add_matrix(&tmp, mat);
			gf_mx2d_copy(*mat, tmp);
			while(str[i] != 0 && str[i] == ' ') i++;
		} else if (strstr(str+i, "rotate")==str+i) {
			Fixed angle, cx, cy;
			i += 6;
			while(str[i] != 0 && str[i] == ' ') i++;
			if (str[i] == '(') {
				i++;
				i+=svg_parse_float(&(str[i]), &angle, 1);
				if (str[i] == ')') {
					cx = cy = 0;
				} else {
					i+=svg_parse_float(&(str[i]), &cx, 0);
					i+=svg_parse_float(&(str[i]), &cy, 0);
				}
				i++;
			}
			gf_mx2d_init(tmp);
			gf_mx2d_add_rotation(&tmp, cx, cy, angle);
			gf_mx2d_add_matrix(&tmp, mat);
			gf_mx2d_copy(*mat, tmp);
			while(str[i] != 0 && str[i] == ' ') i++;
		} else if (strstr(str+i, "skewX")==str+i) {
			Fixed angle;
			i += 5;
			while(str[i] != 0 && str[i] == ' ') i++;
			if (str[i] == '(') {
				i++;
				i+=svg_parse_float(&(str[i]), &angle, 1);
				i++;
			}
			gf_mx2d_init(tmp);
			gf_mx2d_add_skew_x(&tmp, angle);
			gf_mx2d_add_matrix(&tmp, mat);
			gf_mx2d_copy(*mat, tmp);
			while(str[i] != 0 && str[i] == ' ') i++;
		} else if (strstr(str+i, "skewY")==str+i) {
			Fixed angle;
			i += 5;
			while(str[i] != 0 && str[i] == ' ') i++;
			if (str[i] == '(') {
				i++;
				i+=svg_parse_float(&(str[i]), &angle, 1);
				i++;
			}
			gf_mx2d_init(tmp);
			gf_mx2d_add_skew_y(&tmp, angle);
			gf_mx2d_add_matrix(&tmp, mat);
			gf_mx2d_copy(*mat, tmp);
			while(str[i] != 0 && str[i] == ' ') i++;
		} else if (strstr(str+i, "matrix")==str+i) {
			i+=6;
			while(str[i] != 0 && str[i] == ' ') i++;
			if (str[i] == '(') {
				i++;
				i+=svg_parse_float(&(str[i]), &(tmp.m[0]), 0);
				i+=svg_parse_float(&(str[i]), &(tmp.m[3]), 0);
				i+=svg_parse_float(&(str[i]), &(tmp.m[1]), 0);
				i+=svg_parse_float(&(str[i]), &(tmp.m[4]), 0);
				i+=svg_parse_float(&(str[i]), &(tmp.m[2]), 0);
				i+=svg_parse_float(&(str[i]), &(tmp.m[5]), 0);
				i++;
			}
			gf_mx2d_add_matrix(&tmp, mat);
			gf_mx2d_copy(*mat, tmp);
			while(str[i] != 0 && str[i] == ' ') i++;
		}
	}
}

/* TODO: Change the function to handle elliptical arcs, requires changing data structure */
void svg_parse_path(SVG_PathData *d_attribute, char *attribute_content) 
{
	GF_List *d_commands = d_attribute->commands;
	GF_List *d_points = d_attribute->points;
	char *d = attribute_content;
	Bool first_command = 1;
	if (strlen(d)) {
		Bool pt0_inited, subpath_closed;
		SVG_Point *pt, pt0, cur_pt;
		u8 *command;
		u32 i, k;
		char c, prev_c = 'M';
		i = 0;
		cur_pt.x = cur_pt.y = 0;
		pt0_inited = 0;
		subpath_closed = 0;
		while(1) {
			while ( (d[i]==' ') || (d[i] =='\t') ) i++;			
			c = d[i];
			if (c == 0) break;
next_command:
			switch (c) {
			case 'm':
				if (first_command) cur_pt.x = cur_pt.y = 0;
			case 'M':
			case 'L':
			case 'l':
				i++;
				GF_SAFEALLOC(command, sizeof(u8))
				gf_list_add(d_commands, command);
				if (c == 'M' || c == 'm') *command = 0;
				if (c == 'L' || c == 'l') {
					*command = 1;
					subpath_closed = 0;	
				}
				
				GF_SAFEALLOC(pt, sizeof(SVG_Point))
				gf_list_add(d_points, pt);
				i += svg_parse_float(&(d[i]), &(pt->x), 0);
				i += svg_parse_float(&(d[i]), &(pt->y), 0);				
				if (c == 'm') {
					if (!subpath_closed) {
						pt->x += cur_pt.x;
						pt->y += cur_pt.y;
					} else {
						pt->x += pt0.x;
						pt->y += pt0.y;
					}
				} else if (c == 'l') {
					pt->x += cur_pt.x;
					pt->y += cur_pt.y;
				}
				cur_pt.x = pt->x;
				cur_pt.y = pt->y;
				if (!pt0_inited) {
					pt0.x = cur_pt.x;
					pt0.y = cur_pt.y;
					pt0_inited = 1;
				}
				prev_c = c;
				break;
			case 'H':
			case 'h':
				i++;				
				GF_SAFEALLOC(command, sizeof(u8))
				gf_list_add(d_commands, command);
				*command = 1;
				subpath_closed = 0;

				GF_SAFEALLOC(pt, sizeof(SVG_Point))
				gf_list_add(d_points, pt);
				i += svg_parse_float(&(d[i]), &(pt->x), 0);
				if (c == 'h') {
					pt->x += cur_pt.x;
				}
				pt->y = cur_pt.y;
				cur_pt.x = pt->x;
				prev_c = c;
				break;
			case 'V':
			case 'v':
				i++;				
				GF_SAFEALLOC(command, sizeof(u8))
				gf_list_add(d_commands, command);
				*command = 1;
				subpath_closed = 0;

				GF_SAFEALLOC(pt, sizeof(SVG_Point))
				gf_list_add(d_points, pt);
				i += svg_parse_float(&(d[i]), &(pt->y), 0);
				if (c == 'v') {
					pt->y += cur_pt.y;
				}
				pt->x = cur_pt.x;
				cur_pt.y = pt->y;
				prev_c = c;
				break;
			case 'C':
			case 'c':
				i++;				
				GF_SAFEALLOC(command, sizeof(u8))
				gf_list_add(d_commands, command);
				*command = 2;
				subpath_closed = 0;
				
				for (k=0; k<3; k++) {
					GF_SAFEALLOC(pt, sizeof(SVG_Point))
					gf_list_add(d_points, pt);
					i += svg_parse_float(&(d[i]), &(pt->x), 0);
					i += svg_parse_float(&(d[i]), &(pt->y), 0);				
					if (c == 'c') {
						pt->x += cur_pt.x;
						pt->y += cur_pt.y;
					}
				}				
				cur_pt.x = pt->x;
				cur_pt.y = pt->y;
				prev_c = c;
				break;
			case 'S':
			case 's':
				i++;				
				GF_SAFEALLOC(command, sizeof(u8))
				gf_list_add(d_commands, command);
				*command = 3;
				subpath_closed = 0;
				
				for (k=0; k<2; k++) {
					GF_SAFEALLOC(pt, sizeof(SVG_Point))
					gf_list_add(d_points, pt);
					i += svg_parse_float(&(d[i]), &(pt->x), 0);
					i += svg_parse_float(&(d[i]), &(pt->y), 0);				
					if (c == 's') {
						pt->x += cur_pt.x;
						pt->y += cur_pt.y;
					}
				}				
				cur_pt.x = pt->x;
				cur_pt.y = pt->y;
				prev_c = c;
				break;
			case 'Q':
			case 'q':
				i++;				
				GF_SAFEALLOC(command, sizeof(u8))
				gf_list_add(d_commands, command);
				*command = 4;
				subpath_closed = 0;
				
				for (k=0; k<2; k++) {
					GF_SAFEALLOC(pt, sizeof(SVG_Point))
					gf_list_add(d_points, pt);
					i += svg_parse_float(&(d[i]), &(pt->x), 0);
					i += svg_parse_float(&(d[i]), &(pt->y), 0);				
					if (c == 'q') {
						pt->x += cur_pt.x;
						pt->y += cur_pt.y;
					}
				}				
				cur_pt.x = pt->x;
				cur_pt.y = pt->y;
				prev_c = c;
				break;
			case 'T':
			case 't':
				i++;				
				GF_SAFEALLOC(command, sizeof(u8))
				gf_list_add(d_commands, command);
				*command = 5;
				subpath_closed = 0;
				
				GF_SAFEALLOC(pt, sizeof(SVG_Point))
				gf_list_add(d_points, pt);
				i += svg_parse_float(&(d[i]), &(pt->x), 0);
				i += svg_parse_float(&(d[i]), &(pt->y), 0);				
				if (c == 't') {
					pt->x += cur_pt.x;
					pt->y += cur_pt.y;
				}
				cur_pt.x = pt->x;
				cur_pt.y = pt->y;
				prev_c = c;
				break;
			case 'A':
			case 'a':
				{
					Fixed tmp;
					i++;				
					GF_SAFEALLOC(command, sizeof(u8))
					gf_list_add(d_commands, command);
					*command = 7;
					subpath_closed = 0;
	
					GF_SAFEALLOC(pt, sizeof(SVG_Point))
					gf_list_add(d_points, pt);
					i += svg_parse_float(&(d[i]), &(pt->x), 0);	
					i += svg_parse_float(&(d[i]), &(pt->y), 0);				

					i += svg_parse_float(&(d[i]), &(tmp), 0);	
					i += svg_parse_float(&(d[i]), &(tmp), 0);				
					i += svg_parse_float(&(d[i]), &(tmp), 0);	
					
					GF_SAFEALLOC(pt, sizeof(SVG_Point))
					gf_list_add(d_points, pt);
					i += svg_parse_float(&(d[i]), &(pt->x), 0);	
					i += svg_parse_float(&(d[i]), &(pt->y), 0);				
					if (c == 'a') {
						pt->x += cur_pt.x;
						pt->y += cur_pt.y;
					}
					cur_pt.x = pt->x;
					cur_pt.y = pt->y;
				}
				prev_c = c;
				break;
			case 'Z':
			case 'z':
				i++;				
				GF_SAFEALLOC(command, sizeof(u8))
				gf_list_add(d_commands, command);
				*command = 6;
				subpath_closed = 1;
				prev_c = c;
				break;
			default:
				i--;
				switch (prev_c) {
				case 'M':
					c = 'L';
					break;
				case 'm':
					c = 'l';
					break;
				default:
					c = prev_c;
				}
				goto next_command;
			}
			first_command = 0;
		}
	}
}

/* Parses a paint attribute: none, inherit or color */
void svg_parse_paint(SVG_Paint *paint, char *attribute_content)
{
	if (!strcmp(attribute_content, "none")) {
		paint->type = SVG_PAINT_NONE;
	} else if (!strcmp(attribute_content, "inherit")) {
		paint->type = SVG_PAINT_INHERIT;
	} else if (!strnicmp(attribute_content, "url(", 4) ) {
		paint->type = SVG_PAINT_URI;
		paint->uri = strdup(attribute_content+4);
		paint->uri[strlen(paint->uri)-1] = 0;
	} else {
		paint->type = SVG_PAINT_COLOR;
		svg_parse_color(paint->color, attribute_content);
	}
}

void svg_convert_length_unit_to_user_unit(SVG_Length *length)
{
	// Assuming the environment is 90dpi
	switch (length->type) {
	case SVG_LENGTH_PERCENTAGE:
		break;
	case SVG_LENGTH_EMS:
		break;
	case SVG_LENGTH_EXS:
		break;
	case SVG_LENGTH_NUMBER:
		break;
	case SVG_LENGTH_PX:
		length->type = SVG_LENGTH_NUMBER;
		break;
	case SVG_LENGTH_CM:
		length->type = SVG_LENGTH_NUMBER;
		length->number = gf_mulfix(length->number, FLT2FIX(35.43307f));
		break;
	case SVG_LENGTH_MM:
		length->type = SVG_LENGTH_NUMBER;
		length->number = gf_mulfix(length->number, FLT2FIX(3.543307f));
		break;
	case SVG_LENGTH_IN:
		length->type = SVG_LENGTH_NUMBER;
		length->number *= 90;
		break;
	case SVG_LENGTH_PT:
		length->type = SVG_LENGTH_NUMBER;
		length->number *= 5; length->number /= 4;
		break;
	case SVG_LENGTH_PC:
		length->type = SVG_LENGTH_NUMBER;
		length->number *= 15;
		break;
	case SVG_LENGTH_INHERIT:
		break;
	}
}

u32 svg_parse_length(SVG_Length *length, char *value_string)
{
	char *unit = NULL;
	u32 len = 0;
	if (!strcmp(value_string, "inherit")) {
		length->type = SVG_LENGTH_INHERIT;
		return 7;
	} else if (unit = strstr(value_string, "%")) {
		length->type = SVG_LENGTH_PERCENTAGE;
	} else if (unit = strstr(value_string, "em")) {
		length->type = SVG_LENGTH_EMS;
	} else if (unit = strstr(value_string, "ex")) {
		length->type = SVG_LENGTH_EXS;
	} else if (unit = strstr(value_string, "px")) {
		length->type = SVG_LENGTH_PX;
	} else if (unit = strstr(value_string, "cm")) {
		length->type = SVG_LENGTH_CM;
	} else if (unit = strstr(value_string, "mm")) {
		length->type = SVG_LENGTH_MM;
	} else if (unit = strstr(value_string, "in")) {
		length->type = SVG_LENGTH_IN;
	} else if (unit = strstr(value_string, "pt")) {
		length->type = SVG_LENGTH_PT;
	} else if (unit = strstr(value_string, "pc")) {
		length->type = SVG_LENGTH_PC;
	} else {
		length->type = SVG_LENGTH_NUMBER;
	}
	if (unit) len = unit - value_string; 
	len+=svg_parse_float(value_string, &(length->number), 0);

	return len;
}

void svg_parse_visibility(SVG_Visibility *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_VISIBILITY_INHERIT;
	} else if (!strcmp(value_string, "visible")) {
		*value = SVG_VISIBILITY_VISIBLE;
	} else if (!strcmp(value_string, "hidden")) {
		*value = SVG_VISIBILITY_HIDDEN;
	} else if (!strcmp(value_string, "collapse")) {
		*value = SVG_VISIBILITY_COLLAPSE;
	} 
}

void svg_parse_display(SVG_Display *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_DISPLAY_INHERIT;
	} else if (!strcmp(value_string, "none")) {
		*value = SVG_DISPLAY_NONE;
	} else if (!strcmp(value_string, "inline")) {
		*value = SVG_DISPLAY_INLINE;
	} else if (!strcmp(value_string, "block")) {
		*value = SVG_DISPLAY_BLOCK;
	} else if (!strcmp(value_string, "list-item")) {
		*value = SVG_DISPLAY_LIST_ITEM;
	} else if (!strcmp(value_string, "run-in")) {
		*value = SVG_DISPLAY_RUN_IN;
	} else if (!strcmp(value_string, "compact")) {
		*value = SVG_DISPLAY_COMPACT;
	} else if (!strcmp(value_string, "marker")) {
		*value = SVG_DISPLAY_MARKER;
	} else if (!strcmp(value_string, "table")) {
		*value = SVG_DISPLAY_TABLE;
	} else if (!strcmp(value_string, "inline-table")) {
		*value = SVG_DISPLAY_INLINE_TABLE;
	} else if (!strcmp(value_string, "table-row-group")) {
		*value = SVG_DISPLAY_TABLE_ROW_GROUP;
	} else if (!strcmp(value_string, "table-header-group")) {
		*value = SVG_DISPLAY_TABLE_HEADER_GROUP;
	} else if (!strcmp(value_string, "table-footer-group")) {
		*value = SVG_DISPLAY_TABLE_FOOTER_GROUP;
	} else if (!strcmp(value_string, "table-row")) {
		*value = SVG_DISPLAY_TABLE_ROW;
	} else if (!strcmp(value_string, "table-column-group")) {
		*value = SVG_DISPLAY_TABLE_COLUMN_GROUP;
	} else if (!strcmp(value_string, "table-column")) {
		*value = SVG_DISPLAY_TABLE_COLUMN;
	} else if (!strcmp(value_string, "table-cell")) {
		*value = SVG_DISPLAY_TABLE_CELL;
	} else if (!strcmp(value_string, "table-caption")) {
		*value = SVG_DISPLAY_TABLE_CAPTION;
	} 
}

void svg_parse_displayalign(SVG_DisplayAlign *value, char *value_string)
{ 
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_DISPLAYALIGN_INHERIT;
	} else if (!strcmp(value_string, "auto")) {
		*value = SVG_DISPLAYALIGN_AUTO;
	} else if (!strcmp(value_string, "before")) {
		*value = SVG_DISPLAYALIGN_BEFORE;
	} else if (!strcmp(value_string, "center")) {
		*value = SVG_DISPLAYALIGN_CENTER;
	} else if (!strcmp(value_string, "after")) {
		*value = SVG_DISPLAYALIGN_AFTER;
	}
}

void svg_parse_pointerevents(SVG_PointerEvents *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_POINTEREVENTS_INHERIT;
	} else if (!strcmp(value_string, "visiblePainted")) {
		*value = SVG_POINTEREVENTS_VISIBLEPAINTED;
	} else if (!strcmp(value_string, "visibleFill")) {
		*value = SVG_POINTEREVENTS_VISIBLEFILL;
	} else if (!strcmp(value_string, "visibleStroke")) {
		*value = SVG_POINTEREVENTS_VISIBLESTROKE;
	} else if (!strcmp(value_string, "visible")) {
		*value = SVG_POINTEREVENTS_VISIBLE;
	} else if (!strcmp(value_string, "painted")) {
		*value = SVG_POINTEREVENTS_PAINTED;
	} else if (!strcmp(value_string, "fill")) {
		*value = SVG_POINTEREVENTS_FILL;
	} else if (!strcmp(value_string, "stroke")) {
		*value = SVG_POINTEREVENTS_STROKE;
	} else if (!strcmp(value_string, "all")) {
		*value = SVG_POINTEREVENTS_ALL;
	} else if (!strcmp(value_string, "none")) {
		*value = SVG_POINTEREVENTS_NONE;
	}
}

void svg_parse_renderinghint(SVG_RenderingHint *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_RENDERINGHINT_INHERIT;
	} else if (!strcmp(value_string, "auto")) {
		*value = SVG_RENDERINGHINT_AUTO;
	} else if (!strcmp(value_string, "optimizeQuality")) {
		*value = SVG_RENDERINGHINT_OPTIMIZEQUALITY;
	} else if (!strcmp(value_string, "optimizeSpeed")) {
		*value = SVG_RENDERINGHINT_OPTIMIZESPEED;
	} else if (!strcmp(value_string, "optimizeLegibility")) {
		*value = SVG_RENDERINGHINT_OPTIMIZELEGIBILITY;
	} else if (!strcmp(value_string, "crispEdges")) {
		*value = SVG_RENDERINGHINT_CRISPEDGES;
	} else if (!strcmp(value_string, "geometricPrecision")) {
		*value = SVG_RENDERINGHINT_GEOMETRICPRECISION;
	}
}

void svg_parse_vectoreffect(SVG_VectorEffect *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_VECTOREFFECT_INHERIT;
	} else if (!strcmp(value_string, "none")) {
		*value = SVG_VECTOREFFECT_NONE;
	} else if (!strcmp(value_string, "non-scaling-stroke")) {
		*value = SVG_VECTOREFFECT_NONSCALINGSTROKE;
	}
}

void svg_parse_playbackorder(SVG_VectorEffect *value, char *value_string)
{
	if (!strcmp(value_string, "forwardOnly")) {
		*value = SVG_PLAYBACKORDER_FORWARDONLY;
	} else if (!strcmp(value_string, "all")) {
		*value = SVG_PLAYBACKORDER_ALL;
	}
}

void svg_parse_timelinebegin(SVG_TimelineBegin *value, char *value_string)
{
	if (!strcmp(value_string, "onStart")) {
		*value = SVG_TIMELINEBEGIN_ONSTART;
	} else if (!strcmp(value_string, "onLoad")) {
		*value = SVG_TIMELINEBEGIN_ONLOAD;
	}
}

void svg_parse_xmlspace(XML_Space *value, char *value_string)
{
	if (!strcmp(value_string, "default")) {
		*value = XML_SPACE_DEFAULT;
	} else if (!strcmp(value_string, "preserve")) {
		*value = XML_SPACE_PRESERVE;
	}
}

void svg_parse_xmlev_propagate(XMLEV_Propagate *value, char *value_string)
{
	if (!strcmp(value_string, "continue")) {
		*value = XMLEVENT_PROPAGATE_CONTINUE;
	} else if (!strcmp(value_string, "stop")) {
		*value = XMLEVENT_PROPAGATE_STOP;
	}
}

void svg_parse_xmlev_defaultAction(XMLEV_DefaultAction *value, char *value_string)
{
	if (!strcmp(value_string, "cancel")) {
		*value = XMLEVENT_DEFAULTACTION_CANCEL;
	} else if (!strcmp(value_string, "perform")) {
		*value = XMLEVENT_DEFAULTACTION_PERFORM;
	}
}

void svg_parse_xmlev_phase(XMLEV_Phase *value, char *value_string)
{
	if (!strcmp(value_string, "default")) {
		*value = XMLEVENT_PHASE_DEFAULT;
	} else if (!strcmp(value_string, "capture")) {
		*value = XMLEVENT_PHASE_CAPTURE;
	}
}

void svg_parse_overflow(SVG_Overflow *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_OVERFLOW_INHERIT;
	} else if (!strcmp(value_string, "visible")) {
		*value = SVG_OVERFLOW_VISIBLE;
	} else if (!strcmp(value_string, "hidden")) {
		*value = SVG_OVERFLOW_HIDDEN;
	} else if (!strcmp(value_string, "scroll")) {
		*value = SVG_OVERFLOW_SCROLL;
	} else if (!strcmp(value_string, "auto")) {
		*value = SVG_OVERFLOW_AUTO;
	}
}

void svg_parse_textanchor(SVG_TextAnchor *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_TEXTANCHOR_INHERIT;
	} else if (!strcmp(value_string, "start")) {
		*value = SVG_TEXTANCHOR_START;
	} else if (!strcmp(value_string, "middle")) {
		*value = SVG_TEXTANCHOR_MIDDLE;
	} else if (!strcmp(value_string, "end")) {
		*value = SVG_TEXTANCHOR_END;
	}
}

void svg_parse_clipfillrule(SVG_FillRule *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_FILLRULE_INHERIT;
	} else if (!strcmp(value_string, "nonzero")) {
		*value = SVG_FILLRULE_NONZERO;
	} else if (!strcmp(value_string, "evenodd")) {
		*value = SVG_FILLRULE_EVENODD;
	} 
}

void svg_parse_strokelinejoin(SVG_StrokeLineJoin *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_STROKELINEJOIN_INHERIT;
	} else if (!strcmp(value_string, "miter")) {
		*value = SVG_STROKELINEJOIN_MITER;
	} else if (!strcmp(value_string, "round")) {
		*value = SVG_STROKELINEJOIN_ROUND;
	} else if (!strcmp(value_string, "bevel")) {
		*value = SVG_STROKELINEJOIN_BEVEL;
	} 
}

void svg_parse_strokelinecap(SVG_StrokeLineCap *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_STROKELINECAP_INHERIT;
	} else if (!strcmp(value_string, "butt")) {
		*value = SVG_STROKELINECAP_BUTT;
	} else if (!strcmp(value_string, "round")) {
		*value = SVG_STROKELINECAP_ROUND;
	} else if (!strcmp(value_string, "square")) {
		*value = SVG_STROKELINECAP_SQUARE;
	} 
}

void svg_parse_fontfamily(SVG_FontFamily *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		value->type = SVG_FONTFAMILY_INHERIT;
	} else {
		value->type = SVG_FONTFAMILY_VALUE;
		value->value = strdup(value_string);
	}
}

void svg_parse_fontstyle(SVG_FontStyle *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_FONTSTYLE_INHERIT;
	} else if (!strcmp(value_string, "normal")) {
		*value = SVG_FONTSTYLE_NORMAL;
	} else if (!strcmp(value_string, "italic")) {
		*value = SVG_FONTSTYLE_ITALIC;
	} else if (!strcmp(value_string, "oblique")) {
		*value = SVG_FONTSTYLE_OBLIQUE;
	} 
}

void svg_parse_fontweight(SVG_FontWeight *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_FONTWEIGHT_INHERIT;
	} else if (!strcmp(value_string, "normal")) {
		*value = SVG_FONTWEIGHT_NORMAL;
	} else if (!strcmp(value_string, "bold")) {
		*value = SVG_FONTWEIGHT_BOLD;
	} else if (!strcmp(value_string, "bolder")) {
		*value = SVG_FONTWEIGHT_BOLDER;
	} else if (!strcmp(value_string, "lighter")) {
		*value = SVG_FONTWEIGHT_LIGHTER;
	} else if (!strcmp(value_string, "100")) {
		*value = SVG_FONTWEIGHT_100;
	} else if (!strcmp(value_string, "200")) {
		*value = SVG_FONTWEIGHT_200;
	} else if (!strcmp(value_string, "300")) {
		*value = SVG_FONTWEIGHT_300;
	} else if (!strcmp(value_string, "400")) {
		*value = SVG_FONTWEIGHT_400;
	} else if (!strcmp(value_string, "500")) {
		*value = SVG_FONTWEIGHT_500;
	} else if (!strcmp(value_string, "600")) {
		*value = SVG_FONTWEIGHT_600;
	} else if (!strcmp(value_string, "700")) {
		*value = SVG_FONTWEIGHT_700;
	} else if (!strcmp(value_string, "800")) {
		*value = SVG_FONTWEIGHT_800;
	} else if (!strcmp(value_string, "900")) {
		*value = SVG_FONTWEIGHT_900;
	} 
}

void svg_parse_fontvariant(SVG_FontVariant *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SVG_FONTVARIANT_INHERIT;
	} else if (!strcmp(value_string, "normal")) {
		*value = SVG_FONTVARIANT_NORMAL;
	} else if (!strcmp(value_string, "small-caps")) {
		*value = SVG_FONTVARIANT_SMALLCAPS;
	} 
}

void svg_parse_inheritablefloat(SVGInheritableFloat *value, char *value_string, u8 clamp0to1)
{
	if (!strcmp(value_string, "inherit")) {
		value->type = SVG_FLOAT_INHERIT;
	} else {
		Float _val;
		value->type = SVG_FLOAT_VALUE;
		sscanf(value_string, "%f", &_val);
		value->value = FLT2FIX(_val); 
	}
	if (clamp0to1) value->value = MAX(0, MIN(1, value->value));
}

void svg_parse_boolean(SVG_Boolean *value, char *value_string)
{
	if (!stricmp(value_string, "1") || !stricmp(value_string, "true"))
		*value = 1;
	else
		*value = 0;
}

/* Should be called after xlink:href of the animation element has been resolved */
void smil_parse_attributename(SVGElement *animation_element, char *value_string)
{
	GF_FieldInfo xlink_href;
	GF_FieldInfo attribute_name;
	SVGElement *targetElement = NULL;
	GF_FieldInfo targetAttribute;

	gf_node_get_field_by_name((GF_Node *)animation_element, "xlink:href", &xlink_href);
	gf_node_get_field_by_name((GF_Node *)animation_element, "attributeName", &attribute_name);

	targetElement = ((SVG_IRI *)xlink_href.far_ptr)->target;

	if (!targetElement) {
		fprintf(stderr, "Warning: element only forward references supported.\n");
		return;
	}
	if (!gf_node_get_field_by_name((GF_Node *)targetElement, value_string, &targetAttribute)) {
		SMIL_AttributeName *attributename_value = (SMIL_AttributeName *)attribute_name.far_ptr;
		attributename_value->type = targetAttribute.fieldType;
		attributename_value->field_ptr = targetAttribute.far_ptr;
	} else 
		fprintf(stderr, "Error: Attribute %s does not belong to target element %s of type %s.\n", value_string, gf_node_get_name((GF_Node *)targetElement), SVG_GetElementName(gf_node_get_tag((GF_Node *)targetElement)));
}

void smil_parse_time_list(SVGElement *e, GF_List *values, char *begin_or_end_list)
{
	SMIL_Time *value;
	char value_string[500];
	char *str = begin_or_end_list, *tmp;
	u32 len;

	/* get rid of leading spaces */
	while (*str == ' ') str++;

	while (tmp = strchr(str, ';')) {
		len = tmp-str;
		memcpy(value_string, str, len);
		while (value_string[len - 1] == ' ' && len > 0) len--;
		value_string[len] = 0;

		GF_SAFEALLOC(value, sizeof(SMIL_Time))
		smil_parse_time(e, value, value_string);
 		gf_list_add(values, value);

		str = tmp + 1;
		while (*str == ' ') str++;
	}

	len = strlen(str);
	memcpy(value_string, str, len);
	while (value_string[len - 1] == ' ' && len > 0) len--;
	value_string[len] = 0;

	GF_SAFEALLOC(value, sizeof(SMIL_Time))
	smil_parse_time(e, value, value_string);
 	gf_list_add(values, value);

	/* sorting timing values */
	if (gf_list_count(values) > 1) {
		SMIL_Time *v, *sv;
		GF_List *sorted = gf_list_new();
		u32 i;
		u8 added = 0;
		do {
			v = gf_list_get(values, 0);
			gf_list_rem(values, 0);
			added = 0;
			for (i=0; i<gf_list_count(sorted); i++) {
				sv = gf_list_get(sorted, i);
				if (v->type >= SMIL_TIME_EVENT) {
					/* unresolved or indefinite so add at the end of the sorted list */
					gf_list_add(sorted, v);
					added = 1;
					break;
				} else {
					if (sv->type >= SMIL_TIME_EVENT) {
						gf_list_insert(sorted, v, i);
						added = 1;
					} else {
						if (v->clock <= sv->clock) {
							gf_list_insert(sorted, v, i);
							added = 1;
						}
					}
				}
			}
			if (!added) gf_list_add(sorted, v);
		} while (gf_list_count(values) > 0);

		for (i = 0; i < gf_list_count(sorted); i++) {
			gf_list_add(values, gf_list_get(sorted, i));
		}
	}
}

void smil_parse_attributeType(SMIL_AttributeType *value, char *value_string)
{
	if (!strcmp(value_string, "auto")) {
		*value = SMIL_ATTRIBUTETYPE_AUTO;
	} else if (!strcmp(value_string, "XML")) {
		*value = SMIL_ATTRIBUTETYPE_XML;
	} else if (!strcmp(value_string, "CSS")) {
		*value = SMIL_ATTRIBUTETYPE_CSS;
	}
}

void smil_parse_min_max_dur_repeatdur(SMIL_Duration *value, char *value_string)
{
	if (!strcmp(value_string, "indefinite")) {
		value->type = SMIL_DURATION_INDEFINITE;
	} else if (!strcmp(value_string, "media")) {
		value->type = SMIL_DURATION_MEDIA;
	} else {
		Double ftime;
		svg_parse_clock_value(value_string, &ftime);
		value->clock_value = ftime;
		value->type = SMIL_DURATION_DEFINED;
	}
}

void smil_parse_repeatcount(SMIL_RepeatCount *value, char *value_string)
{
	if (!strcmp(value_string, "indefinite")) {
		value->type = SMIL_REPEATCOUNT_INDEFINITE;
	} else {
		Float _val;
		sscanf(value_string, "%f", &_val);
		value->type = SMIL_REPEATCOUNT_DEFINED;
		value->count = FLT2FIX(_val);
	}
}

void smil_parse_fill(SMIL_Fill *value, char *value_string)
{
	if (!strcmp(value_string, "freeze")) {
		*value = SMIL_FILL_FREEZE;
	} else if (!strcmp(value_string, "remove")) {
		*value = SMIL_FILL_REMOVE;
	}
}

void smil_parse_restart(SMIL_Restart *value, char *value_string)
{
	if (!strcmp(value_string, "always")) {
		*value = SMIL_RESTART_ALWAYS;
	} else if (!strcmp(value_string, "whenNotActive")) {
		*value = SMIL_RESTART_WHENNOTACTIVE;
	} else if (!strcmp(value_string, "never")) {
		*value = SMIL_RESTART_NEVER;
	}
}

void smil_parse_calcmode(SMIL_CalcMode *value, char *value_string)
{
	if (!strcmp(value_string, "discrete")) {
		*value = SMIL_CALCMODE_DISCRETE;
	} else if (!strcmp(value_string, "linear")) {
		*value = SMIL_CALCMODE_LINEAR;
	} else if (!strcmp(value_string, "paced")) {
		*value = SMIL_CALCMODE_PACED;
	} else if (!strcmp(value_string, "spline")) {
		*value = SMIL_CALCMODE_SPLINE;
	} 
}

void smil_parse_additive(SMIL_Additive *value, char *value_string)
{
	if (!strcmp(value_string, "replace")) {
		*value = SMIL_ADDITIVE_REPLACE;
	} else if (!strcmp(value_string, "sum")) {
		*value = SMIL_ADDITIVE_SUM;
	} 
}

void smil_parse_accumulate(SMIL_Accumulate *value, char *value_string)
{
	if (!strcmp(value_string, "none")) {
		*value = SMIL_ACCUMULATE_NONE;
	} else if (!strcmp(value_string, "sum")) {
		*value = SMIL_ACCUMULATE_SUM;
	} 
}

void smil_parse_syncBehaviorOrDefault(SMIL_SyncBehavior *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		*value = SMIL_SYNCBEHAVIOR_INHERIT;
	} else if (!strcmp(value_string, "default")) {
		*value = SMIL_SYNCBEHAVIOR_DEFAULT;
	} else if (!strcmp(value_string, "locked")) {
		*value = SMIL_SYNCBEHAVIOR_LOCKED;
	} else if (!strcmp(value_string, "canSlip")) {
		*value = SMIL_SYNCBEHAVIOR_CANSLIP;
	} else if (!strcmp(value_string, "independent")) {
		*value = SMIL_SYNCBEHAVIOR_INDEPENDENT;
	} 
}

void smil_parse_syncToleranceOrDefault(SMIL_SyncTolerance *value, char *value_string)
{
	if (!strcmp(value_string, "inherit")) {
		value->type = SMIL_SYNCTOLERANCE_INHERIT;
	} else if (!strcmp(value_string, "default")) {
		value->type = SMIL_SYNCTOLERANCE_DEFAULT;
	} else {
		value->type = SMIL_SYNCBEHAVIOR_LOCKED;
		svg_parse_clock_value(value_string, &(value->value));
	}
}

void svg_parse_viewbox(SVG_ViewBox *value, char *value_string)
{
	char *str = value_string;
	u32 i = 0;
	i+=svg_parse_float(&(str[i]), &(value->x), 0);
	i+=svg_parse_float(&(str[i]), &(value->y), 0);
	i+=svg_parse_float(&(str[i]), &(value->width), 0);
	i+=svg_parse_float(&(str[i]), &(value->height), 0);
}

void svg_parse_coordinates(GF_List *values, char *value_string)
{
	u32 i = 0;
	char *str = value_string;
	u32 len = strlen(str);
	while (i < len) {
		SVG_Coordinate *c;
		GF_SAFEALLOC(c, sizeof(SVG_Coordinate));
		i+=svg_parse_length(c, &(str[i]));
		gf_list_add(values, c);
	}
}

void svg_parse_points(GF_List *values, char *value_string)
{
	u32 i = 0;
	char *str = value_string;
	u32 len = strlen(str);
	while (i < len) {
		SVG_Point *p;
		GF_SAFEALLOC(p, sizeof(SVG_Point))
		i+=svg_parse_float(&(str[i]), &(p->x), 0);
		i+=svg_parse_float(&(str[i]), &(p->y), 0);
		gf_list_add(values, p);
	}
}

void svg_parse_floats(GF_List *values, char *value_string, Bool is_angle)
{
	u32 i = 0;
	char *str = value_string;
	u32 len = strlen(str);
	while (i < len) {
		Fixed *f;
		GF_SAFEALLOC(f, sizeof(Fixed));
		i+=svg_parse_float(&(str[i]), f, is_angle);
		gf_list_add(values, f);
	}
}

void svg_parse_strokedasharray(SVG_StrokeDashArray *value, char *value_string)
{
	if (!strcmp(value_string, "none")) {
		value->type = SVG_STROKEDASHARRAY_NONE;
	} else if (!strcmp(value_string, "inherit")) {
		value->type = SVG_STROKEDASHARRAY_INHERIT;
	} else {
		Array *vals = &(value->array);
		GF_List *values = gf_list_new();
		u32 i = 0;
		u32 len = strlen(value_string);
		char *str = value_string;
		while (i < len) {
			Fixed *f;
			GF_SAFEALLOC(f, sizeof(Fixed));
			i+=svg_parse_float(&(str[i]), f, 0);
			gf_list_add(values, f);
		}
		vals->count = gf_list_count(values);
		GF_SAFEALLOC(vals->vals, (sizeof(Fixed)*vals->count));
		for (i = 0; i < vals->count; i++) {
			Fixed *f = gf_list_get(values, i);
			vals->vals[i] = *f;
			free(f);
		}
		gf_list_del(values);
		value->type = SVG_STROKEDASHARRAY_ARRAY;
	}
}

void svg_parse_transformlist(GF_List *list, char *value_string)
{
	/* To be fixed to parse all the transform values */
	SVG_Transform *tr;
	GF_SAFEALLOC(tr, sizeof(SVG_Transform));
	svg_parse_transform(&(tr->matrix), value_string);
	gf_list_add(list, tr);
}

void svg_parse_iri(SVGElement *elt, SVG_IRI *iri, char *attribute_content)
{
	if (strstr(attribute_content, "data:")) {
#ifdef USE_GPAC_CACHE_MECHANISM
		char cache_name[MAX_URI_LENGTH];
		char mimeType[100], file_extension[4] = "";
		char encoding[100];
		char *mimeType_end = NULL, *encoding_end = NULL;
		char *data;
		u32 data_size;
		char tmp[MAX_URI_LENGTH];

		{ // initializes the path of the file to be cached
			u32 i, last_sep;
			const char *cache_dir = parser->temp_dir;
			if (!cache_dir || !strlen(cache_dir) ) return;

			if (cache_dir[strlen(cache_dir)-1] != GF_PATH_SEPARATOR) {
				sprintf(cache_name, "%s%c", cache_dir, GF_PATH_SEPARATOR);
			} else {
				strcpy(cache_name, cache_dir);
			}
			strcpy(tmp, parser->file_name);
			last_sep = 0;
			for (i=0; i<strlen(tmp); i++) {
				if (tmp[i] == GF_PATH_SEPARATOR) tmp[i] = '_';
				else if (tmp[i] == '.') {
					tmp[i] = '_';
					last_sep = i;
				} else if (tmp[i] == ':') {
					tmp[i] = '_';
					last_sep = i;
				}
			}
			if (last_sep) tmp[last_sep] = '.';
			strcat(cache_name, tmp);
		}

		mimeType_end = strchr(attribute_content+5, ';');
		if (mimeType_end) {
			memcpy(&mimeType, &(attribute_content[5]), (mimeType_end - attribute_content) - 5);
			mimeType[(mimeType_end - attribute_content) - 5] = 0;
		}
		encoding_end = strchr(mimeType_end, ',');
		if (encoding_end) {
			memcpy(&encoding, mimeType_end+1, (encoding_end - mimeType_end)-1);
			encoding[(encoding_end - mimeType_end)-1] = 0;
		}

		GF_SAFEALLOC(data, strlen(attribute_content)*2)
		if (!strcmp(encoding, "base64")) {
			/* decoding is done in place */
			u32 input_len = strlen(encoding_end+1);
			u32 output_len = strlen(attribute_content)*2;
			data_size = gf_base64_decode(encoding_end+1, input_len, data, output_len);
		} else {
			// TODO: Handle other encoding schemes
		}

		/* determine the name of the file based on the mime type 
		   TODO: handle other mime types */
		if (!strcmp(mimeType,"image/jpeg") || !strcmp(mimeType, "image/jpg") ) {
			strcpy(file_extension,"jpg");
		} else if (!strcmp(mimeType,"image/png")) {
			strcpy(file_extension,"png");
		}
		if (file_extension[0]) {
			FILE *cache_image;
			if (parser->cacheID<10) {
				sprintf(tmp, "image0%i.%s", parser->cacheID, file_extension);
			} else {
				sprintf(tmp, "image%2i.%s", parser->cacheID, file_extension);
			}
			parser->cacheID++;
			strcat(cache_name, tmp);
			cache_image = fopen(cache_name, "wb");
			fwrite(data, data_size, 1, cache_image);
			fclose(cache_image);
		} 
		free(data);

		/* Changes the type of the URI and point it to the cache file */
		iri->type = SVG_IRI_IRI;
		iri->iri = strdup(cache_name);
#endif
	} else {
		/* TODO: Handle xpointer(id()) syntax */
		if (attribute_content[0] == '#') {
			iri->type = SVG_IRI_ELEMENTID;
			iri->target = (SVGElement *)gf_sg_find_node_by_name(elt->sgprivate->scenegraph, &(attribute_content[1]));
			iri->iri_owner = elt;
			if (!iri->target) {
/*				href_instance *hi = malloc(sizeof(href_instance));
				hi->elt = elt;
				hi->iri = iri;
				iri->iri = strdup(attribute_content);
				gf_list_add(parser->unresolved_hrefs, hi);
*/
			} else {
				/* reference is resolved we can register it 
				   the target_element is referenced by the current element */
				//gf_node_register((GF_Node *)iri->target, (GF_Node *)elt);
			}
		} else {
			iri->type = SVG_IRI_IRI;
			iri->iri = strdup(attribute_content);
		}
	}
}

void svg_parse_zoomandpan(SVG_ZoomAndPan *value, char *value_string)
{
	if (!strcmp(value_string, "disable")) {
		*value = SVG_ZOOMANDPAN_DISABLE;
	} else if (!strcmp(value_string, "magnify")) {
		*value = SVG_ZOOMANDPAN_MAGNIFY;
	} 
}

void svg_parse_preserveaspectratio(SVG_PreserveAspectRatio *par, char *attribute_content)
{
	char *content = attribute_content;
	while (*content == ' ') content++;
	if (strstr(content, "defer")) {
		par->defer = 1;
		content += 4;
	} else {
		content = attribute_content;
	}
	while (*content == ' ') content++;
	if (strstr(content, "none")) {
		par->align = SVG_PRESERVEASPECTRATIO_NONE;
		content+=4;
	} else if (strstr(content, "xMinYMin")) {
		par->align = SVG_PRESERVEASPECTRATIO_XMINYMIN;
		content+=8;
	} else if (strstr(content, "xMidYMin")) {
		par->align = SVG_PRESERVEASPECTRATIO_XMIDYMIN;
		content+=8;
	} else if (strstr(content, "xMaxYMin")) {
		par->align = SVG_PRESERVEASPECTRATIO_XMAXYMIN;
		content+=8;
	} else if (strstr(content, "xMinYMid")) {
		par->align = SVG_PRESERVEASPECTRATIO_XMINYMID;
		content+=8;
	} else if (strstr(content, "xMidYMid")) {
		par->align = SVG_PRESERVEASPECTRATIO_XMIDYMID;
		content+=8;
	} else if (strstr(content, "xMaxYMid")) {
		par->align = SVG_PRESERVEASPECTRATIO_XMAXYMID;
		content+=8;
	} else if (strstr(content, "xMinYMax")) {
		par->align = SVG_PRESERVEASPECTRATIO_XMINYMAX;
		content+=8;
	} else if (strstr(content, "xMidYMax")) {
		par->align = SVG_PRESERVEASPECTRATIO_XMIDYMAX;
		content+=8;
	} else if (strstr(content, "xMaxYMax")) {
		par->align = SVG_PRESERVEASPECTRATIO_XMAXYMAX;
		content+=8;
	}
	while (*content == ' ') content++;
	if (*content == 0) return;
	if (strstr(content, "meet")) {
		par->meetOrSlice = SVG_MEETORSLICE_MEET;
	} else if (strstr(content, "slice")) {
		par->meetOrSlice = SVG_MEETORSLICE_SLICE;
	}
}

void svg_parse_animatetransform_type(SVG_TransformType *anim_transform_type, char *attribute_content)
{
	*anim_transform_type = SVG_TRANSFORM_MATRIX;
	if (!strcmp(attribute_content, "scale")) {
		*anim_transform_type = SVG_TRANSFORM_SCALE;
	} else if (!strcmp(attribute_content, "rotate")) {
		*anim_transform_type = SVG_TRANSFORM_ROTATE;
	} else if (!strcmp(attribute_content, "translate")) {
		*anim_transform_type = SVG_TRANSFORM_TRANSLATE;
	} else if (!strcmp(attribute_content, "skewX")) {
		*anim_transform_type = SVG_TRANSFORM_SKEWX;
	} else if (!strcmp(attribute_content, "skewY")) {
		*anim_transform_type = SVG_TRANSFORM_SKEWY;
	}
}
/* end of Basic SVG datatype parsing functions */

void svg_parse_one_anim_value(SVGElement *elt, SMIL_AnimateValue *anim_value, char *attribute_content, u8 anim_value_type, u8 transform_type)
{
	GF_FieldInfo info;
	info.fieldType = anim_value_type;
	info.name = "animation value";
	info.far_ptr = svg_create_value_from_attributetype(anim_value_type, transform_type);
	if (info.far_ptr) svg_parse_attribute(elt, &info, attribute_content, anim_value_type, transform_type);
	anim_value->value = info.far_ptr;
	anim_value->datatype = anim_value_type;
}

void svg_parse_anim_values(SVGElement *elt, SMIL_AnimateValues *anim_values, char *anim_values_string, u8 anim_value_type, u8 transform_type)
{
	u32 len, i = 0;
	char *str;
	char *value_string;
	s32 psemi = -1;
	GF_FieldInfo info;
	info.fieldType = anim_value_type;
	info.name = "animation values";

	anim_values->datatype = anim_value_type;
	
	len = strlen(anim_values_string);
	str = anim_values_string;
	for (i = 0; i < len+1; i++) {
		if (str[i] == ';' || str[i] == 0) {
			u32 single_value_len = 0;
			single_value_len = i - (psemi+1);
			GF_SAFEALLOC(value_string, single_value_len+1);
			memcpy(value_string, str + (psemi+1), single_value_len);
			value_string[single_value_len] = 0;
			psemi = i;

			info.far_ptr = svg_create_value_from_attributetype(anim_value_type, transform_type);
			if (info.far_ptr) {
				svg_parse_attribute(elt, &info, value_string, anim_value_type, transform_type);
				gf_list_add(anim_values->values, info.far_ptr);
			}

			free(value_string);
		}
	}
}

void svg_parse_transformtype(SVGElement *elt, void *transform, char *value_string, u8 transform_type) 
{
	u32 i = 0;
	switch(transform_type) {
	case SVG_TRANSFORM_TRANSLATE:
		{
			SVG_Point *p = (SVG_Point *)transform;
			i+=svg_parse_float(&(value_string[i]), &(p->x), 0);
			if (value_string[i] == 0) {
				p->y = 0;
			} else {
				i+=svg_parse_float(&(value_string[i]), &(p->y), 0);
			}
		}
		break;
	case SVG_TRANSFORM_SCALE:
		{
			SVG_Point *p = (SVG_Point *)transform;;
			i+=svg_parse_float(&(value_string[i]), &(p->x), 0);
			if (value_string[i] == 0) {
				p->y = p->x;
			} else {
				i+=svg_parse_float(&(value_string[i]), &(p->y), 0);
			}
		}
		break;
	case SVG_TRANSFORM_ROTATE:
		{
			SVG_Point_Angle *p = (SVG_Point_Angle *)transform;;
			i+=svg_parse_float(&(value_string[i]), &(p->angle), 1);
			if (value_string[i] == 0) {
				p->y = p->x = 0;
			} else {
				i+=svg_parse_float(&(value_string[i]), &(p->x), 0);
				i+=svg_parse_float(&(value_string[i]), &(p->y), 0);
			}					
		}
		break;
	case SVG_TRANSFORM_SKEWX:
	case SVG_TRANSFORM_SKEWY:
		{
			Fixed *p = (Fixed *)transform;
			i+=svg_parse_float(&(value_string[i]), p, 1);
		}
		break;
	default:
		{
			SVG_Matrix *m = (SVG_Matrix *)transform;
			svg_parse_transform(m, value_string);
		}
	}	
}

void svg_parse_motionvalue(SVG_Point *p, char *value_string)
{
	u32 i = 0;
	i+=svg_parse_float(&(value_string[i]), &(p->x), 0);
	i+=svg_parse_float(&(value_string[i]), &(p->y), 0);
}

/* Parse an SVG attribute */
GF_Err svg_parse_attribute(SVGElement *elt, GF_FieldInfo *info, char *attribute_content, u8 anim_value_type, u8 transform_type)
{
	switch (info->fieldType) {
	case SVG_Boolean_datatype:
		svg_parse_boolean((SVG_Boolean *)info->far_ptr, attribute_content);
	    break;
	case SVG_Color_datatype:
		svg_parse_color((SVG_Color *)info->far_ptr, attribute_content);
	    break;
	case SVG_Paint_datatype:
		svg_parse_paint((SVG_Paint *)info->far_ptr, attribute_content);
		break;

/* beginning of keyword type parsing */
	case SVG_FillRule_datatype:
		svg_parse_clipfillrule((SVG_FillRule *)info->far_ptr, attribute_content);
		break;
	case SVG_StrokeLineJoin_datatype:
		svg_parse_strokelinejoin((SVG_StrokeLineJoin *)info->far_ptr, attribute_content);
		break;
	case SVG_StrokeLineCap_datatype:
		svg_parse_strokelinecap((SVG_StrokeLineCap *)info->far_ptr, attribute_content);
		break;
	case SVG_FontStyle_datatype:
		svg_parse_fontstyle((SVG_FontStyle *)info->far_ptr, attribute_content);
		break;
	case SVG_FontWeight_datatype:
		svg_parse_fontweight((SVG_FontWeight *)info->far_ptr, attribute_content);
		break;
	case SVG_FontVariant_datatype:
		svg_parse_fontvariant((SVG_FontVariant *)info->far_ptr, attribute_content);
		break;
	case SVG_TextAnchor_datatype:
		svg_parse_textanchor((SVG_TextAnchor *)info->far_ptr, attribute_content);
		break;
	case SVG_Display_datatype:
		svg_parse_display((SVG_Display *)info->far_ptr, attribute_content);
		break;
	case SVG_Visibility_datatype:
		svg_parse_visibility((SVG_Visibility *)info->far_ptr, attribute_content);
		break;
	case SVG_Overflow_datatype:
		svg_parse_overflow((SVG_Overflow *)info->far_ptr, attribute_content);
		break;
	case SVG_ZoomAndPan_datatype:
		svg_parse_zoomandpan((SVG_ZoomAndPan *)info->far_ptr, attribute_content);
		break;
	case SVG_DisplayAlign_datatype:
		svg_parse_displayalign((SVG_DisplayAlign *)info->far_ptr, attribute_content);
		break;
	case SVG_PointerEvents_datatype:
		svg_parse_pointerevents((SVG_PointerEvents *)info->far_ptr, attribute_content);
		break;
	case SVG_RenderingHint_datatype:
		svg_parse_renderinghint((SVG_RenderingHint *)info->far_ptr, attribute_content);
		break;
	case SVG_VectorEffect_datatype:
		svg_parse_vectoreffect((SVG_VectorEffect *)info->far_ptr, attribute_content);
		break;
	case SVG_PlaybackOrder_datatype:
		svg_parse_playbackorder((SVG_PlaybackOrder *)info->far_ptr, attribute_content);
		break;
	case SVG_TimelineBegin_datatype:
		svg_parse_timelinebegin((SVG_TimelineBegin *)info->far_ptr, attribute_content);
		break;
	case XML_Space_datatype:
		svg_parse_xmlspace((XML_Space *)info->far_ptr, attribute_content);
		break;
	case XMLEV_Propagate_datatype:
		svg_parse_xmlev_propagate((XMLEV_Propagate *)info->far_ptr, attribute_content);
		break;
	case XMLEV_DefaultAction_datatype:
		svg_parse_xmlev_defaultAction((XMLEV_DefaultAction *)info->far_ptr, attribute_content);
		break;
	case XMLEV_Phase_datatype:
		svg_parse_xmlev_phase((XMLEV_Phase *)info->far_ptr, attribute_content);
		break;
	case SMIL_SyncBehavior_datatype:
		smil_parse_syncBehaviorOrDefault((SMIL_SyncBehavior *)info->far_ptr, attribute_content);
		break;
	case SMIL_SyncTolerance_datatype:
		smil_parse_syncToleranceOrDefault((SMIL_SyncTolerance *)info->far_ptr, attribute_content);
		break;
	case SMIL_AttributeType_datatype:
		smil_parse_attributeType((SMIL_AttributeType *)info->far_ptr, attribute_content);
		break;	
	case SMIL_CalcMode_datatype:
		smil_parse_calcmode((SMIL_CalcMode *)info->far_ptr, attribute_content);
		break;
	case SMIL_Additive_datatype:
		smil_parse_additive((SMIL_CalcMode *)info->far_ptr, attribute_content);
		break;
	case SMIL_Accumulate_datatype:
		smil_parse_accumulate((SMIL_Accumulate *)info->far_ptr, attribute_content);
		break;
	case SMIL_Restart_datatype:
		smil_parse_restart((SMIL_Restart *)info->far_ptr, attribute_content);
		break;
	case SMIL_Fill_datatype:
		smil_parse_fill((SMIL_Fill *)info->far_ptr, attribute_content);
		break;

	case SVG_GradientUnit_datatype:
		*((SVG_GradientUnit *)info->far_ptr) = !stricmp(attribute_content, "userSpaceOnUse") ? SVG_GRADIENTUNITS_USER : SVG_GRADIENTUNITS_OBJECT;
		break;
/* end of keyword type parsing */

	/* inheritable floats */
	case SVG_Opacity_datatype:
		svg_parse_inheritablefloat((SVGInheritableFloat *)info->far_ptr, attribute_content, 1);
		break;
	case SVG_StrokeMiterLimit_datatype:
	case SVG_FontSize_datatype:
	case SVG_StrokeDashOffset_datatype:
	case SVG_AudioLevel_datatype:
	case SVG_LineIncrement_datatype:
		svg_parse_inheritablefloat((SVGInheritableFloat *)info->far_ptr, attribute_content, 0);
		break;

	/* inheritable float and unit */
	case SVG_StrokeWidth_datatype:
	case SVG_Length_datatype:
	case SVG_Coordinate_datatype:
		svg_parse_length((SVG_Length *)info->far_ptr, attribute_content);
		break;

	case SMIL_AnimateValue_datatype:
		svg_parse_one_anim_value(elt, (SMIL_AnimateValue *)info->far_ptr, attribute_content, anim_value_type, transform_type);
		break;
	case SMIL_AnimateValues_datatype:
		svg_parse_anim_values(elt, (SMIL_AnimateValues *)info->far_ptr, attribute_content, anim_value_type, transform_type);
		break;
	case SVG_IRI_datatype:
		svg_parse_iri(elt, (SVG_IRI*)info->far_ptr, attribute_content);
		break;
	case SMIL_AttributeName_datatype:
		/* Should be not called here but directly */
		break;
	case SMIL_Times_datatype:
		smil_parse_time_list(elt, *(GF_List **)info->far_ptr, attribute_content);
		break;
	case SMIL_Duration_datatype:
		smil_parse_min_max_dur_repeatdur((SMIL_Duration *)info->far_ptr, attribute_content);
		break;
	case SMIL_RepeatCount_datatype:
		smil_parse_repeatcount((SMIL_RepeatCount *)info->far_ptr, attribute_content);
		break;

	case SVG_PathData_datatype:
		svg_parse_path((SVG_PathData *)info->far_ptr, attribute_content);
		break;
	case SVG_Points_datatype:
		svg_parse_points(*(GF_List **)(info->far_ptr), attribute_content);
		break;
	case SMIL_KeyTimes_datatype:
	case SMIL_KeyPoints_datatype:
	case SMIL_KeySplines_datatype:
		svg_parse_floats(*(GF_List **)(info->far_ptr), attribute_content, 0);
		break;
	case SVG_Coordinates_datatype:
		svg_parse_coordinates(*(GF_List **)(info->far_ptr), attribute_content);
		break;
	case SVG_ViewBox_datatype:
		svg_parse_viewbox(info->far_ptr, attribute_content);
		break;
	case SVG_StrokeDashArray_datatype:
		svg_parse_strokedasharray((SVG_StrokeDashArray *)info->far_ptr, attribute_content);
		break;
	case SVG_FontFamily_datatype:
		svg_parse_fontfamily((SVG_FontFamily *)info->far_ptr, attribute_content);
		break;
	case SVG_TransformList_datatype:
		svg_parse_transformlist(*(GF_List **)info->far_ptr, attribute_content);
		break;
	case SVG_PreserveAspectRatio_datatype:
		svg_parse_preserveaspectratio((SVG_PreserveAspectRatio *)info->far_ptr, attribute_content);
		break;
	case SVG_Number_datatype:
		svg_parse_float(attribute_content, (SVG_Number *)info->far_ptr, 0);
		break;

	/* required for animateMotion */
	case SVG_Motion_datatype:
		svg_parse_motionvalue((SVG_Point *)info->far_ptr, attribute_content);
		break;
	/* required for animateTransform */
	case SVG_TransformType_datatype:
		svg_parse_transformtype(elt, info->far_ptr, attribute_content, transform_type);
		break;

	case SVG_String_datatype:
//		*(SVG_String *)info->far_ptr = strdup(attribute_content);
		break;
	case SVG_ContentType_datatype:
		*(SVG_String *)info->far_ptr = strdup(attribute_content);
		break;
	default:
		fprintf(stdout, "Warning: skipping unsupported attribute %s\n", info->name);
		break;
	}
	return GF_OK;
}

void svg_parse_one_style(SVGElement *elt, char *one_style) 
{
	GF_FieldInfo info;
	char *c;
	u32 attributeNameLen;
	char *attributeName;

	while (*one_style == ' ') one_style++;
	c = strchr(one_style, ':');
	if (!c) return;
	attributeNameLen = (c - one_style);
	attributeName = malloc(attributeNameLen+1);
	memcpy(attributeName, one_style, attributeNameLen);
	attributeName[attributeNameLen] = 0;
	if (!gf_node_get_field_by_name((GF_Node *)elt, attributeName, &info)) {
		c++;
		svg_parse_attribute(elt, &info, c, 0, 0);
	} else {
#ifndef _WIN32_WCE
		fprintf(stderr, "Error: Attribute %s does not belong to element %s.\n", attributeName, SVG_GetElementName(gf_node_get_tag((GF_Node*)elt)));
#endif
	}
}

void svg_parse_style(SVGElement *elt, char *style) 
{
	u32 i = 0;
	u32 len = strlen(style);
	char *str = style;
	s32 psemi = -1;
	
	for (i = 0; i < len+1; i++) {
		if (str[i] == ';' || str[i] == 0) {
			char *value_string;
			u32 single_value_len = 0;
			single_value_len = i - (psemi+1);
			if (single_value_len) {
				GF_SAFEALLOC(value_string, single_value_len+1);
				memcpy(value_string, str + (psemi+1), single_value_len);
				value_string[single_value_len] = 0;
				psemi = i;
				svg_parse_one_style(elt, value_string);
				free(value_string);
			}
		}
	}

}

void *svg_create_value_from_attributetype(u8 attribute_type, u8 transform_type)
{
	switch (attribute_type) {
	case SVG_Boolean_datatype:
		{
			SVG_Boolean *b;
			GF_SAFEALLOC(b, sizeof(SVG_Boolean_datatype))
			return b;
		}
		break;
	case SVG_Color_datatype:
		{
			SVG_Color *color;
			GF_SAFEALLOC(color, sizeof(SVG_Color))
			return color;
		}
		break;
	case SVG_Paint_datatype:
		{
			SVG_Paint *paint;				
			GF_SAFEALLOC(paint, sizeof(SVG_Paint));
			GF_SAFEALLOC(paint->color, sizeof(SVG_Color));
			return paint;
		}
		break;
	
	/* keyword types */
	case SVG_FillRule_datatype:
	case SVG_StrokeLineJoin_datatype:
	case SVG_StrokeLineCap_datatype:
	case SVG_FontStyle_datatype:
	case SVG_FontWeight_datatype:
	case SVG_FontVariant_datatype:
	case SVG_TextAnchor_datatype:
	case SVG_Display_datatype:
	case SVG_Visibility_datatype:
	case SVG_Overflow_datatype:
	case SVG_ZoomAndPan_datatype:
	case SVG_DisplayAlign_datatype:
	case SVG_PointerEvents_datatype:
	case SVG_RenderingHint_datatype:
	case SVG_VectorEffect_datatype:
	case SVG_PlaybackOrder_datatype:
	case SVG_TimelineBegin_datatype:
	case XML_Space_datatype:
	case XMLEV_Propagate_datatype:
	case XMLEV_DefaultAction_datatype:
	case XMLEV_Phase_datatype:
	case SMIL_SyncBehavior_datatype:
	case SMIL_SyncTolerance_datatype:
	case SMIL_AttributeType_datatype:
	case SMIL_CalcMode_datatype:
	case SMIL_Additive_datatype:
	case SMIL_Accumulate_datatype:
	case SMIL_Restart_datatype:
	case SMIL_Fill_datatype:
		{
			u8 *keyword;
			GF_SAFEALLOC(keyword, sizeof(u8))
			return keyword;
		}
		break;

	/* inheritable floats */
	case SVG_Opacity_datatype:
	case SVG_StrokeMiterLimit_datatype:
	case SVG_FontSize_datatype:
	case SVG_StrokeDashOffset_datatype:
	case SVG_AudioLevel_datatype:
	case SVG_LineIncrement_datatype:
		{
			SVGInheritableFloat *p;
			GF_SAFEALLOC(p, sizeof(SVGInheritableFloat));
			return p;
		}
		break;

	/* inheritable float and unit */
	case SVG_StrokeWidth_datatype:
	case SVG_Length_datatype:
	case SVG_Coordinate_datatype:
		{
			SVG_Length *length;
			GF_SAFEALLOC(length, sizeof(SVG_Length))
			return length;
		}
		break;	
	
	case SVG_StrokeDashArray_datatype:
		{
			SVG_StrokeDashArray *array;
			GF_SAFEALLOC(array, sizeof(SVG_StrokeDashArray))
			return array;
		}
		break;

	case SVG_TransformList_datatype:
		{
			SVG_Matrix *m;
			GF_SAFEALLOC(m, sizeof(SVG_Matrix))
			return m;
		}
		break;
	case SVG_TransformType_datatype:
		{
			switch(transform_type) {
			case SVG_TRANSFORM_TRANSLATE:
			case SVG_TRANSFORM_SCALE:
				{
					SVG_Point *p;
					GF_SAFEALLOC(p, sizeof(SVG_Point));
					return p;
				}
				break;
			case SVG_TRANSFORM_ROTATE:
				{
					SVG_Point_Angle *p;
					GF_SAFEALLOC(p, sizeof(SVG_Point_Angle));
					return p;
				}
				break;
			case SVG_TRANSFORM_SKEWX:
			case SVG_TRANSFORM_SKEWY:
				{
					Fixed *p;
					GF_SAFEALLOC(p, sizeof(Fixed));
					return p;
				}
				break;
			default:
				{
					SVG_Matrix *m;
					GF_SAFEALLOC(m, sizeof(SVG_Matrix))
					return m;
				}
			}
		}
		break;
	case SVG_Motion_datatype:
		{
			SVG_Point *p;
			GF_SAFEALLOC(p, sizeof(SVG_Point));
			return p;
		}
		break;
	case SVG_ViewBox_datatype:
		{
			SVG_ViewBox *viewbox;
			GF_SAFEALLOC(viewbox, sizeof(SVG_ViewBox))
			return viewbox;
		}
		break;
	case SVG_IRI_datatype:
		{
			SVG_IRI *iri;
			GF_SAFEALLOC(iri, sizeof(SVG_IRI))
			return iri;
		}
		break;
	case SVG_FontFamily_datatype:
		{
			SVG_FontFamily *fontfamily;
			GF_SAFEALLOC(fontfamily, sizeof(SVG_FontFamily))
			return fontfamily;
		}
		break;
	case SVG_String_datatype:
		{
			SVG_String *string;
			GF_SAFEALLOC(string, sizeof(SVG_String))
			return string;
		}
		break;
	case SVG_Points_datatype:
		{
			SVG_Points *points;
			GF_SAFEALLOC(points, sizeof(SVG_Points))
			*points = gf_list_new();
			return points;
		}
		break;
	case SVG_Coordinates_datatype:
		{
			SVG_Coordinates *coords;
			GF_SAFEALLOC(coords, sizeof(SVG_Coordinates))
			*coords = gf_list_new();
			return coords;
		}
		break;
	case SVG_PathData_datatype:
		{
			SVG_PathData *path = malloc(sizeof(SVG_PathData));
			path->commands = gf_list_new();
			path->points = gf_list_new();
			return path;
		}
		break;
	default:
		fprintf(stdout, "Error: Type not supported: %d.\n", attribute_type);
	} 
	return NULL;
}

static void svg_dump_color(SVG_Color *col, char *attValue)
{
	if (col->type == SVG_COLOR_CURRENTCOLOR) strcpy(attValue, "currentColor");
	else if (col->type == SVG_COLOR_INHERIT) strcpy(attValue, "inherit");
	else {
		u32 i, count;
		count = sizeof(predefined_colors) / sizeof(struct predef_col);
		for (i=0; i<count; i++) {
			if (
				(255*FIX2FLT(col->red) == predefined_colors[i].r)
				&& (255*FIX2FLT(col->green) == predefined_colors[i].g)
				&& (255*FIX2FLT(col->blue) == predefined_colors[i].b)
			) {
				strcpy(attValue, predefined_colors[i].name);
				return;
			}
		}
		sprintf(attValue, "#%02X%02X%02X", (u32) (255*FIX2FLT(col->red)), (u32) (255*FIX2FLT(col->green)), (u32) (255*FIX2FLT(col->blue)) );
		/*compress it...*/
		if ( (attValue[1]==attValue[2]) && (attValue[3]==attValue[4]) && (attValue[5]==attValue[6]) )
			sprintf(attValue, "#%c%c%c", attValue[1], attValue[3], attValue[5]);
	}
}

static void svg_dump_length(SVG_Length *l, char *attValue)
{
	if (l->type==SVG_LENGTH_INHERIT) strcpy(attValue, "inherit");
	else {
		sprintf(attValue, "%g", l->number);
		if (l->type == SVG_LENGTH_PERCENTAGE) strcat(attValue, "%");
		else if (l->type == SVG_LENGTH_EMS) strcat(attValue, "em");
		else if (l->type == SVG_LENGTH_EXS) strcat(attValue, "ex");
		else if (l->type == SVG_LENGTH_PX) strcat(attValue, "px");
		else if (l->type == SVG_LENGTH_CM) strcat(attValue, "cm");
		else if (l->type == SVG_LENGTH_MM) strcat(attValue, "mm");
		else if (l->type == SVG_LENGTH_IN) strcat(attValue, "in");
		else if (l->type == SVG_LENGTH_PT) strcat(attValue, "pt");
		else if (l->type == SVG_LENGTH_PC) strcat(attValue, "pc");
	}
}

static void svg_dump_iri(SVG_IRI*iri, char *attValue)
{
	if (iri->type == SVG_IRI_ELEMENTID) sprintf(attValue, "#%s", gf_node_get_name((GF_Node *)iri->target));
	else if ((iri->type == SVG_IRI_IRI) && iri->iri) strcpy(attValue, iri->iri);
	else strcpy(attValue, "");
}

GF_Err svg_dump_attribute(SVGElement *elt, GF_FieldInfo *info, char *attValue)
{
	u32 intVal = *(u32 *)info->far_ptr;
	strcpy(attValue, "");

	switch (info->fieldType) {
	case SVG_Boolean_datatype:
		sprintf(attValue, "%s", *(SVG_Boolean *)info->far_ptr ? "true" : "false");
	    break;
	case SVG_Color_datatype:
	{
		SVG_Color *col = (SVG_Color *)info->far_ptr;
		svg_dump_color(col, attValue);
	}
	    break;
	case SVG_Paint_datatype:
	{
		SVG_Paint *paint = (SVG_Paint *)info->far_ptr;
		if (paint->type == SVG_PAINT_NONE) strcpy(attValue, "none");
		else if (paint->type == SVG_PAINT_INHERIT) strcpy(attValue, "inherit");
		else if (paint->type == SVG_PAINT_URI) sprintf(attValue, "url(%s)", paint->uri);
		else svg_dump_color(paint->color, attValue);
	}
		break;

/* beginning of keyword type parsing */
	case SVG_FillRule_datatype:
		if (intVal == SVG_FILLRULE_INHERIT) strcpy(attValue, "inherit");
		else if (intVal == SVG_FILLRULE_NONZERO) strcpy(attValue, "nonzero");
		else strcpy(attValue, "evenodd");
		break;

	case SVG_StrokeLineJoin_datatype:
		if (intVal==SVG_STROKELINEJOIN_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_STROKELINEJOIN_MITER) strcpy(attValue, "miter");
		else if (intVal==SVG_STROKELINEJOIN_ROUND) strcpy(attValue, "round");
		else if (intVal==SVG_STROKELINEJOIN_BEVEL) strcpy(attValue, "bevel");
		break;
	case SVG_StrokeLineCap_datatype:
		if (intVal==SVG_STROKELINECAP_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_STROKELINECAP_BUTT) strcpy(attValue, "butt");
		else if (intVal==SVG_STROKELINECAP_ROUND) strcpy(attValue, "round");
		else if (intVal==SVG_STROKELINECAP_SQUARE) strcpy(attValue, "square");
		break;
	case SVG_FontStyle_datatype:
		if (intVal==SVG_FONTSTYLE_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_FONTSTYLE_NORMAL) strcpy(attValue, "normal");
		else if (intVal==SVG_FONTSTYLE_ITALIC) strcpy(attValue, "italic");
		else if (intVal==SVG_FONTSTYLE_OBLIQUE) strcpy(attValue, "oblique");
		break;
	case SVG_FontWeight_datatype:
		if (intVal==SVG_FONTWEIGHT_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_FONTWEIGHT_NORMAL) strcpy(attValue, "normal");
		else if (intVal==SVG_FONTWEIGHT_BOLD) strcpy(attValue, "bold");
		else if (intVal==SVG_FONTWEIGHT_BOLDER) strcpy(attValue, "bolder");
		else if (intVal==SVG_FONTWEIGHT_LIGHTER) strcpy(attValue, "lighter");
		else if (intVal==SVG_FONTWEIGHT_100) strcpy(attValue, "100");
		else if (intVal==SVG_FONTWEIGHT_200) strcpy(attValue, "200");
		else if (intVal==SVG_FONTWEIGHT_300) strcpy(attValue, "300");
		else if (intVal==SVG_FONTWEIGHT_400) strcpy(attValue, "400");
		else if (intVal==SVG_FONTWEIGHT_500) strcpy(attValue, "500");
		else if (intVal==SVG_FONTWEIGHT_600) strcpy(attValue, "600");
		else if (intVal==SVG_FONTWEIGHT_700) strcpy(attValue, "700");
		else if (intVal==SVG_FONTWEIGHT_800) strcpy(attValue, "800");
		else if (intVal==SVG_FONTWEIGHT_900) strcpy(attValue, "900");
		break;
	case SVG_FontVariant_datatype:
		if (intVal==SVG_FONTVARIANT_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_FONTVARIANT_NORMAL) strcpy(attValue, "normal");
		else if (intVal==SVG_FONTVARIANT_SMALLCAPS) strcpy(attValue, "small-caps");
		break;
	case SVG_TextAnchor_datatype:
		if (intVal==SVG_TEXTANCHOR_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_TEXTANCHOR_START) strcpy(attValue, "start");
		else if (intVal==SVG_TEXTANCHOR_MIDDLE) strcpy(attValue, "middle");
		else if (intVal==SVG_TEXTANCHOR_END) strcpy(attValue, "end");
		break;
	case SVG_Display_datatype:
		if (intVal==SVG_DISPLAY_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_DISPLAY_NONE) strcpy(attValue, "none");
		else if (intVal==SVG_DISPLAY_INLINE) strcpy(attValue, "inline");
		else if (intVal==SVG_DISPLAY_BLOCK) strcpy(attValue, "block");
		else if (intVal==SVG_DISPLAY_LIST_ITEM) strcpy(attValue, "list-item");
		else if (intVal==SVG_DISPLAY_RUN_IN) strcpy(attValue, "run-in");
		else if (intVal==SVG_DISPLAY_COMPACT) strcpy(attValue, "compact");
		else if (intVal==SVG_DISPLAY_MARKER) strcpy(attValue, "marker");
		else if (intVal==SVG_DISPLAY_TABLE) strcpy(attValue, "table");
		else if (intVal==SVG_DISPLAY_INLINE_TABLE) strcpy(attValue, "inline-table");
		else if (intVal==SVG_DISPLAY_TABLE_ROW_GROUP) strcpy(attValue, "table-row-group");
		else if (intVal==SVG_DISPLAY_TABLE_HEADER_GROUP) strcpy(attValue, "table-header-group");
		else if (intVal==SVG_DISPLAY_TABLE_FOOTER_GROUP) strcpy(attValue, "table-footer-group");
		else if (intVal==SVG_DISPLAY_TABLE_ROW) strcpy(attValue, "table-row");
		else if (intVal==SVG_DISPLAY_TABLE_COLUMN_GROUP) strcpy(attValue, "table-column-group");
		else if (intVal==SVG_DISPLAY_TABLE_COLUMN) strcpy(attValue, "table-column");
		else if (intVal==SVG_DISPLAY_TABLE_CELL) strcpy(attValue, "table-cell");
		else if (intVal==SVG_DISPLAY_TABLE_CAPTION) strcpy(attValue, "table-caption");
		break;
	case SVG_Visibility_datatype:
		if (intVal==SVG_VISIBILITY_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_VISIBILITY_VISIBLE) strcpy(attValue, "visible");
		else if (intVal==SVG_VISIBILITY_HIDDEN) strcpy(attValue, "hidden");
		else if (intVal==SVG_VISIBILITY_COLLAPSE) strcpy(attValue, "collapse");
		break;
	case SVG_Overflow_datatype:
		if (intVal==SVG_OVERFLOW_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_OVERFLOW_VISIBLE) strcpy(attValue, "visible");
		else if (intVal==SVG_OVERFLOW_HIDDEN) strcpy(attValue, "hidden");
		else if (intVal==SVG_OVERFLOW_SCROLL) strcpy(attValue, "scroll");
		else if (intVal==SVG_OVERFLOW_AUTO) strcpy(attValue, "auto");
		break;
	case SVG_ZoomAndPan_datatype:
		if (intVal==SVG_ZOOMANDPAN_DISABLE) strcpy(attValue, "disable");
		else strcpy(attValue, "magnify");
		break;
	case SVG_DisplayAlign_datatype:
		if (intVal==SVG_DISPLAYALIGN_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_DISPLAYALIGN_AUTO) strcpy(attValue, "auto");
		else if (intVal==SVG_DISPLAYALIGN_BEFORE) strcpy(attValue, "before");
		else if (intVal==SVG_DISPLAYALIGN_CENTER) strcpy(attValue, "center");
		else if (intVal==SVG_DISPLAYALIGN_AFTER) strcpy(attValue, "after");
		break;
	case SVG_PointerEvents_datatype:
		if (intVal==SVG_POINTEREVENTS_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_POINTEREVENTS_VISIBLEPAINTED) strcpy(attValue, "visiblePainted");
		else if (intVal==SVG_POINTEREVENTS_VISIBLEFILL) strcpy(attValue, "visibleFill");
		else if (intVal==SVG_POINTEREVENTS_VISIBLESTROKE) strcpy(attValue, "visibleStroke");
		else if (intVal==SVG_POINTEREVENTS_VISIBLE) strcpy(attValue, "visible");
		else if (intVal==SVG_POINTEREVENTS_PAINTED) strcpy(attValue, "painted");
		else if (intVal==SVG_POINTEREVENTS_FILL) strcpy(attValue, "fill");
		else if (intVal==SVG_POINTEREVENTS_STROKE) strcpy(attValue, "stroke");
		else if (intVal==SVG_POINTEREVENTS_ALL) strcpy(attValue, "all");
		else if (intVal==SVG_POINTEREVENTS_NONE) strcpy(attValue, "none");
		break;
	case SVG_RenderingHint_datatype:
		if (intVal==SVG_RENDERINGHINT_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_RENDERINGHINT_AUTO) strcpy(attValue, "auto");
		else if (intVal==SVG_RENDERINGHINT_OPTIMIZEQUALITY) strcpy(attValue, "optimizeQuality");
		else if (intVal==SVG_RENDERINGHINT_OPTIMIZESPEED) strcpy(attValue, "optimizeSpeed");
		else if (intVal==SVG_RENDERINGHINT_OPTIMIZELEGIBILITY) strcpy(attValue, "optimizeLegibility");
		else if (intVal==SVG_RENDERINGHINT_CRISPEDGES) strcpy(attValue, "crispEdges");
		else if (intVal==SVG_RENDERINGHINT_GEOMETRICPRECISION) strcpy(attValue, "geometricPrecision");
		break;
	case SVG_VectorEffect_datatype:
		if (intVal==SVG_VECTOREFFECT_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SVG_VECTOREFFECT_NONE) strcpy(attValue, "none");
		else if (intVal==SVG_VECTOREFFECT_NONSCALINGSTROKE) strcpy(attValue, "non-scaling-stroke");
		break;
	case SVG_PlaybackOrder_datatype:
		if (intVal== SVG_PLAYBACKORDER_FORWARDONLY) strcpy(attValue, "forwardOnly");
		else if (intVal== SVG_PLAYBACKORDER_ALL) strcpy(attValue, "all");
		break;
	case SVG_TimelineBegin_datatype:
		if (intVal== SVG_TIMELINEBEGIN_ONSTART) strcpy(attValue, "onStart");
		else if (intVal== SVG_TIMELINEBEGIN_ONLOAD) strcpy(attValue, "onLoad");
		break;
	case XML_Space_datatype:
		if (intVal==XML_SPACE_DEFAULT) strcpy(attValue, "default");
		else if (intVal==XML_SPACE_PRESERVE) strcpy(attValue, "preserve");
		break;
	case XMLEV_Propagate_datatype:
		if (intVal==XMLEVENT_PROPAGATE_CONTINUE) strcpy(attValue, "continue");
		else if (intVal==XMLEVENT_PROPAGATE_STOP) strcpy(attValue, "stop");
		break;
	case XMLEV_DefaultAction_datatype:
		if (intVal==XMLEVENT_DEFAULTACTION_CANCEL) strcpy(attValue, "cancel");
		else if (intVal==XMLEVENT_DEFAULTACTION_PERFORM) strcpy(attValue, "perform");
		break;
	case XMLEV_Phase_datatype:
		if (intVal==XMLEVENT_PHASE_DEFAULT) strcpy(attValue, "default");
		else if (intVal==XMLEVENT_PHASE_CAPTURE) strcpy(attValue, "capture");
		break;
	case SMIL_SyncBehavior_datatype:
		if (intVal==SMIL_SYNCBEHAVIOR_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SMIL_SYNCBEHAVIOR_DEFAULT) strcpy(attValue, "default");
		else if (intVal==SMIL_SYNCBEHAVIOR_LOCKED) strcpy(attValue, "locked");
		else if (intVal==SMIL_SYNCBEHAVIOR_CANSLIP) strcpy(attValue, "canSlip");
		else if (intVal==SMIL_SYNCBEHAVIOR_INDEPENDENT) strcpy(attValue, "independent");
		break;
	case SMIL_SyncTolerance_datatype:
		if (intVal==SMIL_SYNCTOLERANCE_INHERIT) strcpy(attValue, "inherit");
		else if (intVal==SMIL_SYNCTOLERANCE_DEFAULT) strcpy(attValue, "default");
		else if (intVal==SMIL_SYNCBEHAVIOR_LOCKED) {
			strcpy(attValue, "default");
			/*FIXME - DIMP SMIL TIMES*/
			//svg_parse_clock_value(value_string, &(value->value));
		}
		break;
	case SMIL_AttributeType_datatype:
		if (intVal==SMIL_ATTRIBUTETYPE_AUTO) strcpy(attValue, "auto");
		else if (intVal==SMIL_ATTRIBUTETYPE_XML) strcpy(attValue, "XML");
		else if (intVal==SMIL_ATTRIBUTETYPE_CSS) strcpy(attValue, "CSS");
		break;	
	case SMIL_CalcMode_datatype:
		if (intVal==SMIL_CALCMODE_DISCRETE) strcpy(attValue, "discrete");
		else if (intVal==SMIL_CALCMODE_LINEAR) strcpy(attValue, "linear");
		else if (intVal==SMIL_CALCMODE_PACED) strcpy(attValue, "paced");
		else if (intVal==SMIL_CALCMODE_SPLINE) strcpy(attValue, "spline");
		break;
	case SMIL_Additive_datatype:
		if (intVal==SMIL_ADDITIVE_REPLACE) strcpy(attValue, "replace");
		else if (intVal==SMIL_ADDITIVE_SUM) strcpy(attValue, "sum");
		break;
	case SMIL_Accumulate_datatype:
		if (intVal==SMIL_ACCUMULATE_NONE) strcpy(attValue, "none");
		else if (intVal==SMIL_ACCUMULATE_SUM) strcpy(attValue, "sum");
		break;
	case SMIL_Restart_datatype:
		if (intVal==SMIL_RESTART_ALWAYS) strcpy(attValue, "always");
		else if (intVal==SMIL_RESTART_WHENNOTACTIVE) strcpy(attValue, "whenNotActive");
		else if (intVal==SMIL_RESTART_NEVER) strcpy(attValue, "never");
		break;
	case SMIL_Fill_datatype:
		if (intVal==SMIL_FILL_FREEZE) strcpy(attValue, "freeze");
		else if (intVal==SMIL_FILL_REMOVE) strcpy(attValue, "remove");
		break;

	case SVG_GradientUnit_datatype:
		if (intVal==SVG_GRADIENTUNITS_USER) strcpy(attValue, "userSpaceOnUse");
		else if (intVal==SVG_GRADIENTUNITS_OBJECT) strcpy(attValue, "objectBoundingBox");
		break;
/* end of keyword type parsing */

	/* inheritable floats */
	case SVG_Opacity_datatype:
	case SVG_StrokeMiterLimit_datatype:
	case SVG_FontSize_datatype:
	case SVG_StrokeDashOffset_datatype:
	case SVG_AudioLevel_datatype:
	case SVG_LineIncrement_datatype:
	{
		SVGInheritableFloat *ft = (SVGInheritableFloat *)info->far_ptr;
		if (ft->type==SVG_FLOAT_INHERIT) strcpy(attValue, "inherit");
		else sprintf(attValue, "%g", ft->value);
	}
		break;

	/* inheritable float and unit */
	case SVG_StrokeWidth_datatype:
	case SVG_Length_datatype:
	case SVG_Coordinate_datatype:
		svg_dump_length((SVG_Length *)info->far_ptr, attValue);
		break;

	case SVG_IRI_datatype:
		svg_dump_iri((SVG_IRI*)info->far_ptr, attValue);
		break;
	case SVG_ListOfIRI_datatype:
	{
		GF_List *l = *(GF_List **)info->far_ptr;
		u32 i, count = gf_list_count(l);
		for (i=0; i<count; i++) {
			char szT[1024];
			SVG_IRI *iri = gf_list_get(l, i);
			svg_dump_iri(iri, szT);
			if (strlen(szT)) {
				if (strlen(attValue)) strcat(attValue, " ");
				strcat(attValue, szT);
			}
		}
	}
		break;

	case SVG_PathData_datatype:
		//svg_parse_path((SVG_PathData *)info->far_ptr, attribute_content);
		break;
	case SVG_Points_datatype:
	{
		GF_List *l = *(GF_List **) info->far_ptr;
		u32 i = 0;
		u32 count = gf_list_count(l);
		for (i=0; i<count; i++) {
			char szT[1000];
			SVG_Point *p = gf_list_get(l, i);
			sprintf(szT, "%g %g", FIX2FLT(p->x), FIX2FLT(p->x));
			if (i) strcat(attValue, " ");
			strcat(attValue, szT);
		}
	}
		break;
	case SMIL_KeyTimes_datatype:
	case SMIL_KeyPoints_datatype:
	case SMIL_KeySplines_datatype:
	{
		GF_List *l = *(GF_List **) info->far_ptr;
		u32 i = 0;
		u32 count = gf_list_count(l);
		for (i=0; i<count; i++) {
			char szT[1000];
			Fixed *p = gf_list_get(l, i);
			sprintf(szT, "%g", FIX2FLT(*p));
			if (i) strcat(attValue, " ");
			strcat(attValue, szT);
		}
	}
		break;
	case SVG_Coordinates_datatype:
	{
		GF_List *l = *(GF_List **) info->far_ptr;
		u32 i = 0;
		u32 count = gf_list_count(l);
		for (i=0; i<count; i++) {
			char szT[1000];
			SVG_Coordinate *p = gf_list_get(l, i);
			svg_dump_length((SVG_Length *)p, szT);
			if (i) strcat(attValue, " ");
			strcat(attValue, szT);
		}
	}
		break;
	case SVG_ViewBox_datatype:
	{
		SVG_ViewBox *v = (SVG_ViewBox *)info->far_ptr;
		sprintf(attValue, "%g %g %g %g", v->x, v->y, v->width, v->height);
	}
		break;
	case SVG_StrokeDashArray_datatype:
	{
		SVG_StrokeDashArray *p = (SVG_StrokeDashArray *)info->far_ptr;
		if (p->type==SVG_STROKEDASHARRAY_NONE) strcpy(attValue, "none");
		else if (p->type==SVG_STROKEDASHARRAY_INHERIT) strcpy(attValue, "inherit");
		else if (p->type==SVG_STROKEDASHARRAY_ARRAY) {
			u32 i = 0;
			for (i=0; i<p->array.count; i++) {
				char szT[1000];
				sprintf(szT, "%g", FIX2FLT(p->array.vals[i]));
				if (i) strcat(attValue, " ");
				strcat(attValue, szT);
			}
		}
	}
		break;
	case SVG_FontFamily_datatype:
	{
		SVG_FontFamily *f = (SVG_FontFamily *)info->far_ptr;
		strcpy(attValue, (f->type==SVG_FONTFAMILY_INHERIT) ? "inherit" : f->value);
	}
		break;
	case SVG_PreserveAspectRatio_datatype:
	{
		SVG_PreserveAspectRatio *par = (SVG_PreserveAspectRatio *)info->far_ptr;
		if (par->defer) strcat(attValue, "defer ");
		if (par->align == SVG_PRESERVEASPECTRATIO_NONE) strcat(attValue, "none");
		else if (par->align == SVG_PRESERVEASPECTRATIO_XMINYMIN) strcat(attValue, "xMinYMin");
		else if (par->align == SVG_PRESERVEASPECTRATIO_XMIDYMIN) strcat(attValue, "xMidYMin");
		else if (par->align == SVG_PRESERVEASPECTRATIO_XMAXYMIN) strcat(attValue, "xMaxYMin");
		else if (par->align == SVG_PRESERVEASPECTRATIO_XMINYMID) strcat(attValue, "xMinYMid");
		else if (par->align == SVG_PRESERVEASPECTRATIO_XMIDYMID) strcat(attValue, "xMidYMid");
		else if (par->align == SVG_PRESERVEASPECTRATIO_XMAXYMID) strcat(attValue, "xMaxYMid");
		else if (par->align == SVG_PRESERVEASPECTRATIO_XMINYMAX) strcat(attValue, "xMinYMax");
		else if (par->align == SVG_PRESERVEASPECTRATIO_XMIDYMAX) strcat(attValue, "xMidYMax");
		else if (par->align == SVG_PRESERVEASPECTRATIO_XMAXYMAX) strcat(attValue, "xMaxYMax");
		if (par->meetOrSlice== SVG_MEETORSLICE_SLICE) strcat(attValue, " slice");
	}
		break;
	case SVG_Number_datatype:
		sprintf(attValue, "%g", FIX2FLT( * (SVG_Number *)info->far_ptr ) );
		break;
	case SVG_Clock_datatype:
		sprintf(attValue, "%g", * (SVG_Clock *)info->far_ptr );
		break;

	/* required for animateMotion */
	case SVG_Motion_datatype:
	{
		SVG_Point *pt = (SVG_Point *)info->far_ptr;
		sprintf(attValue, "%g %g", FIX2FLT(pt->x), FIX2FLT(pt->y));
	}
		break;
	case SVG_Focus_datatype:
	case SVG_ID_datatype:
	case SVG_LinkTarget_datatype:
	case SVG_LanguageID_datatype:
	case SVG_GradientOffset_datatype:
	case SVG_String_datatype:
	case SVG_ContentType_datatype:
		if (*(SVG_String *)info->far_ptr) 
			strcpy(attValue, *(SVG_String *)info->far_ptr );
		break;


	/*not sure what we'll put in requiredFormats*/
	case SVG_FormatList_datatype:
	case SVG_LanguageIDs_datatype:
	case SVG_FontList_datatype:
	{
		GF_List *l1 = *(GF_List **) info->far_ptr;
		u32 i = 0;
		u32 count = gf_list_count(l1);
		for (i=0; i<count; i++) {
			char *p1 = gf_list_get(l1, i);
			if (strlen(attValue)) strcat(attValue, " ");
			strcat(attValue, p1);
		}
	}
		break;
	case SVG_Numbers_datatype:
	{
		GF_List *l1 = *(GF_List **) info->far_ptr;
		u32 i = 0;
		u32 count = gf_list_count(l1);
		for (i=0; i<count; i++) {
			char szT[1024];
			SVG_Number *p = gf_list_get(l1, i);
			sprintf(szT, "%g", FIX2FLT(*p) );
			if (i) strcat(attValue, " ");
			strcat(attValue, szT);
		}
	}
		break;
	case SVG_TransformList_datatype:
	{
		SVG_Transform *mat = gf_list_get(*(GF_List **) info->far_ptr, 0);
		if (mat) {
			/*try to do a simple decomposition...*/
			if (!mat->matrix.m[1] && !mat->matrix.m[3]) {
				sprintf(attValue, "translate(%g %g)", FIX2FLT(mat->matrix.m[2]), FIX2FLT(mat->matrix.m[5]) );
				if ((mat->matrix.m[0]!=FIX_ONE) || (mat->matrix.m[4]!=FIX_ONE)) {
					char szT[1024];
					sprintf(szT, " scale(%g %g)", FIX2FLT(mat->matrix.m[2]), FIX2FLT(mat->matrix.m[5]), FIX2FLT(mat->matrix.m[0]), FIX2FLT(mat->matrix.m[4]) );
					strcat(attValue, szT);
				}
			} else if (mat->matrix.m[1] == - mat->matrix.m[3]) {
				Fixed angle = gf_asin(mat->matrix.m[3]);
				Fixed cos_a = gf_cos(angle);
				if (cos_a) {
					Fixed sx, sy;
					sx = gf_divfix(mat->matrix.m[0], cos_a);
					sy = gf_divfix(mat->matrix.m[4], cos_a);
					angle = gf_divfix(180*angle, GF_PI);
					sprintf(attValue, "translate(%g %g) scale(%g %g) rotate(%g)", FIX2FLT(mat->matrix.m[2]), FIX2FLT(mat->matrix.m[5]), FIX2FLT(sx), FIX2FLT(sy), FIX2FLT(angle) );
				}
			} 
			/*default*/
			if (!strlen(attValue))
				sprintf(attValue, "matrix(%g %g %g %g %g %g)", FIX2FLT(mat->matrix.m[0]), FIX2FLT(mat->matrix.m[3]), FIX2FLT(mat->matrix.m[1]), FIX2FLT(mat->matrix.m[4]), FIX2FLT(mat->matrix.m[2]), FIX2FLT(mat->matrix.m[5]) );
		}
	}
		break;

	case SVG_TransformType_datatype:
	case SMIL_AnimateValue_datatype:
	case SMIL_AnimateValues_datatype:
	case SMIL_AttributeName_datatype:
	case SMIL_Times_datatype:
	case SMIL_Duration_datatype:
	case SMIL_RepeatCount_datatype:
	default:
		fprintf(stdout, "SVG: Warning, dumping for field %s not supported\n", info->name);
		break;
	}
	return GF_OK;
}

static Bool svg_comp_color(SVG_Color *c1, SVG_Color *c2)
{
	if (c1->type != c2->type) return 0;
	if (c1->red != c2->red) return 0;
	if (c1->green != c2->green) return 0;
	if (c1->blue != c2->blue) return 0;
	return 1;
}
static Bool svg_comp_length(SVG_Length *l1, SVG_Length *l2)
{
	if (l1->type!=l2->type) return 0;
	return (l1->number==l2->number) ? 1 : 0;
}
static Bool svg_comp_iri(SVG_IRI*iri1, SVG_IRI*iri2)
{
	if (iri1->type!=iri2->type) return 0;
	if ((iri1->type == SVG_IRI_ELEMENTID) && (iri1->target==iri2->target)) return 1;
	if (iri1->iri && iri2->iri && !strcmp(iri1->iri, iri2->iri)) return 1;
	return 0;
}

Bool svg_attribute_equals(GF_FieldInfo *f1, GF_FieldInfo *f2)
{
	u32 v1, v2;
	if (f1->fieldType!=f2->fieldType) return 0;
	if (f1->far_ptr && !f2->far_ptr) return 0;
	if (f2->far_ptr && !f1->far_ptr) return 0;
	v1 = *(u32 *)f1->far_ptr;
	v2 = *(u32 *)f2->far_ptr;

	switch (f1->fieldType) {
	case SVG_Boolean_datatype:
	case SVG_FillRule_datatype:
	case SVG_StrokeLineJoin_datatype:
	case SVG_StrokeLineCap_datatype:
	case SVG_FontStyle_datatype:
	case SVG_FontWeight_datatype:
	case SVG_FontVariant_datatype:
	case SVG_TextAnchor_datatype:
	case SVG_Display_datatype:
	case SVG_Visibility_datatype:
	case SVG_GradientUnit_datatype:
	case SVG_PreserveAspectRatio_datatype:
	case XML_Space_datatype:
	case XMLEV_Propagate_datatype:
	case XMLEV_DefaultAction_datatype:
	case XMLEV_Phase_datatype:
	case SMIL_SyncBehavior_datatype:
	case SMIL_SyncTolerance_datatype:
	case SMIL_AttributeType_datatype:
	case SMIL_CalcMode_datatype:
	case SMIL_Additive_datatype:
	case SMIL_Accumulate_datatype:
	case SMIL_Restart_datatype:
	case SMIL_Fill_datatype:
	case SVG_Overflow_datatype:
	case SVG_ZoomAndPan_datatype:
	case SVG_DisplayAlign_datatype:
	case SVG_PointerEvents_datatype:
	case SVG_RenderingHint_datatype:
	case SVG_VectorEffect_datatype:
	case SVG_PlaybackOrder_datatype:
	case SVG_TimelineBegin_datatype:

		return (v1==v2) ? 1 : 0;
	case SVG_Color_datatype:
		return svg_comp_color((SVG_Color *)f1->far_ptr, (SVG_Color *)f2->far_ptr);

	case SVG_Paint_datatype:
	{
		SVG_Paint *p1 = (SVG_Paint *)f1->far_ptr;
		SVG_Paint *p2 = (SVG_Paint *)f2->far_ptr;
		if (p1->type != p2->type) return 0;
		if (p1->type==SVG_PAINT_COLOR) return svg_comp_color(p1->color, p2->color);
		else if (p1->type==SVG_PAINT_URI) return (p1->uri && p2->uri && !strcmp(p1->uri, p2->uri)) ? 1 : 0;
		return 1;
	}
		break;

	/* inheritable floats */
	case SVG_Opacity_datatype:
	case SVG_StrokeMiterLimit_datatype:
	case SVG_FontSize_datatype:
	case SVG_StrokeDashOffset_datatype:
	case SVG_AudioLevel_datatype:
	case SVG_LineIncrement_datatype:
	{
		SVGInheritableFloat *if1 = (SVGInheritableFloat *)f1->far_ptr;
		SVGInheritableFloat *if2 = (SVGInheritableFloat *)f2->far_ptr;
		if (if1->type!=if2->type) return 0;
		return (if1->value==if2->value) ? 1 : 0;
	}
		break;

	/* inheritable float and unit */
	case SVG_StrokeWidth_datatype:
	case SVG_Length_datatype:
	case SVG_Coordinate_datatype:
		return svg_comp_length((SVG_Length *)f1->far_ptr, (SVG_Length *)f2->far_ptr);
	case SVG_IRI_datatype:
		return svg_comp_iri((SVG_IRI*)f1->far_ptr, (SVG_IRI*)f2->far_ptr);
	case SVG_ListOfIRI_datatype:
	{
		GF_List *l1 = *(GF_List **)f1->far_ptr;
		GF_List *l2 = *(GF_List **)f2->far_ptr;
		u32 i, count = gf_list_count(l1);
		if (gf_list_count(l2)!=count) return 0;
		for (i=0; i<count; i++) {
			if (!svg_comp_iri(gf_list_get(l1, i), gf_list_get(l2, i) )) return 0;
		}
		return 1;
	}

	case SVG_PathData_datatype:
	{
		SVG_PathData *d1 = (SVG_PathData *)f1->far_ptr;
		SVG_PathData *d2 = (SVG_PathData *)f2->far_ptr;
		if (!gf_list_count(d1->commands) && !gf_list_count(d2->commands)) return 1;
		/*FIXME - be less lazy..*/
		return 0;
	}
	case SVG_Points_datatype:
	{
		GF_List *l1 = *(GF_List **) f1->far_ptr;
		GF_List *l2 = *(GF_List **) f2->far_ptr;
		u32 i = 0;
		u32 count = gf_list_count(l1);
		if (gf_list_count(l2)!=count) return 0;
		for (i=0; i<count; i++) {
			SVG_Point *p1 = gf_list_get(l1, i);
			SVG_Point *p2 = gf_list_get(l2, i);
			if (p1->x != p2->x) return 0;
			if (p1->y != p2->y) return 0;
		}
		return 1;
	}
	case SMIL_KeyTimes_datatype:
	case SMIL_KeyPoints_datatype:
	case SMIL_KeySplines_datatype:
	{
		GF_List *l1 = *(GF_List **) f1->far_ptr;
		GF_List *l2 = *(GF_List **) f2->far_ptr;
		u32 i = 0;
		u32 count = gf_list_count(l1);
		if (gf_list_count(l2)!=count) return 0;
		for (i=0; i<count; i++) {
			Fixed *p1 = gf_list_get(l1, i);
			Fixed *p2 = gf_list_get(l2, i);
			if (*p1 != *p2) return 0;
		}
		return 1;
	}
	case SVG_Coordinates_datatype:
	{
		GF_List *l1 = *(GF_List **) f1->far_ptr;
		GF_List *l2 = *(GF_List **) f2->far_ptr;
		u32 i = 0;
		u32 count = gf_list_count(l1);
		if (gf_list_count(l2) != count) return 0;
		for (i=0; i<count; i++) {
			SVG_Coordinate *p1 = gf_list_get(l1, i);
			SVG_Coordinate *p2 = gf_list_get(l2, i);
			if (!svg_comp_length(p1, p2)) return 0;
		}
		return 1;
	}
	case SVG_ViewBox_datatype:
	{
		SVG_ViewBox *v1 = (SVG_ViewBox *)f1->far_ptr;
		SVG_ViewBox *v2 = (SVG_ViewBox *)f2->far_ptr;
		return gf_rect_equal(*(GF_Rect *)v1, *(GF_Rect *)v2);
	}
	case SVG_StrokeDashArray_datatype:
	{
		SVG_StrokeDashArray *p1 = (SVG_StrokeDashArray *)f1->far_ptr;
		SVG_StrokeDashArray *p2 = (SVG_StrokeDashArray *)f2->far_ptr;
		if (p1->type!=p2->type) return 0;
		if (p1->type==SVG_STROKEDASHARRAY_ARRAY) {
			u32 i = 0;
			if (p1->array.count != p2->array.count) return 0;
			for (i=0; i<p1->array.count; i++) {
				if (p1->array.vals[i] != p2->array.vals[i]) return 0;
			}
		}
		return 1;
	}
	case SVG_FontFamily_datatype:
	{
		SVG_FontFamily *ff1 = (SVG_FontFamily *)f1->far_ptr;
		SVG_FontFamily *ff2 = (SVG_FontFamily *)f2->far_ptr;
		if (ff1->type!=ff2->type) return 0;
		if (ff1->type==SVG_FONTFAMILY_INHERIT) return 1;
		return (ff1->value && ff2->value && !strcmp(ff1->value, ff2->value)) ? 1 : 0;
	}
	case SVG_Number_datatype:
		return (* (SVG_Number *)f1->far_ptr == * (SVG_Number *)f2->far_ptr) ? 1 : 0;

	case SVG_Clock_datatype:
		return (* (SVG_Clock *)f1->far_ptr == * (SVG_Clock *)f2->far_ptr) ? 1 : 0;

	/* required for animateMotion */
	case SVG_Motion_datatype:
	{
		SVG_Point *pt1 = (SVG_Point *)f1->far_ptr;
		SVG_Point *pt2 = (SVG_Point *)f2->far_ptr;
		if (pt1->x != pt2->x) return 0;
		if (pt1->y != pt2->y) return 0;
		return 1;
	}
		break;

	case SVG_Focus_datatype:
	case SVG_ID_datatype:
	case SVG_LinkTarget_datatype:
	case SVG_LanguageID_datatype:
	case SVG_GradientOffset_datatype:
	case SVG_String_datatype:
	case SVG_ContentType_datatype:
	{
		char *str1 = *(SVG_String *)f1->far_ptr;
		char *str2 = *(SVG_String *)f2->far_ptr;
		return (str1 && str2 && !strcmp(str1, str2)) ? 1 : 0;
	}
	case SVG_TransformType_datatype:
	case SVG_TransformList_datatype:
	case SMIL_AnimateValue_datatype:
	case SMIL_AnimateValues_datatype:
	case SMIL_AttributeName_datatype:
	case SMIL_Times_datatype:
	case SMIL_Duration_datatype:
	case SMIL_RepeatCount_datatype:
		return 0;

	/*not sure what we'll put in requiredFormats*/
	case SVG_FormatList_datatype:
	case SVG_LanguageIDs_datatype:
	case SVG_FontList_datatype:
	{
		GF_List *l1 = *(GF_List **) f1->far_ptr;
		GF_List *l2 = *(GF_List **) f2->far_ptr;
		u32 i = 0;
		u32 count = gf_list_count(l1);
		if (gf_list_count(l2) != count) return 0;
		for (i=0; i<count; i++) {
			char *p1 = gf_list_get(l1, i);
			char *p2 = gf_list_get(l2, i);
			if (strcmp(p1, p2)) return 0;
		}
		return 1;
	}
	case SVG_Numbers_datatype:
	{
		GF_List *l1 = *(GF_List **) f1->far_ptr;
		GF_List *l2 = *(GF_List **) f2->far_ptr;
		u32 i = 0;
		u32 count = gf_list_count(l1);
		if (gf_list_count(l2) != count) return 0;
		for (i=0; i<count; i++) {
			SVG_Number *p1 = gf_list_get(l1, i);
			SVG_Number *p2 = gf_list_get(l2, i);
			if (*p1 != *p2) return 0;
		}
		return 1;
	}
	default:
		fprintf(stdout, "SVG: Warning, comparaison for field %s not supported\n", f1->name);
		return 0;
	}
}


