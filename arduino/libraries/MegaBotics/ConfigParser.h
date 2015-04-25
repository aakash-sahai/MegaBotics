/*
 * ConfigParser.h
 *
 *  Created on: May 11, 2014
 *      Author: Aakash Sahai
 */

#ifndef MEGABOTICS_CONFIGPARSER_H_
#define MEGABOTICS_CONFIGPARSER_H_

#define	CONF_PARAM_CHAR		0
#define	CONF_PARAM_FLOAT	1
#define	CONF_PARAM_INT16	2
#define	CONF_PARAM_INT32	3

#define MAX_SECTIONS	10
#define MAX_PARAMS		20


class ConfigParser {
public:
	struct Param {
		const char	*name;
		char	type;
		char	offset;

		Param(const char *n, char t, char o):name(n),type(t),offset(o) { }
	};

	struct Section {
		const char *name;
		int paramQty;
		char *target;
		struct Param *params[MAX_PARAMS];

		Section(const char *n, char *t):name(n), target(t) { paramQty = 0; }

		int addParam(Param *param);
	};

	struct Config {
		int	sectionQty;
		struct Section *sections[MAX_SECTIONS];

		Config() { sectionQty = 0; }

		int addSection(Section *section);
	};

	ConfigParser();
	virtual ~ConfigParser();
	char * parse(Config &file, const char *input);

private:
	enum ParserState {
		SECTION_BEGIN = 1,
		PARAM_NAME = 2,
		PARAM_VALUE = 3
	};
	ParserState state;
	static char *skipWhiteSpace(char *in);
	static char *skipTillWhiteOrChar(char *in, char c);
	static char *skipToChar(char *in, char c);
};

#endif /* MEGABOTICS_CONFIGPARSER_H_ */
