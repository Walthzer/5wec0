#!/usr/bin/expect -f

set timeout -1
spawn bash -c "scp -r mft_*/ student@10.43.0.10:/home/student/libpynq/applications/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}