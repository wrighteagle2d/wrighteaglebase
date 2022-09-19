/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D) * BASE SOURCE CODE RELEASE 2016 *
 * Copyright (c) 1998-2016 WrightEagle 2D Soccer Simulation Team, * Multi-Agent
 *Systems Lab.,                                * School of Computer Science and
 *Technology,               * University of Science and Technology of China *
 * All rights reserved. *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without *
 * modification, are permitted provided that the following conditions are met: *
 *     * Redistributions of source code must retain the above copyright *
 *       notice, this list of conditions and the following disclaimer. *
 *     * Redistributions in binary form must reproduce the above copyright *
 *       notice, this list of conditions and the following disclaimer in the *
 *       documentation and/or other materials provided with the distribution. *
 *     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the *
 *       names of its contributors may be used to endorse or promote products *
 *       derived from this software without specific prior written permission. *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED    * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *PURPOSE ARE           * DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer
 *Simulation Team BE LIABLE    * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *EXEMPLARY, OR CONSEQUENTIAL       * DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *PROCUREMENT OF SUBSTITUTE GOODS OR       * SERVICES; LOSS OF USE, DATA, OR
 *PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       * CAUSED AND ON ANY THEORY OF
 *LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,    * OR TORT (INCLUDING
 *NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF * THIS SOFTWARE,
 *EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                *
 ************************************************************************************/

#include "Plotter.h"
#include "PlayerParam.h"

#include <cstdarg>
#include <cstdio>

Plotter::Plotter() : mIsDisplayOk(false), mIsGnuplotOk(false), mpGnupolot(0) {
  Init();
}

Plotter::~Plotter() { Close(); }

Plotter &Plotter::instance() {
  static Plotter plotter;
  return plotter;
}

void Plotter::Init() {
#ifndef WIN32
  mIsDisplayOk = getenv("DISPLAY") != 0;

  if (PlayerParam::instance().UsePlotter()) {
    mpGnupolot = popen("gnuplot", "w");
  }

  if (mpGnupolot) {
    if (mIsDisplayOk) {
      GnuplotExecute("set terminal x11");
    }
  }
#endif
}

void Plotter::Close() {
#ifndef WIN32
  if (mpGnupolot && pclose(mpGnupolot) == -1) {
    fprintf(stderr, "problem closing communication to gnuplot\n");
    return;
  }
#endif
}

void Plotter::GnuplotExecute(const char *cmd, ...) {
#ifndef WIN32
  if (!mpGnupolot)
    return;

  va_list ap;
  char local_cmd[GP_CMD_SIZE];

  va_start(ap, cmd);
  vsprintf(local_cmd, cmd, ap);
  va_end(ap);

  strcat(local_cmd, "\n");

  fputs(local_cmd, mpGnupolot);
  fflush(mpGnupolot);
#endif
}

void Plotter::SetXLabel(char *label) {
  char cmd[GP_CMD_SIZE];
  sprintf(cmd, "set xlabel \"%s\"", label);
  GnuplotExecute(cmd);
}

void Plotter::SetYLabel(char *label) {
  char cmd[GP_CMD_SIZE];
  sprintf(cmd, "set ylabel \"%s\"", label);
  GnuplotExecute(cmd);
}

void Plotter::Reset() { GnuplotExecute("reset"); }

void Plotter::PlotToFile(char *file_name) {
  GnuplotExecute("set terminal png");
  GnuplotExecute("set output Logfiles/\"%s\"", file_name);
}

void Plotter::PlotToDisplay() { GnuplotExecute("set termianl x11"); }
