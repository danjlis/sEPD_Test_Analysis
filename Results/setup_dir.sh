for i in {0..31}
do
    if (( $i > 9 ))
    then
	dir="s$i"
        mkdir $dir
        cd $dir
        touch log.log
        mkdir plots
        cd plots
        touch log.log
        cd ..
        mkdir root_hist
        cd root_hist
        touch log.log
        cd ..
        mkdir root_raw
        cd root_raw
        touch log.log
        cd ../..
    else
	dir="s0$i"
	mkdir $dir
	cd $dir
	touch log.log
	mkdir plots
	cd plots
	touch log.log
	cd ..
	mkdir root_hist
	cd root_hist
	touch log.log
	cd ..
	mkdir root_raw
        cd root_raw
	touch log.log
	cd ../..
    fi
done

