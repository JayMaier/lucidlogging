SUBDIRS = jay_testing                                    \


SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=%.o)
DEPS = $(OBJS:%.o=%.d)

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	@echo '\n'[$@]
	$(MAKE) -C $@

SUBCLEAN = $(addsuffix .clean,$(SUBDIRS))

.PHONY: clean $(SUBCLEAN)
clean: $(SUBCLEAN)

$(SUBCLEAN): %.clean:
	$(MAKE) -C $* clean


SUBRELEASE = $(addsuffix .release,$(SUBDIRS))

.PHONY: release $(SUBRELEASE)
release: $(SUBRELEASE)

$(SUBRELEASE): %.release:
	$(MAKE) -C $* release


SUBDEBUG = $(addsuffix .debug,$(SUBDIRS))

.PHONY: debug $(SUBDEBUG)
debug: $(SUBDEBUG)

$(SUBDEBUG): %.debug:
	$(MAKE) -C $* debug
