#!/usr/bin/expect -f

set timeout -1
# spawn bash -c "scp -r com_*/ pynq7:/home/student/5wec0/applications/";
# set pass "student"
# expect {
#         password: {send "$pass\r" ; exp_continue}
# }

# spawn bash -c "scp -r ../library/ pynq7:/home/student/5wec0/";
# set pass "student"
# expect {
#         password: {send "$pass\r" ; exp_continue}
# }

spawn bash -c "scp -r com_*/ pynq9:/home/student/libpynq/applications/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}

spawn bash -c "scp -r ../library/libui* ../library/libcom* pynq9:/home/student/libpynq/library/";
set pass "student"
expect {
        password: {send "$pass\r" ; exp_continue}
}

#spawn ssh pynq9 "cd libpynq/applications/com_motors; make clean all; sudo ./main";
#set pass "student"
#expect {
#        password: {send "$pass\r" ; exp_continue}
#}