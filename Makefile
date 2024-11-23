# Compiler settings
#
# CC: 사용할 컴파일러를 지정 (여기서는 GCC를 사용)
# CFLAGS: 컴파일러 플래그를 설정
#   - -Iinclude: `include` 디렉토리를 헤더 파일 검색 경로로 추가
#   - -Wall: 모든 경고 메시지를 출력하여 잠재적인 오류를 확인
CC = gcc
CFLAGS = -Iinclude -Wall


# Target file
#
# TARGET: 생성할 실행 파일의 경로와 이름을 지정
#   - `bin/myshell`: `bin` 디렉토리에 `myshell`이라는 실행 파일을 생성
TARGET = bin/myshell


# Source files, object files, and target settings
#
# SRCS: 컴파일에 사용될 소스 파일 목록
#   - `src/myshell.c`: 메인 프로그램 파일
#   - `src/builtin_commands.c`: 내장 명령어를 처리하는 기능을 구현한 파일
#
# OBJS: 소스 파일(.c)을 객체 파일(.o)로 변환한 결과물 목록
#   - `$(SRCS:.c=.o)`는 확장자를 `.c`에서 `.o`로 변경하여 객체 파일 목록을 자동으로 생성
SRCS = src/myshell.c src/builtin_commands.c 
OBJS = $(SRCS:.c=.o)


# Default target
#
# all: 기본 타겟으로, Makefile 실행 시 가장 먼저 호출
#      여기서는 실행 파일 `$(TARGET)`을 빌드하는 역할
#      all 타겟을 사용하면 명시적으로 `make all`을 호출할 필요 없이 `make`만 실행해도 됨
all: $(TARGET)


# Rule to create the myshell executable
#
# $(TARGET): `myshell` 실행 파일을 생성하기 위한 규칙을 명시
#   - 의존성: 실행 파일을 생성하기 위해 `$(OBJS)` (객체 파일들)가 필요
#   - 명령어:
#     - $(CC): 지정된 컴파일러(GCC)를 사용
#     - -o $@: 출력 파일 이름(`$(TARGET)`)을 지정
#     - $^: 의존성 목록(여기서는 `$(OBJS)`)을 나타냄
$(TARGET): $(OBJS)
	$(CC) -o $@ $^


# Compile individual object files
#
# src/%.o: src/%.c
#   - 패턴 규칙: `src/` 디렉토리의 `.c` 파일을 `.o` 파일로 컴파일
#     - `src/%.o`: `src/` 디렉토리의 `.o` 파일 이름
#     - `src/%.c`: 대응하는 소스 파일 이름
#
#   - 명령어:
#     - $(CC): 지정된 컴파일러(GCC) 사용
#     - $(CFLAGS): 컴파일러 플래그 사용
#     - -c: 컴파일만 수행하며, 링킹은 사용 X
#     - $<: 현재 규칙의 소스 파일
#     - -o $@: 출력 파일 이름 (여기서는 `src/%.o`)
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@


# Clean rule
#
# clean: 컴파일 결과물(객체 파일 및 실행 파일)을 삭제하는 규칙
#   - 명령어:
#     - rm -f $(OBJS): `$(OBJS)`에 포함된 객체 파일을 삭제
#     - rm -f $(TARGET): 생성된 실행 파일(`$(TARGET)`)을 삭제
#
#   - clean 규칙은 파일을 생성하지 않으므로, 의존성 없음
#     따라서 `make clean`을 호출하면 명령어만 실행
clean:
	rm -f $(OBJS) $(TARGET)
