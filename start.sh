#!/bin/bash

# WrightEagle (Soccer Simulation League 2D)                                       
# BASE SOURCE CODE RELEASE 2013                                                   
# Copyright (c) 1998-2013 WrightEagle 2D Soccer Simulation Team,                  
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

HOST="localhost"
PORT="6000"
VERSION="Release"
BINARY="WEBase"
TEAM_NAME="WEBase"

while getopts  "h:p:v:b:t:" flag; do
    case "$flag" in
        h) HOST=$OPTARG;;
        p) PORT=$OPTARG;;
        v) VERSION=$OPTARG;;
        b) BINARY=$OPTARG;;
        t) TEAM_NAME=$OPTARG;;
    esac
done

if [ $VERSION = "Debug" ]; then
    ulimit -c unlimited
    make debug
else
    make release
fi

CLIENT="./$VERSION/$BINARY"
LOG_DIR="Logfiles"
mkdir $LOG_DIR 2>/dev/null
SLEEP_TIME=0.1

COACH_PORT=`expr $PORT + 1`
OLCOACH_PORT=`expr $PORT + 2`
N_PARAM="-team_name $TEAM_NAME -host $HOST -port $PORT -coach_port $COACH_PORT -olcoach_port $OLCOACH_PORT -log_dir $LOG_DIR"
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

wait

