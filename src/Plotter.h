#ifndef PLOTTER_H_
#define PLOTTER_H_

/**
* refer to gnuplot_i.[ch]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef WIN32
#include <unistd.h>
#endif

#include <list>
#include <string>

class Plotter {
	Plotter();

public:
	virtual ~Plotter();
	static Plotter & instance();

	/**
	@brief    Sets the x label of a gnuplot session.
	@param    h Gnuplot session control handle.
	@param    label Character string to use for X label.
	@return   void

	Sets the x label for a gnuplot session.
	*/
	void SetXLabel(char * label);

	/**
	@brief    Sets the y label of a gnuplot session.
	@param    h Gnuplot session control handle.
	@param    label Character string to use for Y label.
	@return   void

	Sets the y label for a gnuplot session.
	*/
	void SetYLabel(char * label);


	/**
	@brief    Resets a gnuplot session (next plot will erase previous ones).
	@param    h Gnuplot session control handle.
	@return   void

	Resets a gnuplot session, i.e. the next plot will erase all previous
	ones.
	*/
	void Reset();

	/**
	@brief    Sends a command to an active gnuplot session.
	@param    handle Gnuplot session control handle
	@param    cmd    Command to send, same as a printf statement.

	This sends a string to an active gnuplot session, to be executed.
	There is strictly no way to know if the command has been
	successfully executed or not.
	The command syntax is the same as printf.

	Examples:

	@code
	gnuplot_cmd(g, "plot %d*x", 23.0);
	gnuplot_cmd(g, "plot %g * cos(%g * x)", 32.0, -3.0);
	@endcode

	Since the communication to the gnuplot process is run through
	a standard Unix pipe, it is only unidirectional. This means that
	it is not possible for this interface to query an error status
	back from gnuplot.
	*/
	void GnuplotExecute(const char *  cmd, ...);

	/**
	 * save to file_name which will be a png file
	 * @param file_name
	 */
	void PlotToFile(char * file_name);

	/**
	 * plot to display
	 */
	void PlotToDisplay();

private:
	void Init();
	void Close();

private:
	bool mIsDisplayOk;
	bool mIsGnuplotOk;

	FILE *mpGnupolot; //pointer to gnuplot process stdin

private:
	static const int GP_CMD_SIZE = 2048;
};

#endif /* PLOTTER_H_ */
