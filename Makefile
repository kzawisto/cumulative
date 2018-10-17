
CFLAGS := -std=c++14 -I.
CXX := g++
LDFLAGS := 
ALL_FILES = $(shell find . \( -name "*.h" -or -name *.cxx \) -exec ls {} \;)
SRCS := $(shell find . -name '*.cxx' | sed 's:^\./::g')
OBJDIR := obj

OBJS :=  $(SRCS:%.cxx=$(OBJDIR)/%.o)

program: $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $@

-include $(OBJS:.o=.d)


$(OBJDIR)/%.o: %.cxx
	@echo $(SRCS)
	@mkdir -p `dirname $@`
	$(CXX) -c $(CFLAGS) $< -o $@
	$(CXX) -MM $(CFLAGS) $*.cxx | sed "s:^:`dirname $@`/:" > $(OBJDIR)/$*.d


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


