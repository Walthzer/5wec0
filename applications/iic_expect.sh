#!/usr/bin/expect -f

set timeout -1
spawn scp iic_master/main.c pynq9:/home/student/libpynq/applications/iic_master;
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}
spawn scp iic_master/main.c pynq10:/home/student/libpynq/applications/iic_master;
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}
spawn scp iic_slave/main.c pynq9:/home/student/libpynq/applications/iic_slave;
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}
spawn scp iic_slave/main.c pynq10:/home/student/libpynq/applications/iic_slave;
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}