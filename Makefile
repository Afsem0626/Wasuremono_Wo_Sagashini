EXEC = ./step22_test

# 使用するCコンパイラ
CC = gcc


SRCS = main.c init.c input.c draw.c logic.c sound.c

# ソースファイルからオブジェクトファイル (.o ファイル) のリストを自動生成
OBJS = $(SRCS:.c=.o)

# コンパイルオプション
# -g          : デバッグ情報を付加する
# -Wall       : 有用な警告をすべて表示する
CFLAGS = -g -Wall `pkg-config --cflags sdl2 SDL2_image SDL2_ttf SDL2_mixer`

LIBS = `pkg-config --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer` -lm

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(LIBS)

# オブジェクトファイルの生成ルール (パターンルール)
# .c ファイルから .o ファイルを生成する方法を定義
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC) $(OBJS)

# 偽ターゲットの宣言
# 'all' や 'clean' という名前のファイルが万が一存在しても、正しく動作するようにする
.PHONY: all clean