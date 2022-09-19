# WrightEagle (Soccer Simulation League 2D)
# BASE SOURCE CODE RELEASE 2016
# Copyright (C) 1998-2016 WrightEagle 2D Soccer Simulation Team,
# Multi-Agent Systems Lab.,
# School of Computer Science and Technology,
# University of Science and Technology of China, China.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

DEBUG = Debug
RELEASE = Release

first: debug

all: debug release

debug:
	cd ${DEBUG}; make -j3 all

release:
	cd ${RELEASE}; make -j3 all

clean:
	cd ${DEBUG}; make clean
	cd ${RELEASE}; make clean
	rm -f core*
