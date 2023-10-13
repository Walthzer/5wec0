expect -timeout -1 eof
expect -c 'spawn scp iic_test/main.c pynq9:/home/student/libpynq/applications/iic_test; expect "password:"; send "student";'
expect -c 'spawn scp iic_test/main.c pynq10:/home/student/libpynq/applications/iic_test; expect "password:"; send "student";'
expect -c 'spawn scp iic_slave/main.c pynq9:/home/student/libpynq/applications/iic_slave; expect "password:"; send "student";'
expect -c 'spawn scp iic_slave/main.c pynq10:/home/student/libpynq/applications/iic_slave; expect "password:"; send "student";'
expect -c 'spawn scp iic_slave/main.c pynq10:/home/student/libpynq/applications/iic_slave; expect "password:"; send "student";'
echo "Done!"


