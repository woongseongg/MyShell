/* 
 *
 * Main Shell Program
 * shell 인터페이스를 구현
 * 
 * 주요 기능:
 * 1. 사용자 입력을 받아 명령어와 인자를 처리
 * 2. 내장 명령어를 확인하고 실행
 * 3. 외부 명령어를 PATH에서 검색하여 실행
 * 4. SIGINT(Ctrl+C) 입력을 처리하여 프로그램 종료 없이 동작을 유지
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include "../include/builtin_commands.h"	// 내장 명령어 처리를 위한 헤더 파일 포함

// 사용자 입력 또는 파일 읽기용 버퍼 크기	
#define BUFFER_SIZ 512 				
// 환경 변수 내 최대 경로 수
#define MAX_PATH 256 				
// 사용자 입력에서 파싱 가능한 최대 토큰 수
#define MAX_TOKEN 512				

// 내장 명령어 처리를 위한 전역 변수
char cmd_for_builtin[10];


/**
 * SIGINT(Ctrl+C) 핸들러
 *
 * 목적:
 * 사용자가 Ctrl+C를 눌렀을 때 프로그램이 종료되지 않고 메시지를 출력한 후 계속 실행되도록 설정
 * 
 * 주요 동작:
 * 1. SIGINT 시그널을 받으면 메시지를 출력
 * 2. 프로그램 동작을 중단하지 않고 shell 동작을 유지
 *
 * @param sig 처리할 시그널 번호 (SIGINT에 해당)
 */
void sigint_handler(int sig) {
	printf("\nsighandler occurred :%d\n\n", sig);
}


/**
 * 환경 변수 설정 함수
 *
 * 목적:
 * .myshell 파일에서 PATH 정보를 읽어와 shell이 명령어를 검색할 경로 목록을 설정
 *
 * 파일 형식:
 * .myshell 파일은 "PATH=경로1:경로2:..." 형식이며, ':'로 경로를 구분
 *
 * 주요 동작:
 * 1. .myshell 파일을 열고 PATH 값 읽어오기
 * 2. ':'로 구분된 각 경로를 파싱하여 pathPtr 배열에 저장
 * 3. 잘못된 파일 형식이거나 메모리 할당 실패 시 오류를 출력하고 프로그램을 종료
 *
 * @param pathPtr[] PATH 경로를 저장할 배열
 * @param pathCount 경로 개수를 저장할 변수
 */
void load_environment(char *pathPtr[], int *pathCount) {
	int fd;
	char buffer[BUFFER_SIZ];	// 설정 파일 내용을 읽을 버퍼
	char *path_start, *tok;		// PATH 값을 추출하기 위한 포인터
	ssize_t nread;			// 읽은 바이트 수

	// .myshell 파일 읽기 전용으로 열기
	if((fd = open("config/.myshell", O_RDONLY)) < 0) {
		perror("Failed to open .myshell\n");
		exit(-1);
	}

	// 파일 읽기
	if((nread = read(fd, buffer, BUFFER_SIZ - 1)) < 0) {
		perror("Failed to read .myshell\n");
		close(fd);
		exit(-1);
	}
	close(fd);

	// 읽은 데이터 끝에 NULL 문자 추가
	buffer[nread] = '\0';	
	// 디버깅용 메세지
	// printf("Read paths: %s\n", buffer);

	// "PATH=" 문자열 찾기기
	path_start = strstr(buffer, "PATH=");
	if(!path_start) {
		fprintf(stderr, "Invalid .myshell format\n");
		exit(-2);	// 올바른 PATH 값 없으면 종료
	}
	path_start += 5;	// "PATH=" 이후 값으로 포인터 이동("PATH=" 문자 길이 5)

	// PATH 값을 ':' 기준으로 나누어 pathPtr 배열에 저장
	tok = strtok(path_start, ":");
	*pathCount = 0;

	while(tok && *pathCount < MAX_PATH) {
		if(strlen(tok) > 0) {
			pathPtr[*pathCount] = strdup(tok);	// 경로 복사
			if(!pathPtr[*pathCount]) {
				perror("Failed to allocate memory");
				exit(-3);			// 메모리 할당 실패 시 프로그램 종료
			}
			// 추가된 경로 출
			printf("Path added: %s\n", pathPtr[*pathCount]);
			(*pathCount)++;				// 경로 개수 증가
		}
		tok = strtok(NULL, ":");			// 다음 경로로 이동
	}
}


