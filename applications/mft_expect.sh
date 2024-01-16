#!/usr/bin/expect -f

set timeout -1
spawn bash -c "scp -r mft_*/ student@10.43.0.7:/home/student/5wec0/applications/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}
spawn bash -c "scp -r student@10.43.0.7:/home/student/5wec0/applications/mft_crying/*.csv data_crying/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}