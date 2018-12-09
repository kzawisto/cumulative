
CFLAGS := -std=c++14 -I. -g -O0 
CXX ?= g++
LDFLAGS := -g 
ALL_FILES = $(shell find . \( -name "*.h" -or -name *.cxx \) -exec ls {} \;)
SRCS := $(shell find . -name '*.cxx' | sed 's:^\./::g')

OBJDIR := obj
SRC_TEST := $(shell echo $(SRCS) | tr ' ' '\n' | grep test | tr '\n' ' ')

OBJS :=  $(SRCS:%.cxx=$(OBJDIR)/%.o)

OBJS_SRC := $(shell echo $(OBJS) | tr ' ' '\n' | grep src | tr '\n' ' ')
OBJS_TEST := $(shell echo $(OBJS) | tr ' ' '\n' | grep -v src | grep cpp_tests | tr '\n' ' ')
OBJS_OTHER := $(shell echo $(OBJS) | tr ' ' '\n' | grep -v src | grep execs | tr '\n' ' ')

BINS = $(OBJS_OTHER:%.o=%.bin)

all: program test_all $(BINS)

%.bin : %.o $(OBJS_SRC)
	$(CXX) $<  $(OBJS_SRC) $(LDFLAGS) -o $@

program: $(OBJS_OTHER) $(OBJS_SRC)
	$(CXX) $(OBJS_OTHER)  $(OBJS_SRC) $(LDFLAGS) -o $@ 

test_all :$(OBJS_TEST) $(OBJS_SRC)
	$(CXX) $(OBJS_TEST)  $(OBJS_SRC) $(LDFLAGS) -o $@ -lgtest -lpthread


-include $(OBJS:.o=.d)


$(OBJDIR)/%.o: %.cxx
	echo $(SRC_TEST)
	@mkdir -p `dirname $@`
	$(CXX) -c $(CFLAGS) $< -o $@
	$(CXX) -MM $(CFLAGS) $*.cxx | sed "1s:^:`dirname $@`/:" > $(OBJDIR)/$*.d


.PHONY: clang-format clean clang-tidy
clang-format:
	 clang-format -i $(ALL_FILES) -style=file


clang-tidy:
	@for src in $(SRCS) ; do \
		echo "Running clang-tidy on $$src..." ; \
		clang-tidy -checks="-*,modernize-use-auto,modernize-use-nullptr,\
			readability-else-after-return,readability-simplify-boolean-expr,\
			readability-redundant-member-init,modernize-use-default-member-init,\
			modernize-use-equals-default,modernize-use-equals-delete,\
			modernize-use-using,modernize-loop-convert,\
			cppcoreguidelines-no-malloc,misc-redundant-expression" \
			-header-filter=".*" \
			"$(SRC_DIR)/$$src" $(CFLAGS) ; \
	done
	@echo "Done"
  


clean:
	rm -rf obj 
	find . -type f -executable -not -name "*.sh" -exec rm {} +


