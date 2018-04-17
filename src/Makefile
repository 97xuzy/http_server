.DEFAULT_GOAL=http_server


CFLAGS=-Wall -Werror


# Single Process
SP_obj=parse_request.o http_server.o http.o process_request.o utility.o generate_response_string.o


####################
# Executable
####################
http_server: $(SP_obj)
	gcc -o http_server $(CFLAGS) $(SP_obj)

####################
# Object file
####################
parse_request.o: core/parse_request.c
	gcc -c $(CFLAGS) $< -o $@

http_server.o: http_server.c
	gcc -c $(CFLAGS) $< -o $@

http.o: core/http.c
	gcc -c $(CFLAGS) $< -o $@

process_request.o: core/process_request.c
	gcc -c $(CFLAGS) $< -o $@

generate_response_string.o: core/generate_response_string.c
	gcc -c $(CFLAGS) $< -o $@

utility.o: utility/utility.c
	gcc -c $(CFLAGS) $< -o $@

config.o: config.c
	gcc -c $(CFLAGS) $< -o $@

flag.o: utility/flag.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

.PHONY: clean
clean:
	rm -f $(SP_obj)
	rm -f http_server http_server_test

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
	gcc $(CFLAGS) -DTEST $< -o test_parse_request_c
	./test_parse_request_c

.PHONY: help
help:
	echo -e "make\nmake clean\nmake test\n"
