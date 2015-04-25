/*
 * ConfigParser.cpp
 *
 *  Created on: May 10, 2014
 *      Author: Aakash Sahai
 */

#include "MegaBotics.h"
#include "ConfigParser.h"

/*
 * The Singleton and global Configuration File instance.
 */

ConfigParser::ConfigParser() {
	state = SECTION_BEGIN;
}

ConfigParser::~ConfigParser() {

}

int ConfigParser::Section::addParam(Param *param) {
	if (paramQty == MAX_PARAMS)
		return -1;
	params[paramQty] = param;
	paramQty++;

	return 0;
}

int ConfigParser::Config::addSection(Section *section) {
	if (sectionQty == MAX_SECTIONS)
		return -1;
	sections[sectionQty] = section;
	sectionQty++;
	DBG_PRINT ("Registering section: ");
	DBG_PRINTLN (section->name);

	return 0;
}

char * ConfigParser::skipWhiteSpace(char *in) {
	while ((*in == ' ' || *in == '\t' || *in == '\n' || *in == '\r') && *in != 0)
		in++;
	return in;
}

char * ConfigParser::skipTillWhiteOrChar(char *in, char c) {
	while (*in != ' ' && *in != '\t' && *in != '\n' && *in != '\r'  && *in != c && *in != 0)
		in++;
	return in;
}

char * ConfigParser::skipToChar(char *in, char c) {
	while (*in != c  && *in != 0)
		in++;
	return in;
}

char * ConfigParser::parse(Config &cfile, const char *input)
{
	char	*current, *start;
	unsigned long	len;
	Section *section;
	Param *param;
	int		i;

	current = (char *)input;

	while (*current != 0) {
		switch (state) {
		case SECTION_BEGIN:
			current = skipWhiteSpace(current);
			if (*current == 0)
				return 0;
			if (*current++ != '[')
				return current;
			start = current;
			current = skipToChar(current, ']');
			len = current - start;
			for (i = 0; i < cfile.sectionQty; i++) {
				section = cfile.sections[i];
				DBG_PRINT ("Matching section: ");
				DBG_PRINTLN (section->name);
				if (len == strlen(section->name) && strncmp(section->name, start, len) == 0) {
					DBG_PRINTLN ("Matched.");
					break;
				}
			}
			if (i == cfile.sectionQty) {
				return current;
			}
			current = skipToChar(current, '\n');
			state = PARAM_NAME;
			break;
		case PARAM_NAME:
			start = current = skipWhiteSpace(current);
			if (*current == 0)
				return 0;
			if (*current++ == '[') {
				current--;
				state = SECTION_BEGIN;
				break;
			}
			current = skipTillWhiteOrChar(current, '=');
			len = current - start;
			for (i = 0; i < section->paramQty; i++) {
				param = section->params[i];
				DBG_PRINT ("Matching param: ");
				DBG_PRINTLN (param->name);
				if (len == strlen(param->name) && strncmp(param->name, start, len) == 0) {
					DBG_PRINTLN ("Matched.");
					break;
				}
			}
			if (i == section->paramQty) {
				return current;
			}
			if (*current != '=')
				current = skipToChar(current, '=');
			current++;
			state = PARAM_VALUE;
			break;
		case PARAM_VALUE:
			start = current = skipWhiteSpace(current);
			current = skipTillWhiteOrChar(current, '\n');
			len = current - start;
			if (len == 0)
				return current;
			switch (param->type) {
			case 'C':
				*(section->target + param->offset) = *start;
				break;
			case 'I':
				sscanf(start, "%d", (int *)(section->target + param->offset));
				break;
			case 'F':
				double val = strtod(start, 0);
				*(float *)(section->target + param->offset) = (float)val;
				//sscanf(start, "%f", (float *)(section->target + param->offset));
				break;
			}
			current = skipToChar(current, '\n');
			if (*current != 0)
				current++;
			state = PARAM_NAME;
			break;
		}
	}

	return 0;
}
