.DEFAULT_GOAL=concurrent_http_server_B


CFLAGS=-Wall -Werror


CORE_obj=parse_request.o http.o process_request.o utility.o generate_response_string.o

# Single Process
SP_obj=$(CORE_obj) http_server.o

# Multi Process
MPM_A_obj=worker_process_A.o mpm_A.o mpm.o
MPM_B_obj=handle_request.o worker_process_B.o mpm_B.o mpm.o
PLAN_A_obj=$(CORE_obj) flag.o $(MPM_A_obj) concurrent_http_server_A.o
PLAN_B_obj=$(CORE_obj) flag.o $(MPM_B_obj) concurrent_http_server_B.o
PLAN_C_obj=$(CORE_obj)

# Performace test
HTTP_PARSE_SPEED_obj=$(CORE_obj) request_parse_performance_test.o

####################
# Executable
####################
.PHONY: A
A: concurrent_http_server_A
concurrent_http_server_A: $(PLAN_A_obj)
	gcc -o concurrent_http_server_A $(CFLAGS) -lpthread $(PLAN_A_obj)

.PHONY: B
B: concurrent_http_server_B
concurrent_http_server_B: $(PLAN_B_obj)
	gcc -o concurrent_http_server_B $(CFLAGS) -lpthread $(PLAN_B_obj)

.PHONY: C
C: concurrent_http_server_C
concurrent_http_server_C: $(PLAN_C_obj)
	gcc -o concurrent_http_server_C $(CFLAGS) -lpthread $(PLAN_C_obj)

http_server: $(SP_obj)
	gcc -o http_server $(CFLAGS) $(SP_obj)

request_parse_performance_test: $(HTTP_PARSE_SPEED_obj)
	gcc -o request_parse_performance_test $(CFLAGS) $(HTTP_PARSE_SPEED_obj)

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

request_parse_performance_test.o: code_snipet/request_parse_performance_test.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

#
# MPM B
#
handle_request.o: mpm_B/handle_request.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

worker_process_B.o: mpm_B/worker_process.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

mpm_B.o: mpm_B/mpm_B.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

concurrent_http_server_B.o: concurrent_http_server_B.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

#
# MPM A
#
worker_process_A.o: mpm_A/worker_process_A.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

mpm_A.o: mpm_A/mpm_A.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

mpm.o: mpm.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

concurrent_http_server_A.o: concurrent_http_server_A.c
	gcc -c $(CFLAGS) -lpthread $< -o $@


.PHONY: clean
clean: clean_test
	rm -f $(SP_obj)
	rm -f $(PLAN_A_obj) $(PLAN_B_obj) $(PLAN_C_obj)
	rm -f $(HTTP_PARSE_SPEED_obj)
	rm -f request_parse_performance_test
	rm -f http_server http_server_test
	rm -f concurrent_http_server_A concurrent_http_server_B concurrent_http_server_C

####################
# Testing
####################
TEST_obj=config_t.o parse_request_t.o
TEST=test_parse_request_c test_config_c

.PHONY: test
test: test_parse_request_c test_config_c
	echo "Test"

.PHONY: clean_test
clean_test:
	rm -f test_parse_request_c
	rm -f test_config_c
	rm -f $(TEST_obj)

test_parse_request_c: parse_request_t.o utility.o
	gcc $(CFLAGS) -g3 -DTEST $^ -o $@
	./$@

test_config_c: config_t.o
	gcc $(CFLAGS) -g3 -DTEST $^ -o $@
	./$@

config_t.o: config.c
	gcc -c $(CFLAGS) -g3 -DTEST $< -o $@

parse_request_t.o: core/parse_request.c
	gcc -c $(CFLAGS) -g3 -DTEST $< -o $@

.PHONY: help
help:
	echo -e "make\nmake clean\nmake test\n"
