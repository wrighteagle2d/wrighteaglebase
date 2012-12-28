#!/bin/bash

echo ""
echo "#######################################################################"
echo "#              WrightEagle 2D Soccer Simulation Team                  #"
echo "#                                                                     #"
echo "#                       Multi-Agent Systems Lab.                      #"
echo "#              School of Computer Science and Technology              #"
echo "#            University of Science and Technology of China            #"
echo "#                     Hefei, Anhui Province, China                    #"
echo "#                                                                     #"
echo "#                   Supervisor: Prof. Xiaoping Chen                   #"
echo "#   Team members: Aijun Bai, Guanghui Lu, Haochong Zhang, Miao Jiang  #"
echo "#                                                                     #"
echo "#                   WWW: http://wrighteagle.org/2D/                   #"
echo "#######################################################################"
echo ""

HOST="127.0.0.1"
PORT="6000"
VERSION="Release"
BINARY="WrightEagleBASE"
TEAM_NAME="WrightEagleBASE"
OPP_DIR="/home/test/src"
SELF_DIR=`pwd`
PLAYER_SEED=-1
TRAIN_DIR="./train"

while getopts  "h:p:v:b:t:s:" flag; do
    case "$flag" in
        p) PORT=$OPTARG;;
        v) VERSION=$OPTARG;;
        b) BINARY=$OPTARG;;
        t) TEAM_NAME=$OPTARG;;
	s) PLAYER_SEED=$OPTARG
    esac
done

COACH_PORT=`expr $PORT + 1`
OLCOACH_PORT=`expr $PORT + 2`

flag="false"
while read myline  
do  
	OLD_IFS="$IFS"
	IFS=")("
	arr=($myline)
	IFS="$OLD_IFS"
	for s in ${arr[@]}
	do
		if [ $flag = "true" ]; then
			PLAYER_SEED=$s
			echo "Random seed is $PLAYER_SEED"
			break
		fi

		if [ $s = "random_seed" ]; then
			flag="true"
		fi
	done
	if [ $flag = "true" ]; then
		break
	fi
done < $TRAIN_DIR/demo.rcg

cd $TRAIN_DIR

OPTIONS="$OPTIONS -server::port=$PORT"
OPTIONS="$OPTIONS -server::coach_port=$COACH_PORT"
OPTIONS="$OPTIONS -server::olcoach_port=$OLCOACH_PORT"
OPTIONS="$OPTIONS -player::random_seed=$PLAYER_SEED -server::coach=true -server::coach_w_referee=true"

rcssserver $OPTIONS &

cd $SELF_DIR

sleep 1

rcssmonitor --server-host $HOST --server-port $PORT &

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

N_PARAM="-team_name $TEAM_NAME -host $HOST -port $PORT -coach_port $COACH_PORT -olcoach_port $OLCOACH_PORT -log_dir $LOG_DIR"
G_PARAM="$N_PARAM -goalie on"
C_PARAM="$N_PARAM -coach on"
T_PARAM="$N_PARAM -trainer on"

echo ">>>>>>>>>>>>>>>>>>>>>> $TEAM_NAME Goalie: 1"
$CLIENT $G_PARAM &
sleep 1

i=2
while [ $i -le 11 ]; do
	echo ">>>>>>>>>>>>>>>>>>>>>> $TEAM_NAME Player: $i"
	$CLIENT $N_PARAM &
	sleep $SLEEP_TIME
	i=`expr $i + 1`
done

sleep 1
cd $OPP_DIR
./start.sh >/dev/null &
sleep 1
cd $SELF_DIR


echo ">>>>>>>>>>>>>>>>>>>>>> $TEAM_NAME Coach"
$CLIENT $C_PARAM &

sleep 1

echo ">>>>>>>>>>>>>>>>>>>>>> $TEAM_NAME Trainer"
$CLIENT $T_PARAM &

wait
