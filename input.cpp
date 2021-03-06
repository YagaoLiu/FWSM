#include <iostream>
#include <string>
#include <cstring>
#include <getopt.h>

#include "defs.h"

static struct option long_options[] =
{
	{ "mode",				required_argument,	NULL,	'm' },
	{ "pattern",			required_argument,	NULL,	'p' },
	{ "text",				required_argument,	NULL,	't' },
	{ "output",				required_argument,	NULL,	'o' },
	{ "threshold",			required_argument,	NULL,	'z'	},
	{ "help",				0,					NULL,	'h' },
};

int decode_switches ( int argc, char * argv[], struct TSwitch * sw )
{
	int opt;
	char *ep;
	double val;
	int args;
	int mod;

	/* initialisation */
	sw -> z						=	1;
	sw -> mod					=	-1;

	args = 0;

	while ( ( opt = getopt_long ( argc, argv, "m:p:t:o:z:h", long_options, NULL ) ) != -1 )
	{
		switch ( opt )
		{
			case 'm':
				mod = strtol ( optarg, &ep, 10 );
				if ( optarg == ep )
				{
					return 0;
				}
				sw -> mod = mod;
				args ++;
				break;
			case 'p':
				sw -> pattern_file_name = optarg;
				args ++;
				break;
			case 't':
				sw -> text_file_name = optarg;
				args ++;
				break;
			case 'o':
				sw -> output_file_name = optarg;
				args ++;
				break;
			case 'z':
				val = strtod ( optarg, &ep );
				if ( optarg == ep )
				{
					return 0;
				}
				sw -> z = val;
				args ++;
				break;
			case 'h':
				return 0;
		}
	}

	if ( args < 5 )
	{
		usage();
		exit ( 1 );
	}
	else
		return ( optind );
}

void usage ( void )
{
	cout << "Usage: fwsm <options>"  << endl;
	cout << "Standard (Mandatory):" << endl;
	cout << "	-m, --mode\t<int>\tchoose the model for program.\n\t\t '1' for Weighted Pattern Match (WPM) \n\t\t '2' for Weighted Text Matching (WTM)" << endl;
	cout << "	-p, --pattern\t<str>\tFilename for Pattern." << endl;
	cout << "	-t, --text\t<str>\tFilename for Text." << endl;
	cout << "	-o, --output\t<str>\tFilename for output." << endl;
	cout << "	-z, --threshold\t <dbl>\tcumulative weight threshold."<<endl;
}