/**
 * 사용자 입력 처리 함수
 *
 * 목적:
 * 사용자로부터 입력받은 문자열을 공백 단위로 나누어 명령어와 인자를 추출
 *
 * 주요 동작:
 * 1. 입력 문자열에서 공백을 기준으로 토큰을 나눔
 * 2. 각 토큰을 동적으로 메모리 할당하여 token 배열에 저장
 * 3. 파싱된 토큰 개수를 반환하며, 실패 시 -1을 반환
 *
 * @param cmd 사용자 입력 문자열
 * @param token 파싱된 명령어와 인자를 저장할 배열
 * @param token_index 파싱된 토큰 개수를 저장할 변수
 * @return 파싱된 토큰 개수, 실패 시 -1 반환
 */
int extract_command(char *cmd, char *token[], int *token_index) {
	char *start = cmd;	// 입력 문자열의 현재 위치
	char *token_start;	// 현재 토큰의 시작 위치
	int len = 0;
	*token_index = 0;	// 토큰 개수 초기화

	memset(cmd_for_builtin, 0x00, 10);	// 내장 명령어 버퍼 초기화

	// 공백 단위로 문자열 나누기
	while(*start) {
		while(*start == ' ' || *start == '\t') {
			start++;	// 공백 건너뛰기
		}

		if(*start) {
			token_start = start;		// 토큰 시작 위치 저장
			while(*start != ' ' && *start != '\t' && *start != '\0') {
				start++;
			}
			
			len = start - token_start;	// 토큰 길이 계산
			token[*token_index] = (char*)malloc(len + 1);	// 토큰 메모리 할당
			if(!token[*token_index]) {
				perror("Failed to allocate memory\n");
				exit(-3);		// 메모리 할당 실패 시 종료
			}
			strncpy(token[*token_index], token_start, len);	// 토큰 복사
			token[*token_index][len] = '\0';		// NULL 문자 추가
			(*token_index)++;		// 토큰 개수 증가
		 }
	}
	token[*token_index] = NULL;			// 토큰 배열 끝에 NULL 추가
	strcpy(cmd_for_builtin, token[(*token_index) - 1]);	// 마지막 토큰 저장
	if(*token_index > 0) 
		return *token_index;	// 성공 시 토큰 개수 반환
	else
		return -1;		// 실패 시 -1 반환		
}


/**
 * 명령어 실행 가능 여부 확인 함수
 *
 * 목적:
 * 주어진 명령어가 PATH 경로 내에서 실행 가능한지 확인
 *
 * 주요 동작:
 * 1. PATH 배열의 각 경로에서 명령어를 검색
 * 2. 해당 명령어가 실행 가능한 경우 1을 반환
 * 3. 실행 불가능한 경우 0을 반환
 *
 * @param cmd 확인할 명령어 이름
 * @param pathPtr PATH 경로 배열
 * @param pathCount PATH 배열 크기
 * @param fullPath 명령어의 전체 경로를 저장할 버퍼
 * @return 명령어를 찾으면 1, 없으면 0 반환
 */
int find_command(char *cmd, char *pathPtr[], int pathCount, char *fullPath) {
	for(int i=0;i<pathCount;i++) {

		snprintf(fullPath, MAX_PATH, "%s/%s", pathPtr[i], cmd);	// 전체 경로 생성
		if(access(fullPath, X_OK) == 0) {			// 실행 가능한지 확인	
			return 1;	// 가능하면 1 반환
		}
	}
	return 0;			// 명령어를 찾지 못했으면 0 반환
}


/**
 * 메인 함수
 *
 * 목적:
 * shell 프로그램의 진입점으로, 전체 실행 흐름을 제어
 *
 * 주요 동작:
 * 1. 환경 변수(PATH) 설정
 *    - .myshell 파일에서 PATH 값을 읽어와 명령어를 검색할 경로를 설정
 *
 * 2. 사용자 입력 처리
 *    - 사용자로부터 명령어를 입력받아 공백으로 구분된 명령어와 인자를 파싱
 *    - 입력된 명령어가 내장 명령어인지 확인하고 실행
 *
 * 3. 외부 명령어 처리
 *    - 입력된 명령어가 외부 명령어인 경우 PATH에서 실행 파일을 검색하고 실행
 *    - 자식 프로세스를 생성하여 명령어를 실행
 *    - 부모 프로세스는 자식 프로세스의 종료를 대기
 *
 * 4. 반복 실행
 *    - 사용자로부터 "EOF(Ctrl+D)" 또는 "SIGQUIT(Ctrl+\)" 또는 "exit" 명령어를 받을 때까지 반복 실행
 *
 * 5. 메모리 해제
 *    - 사용자 입력, PATH 배열 등 동적으로 할당된 메모리를 주기적으로 해제
 *
 * 신호 처리:
 *  - SIGINT(Ctrl+C)를 처리하여 프로그램이 종료되지 않고 계속 실행되도록 설정
 *
 * 주요 변수:
 * - pathPtr[MAX_PATH]: PATH에서 읽어온 경로들을 저장하는 배열
 * - token[MAX_PATH]: 사용자 입력에서 추출한 명령어와 인자를 저장하는 배열
 * - fullPath[BUFFER_SIZ]: 실행할 명령어의 전체 경로를 저장
 * - line[BUFFER_SIZ]: 사용자로부터 입력받은 한 줄의 문자열
 * - pathCount: PATH 배열에 저장된 경로의 개수
 * - token_index: 파싱된 명령어와 인자의 개수
 */
