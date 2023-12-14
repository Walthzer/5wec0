#!/usr/bin/expect -f

set timeout -1
spawn bash -c "scp -r mft_*/ student@10.43.0.9:/home/student/libpynq/applications/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}
spawn bash -c "scp -r student@10.43.0.9:/home/student/libpynq/applications/mft_heartbeat/*.csv mft_heartbeat/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}