To create the executable file, simply use the complileall script:

./complileall


Then, either run the testscript:

p5testscript port1 port2 > result_text_file 2>&1

---OR---

run each keygen like so:

./keygen <num> > key_name

each server like so:

./enc_server port1 &
./dec_server port2 &

and each client like so:

./enc_client <message> key_name port1 > enc_text
./dec_client <message> key_name port2 > dec_text
