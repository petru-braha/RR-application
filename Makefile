all:
	gcc -std=c11 client.c -o cl
	g++ -std=c++11 include/dev/write_xml.cpp -I/usr/include/libxml2 -lxml2 -o include/dev/write_xml
	gcc -std=c11 server.c -I/usr/include/libxml2 -lxml2 -o sv

clean:
	rm -f cl sv
