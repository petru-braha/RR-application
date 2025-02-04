txt:
	gcc client.c -std=c11 -o cl
	cp extra/server_txt.c server.c
	g++ include/dev/write_txt.cpp -std=c++11 -o include/dev/write_txt
	gcc server.c -std=c11 -pthread -w -o sv

xml: /usr/include/libxml2
	gcc client.c -std=c11 -o cl
	cp extra/server_xml.c server.c
	g++ -std=c++11 include/dev/write_xml.cpp -I/usr/include/libxml2 -lxml2 -o include/dev/write_xml
	gcc server.c -std=c11 -pthread -I/usr/include/libxml2 -lxml2 -o sv

clean:
	rm -f **/cl **/sv **/*.out cl sv
	rm -f include/dev/write_txt
	rm -f include/dev/write_xml
	rm -f include/dev/write_file
