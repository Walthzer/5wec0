#!/usr/bin/expect -f

set timeout -1
spawn bash -c "scp -r com_*/ student@10.43.0.1:/home/student/libpynq/applications/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}

spawn bash -c "scp -r ../library/libcom* student@10.43.0.1:/home/student/libpynq/library/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}

spawn ssh student@10.43.0.1 "cd libpynq; make install;";
set pass "student"
expect {
       password: {send "$pass\r" ; exp_continue}
}