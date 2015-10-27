# Sample usage:
# CXXFLAGS="-fpermissive -std=c++11" make # build tests and examples
# CXXFLAGS="-fpermissive -std=c++11" make examples # build just examples

default: test
test: test_compile test_run
examples: examples_cgi examples_fcgi examples_xcgi

.SUFFIXES:
.SUFFIXES: .cc .cpp .S .o .a .s .i
CPPFLAGS += -I $(PWD)
#ifneq ($(BOOST_ROOT),)
#CPPFLAGS += -I $(BOOST_ROOT)
#endif

TEST_COMPILE_SRC := $(wildcard libs/cgi/test/compile/*.cpp)
# Broken, ignore them for now
TEST_COMPILE_SRC := $(filter-out libs/cgi/test/compile/acgi_request.cpp libs/cgi/test/compile/acgi_service.cpp libs/cgi/test/compile/cgi_service.cpp libs/cgi/test/compile/acgi_acceptor.cpp,$(TEST_COMPILE_SRC))
TEST_COMPILE_BIN := $(TEST_COMPILE_SRC:.cpp=$(EXEEXT))
TEST_RUN_SRC     := $(wildcard libs/cgi/test/run/*.cpp)
# Broken, ignore them for now
TEST_RUN_SRC     := $(filter-out libs/cgi/test/run/has_key.cpp libs/cgi/test/run/data_map_proxy.cpp libs/cgi/test/run/request_data.cpp libs/cgi/test/run/sessions.cpp libs/cgi/test/run/acgi_simple_request.cpp,$(TEST_RUN_SRC))
TEST_RUN_BIN     := $(TEST_RUN_SRC:.cpp=$(EXEEXT))

EXAMPLE_CGI_SRC  := $(wildcard libs/cgi/example/cgi/*)
# Broken, ignore them for now
EXAMPLE_CGI_SRC := $(filter-out libs/cgi/example/cgi/DebugServer libs/cgi/example/cgi/DebugServer2 libs/cgi/example/cgi/custom_sessions libs/cgi/example/cgi/sessions,$(EXAMPLE_CGI_SRC))
EXAMPLE_FCGI_SRC := $(wildcard libs/cgi/example/fcgi/*/*.cpp)
# Broken, ignore them for now
EXAMPLE_FCGI_SRC := $(filter-out libs/cgi/example/fcgi/server4/main.cpp libs/cgi/example/fcgi/charts/main.cpp,$(EXAMPLE_FCGI_SRC))
EXAMPLE_XCGI_SRC := $(wildcard libs/cgi/example/xcgi/*/*.cpp)
EXAMPLE_CGI_BIN  :=
EXAMPLE_FCGI_BIN := $(EXAMPLE_FCGI_SRC:.cpp=$(EXEEXT))
EXAMPLE_XCGI_BIN := $(EXAMPLE_XCGI_SRC:.cpp=$(EXEEXT))

define ex_cgi
$(1)_OBJS := $(patsubst %.cpp,%.o,$(wildcard $(1:/demo$(EXEEXT)=)/*.cpp))
ifneq ($$($(1)_OBJS),)
$(1): $$($(1)_OBJS)
EXAMPLE_CGI_BIN  += $(1)
endif
endef
$(foreach i,$(EXAMPLE_CGI_SRC:=/demo$(EXEEXT)),$(eval $(call ex_cgi,$(i))))
$(EXAMPLE_CGI_SRC:=/demo$(EXEEXT)):
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

examples_cgi: $(EXAMPLE_CGI_BIN)
examples_fcgi: $(EXAMPLE_FCGI_BIN)
examples_xcgi: #$(EXAMPLE_XCGI_BIN) # Disabled for now, they do not compile

test_compile: $(TEST_COMPILE_BIN)
test_run: $(TEST_RUN_BIN)

LDFLAGS-libs/cgi/example/cgi/upload := -lctemplate
LDFLAGS-libs/cgi/example/cgi/cookies2 := -lctemplate
LDFLAGS-libs/cgi/example/cgi/cookies := -lctemplate
LDFLAGS-libs/cgi/example/cgi/file_browser_debug := -lboost_filesystem
LDFLAGS-libs/cgi/example/cgi/stencil := -lctemplate
LDFLAGS-libs/cgi/example/fcgi/server2 := -lboost_thread
LDFLAGS-libs/cgi/example/fcgi/amortization := -lctemplate -lboost_regex
LDFLAGS-libs/cgi/example/fcgi/upload := -lctemplate
LDFLAGS-libs/cgi/example/fcgi/stencil := -lctemplate
LDFLAGS-libs/cgi/example/fcgi/server3 := -lboost_thread
LDFLAGS-libs/cgi/example/fcgi/echo_threaded := -lboost_thread
# test/*
LDFLAGS-libs/cgi/test/compile/fcgi_service := -lboost_system
LDFLAGS-libs/cgi/test/compile/scgi_request := -lboost_filesystem -lboost_system
LDFLAGS-libs/cgi/test/compile/scgi_service := -lboost_system
LDFLAGS-libs/cgi/test/compile/response := -lboost_system
LDFLAGS-libs/cgi/test/compile/cgi_request := -lboost_filesystem -lboost_system
LDFLAGS-libs/cgi/test/compile/tcp_connection := -lboost_system
LDFLAGS-libs/cgi/test/compile/fcgi_acceptor := -lboost_system
LDFLAGS-libs/cgi/test/compile/fcgi_request := -lboost_filesystem -lboost_system
LDFLAGS-libs/cgi/test/compile/cgi_header_check := -lboost_filesystem -lboost_system
LDFLAGS-libs/cgi/test/compile/stdio_connection := -lboost_system
LDFLAGS-libs/cgi/test/compile/async_stdio_connection := -lboost_system
LDFLAGS-libs/cgi/test/compile/scgi_acceptor := -lboost_system
LDFLAGS-libs/cgi/test/compile/shareable_tcp_connection := -lboost_system
LDFLAGS-libs/cgi/test/run/cgi_simple_request := -lboost_filesystem
LDFLAGS-libs/cgi/test/run/hello_world := -lboost_filesystem

$(EXAMPLE_CGI_BIN): LDFLAGS += -lboost_filesystem -lboost_system
$(EXAMPLE_FCGI_BIN): LDFLAGS += -lboost_filesystem -lboost_system

$(EXAMPLE_CGI_BIN) $(EXAMPLE_FCGI_BIN) $(EXAMPLE_XCGI_BIN): \
  CXXFLAGS +=
$(EXAMPLE_CGI_BIN) $(EXAMPLE_FCGI_BIN) $(EXAMPLE_XCGI_BIN): \
  LDFLAGS += $(LDFLAGS-$(@D))

$(TEST_COMPILE_BIN): LDFLAGS += $(LDFLAGS-$@)
$(TEST_RUN_BIN): CPPFLAGS += -DBOOST_TEST_DYN_LINK
$(TEST_RUN_BIN): \
  LDFLAGS +=  $(LDFLAGS-$@) -lboost_unit_test_framework -lboost_serialization -lboost_system

clean_examples:
	-$(RM) $(EXAMPLE_CGI_BIN) $(EXAMPLE_FCGI_BIN) $(EXAMPLE_XCGI_BIN) \
		$(wildcard libs/cgi/example/*/*.o) \
		$(wildcard libs/cgi/example/*/*/*.o) \
		$(wildcard libs/cgi/example/*/*/*/*.o)
clean_test:
	-$(RM) $(TEST_RUN_BIN) $(TEST_COMPILE_BIN) \
		$(wildcard libs/cgi/test/compile/*.o) \
		$(wildcard libs/cgi/test/run/*.o)
clean: clean_examples clean_test
.PHONY: clean clean_%
all:
	sudo cp -r ./boost/cgi /usr/include/boost/

