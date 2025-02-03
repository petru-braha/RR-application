gen:
	gcc -std=c11 client.c -o cl
	cp extra/server_gen.c server.c
	gcc -std=c11 server.c -o sv

xml: /usr/include/libxml2
	gcc -std=c11 client.c -o cl
	cp extra/server_xml.c server.c
	g++ -std=c++11 include/dev/write_xml.cpp -I/usr/include/libxml2 -lxml2 -o include/dev/write_xml
	gcc -std=c11 server.c -I/usr/include/libxml2 -lxml2 -o sv

clean:
	rm -f **/cl **/sv **/*.out cl sv
	rm -f include/dev/write_xml
