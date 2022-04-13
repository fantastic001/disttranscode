#!/bin/bash
# set -x 

THIS_DIR="$(dirname $(readlink -f $0))"
REPO_DIR=$(dirname $THIS_DIR)

if [ ! -f $REPO_DIR/deploy_config.sh ]; then 
    echo "Configuration for deployment not found. Please create your deployment configuration and specify there cluster params."
    echo "vim $REPO_DIR/deploy_config.sh"
    cat << EOF


CLUSTER_PASSWORD="mypassword"
CLUSTER_HOSTNAME="192.168.2.2"
CLUSTER_USERNAME="stefan"


EOF
    exit 1
fi 

CLUSTER_PASSWORD=""
CLUSTER_HOSTNAME=""
CLUSTER_USERNAME=""

source $REPO_DIR/deploy_config.sh

HOME_DIRECTORY="~/stefan_nozinic/"

execute_on_cluster() {
    sleep 5
    echo "Executing $*"
    sshpass -p $CLUSTER_PASSWORD ssh $CLUSTER_USERNAME@$CLUSTER_HOSTNAME $*
}

deploy_files() {
    local DESTINATION=$1
    shift
    execute_on_cluster mkdir -p $HOME_DIRECTORY/$DESTINATION
    sleep 5
    echo "Deploying $*"
    sshpass -p $CLUSTER_PASSWORD scp $* $CLUSTER_USERNAME@$CLUSTER_HOSTNAME:$HOME_DIRECTORY/$DESTINATION
}

execute_on_cluster hostname 
execute_on_cluster rm -rf $HOME_DIRECTORY
execute_on_cluster mkdir -p $HOME_DIRECTORY

deploy_files lib  /usr/lib64/libav{codec,filter,util,device,format}*
deploy_files bin $REPO_DIR/disttranscode $REPO_DIR/generate_test_file