
CC = gcc
CFLAGS = -g
INCLUDES = -I$(INCDIR)

SRCDIR = src
INCDIR = include
OBJDIR = objs

SRCS = general.c inode.c dir.c utils.c ext2fs_api.c
OBJ_NAMES = $(patsubst %.c,%.o,$(SRCS))
OBJS = $(addprefix $(OBJDIR)/,$(OBJ_NAMES))

# vpath %.o $(OBJDIR)
vpath %.h $(INCDIR)
vpath %.c $(SRCDIR)

.PHONY: all clean

all: test

test: test.o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

test.o: test.c ext2fs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

#$(OBJDIR)/%.o : %.c
#	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJS) : $(OBJDIR)/%.o : %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

include $(OBJS:.o=.d)

$(OBJDIR)/%.d : %.c | $(OBJDIR)
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $(INCLUDES) $< > $@.tmp; \
	sed 's,\($*\)\.o[ :]*,$(OBJDIR)\/\1.o $@ : ,g' < $@.tmp > $@; \
	rm -f $@.tmp; \
	echo "extract headers to $@"

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	@mkdir $@

clean:
	rm -rf test test.o $(OBJDIR) 

