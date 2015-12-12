
CC = gcc
CFLAGS = -g
INCLUDES = -I$(INCDIR)

SRCDIR = src
INCDIR = include
OBJDIR = objs
IMG_NAME = fs.img

SRCS = general.c inode.c dir.c utils.c ext2fs_api.c
OBJ_NAMES = $(patsubst %.c,%.o,$(SRCS))
OBJS = $(addprefix $(OBJDIR)/,$(OBJ_NAMES))

# vpath %.o $(OBJDIR)
vpath %.h $(INCDIR)
vpath %.c $(SRCDIR)

.PHONY: all clean

all: test image

test: test.o $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

test.o: test.c ext2fs.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

#$(OBJDIR)/%.o : %.c
#	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJS) : $(OBJDIR)/%.o : %.c $(OBJDIR)/%.d
	$(CC) -MT $@ -MMD -MP -MF $(OBJDIR)/$*.d $(CFLAGS) $(INCLUDES) -c $< -o $@;

-include $(OBJS:.o=.d)

$(OBJDIR)/%.d : ;

#$(OBJDIR)/%.d : %.c | $(OBJDIR)
#	@$(CC) -MT $@ -MM -MP $(CFLAGS) $(INCLUDES) $< > $@;
#	@echo "extract dependencies to $@"

$(OBJS): | $(OBJDIR)

$(OBJDIR):
	@mkdir $@

image: | $(IMG_NAME)

$(IMG_NAME): 
	@echo "Launch create_image.sh <folder> to set up test image"

clean:
	rm -rf test test.o $(OBJDIR) 

