.DEFAULT_GOAL=B
DEFAULT_MPM_PLAN=B


CFLAGS=-Wall -Werror


CORE_obj=parse_request.o http.o process_request.o utility.o generate_response_string.o

# Single Process
SP_obj=$(CORE_obj) http_server.o

# Multi Process
CONCURRENT_obj=concurrent_http_server.o libmpm.a flag.o $(CORE_obj)
MPM_LIB_obj=mpm.o worker_process_A.o mpm_A.o handle_request.o worker_process_B.o mpm_B.o
MPM_A_obj=worker_process_A.o mpm_A.o mpm.o
MPM_B_obj=handle_request.o worker_process_B.o mpm_B.o mpm.o

ifeq ($(MAKECMDGOALS),A)
MPM_PLAN=A
else ifeq ($(MAKECMDGOALS),B)
MPM_PLAN=B
else ifeq ($(MAKECMDGOALS), )
MPM_PLAN=$(DEFAULT_MPM_PLAN)
else
MPM_PLAN=NOT_SET
endif

# Performace test
HTTP_PARSE_SPEED_obj=$(CORE_obj) request_parse_performance_test.o

####################
# Executable
####################
# MPM_A
.PHONY: A
A: concurrent_http_server

# MPM_B
.PHONY: B
B: concurrent_http_server

# MPM_C
.PHONY: C
C: concurrent_http_server

concurrent_http_server: $(CONCURRENT_obj)
	gcc -o $@ $(CFLAGS) -lpthread $^

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

concurrent_http_server.o: concurrent_http_server.c
	gcc -c $(CFLAGS) -lpthread $< -o $@


#####################
# libmpm
####################
ifeq ($(MPM_PLAN),A)

mpm.o: mpm.c
	gcc -c $(CFLAGS) -lpthread $< -o $@ -DMPM_A
libmpm.a: $(MPM_A_obj)
	ar rcs $@ $(MPM_A_obj)

else ifeq ($(MPM_PLAN),B)

mpm.o: mpm.c
	gcc -c $(CFLAGS) -lpthread $< -o $@ -DMPM_B
libmpm.a: $(MPM_B_obj)
	ar rcs $@ $(MPM_B_obj)

else

mpm.o:
	$(error MPM_PLAN is not set)
libmpm.a:
	$(error MPM_PLAN is not set)

endif

#
# MPM B
#
handle_request.o: mpm_B/handle_request.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

worker_process_B.o: mpm_B/worker_process.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

mpm_B.o: mpm_B/mpm_B.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

#
# MPM A
#
worker_process_A.o: mpm_A/worker_process_A.c
	gcc -c $(CFLAGS) -lpthread $< -o $@

mpm_A.o: mpm_A/mpm_A.c
	gcc -c $(CFLAGS) -lpthread $< -o $@


.PHONY: clean
clean: clean_test
	rm -f $(SP_obj) $(CONCURRENT_obj)
	rm -f $(MPM_A_obj) $(MPM_B_obj)
	rm -f $(HTTP_PARSE_SPEED_obj)
	rm -f request_parse_performance_test
	rm -f http_server http_server_test
	rm -f libmpm.a concurrent_http_server.o concurrent_http_server

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
