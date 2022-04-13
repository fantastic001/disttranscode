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
    sleep 10
    echo "Executing $*"
    sshpass -p $CLUSTER_PASSWORD ssh $CLUSTER_USERNAME@$CLUSTER_HOSTNAME $*
}

deploy_files() {
    local DESTINATION=$1
    shift
    sleep 10
    echo "Deploying $*"
    sshpass -p $CLUSTER_PASSWORD scp -r $* $CLUSTER_USERNAME@$CLUSTER_HOSTNAME:$HOME_DIRECTORY/$DESTINATION
}

execute_on_cluster mkdir -p $HOME_DIRECTORY $HOME_DIRECTORY/bin $HOME_DIRECTORY/lib
deploy_files lib  \
    /usr/lib/libhwloc.so* \
    /usr/lib/libevent_core-2.1.so* \
    /usr/lib/libevent_pthreads-2.1.so* \
    /usr/lib/libraw1394.so* \
    /usr/lib/libavc1394.so* \
    /usr/lib/librom1394.so* \
    /usr/lib/libiec61883.so* \
    /usr/lib/libjack.so* \
    /usr/lib/libdrm.so* \
    /usr/lib/libxcb.so* \
    /usr/lib/libxcb-shm.so* \
    /usr/lib/libxcb-shape.so* \
    /usr/lib/libxcb-xfixes.so* \
    /usr/lib/libasound.so* \
    /usr/lib/libpulse.so* \
    /usr/lib/libSDL2-2.0.so* \
    /usr/lib/libv4l2.so* \
    /usr/lib/libXv.so* \
    /usr/lib/libX11.so* \
    /usr/lib/libXext.so* \
    /usr/lib/libpostproc.so* \
    /usr/lib/libfribidi.so* \
    /usr/lib/libvmaf.so* \
    /usr/lib/libass.so* \
    /usr/lib/libva.so* \
    /usr/lib/libvidstab.so* \
    /usr/lib/libzimg.so* \
    /usr/lib/libfontconfig.so* \
    /usr/lib/libfreetype.so* \
    /usr/lib/libmfx.so* \
    /usr/lib/libxml2.so* \
    /usr/lib/libbz2.so* \
    /usr/lib/libmodplug.so* \
    /usr/lib/libbluray.so* \
    /usr/lib/libgmp.so* \
    /usr/lib/libgnutls.so* \
    /usr/lib/libsrt.so* \
    /usr/lib/libvpx.so* \
    /usr/lib/libwebpmux.so* \
    /usr/lib/libwebp.so* \
    /usr/lib/liblzma.so* \
    /usr/lib/libdav1d.so* \
    /usr/lib/libopencore-amrwb.so* \
    /usr/lib/librsvg-2.so* \
    /usr/lib/libgobject-2.0.so* \
    /usr/lib/libglib-2.0.so* \
    /usr/lib/libcairo.so* \
    /usr/lib/libaom.so* \
    /usr/lib/libgsm.so* \
    /usr/lib/libmp3lame.so* \
    /usr/lib/libopencore-amrnb.so* \
    /usr/lib/libopenjp2.so* \
    /usr/lib/libopus.so* \
    /usr/lib/librav1e.so* \
    /usr/lib/libspeex.so* \
    /usr/lib/libSvtAv1Enc.so* \
    /usr/lib/libtheoraenc.so* \
    /usr/lib/libtheoradec.so* \
    /usr/lib/libvorbis.so* \
    /usr/lib/libvorbisenc.so* \
    /usr/lib/libx264.so* \
    /usr/lib/libx265.so* \
    /usr/lib/libxvidcore.so* \
    /usr/lib/libsoxr.so* \
    /usr/lib/libva-drm.so* \
    /usr/lib/libva-x11.so* \
    /usr/lib/libvdpau.so* \
    /usr/lib/libdb-5.3.so* \
    /usr/lib/libXau.so* \
    /usr/lib/libXdmcp.so* \
    /usr/lib/pulseaudio/libpulsecommon-15.0.so* \
    /usr/lib/libdbus-1.so* \
    /usr/lib/libv4lconvert.so* \
    /usr/lib/libharfbuzz.so* \
    /usr/lib/libgomp.so* \
    /usr/lib/libexpat.so* \
    /usr/lib/libpng16.so* \
    /usr/lib/libbrotlidec.so* \
    /usr/lib/libicuuc.so* \
    /usr/lib/libp11-kit.so* \
    /usr/lib/libidn2.so* \
    /usr/lib/libtasn1.so* \
    /usr/lib/libnettle.so* \
    /usr/lib/libhogweed.so* \
    /usr/lib/libcrypto.so* \
    /usr/lib/libcairo-gobject.so* \
    /usr/lib/libgdk_pixbuf-2.0.so* \
    /usr/lib/libgio-2.0.so* \
    /usr/lib/libpangocairo-1.0.so* \
    /usr/lib/libpango-1.0.so* \
    /usr/lib/libffi.so* \
    /usr/lib/libpcre.so* \
    /usr/lib/libXrender.so* \
    /usr/lib/libxcb-render.so* \
    /usr/lib/libpixman-1.so* \
    /usr/lib/libogg.so* \
    /usr/lib/libmvec.so* \
    /usr/lib/libXfixes.so* \
    /usr/lib/libsndfile.so* \
    /usr/lib/libasyncns.so* \
    /usr/lib/libjpeg.so* \
    /usr/lib/libgraphite2.so* \
    /usr/lib/libbrotlicommon.so* \
    /usr/lib/libicudata.so* \
    /usr/lib/libgmodule-2.0.so* \
    /usr/lib/libtiff.so* \
    /usr/lib/libpangoft2-1.0.so* \
    /usr/lib/libthai.so* \
    /usr/lib/libFLAC.so* \
    /usr/lib/libzstd.so* \
    /usr/lib/liblz4.so* \
    /usr/lib/libgcrypt.so* \
    /usr/lib/libblkid.so* \
    /usr/lib/libdatrie.so* \
    /usr/lib/libgpg-error.so* \
    /usr/lib64/libswresample.so* \
    /usr/lib64/libswscale.so* \
    /usr/lib64/openmpi/ \
    /usr/lib64/libav{codec,filter,util,device,format}*
deploy_files bin $REPO_DIR/disttranscode $REPO_DIR/generate_test_file