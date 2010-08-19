#!/bin/bash

# WrightEagle (Soccer Simulation League 2D)                                        
# BASE SOURCE CODE RELEASE 2010                                                    
# Copyright (C) 1998-2010 WrightEagle 2D Soccer Simulation Team,                   
#                          Multi-Agent Systems Lab.,                               
#                          School of Computer Science and Technology,               
#                          University of Science and Technology of China, China.    
#                                                                                   
# This program is free software; you can redistribute it and/or                    
# modify it under the terms of the GNU General Public License                      
# as published by the Free Software Foundation; either version 2                   
# of the License, or (at your option) any later version.                           
#                                                                                  
# This program is distributed in the hope that it will be useful,                  
# but WITHOUT ANY WARRANTY; without even the implied warranty of                   
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    
# GNU General Public License for more details.                                     
#                                                                                  
#You should have received a copy of the GNU General Public License                
# along with this program; if not, write to the Free Software                      
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

echo ""
echo "#######################################################################"
echo "#      WrightEagle 2D Soccer Simulation Team -- based on WE2010       #"
echo "#                                                                     #"
echo "#                       Multi-Agent Systems Lab.                      #"
echo "#              School of Computer Science and Technology              #"
echo "#            University of Science and Technology of China            #"
echo "#                     Hefei, Anhui Province, China                    #"
echo "#                                                                     #"
echo "#                   Supervisor: Prof. Xiaoping Chen                   #"
echo "#       Team members: Aijun Bai, Jing Wang, Guanghui Lu               #"
echo "#                     Yuanchong Zhu, Haochong Zhang, Yuhang Wang      #"
echo "#                                                                     #"
echo "#                   WWW: http://wrighteagle.org/2D/                   #"
echo "#######################################################################"
echo ""

HOST="localhost"
VERSION="Release"

#$1:host $2:version
if [ ! -z $1 ]; then
	HOST=$1
	if [ ! -z $2 ]; then
		VERSION=$2
	fi
fi

BINARY="WE2010"
CLIENT="./$VERSION/$BINARY"
TEAM_NAME="WrightEagleBASE
LOG_DIR="Logfiles"
mkdir $LOG_DIR 2>/dev/null
SLEEP_TIME=0.1

make all

N_PARAM="-team_name $TEAM_NAME -host $HOST -log_dir $LOG_DIR"
G_PARAM="$N_PARAM -goalie on"
C_PARAM="$N_PARAM -coach on"

echo ">>>>>>>>>>>>>>>>>>>>>> $TEAM_NAME Goalie: 1"
$CLIENT $G_PARAM &
sleep 5

i=2
while [ $i -le 11 ]; do
	echo ">>>>>>>>>>>>>>>>>>>>>> $TEAM_NAME Player: $i"
	$CLIENT $N_PARAM &
	sleep $SLEEP_TIME
	i=`expr $i + 1`
done
sleep 3

echo ">>>>>>>>>>>>>>>>>>>>>> $TEAM_NAME Coach"
$CLIENT $C_PARAM &
