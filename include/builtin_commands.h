/* 
 * 내장 명령어 헤더 파일
 * 
 * shell 프로그램에서 사용할 내장 명령어(built-in commands)를 정의한 헤더 파일
 * 내장 명령어란 shell 프로그램 자체에서 실행되는 명령어로, 외부 프로그램 실행 없이도 처리
 *
 * 헤더 파일의 주요 역할:
 * 1. 내장 명령어 함수들의 선언을 제공
 * 2. 관련 라이브러리를 포함하여 내장 명령어 구현에 필요한 기능을 제공
 * 3. 다중 포함 방지를 위해 전처리기 사용
 *
 * 선언된 내장 명령어:
 * - my_help: 도움말 출력
 * - my_cd: 디렉토리 변경
 * - my_exit: shell 종료
 * - my_pwd: 현재 디렉토리 출력
 * - my_echo: 문자열 출력
 * - handle_builtin_command: 입력된 명령어를 확인하여 내장 명령어를 실행
 */
#ifndef BUILTIN_COMMANDS_H  // 헤더 파일이 중복 포함되지 않도록 처리
#define BUILTIN_COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int my_help();
int my_cd(char **path);
int my_exit();
int my_pwd();
int my_echo(char **msg);
int handle_builtin_command(char **cmd);

#endif //BUILTIN_COMMANDS_H
