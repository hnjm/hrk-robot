source [file dirname [info script]]/run.tcl
set mac "00:14:01:13:90:AC"
proc hrkblue { { arg0 help } args } {
    global mac
    switch $arg0 {
	open {
	    run sudo hciconfig hci0 up
	    # run sudo hciconfig 0 sspmode 0
	    run sudo hcitool scan
	    catch {
		run sudo killall -9 bt-agent
	    }
	    set c [list sudo bt-agent -p [file dirname [info script]]/hrkblue.txt]
	    puts stderr $c
	    exec {*}$c >@ stdout 2>@ stderr <@ stdin &
	    after 500
	    catch {
		run sudo rfcomm release 0
	    }
	    if {[catch {
		set fp [open |[list bluetoothctl] wb]
		puts $fp "remove $mac"
		after 500
		close $fp
	    } err]} {
		puts stderr "Warning: $err"
	    }
	    run sudo rfcomm bind 0 $mac 1
	    run sudo mv /dev/rfcomm0 /dev/arduino
	    # -r -M -A 
	    # run sudo rfcomm connect 0 $mac 1
	}
	server {
	    exec sudo xterm -e bluetoothd --compat --noplugin=* --nodetach --debug=* \
		2>/dev/null &
	    # sap,pnat
	}
    }
}
if { [info script] eq $::argv0 } {
    if {[catch {
	hrkblue	{*}$argv
    } err]} {
	puts stderr "hrkblue: $err"
	exit 1
    }
}
