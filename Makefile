# 最終的に生成する実行ファイル名
EXEC = ./step1_test

# 使用するCコンパイラ
CC = gcc

# コンパイル対象のソースファイル (.c ファイル)
# ご自身のプロジェクトに合わせて、ファイル名を追加・削除してください。
SRCS = main.c init.c

# ソースファイルからオブジェクトファイル (.o ファイル) のリストを自動生成
OBJS = $(SRCS:.c=.o)

# コンパイルオプション
# -g          : デバッグ情報を付加する
# -Wall       : 有用な警告をすべて表示する
CFLAGS = -g -Wall `pkg-config --cflags sdl2 SDL2_image SDL2_ttf SDL2_mixer`

# リンクオプション
# -lm         : 数学ライブラリをリンクする (sin, cosなどを使う場合に必要)
LIBS = `pkg-config --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer` -lm


# --- ルール定義 ---

# デフォルトのターゲット: 'make' コマンドだけで実行される
all: $(EXEC)

# 実行ファイルの生成ルール
# 依存関係: 全てのオブジェクトファイル (.o) が必要
$(EXEC): $(OBJS)
	$(CC) -o $(EXEC) $(OBJS) $(LIBS)

# オブジェクトファイルの生成ルール (パターンルール)
# .c ファイルから .o ファイルを生成する方法を定義
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# クリーンルール: 生成されたファイルを削除する
clean:
	rm -f $(EXEC) $(OBJS)

# 偽ターゲットの宣言
# 'all' や 'clean' という名前のファイルが万が一存在しても、正しく動作するようにする
.PHONY: all clean