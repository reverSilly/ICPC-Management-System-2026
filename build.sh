#!/usr/bin/env bash
#
# build.sh - 编译项目：先合并源代码为 build/main.cpp，再用 g++ 编译为可执行文件。
#
# 用法: build.sh [out_name]
#   out_name 默认为 main。
# 环境变量:
#   CXX        编译器，默认 g++。
#   FLAGS      编译参数，默认 "-std=c++20 -O2 -Wall -Wextra"。
#   NO_MERGE=1 跳过合并步骤，直接编译已存在的 build/main.cpp。

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUT_NAME="${1:-main}"
CXX="${CXX:-g++}"
FLAGS="${FLAGS:--std=c++20 -O2 -Wall -Wextra}"

BUILD_DIR="$SCRIPT_DIR/build"
MERGED="$BUILD_DIR/main.cpp"

if [ "${NO_MERGE:-0}" != "1" ]; then
    bash "$SCRIPT_DIR/merge.sh"
fi

if [ ! -f "$MERGED" ]; then
    echo "build: 找不到 $MERGED，请先运行 merge.sh" >&2
    exit 1
fi

mkdir -p "$BUILD_DIR"

EXE_EXT=""
case "$(uname -s)" in
    MINGW* | MSYS* | CYGWIN*) EXE_EXT=".exe" ;;
esac
EXE="$BUILD_DIR/$OUT_NAME$EXE_EXT"

echo "[build] $CXX $FLAGS $MERGED -o $EXE"
# shellcheck disable=SC2086  # FLAGS 需要按空格拆分
"$CXX" $FLAGS "$MERGED" -o "$EXE"

echo "[build] OK -> $EXE"
