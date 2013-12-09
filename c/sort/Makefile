CC = gcc
XX = g++
TARGET = sort
CFLAGS = -Wall -O -g

#patsubst函数，用于匹配替换，有3个参数
#第一个是一个需要匹配的字样
#第二个表示用什么来替换它
#第三个是一个变量需要被处理的由空格分隔的列表

#$(wildccard PATTERN…)，在makefile中，它被展开为已经存在的、使用空格分开的、匹配此模式的所有文件列表

#$@，表示目标文件
#$^，表示全部依赖文件
#$<,表示第一个依赖文件

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.o : %.cpp
	$(XX) $(CFLAGS) -c $< -o $@

SOURCES = $(wildcard  *.c  *.cpp)
OBJS = $(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(SOURCES)))

$(TARGET) : $(OBJS)
	$(XX) $(OBJS) -o $(TARGET)
clean:
	rm -rf *.o $(TARGET)
