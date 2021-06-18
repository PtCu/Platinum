#include <iostream>

#include "scene.h"
#include "integrator.h"
#include "parser.h"

using namespace std;
using namespace platinum;

static void usage(const char *msg = nullptr) 
{
	if (msg)
	{
		fprintf(stderr, "Aurora: %s\n\n", msg);
	}

	fprintf(stderr, R"(usage: Aurora [<options>] <filename.json...>
Rendering options:
  --help               Print this help text.

Logging options:
  --logdir <dir>       Specify directory that log files should be written to.
                       Default: system temp directory (e.g. $TMPDIR or /tmp).

  --logtostderr        Print all logging messages to stderr.

  --minloglevel <num>  Log messages at or above this level (0 -> INFO,
                       1 -> WARNING, 2 -> ERROR, 3-> FATAL). Default: 0.

  --v <verbosity>      Set VLOG verbosity.
)");
	exit(msg ? 1 : 0);
}

int main(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);

	if (argc < 2)
	{
		usage("need to set an input file path");
		return 0;
	}

	std::vector<std::string> filenames;
	for (int i = 1; i < argc; ++i) 
	{
		if (!strcmp(argv[i], "--logdir") || !strcmp(argv[i], "-logdir")) 
		{
			if (i + 1 == argc)
				usage("missing value after --logdir argument");
			FLAGS_log_dir = argv[++i];
		}
		else if (!strncmp(argv[i], "--logdir=", 9)) 
		{
			FLAGS_log_dir = &argv[i][9];
		}
		else if (!strcmp(argv[i], "--minloglevel") ||
			!strcmp(argv[i], "-minloglevel")) 
		{
			if (i + 1 == argc)
				usage("missing value after --minloglevel argument");
			FLAGS_minloglevel = atoi(argv[++i]);
		}
		else if (!strncmp(argv[i], "--minloglevel=", 14)) 
		{
			FLAGS_minloglevel = atoi(&argv[i][14]);
		}
		else if (!strcmp(argv[i], "--v") || !strcmp(argv[i], "-v")) 
		{
			if (i + 1 == argc)
				usage("missing value after --v argument");
			FLAGS_v = atoi(argv[++i]);
		}
		else if (!strncmp(argv[i], "--v=", 4)) 
		{
			FLAGS_v = atoi(argv[i] + 4);
		}
		else if (!strcmp(argv[i], "--logtostderr")) 
		{
			FLAGS_logtostderr = true;
		}
		else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-help") ||
			!strcmp(argv[i], "-h")) 
		{
			usage();
			return 0;
		}
		else
		{
			filenames.push_back(argv[i]);
		}
	}

	//Banner
	{
		printf("Aurora (built %s at %s) [Detected %d cores]\n", __DATE__, __TIME__, numSystemCores());
		printf("Copyright (c)2021-Present Wencong Yang\n");
		printf("The source code to Aurora is covered by the MIT License.\n");
		printf("See the file LICENSE.txt for the conditions of the license.\n");
	}

	
	return 0;
}