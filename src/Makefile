
http_server: parse_request.o http_server.o http.o process_request.o utility.o generate_response_string.o
	gcc -o  http_server -Wall parse_request.o http_server.o http.o process_request.o utility.o generate_response_string.o

parse_request.o: parse_request.c
	gcc -c -Wall parse_request.c -o parse_request.o

http_server.o: http_server.c
	gcc -c -Wall http_server.c -o http_server.o

http.o: http.c
	gcc -c -Wall http.c -o http.o

process_request.o: process_request.c
	gcc -c -Wall process_request.c -o process_request.o

generate_response_string.o: generate_response_string.c
	gcc -c -Wall generate_response_string.c -o generate_response_string.o

utility.o: utility.c
	gcc -c -Wall utility.c -o utility.o

config.o: config.c
	gcc -c -Wall config.c -o config.o

####################
# Debug
####################
.PHONY: debug
debug: parse_request_debug.o http_server_debug.o http_debug.o process_request_debug.o utility_debug.o generate_response_string_debug.o
	gcc -o http_server_debug -Wall -g3 http_server_debug.o parse_request_debug.o http_debug.o process_request_debug.o utility_debug.o generate_response_string_debug.o

parse_request_debug.o: parse_request.c
	gcc -c -g3 -Wall parse_request.c -o parse_request_debug.o

http_server_debug.o: http_server.c
	gcc -c -g3 -Wall http_server.c -o http_server_debug.o

http_debug.o: http.c
	gcc -c -g3 -Wall http.c -o http_debug.o

process_request_debug.o: process_request.c
	gcc -c -g3 -Wall process_request.c -o process_request_debug.o

generate_response_string_debug.o: generate_response_string.c
	gcc -c -g3 -Wall generate_response_string.c -o generate_response_string_debug.o

utility_debug.o: utility.c
	gcc -c -g3 -Wall utility.c -o utility_debug.o

config_debug.o: config.c
	gcc -c -g3 -Wall config.c -o config_debug.o

.PHONY: clean
clean:
	rm -f http_debug.o parse_request_debug.o http_server_debug.o process_request_debug.o generate_response_string_debug.o utility_debug.o config_debug.o
	rm -f http.o parse_request.o http_server.o process_request.o generate_response_string.o utility.o config.o
	rm -f http_server http_server_debug http_server_test

####################
# Testing
####################
.PHONY: test
test: test_parse_request_c
	echo "Test"

.PHONY: clean_test
clean_test:
	rm -f test_parse_request_c

test_parse_request_c: parse_request.c
	gcc -Wall -DTEST parse_request.c -o test_parse_request_c
	./test_parse_request_c

.PHONY: help
help:
	echo -e "make\nmake debug\nmake clean\nmake clean\n"
