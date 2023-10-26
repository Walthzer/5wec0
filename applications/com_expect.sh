#!/usr/bin/expect -f

set timeout -1
spawn bash -c "scp -r com_*/ pynq10:/home/student/libpynq/applications/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}

spawn bash -c "scp -r ../library/libcom*  pynq10:/home/student/libpynq/library/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}

spawn bash -c "scp -r com_*/ pynq9:/home/student/libpynq/applications/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}

spawn bash -c "scp -r ../library/libcom* pynq9:/home/student/libpynq/library/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}

spawn ssh pynq9 "cd libpynq; make install;";
set pass "student"
expect {
       password: {send "$pass\r" ; exp_continue}
}

spawn ssh pynq10 "cd libpynq; make install;";
set pass "student"
expect {
       password: {send "$pass\r" ; exp_continue}
}