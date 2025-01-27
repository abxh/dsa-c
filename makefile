TOPTARGETS := all test clean

SUBDIRS := ./fstack/example
SUBDIRS += ./fstack/test
SUBDIRS += ./fqueue/example
SUBDIRS += ./fqueue/test/fqueue
SUBDIRS += ./fqueue/test/round_up_pow2_32
SUBDIRS += ./fhashtable/example
SUBDIRS += ./fhashtable/test/benchmark
SUBDIRS += ./fhashtable/test/correctness/fhashtable
SUBDIRS += ./fhashtable/test/correctness/round_up_pow2_32
SUBDIRS += ./fpqueue/example
SUBDIRS += ./fpqueue/test
SUBDIRS += ./rbtree/example
SUBDIRS += ./rbtree/test
SUBDIRS += ./arena/example
SUBDIRS += ./arena/test/arena
SUBDIRS += ./arena/test/align

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)