int main(int argc, char *argv[]) {
	char *pathPtr[MAX_PATH];	// PATH에서 읽은 경로를 저장할 배열
	char *token[MAX_PATH];		// 사용자 입력에서 파싱된 명령어와 인자 저장
	char fullPath[BUFFER_SIZ];	// 명령어의 전체 경로 저장
	char line[BUFFER_SIZ];		// 사용자 입력을 저장할 버퍼
	int pid;			// 자식 프로세스 ID
	int status;			// 자식 프로세스 종료 상태
	int pathCount = 0;		// PATH 배열에 저장된 경로 개수
	int token_index = 0;		// 파싱된 명령어와 인자 개수

    	// 사용자가 "Ctrl+C"를 입력해도 프로그램이 종료되지 않고 계속 실행
	signal(SIGINT, sigint_handler);		

	// STEP 1: 환경 변수(PATH) 설정
    	// .myshell 파일에서 PATH 값을 읽어와 pathPtr 배열에 저장
	load_environment(pathPtr, &pathCount);

	// STEP 2: shell 실행 루프
   	// 사용자가 EOF(Ctrl+D), exit를 입력할 때까지 계속 실행
	while(1) {
		printf("wooseong %% ");		// 프롬프트 출력
		fflush(stdout);			// 출력 버퍼 비우
		
		// fgets로 한 줄의 사용자 입력을 받아 line 배열에 저장
		if(fgets(line, BUFFER_SIZ, stdin) == NULL) {
			// EOF가 입력된 경우 프로그램 종료
			if(feof(stdin)) {
				break;
			}
			// 입력 중 오류가 발생한 경우 처리
			if(ferror(stdin)) {	
				perror("Error reading input");
				clearerr(stdin);	// 에러 플래그 초기화
				continue;		// 다음 루프로 이동
			}
		}

		 // 입력된 줄 끝의 개행 문자 제거
		line[strlen(line) - 1] = '\0';
		
		// STEP 3: 사용자 입력 파싱
        	// 사용자 입력을 공백 기준으로 나누어 명령어와 인자로 분리
		if(!extract_command(line, token, &token_index))	{
			// 입력이 없으면 루프 계속 실행
			continue;
		}

		// STEP 4: 내장 명령어 처리
        	// 입력된 명령어가 내장 명령어인지 확인하고 실행
		if(handle_builtin_command(token) != -1) {
			// 내장 명령어 처리 후 다음 루프 계속 실행
			continue;
		}

		// STEP 5: 외부 명령어 처리
        	// PATH에서 실행 파일을 검색하고 명령어 실행
		if(find_command(token[0], pathPtr, pathCount, fullPath)) {
			// 자식 프로세스 생성
			if((pid = fork()) == 0) {
				// execv로 명령어 실행
				execv(fullPath, token);
				
				/* 명령어 정상 실행 시 이 부분은 실행되지 않음 */
				// 실행 실패 시 오류 출력
				perror("execv failed\n");
				// 자식 프로세스 종료
				exit(-2);
			}
			// 부모 프로세스
			else if(pid > 0) {
				// 자식 프로세스 종료 대기
				wait(&status);
				// 디버깅용 메세지
				// printf("\n\tChild process termintaed\n\n");
			}
			else {
				// fork 실패 시 오류 출력
				perror("fork error\n");
			}
		}

		// STEP 6: 메모리 해제
        	// 동적으로 할당된 토큰 메모리를 해제
		for(int i=0;i<token_index;i++) {
			free(token[i]);
		}
	}
	// STEP 7: 종료 시 메모리 해제
    	// PATH 배열에 할당된 메모리를 모두 해제
	for(int i=0;i<pathCount;i++) {
		free(pathPtr[i]);
	}

	// 프로그램 종료
	return 0;	
}
