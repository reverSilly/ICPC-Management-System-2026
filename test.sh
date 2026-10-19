#!/usr/bin/env bash
#
# test.sh - 编译并运行测试：先调用 build.sh 编译，再用 data 目录下的 *.in/*.out 逐点评测。
#   比较时忽略行尾空白与末尾空行、统一换行符；
#   失败用例的实际输出保存到 build/test/<case>.actual.out。
#
# 用法: test.sh [exe_name]
#   exe_name 默认为 main。
# 环境变量:
#   DATA_DIR   测试数据目录，默认 <脚本目录>/data。
#   NO_BUILD=1 跳过编译，直接用已有可执行文件。

set -euo pipefail
export LC_ALL=C

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EXE_NAME="${1:-main}"
DATA_DIR="${DATA_DIR:-$SCRIPT_DIR/data}"
BUILD_DIR="$SCRIPT_DIR/build"

if [ "${NO_BUILD:-0}" != "1" ]; then
    bash "$SCRIPT_DIR/build.sh" "$EXE_NAME"
fi

EXE_EXT=""
case "$(uname -s)" in
    MINGW* | MSYS* | CYGWIN*) EXE_EXT=".exe" ;;
esac
EXE="$BUILD_DIR/$EXE_NAME$EXE_EXT"

if [ ! -f "$EXE" ]; then
    echo "test: 找不到可执行文件 $EXE" >&2
    exit 1
fi
if [ ! -d "$DATA_DIR" ]; then
    echo "test: 找不到测试数据目录 $DATA_DIR" >&2
    exit 1
fi

if [ -t 1 ]; then
    RED=$'\033[31m'; GREEN=$'\033[32m'; YELLOW=$'\033[33m'; RESET=$'\033[0m'
else
    RED=''; GREEN=''; YELLOW=''; RESET=''
fi

# 统一换行符、去除行尾空白与末尾空行
normalize() {
    sed -E -e 's/\r$//' -e 's/[[:space:]]+$//' "$1" \
        | awk '{ lines[NR] = $0 } END { last = NR; while (last > 0 && lines[last] == "") last--; for (i = 1; i <= last; i++) print lines[i] }'
}

# 返回首个不同行的行号（从 1 开始），完全相同返回 0
first_diff_line() {
    awk 'NR == FNR { a[FNR] = $0; n = FNR; next }
         { b[FNR] = $0; m = FNR }
         END {
             len = (n > m ? n : m)
             for (i = 1; i <= len; i++) {
                 x = (i <= n ? a[i] : "<EOF>")
                 y = (i <= m ? b[i] : "<EOF>")
                 if (x != y) { print i; exit }
             }
         }' <(normalize "$1") <(normalize "$2")
}

shopt -s nullglob
cases=("$DATA_DIR"/*.in)
if [ "${#cases[@]}" -eq 0 ]; then
    echo "test: $DATA_DIR 下没有 *.in 测试数据" >&2
    exit 1
fi

FAIL_DIR="$BUILD_DIR/test"
pass_count=0
fail_count=0
total_start=$(date +%s%N)

for in_file in "${cases[@]}"; do
    base="$(basename "$in_file")"
    case_name="${base%.in}"
    expected="${in_file%.in}.out"

    if [ ! -f "$expected" ]; then
        echo "${YELLOW}[skip] ${case_name}: 缺少 ${base%.in}.out${RESET}"
        continue
    fi

    tmp="$(mktemp)"
    start=$(date +%s%N)
    set +e
    "$EXE" < "$in_file" > "$tmp" 2>/dev/null
    rc=$?
    set -e
    end=$(date +%s%N)
    ms=$(( (end - start) / 1000000 ))

    if [ "$rc" -ne 0 ]; then
        fail_count=$((fail_count + 1))
        echo "${RED}[FAIL] ${case_name}: 运行时异常退出 (exit code ${rc}, ${ms} ms)${RESET}"
    elif diff -q <(normalize "$expected") <(normalize "$tmp") >/dev/null 2>&1; then
        pass_count=$((pass_count + 1))
        echo "${GREEN}[PASS] ${case_name} (${ms} ms)${RESET}"
    else
        fail_count=$((fail_count + 1))
        line="$(first_diff_line "$expected" "$tmp")"
        [ -n "$line" ] || line="?"
        echo "${RED}[FAIL] ${case_name}: 输出不一致，首个不同在第 ${line} 行 (${ms} ms)${RESET}"
        mkdir -p "$FAIL_DIR"
        cp "$tmp" "$FAIL_DIR/${case_name}.actual.out"
    fi

    rm -f "$tmp"
done

total_end=$(date +%s%N)
total_ms=$(( (total_end - total_start) / 1000000 ))

echo
if [ "$fail_count" -eq 0 ]; then
    echo "${GREEN}[test] ${pass_count} passed, ${fail_count} failed, total ${total_ms} ms${RESET}"
else
    echo "${RED}[test] ${pass_count} passed, ${fail_count} failed, total ${total_ms} ms${RESET}"
    echo "[test] 失败用例的实际输出已保存到 $FAIL_DIR"
    exit 1
fi
