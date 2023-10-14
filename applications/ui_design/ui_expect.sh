#!/usr/bin/expect -f
#exp_internal 1
set timeout -1
spawn scp main.c pynq9:/home/student/libpynq/applications/ui_design;
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}



spawn ssh pynq9 "cd libpynq/applications/ui_design; make; ./main";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}