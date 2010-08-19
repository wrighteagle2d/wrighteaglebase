#include "Plotter.h"
#include "PlayerParam.h"

#include <cstdio>

Plotter::Plotter():
	mIsDisplayOk(false),
	mIsGnuplotOk(false),
	mpGnupolot(0)
{
	Init();
}

Plotter::~Plotter() {
	Close();
}

Plotter & Plotter::instance()
{
	static Plotter plotter;
	return plotter;
}

void Plotter::Init()
{
#ifndef WIN32
	mIsDisplayOk = getenv("DISPLAY") != 0;

	if (PlayerParam::instance().UsePlotter()) {
		mpGnupolot = popen("gnuplot", "w");
	}

	if (mpGnupolot){
		if (mIsDisplayOk) {
			GnuplotExecute("set terminal x11");
		}
	}
#endif
}

void Plotter::Close()
{
#ifndef WIN32
    if (mpGnupolot && pclose(mpGnupolot) == -1) {
        fprintf(stderr, "problem closing communication to gnuplot\n") ;
        return ;
    }
#endif
}

void Plotter::GnuplotExecute(const char *  cmd, ...)
{
#ifndef WIN32
	if (!mpGnupolot) return;

    va_list ap;
    char    local_cmd[GP_CMD_SIZE];

    va_start(ap, cmd);
    vsprintf(local_cmd, cmd, ap);
    va_end(ap);

    strcat(local_cmd, "\n");

    fputs(local_cmd, mpGnupolot);
    fflush(mpGnupolot);
#endif
}

void Plotter::SetXLabel(char * label)
{
    char    cmd[GP_CMD_SIZE];
    sprintf(cmd, "set xlabel \"%s\"", label);
    GnuplotExecute(cmd);
}

void Plotter::SetYLabel(char * label)
{
    char    cmd[GP_CMD_SIZE];
    sprintf(cmd, "set ylabel \"%s\"", label);
    GnuplotExecute(cmd);
}

void Plotter::Reset()
{
	GnuplotExecute("reset");
}

void Plotter::PlotToFile(char * file_name)
{
	GnuplotExecute("set terminal png");
	GnuplotExecute("set output Logfiles/\"%s\"", file_name);
}

void Plotter::PlotToDisplay()
{
	GnuplotExecute("set termianl x11");
}
