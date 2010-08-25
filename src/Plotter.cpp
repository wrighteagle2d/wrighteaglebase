/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2010                                                    *
 * Copyright (C) 1998-2010 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China, China.    *
 *                                                                                  *
 * This program is free software; you can redistribute it and/or                    *
 * modify it under the terms of the GNU General Public License                      *
 * as published by the Free Software Foundation; either version 2                   *
 * of the License, or (at your option) any later version.                           *
 *                                                                                  *
 * This program is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 * GNU General Public License for more details.                                     *
 *                                                                                  *
 * You should have received a copy of the GNU General Public License                *
 * along with this program; if not, write to the Free Software                      *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.  *
 ************************************************************************************/

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
