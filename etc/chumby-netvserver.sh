#!/bin/sh

case "$1" in
	start)
	        # sanity check symlink and reset if it's bad
	        if [ ! -e /media/storage/docroot ]; then
		    mount -o remount,rw /
                    rm -f /www/netvserver
		    ln -s /usr/share/netvserver/docroot /www/netvserver
		    mount -o remount,ro /
		else 
		    if [ ! -e /media/storage/docroot/index.html ]; then
			mount -o remount,rw /
			rm -f /www/netvserver
			ln -s /usr/share/netvserver/docroot /www/netvserver
			mount -o remount,ro /
		    fi
	        fi

		# Check /psp/homepage and set docroot accordingly
		/usr/share/netvserver/docroot/scripts/psphomepage.sh >> /var/log/cron_psphomepage.log 2>&1

		# Start lighttpd if it's not started yet
		if [ -z "$(pidof lighttpd)" ]; then
			/etc/init.d/lighttpd start
		fi

		# start in the background so we don't hog the console
		NeTVServer > /dev/null 2>&1 &
		;;

	stop)
		killall NeTVServer
		;;

	restart)
		$0 stop
		sleep 1
		$0 start
		;;

	*)
		echo "Usage: $0 {start|stop|restart}"
		;;
esac
