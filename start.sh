#!/bin/bash

# WrightEagle (Soccer Simulation League 2D)                                       
# BASE SOURCE CODE RELEASE 2010                                                   
# Copyright (c) 1998-2010 WrightEagle 2D Soccer Simulation Team,                  
#                         Multi-Agent Systems Lab.,                               
#                         School of Computer Science and Technology,              
#                         University of Science and Technology of China           
# All rights reserved.                                                            
#                                                                                 
# Redistribution and use in source and binary forms, with or without              
# modification, are permitted provided that the following conditions are met:     
#     * Redistributions of source code must retain the above copyright            
#       notice, this list of conditions and the following disclaimer.             
#     * Redistributions in binary form must reproduce the above copyright         
#       notice, this list of conditions and the following disclaimer in the       
#       documentation and/or other materials provided with the distribution.      
#     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the     
#       names of its contributors may be used to endorse or promote products      
#       derived from this software without specific prior written permission.     
#                                                                                 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          
# DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer Simulation Team BE LIABLE   
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL      
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR      
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               

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
echo "#       Team members: Aijun Bai, Guanghui Lu, Haochong Zhang,         #"
echo "#                     Yuhang Wang, Miao Jiang                         #"
echo "#                                                                     #"
echo "#                 Homepage: http://wrighteagle.org/2D/                #"
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

BINARY="WrightEagleBASE"
CLIENT="./$VERSION/$BINARY"
TEAM_NAME="WrightEagleBASE"
LOG_DIR="Logfiles"
mkdir $LOG_DIR 2>/dev/null
SLEEP_TIME=0.1

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
