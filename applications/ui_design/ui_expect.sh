#!/usr/bin/expect -f
#exp_internal 1
set timeout -1
spawn scp -r ../ui_design libui.c libui.h pynq10:/home/student/libpynq/applications/;
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}



spawn ssh pynq10 "cd libpynq/applications/ui_design; make clean all; ./main";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}
