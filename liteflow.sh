#!/bin/sh
### BEGIN INIT INFO
# Provides:          liteflow
# Required-Start:    $syslog
# Required-Stop:     $syslog
# Should-Start:      $local_fs
# Should-Stop:       $local_fs
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: Monitor for liteflow activity
# Description:       LiteFlow Port Forwarder
### END INIT INFO

# **NOTE** bash will exit immediately if any command exits with non-zero.
set -e

PACKAGE_NAME=liteflow
PACKAGE_DESC="LiteFlow Port Forwarder"
PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:${PATH}

HOME_PATH=/home/pi/liteflow/src
BIN="./liteflow"

start() {
    echo -n "Starting ${PACKAGE_DESC}: "
	ulimit -n 65536
    if [ ! -d /etc/logrotate.d ]; then
		cd $HOME_PATH
        nohup /usr/bin/env TZ=Asia/Shanghai $BIN 2>&1 | /usr/bin/logger -t ${PACKAGE_NAME} &
    else
        if [ ! -f /etc/logrotate.d/liteflow ]; then
            cat > /etc/logrotate.d/liteflow <<EOF
/var/log/liteflow.log {
    daily
    rotate 7
    size=100k
    compress
	copytruncate
    missingok
    notifempty
    nocreate
    postrotate
    endscript
}
EOF
        fi
		cd $HOME_PATH
        nohup /usr/bin/env TZ=Asia/Shanghai $BIN 2>&1 >> /var/log/liteflow.log &
    fi
    echo "${PACKAGE_NAME}."
}

stop() {
    echo -n "Stopping ${PACKAGE_DESC}: "
    kill -9 `ps aux | grep 'liteflow' | grep -v '/bin/sh' | grep -v grep | awk '{print $2}'` >/dev/null 2>&1 || true
    echo "${PACKAGE_NAME}."
}

restart() {
    stop || true
    sleep 1
    start
}

usage() {
    N=$(basename "$0")
    echo "Usage: [sudo] $N {start|stop|restart}" >&2
    exit 1
}

if [ "$(id -u)" != "0" ]; then
    echo "please use sudo to run ${PACKAGE_NAME}"
    exit 0
fi

# `readlink -f` won't work on Mac, this hack should work on all systems.

case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    #reload)
    restart | force-reload)
        restart
        ;;
    *)
        usage
        ;;
esac

exit 0
