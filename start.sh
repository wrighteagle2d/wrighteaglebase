#!/bin/bash

echo ""
echo "#######################################################################"
echo "#           WrightEagle 2D Soccer Simulation Team -- WE2010           #"
echo "#                                                                     #"
echo "#                       Multi-Agent Systems Lab.                      #"
echo "#              School of Computer Science and Technology              #"
echo "#            University of Science and Technology of China            #"
echo "#                     Hefei, Anhui Province, China                    #"
echo "#                                                                     #"
echo "#                   Supervisor: Prof. Xiaoping Chen                   #"
echo "#       Team members: Ke Shi, Aijun Bai, Yuhang Wang, Guanghui Lu     #"
echo "#               Haochong Zhang, Yuanchong Zhu, Song Luan              #"
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
TEAM_NAME="WE2010"
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
