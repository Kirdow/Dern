CC=gcc
CXX=g++
RM=rm -f
CFLAGS=-g -Wall -std=c++17
LDFLAGS=-g -Wall
LDLIBS=

SRCDIR=./src
DISTDIR=./dist
PROG=$(DISTDIR)/dernapp

#ifdef USE_PCH
  PRECOMPILED_HEADER=pch.h
  PCH_TARGET=$(PRECOMPILED_HEADER).gch
#  CFLAGS+= -include $(PRECOMPILED_HEADER)
  OBJDIR=./obj-pch
#  $(info Building WITH precompiled headers...)
#else
#  $(info Building WITHOUT precompiled headers...)
#  OBJDIR=./obj-no-pch
#endif

SRCS:=$(shell find $(SRCDIR) -name '*.cpp' -or -name '*.c' -or -name '*.s')
OBJS:=$(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEPS:=$(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.d)

all: $(PROG)

debug:
	echo SRCS $(SRCS)
	echo OBJS $(OBJS)
	echo DEPS $(DEPS)

$(OBJDIR):
	mkdir -p $@

$(PROG): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@

$(OBJDIR)/$(PCH_TARGET): $(SRCDIR)/$(PRECOMPILED_HEADER) | $(OBJDIR)
	$(CXX) $(CFLAGS) -MP -MMD -MT$(@:.gch=.d) -o $@ -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(OBJDIR)/$(PCH_TARGET) | $(OBJDIR)
	$(CXX) $(CFLAGS) -MP -MMD -MT$(@:.o=.d) -o $@ -c $<

.PHONY: clean
clean:
	rm -r $(OBJDIR)
	rm -f $(PCH_TARGET)

distclean:
	rm -f $(PROG)

-include $(DEPS)


