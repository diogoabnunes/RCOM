# How to compile

* VPN FEUP
* Connect to server netlab1.fe.up.pt
    * user: rcom
    * pass: rcom
* ssh up201808546@tom.fe.up.pt (and password) -> pinguim.fe.up.pt
* make clean && make
    * **./download ftp://rcom:rcom@netlab1.fe.up.pt/pipe.txt** (works!)
    * **./download ftp://rcom:rcom@netlab1.fe.up.pt/files/pic1.jpg** (not working: Error creating file, Error transfering file)
    * **./download ftp://rcom:rcom@netlab1.fe.up.pt/files/pic2.png** (not working: Error creating file, Error transfering file)