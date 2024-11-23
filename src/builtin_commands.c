/*
 * 내장 명령어 정의 파일
 *
 * shell 프로그램에서 사용되는 내장 명령어를 구현
 * 내장 명령어란 외부 프로그램을 실행하지 않고, shell 자체에서 수행되는 명령어
 *
 * 구현된 내장 명령어:
 * 1. my_cd: 디렉토리 변경
 * 2. my_help: 도움말 출력 (지원 명령어 목록)
 * 3. my_exit: shell 종료
 * 4. my_pwd: 현재 디렉토리 경로 출력
 * 5. my_echo: 문자열 출력
 *
 * 또한 handle_builtin_command 함수는 입력된 명령어가 내장 명령어인지 확인하고, shell이 종료되지 않고 계속 실행
 */
int my_help() {
	printf("Wooseong's Shell\n");
	printf("Type program names and arguments, and hit enter.\n");
	printf("The following are built in:\n\n");

	// 현재 shell에서 지원하는 내장 명령어 목록 출력
	printf("\tmy_cd\n");
	printf("\tmy_help\n");
	printf("\tmy_exit\n");
	printf("\tmy_pwd\n");
	printf("\tmy_echo\n");

	return 1;	// 현재 shell에서 지원하는 내장 명령어 목록 출력
}


/**
 * my_cd: 작업 디렉토리를 변경하는 함수
 *
 * 목적:
 * - 사용자가 `cd [디렉토리 경로]` 명령어를 입력하면, 해당 디렉토리로 이동
 * - 디렉토리 경로가 주어지지 않거나 이동에 실패하면 오류 메시지 출력
 *
 * 매개변수:
 * - char **path: 명령어와 인자를 포함한 문자열 배열
 *   - path[0]: "cd"
 *   - path[1]: 이동할 디렉토리 경로
 *
 * 주요 동작:
 * 1. 디렉토리 경로가 주어지지 않으면 오류 메시지 출력
 * 2. `chdir` 함수를 이용해 디렉토리 변경
 * 3. 변경 실패 시 시스템 오류 메시지 출력
 *
 * 반환값:
 * - 항상 1을 반환하여 shell이 종료되지 않고 계속 실행
 */
int my_cd(char **path) {
	if(path[1] == NULL) {	// 디렉토리 경로가 입력되지 않은 경우
		fprintf(stderr, "expected argument to \"cd\"\n");	// 오류 메시지 출력
	}
	else {
		if(chdir(path[1]) != 0) {	// 디렉토리 변경 시도, 실패하면 오류 출력
			perror("my_cd");
		}
	}
	return 1;		// shell이 계속 실행되도록 1 반환
}


/**
 * my_exit: shell 프로그램을 종료하는 함수
 *
 * 목적:
 * - 사용자가 `exit` 명령어를 입력하면 프로그램 종료
 *
 * 주요 동작:
 * 1. `exit` 함수를 호출하여 프로그램을 종료
 * 2. 종료 상태로 `EXIT_SUCCESS`를 반환
 *
 * 반환값:
 * - 반환값이 없으며, 프로그램 종료
 */
int my_exit() {
	exit(EXIT_SUCCESS);
}


/**
 * my_pwd: 현재 작업 디렉토리 경로를 출력하는 함수
 *
 * 목적:
 * - 사용자가 `pwd` 명령어를 입력하면, 현재 디렉토리의 경로를 출력
 *
 * 주요 동작:
 * 1. `getcwd` 함수를 이용해 현재 디렉토리의 전체 경로를 가져옴
 * 2. 경로를 출력하며, 가져오는 데 실패하면 오류 메시지를 출력
 *
 * 반환값:
 * - 항상 1을 반환하여 shell이 종료되지 않고 계속 실행
 */
int my_pwd() {
	char cwd[512];		// 현재 디렉토리 경로를 저장할 버퍼
	if(getcwd(cwd, sizeof(cwd)) != NULL) {	// 경로 가져오기 성공 시 출력
		printf("%s\n", cwd);
	}
	else {					// 경로 가져오기 실패 시 오류 메시지 출력
		perror("my_pwd");
	}
	return 1;		// shell이 계속 실행되도록 1 반환
}


/**
 * my_echo: 입력받은 문자열을 출력하는 함수
 *
 * 목적:
 * - 사용자가 `echo [문자열]` 명령어를 입력하면, 입력받은 문자열을 출력
 *
 * 매개변수:
 * - char **msg: 명령어와 출력할 문자열을 포함한 배열
 *   - msg[0]: "echo"
 *   - msg[1] 이후: 출력할 문자열
 *
 * 주요 동작:
 * 1. 명령어 이름(msg[0])을 제외한 나머지 문자열(msg[1] 이후)을 출력
 * 2. 각 문자열 사이에 공백을 추가하여 출력
 * 3. 출력이 끝난 후 줄 바꿈 추가
 *
 * 반환값:
 * - 항상 1을 반환하여 shell이 종료되지 않고 계속 실행
 */
int my_echo(char **msg) {
	for(int i=1;msg[i]!=NULL;i++) {		// 첫 번째 인자 이후 출력
		printf("%s ", msg[i]);
	}
	printf("\n");		// 마지막 줄 바꿈 추가
	return 1;		// shell이 계속 실행되도록 1 반환
}


/**
 * handle_builtin_command: 입력된 명령어가 내장 명령어인지 확인하고 실행하는 함수
 *
 * 목적:
 * - 사용자가 입력한 명령어가 내장 명령어인지 확인
 * - 내장 명령어라면 해당 함수를 호출하여 실행
 *
 * 매개변수:
 * - char **cmd: 명령어와 인자를 포함한 문자열 배열
 *   - cmd[0]: 명령어 이름
 *   - cmd[1] 이후: 명령어 인자
 *
 * 주요 동작:
 * 1. 입력된 명령어가 NULL인 경우 처리하지 않음
 * 2. 입력된 명령어가 "cd", "exit", "help", "pwd", "echo" 중 하나인지 확인
 * 3. 해당 명령어가 내장 명령어일 경우, 해당 함수를 호출하여 실행
 * 4. 내장 명령어가 아닌 경우 오류 메시지 출력
 *
 * 반환값:
 * - 내장 명령어를 실행한 경우: 해당 함수의 반환값(항상 1)
 * - 내장 명령어가 아닌 경우: -1
 */
int handle_builtin_command(char **cmd) {
	if(cmd[0] == NULL) {	// 명령어가 입력되지 않은 경우
		return -1;	// 처리하지 않음
	}	

	// 입력된 명령어를 확인하고 해당 함수 호출
	if(!strcmp(cmd[0], "cd")) {
		return my_cd(cmd);
	} else if(!strcmp(cmd[0], "exit")) {
		return my_exit();
	} else if(!strcmp(cmd[0], "help")) {
		return my_help();
	} else if(!strcmp(cmd[0], "pwd")) {
		return my_pwd();
	} else if(!strcmp(cmd[0], "echo")) {
		return my_echo(cmd);
	}

	// 내장 명령어가 아닌 경우 오류 메시지 출력
	fprintf(stderr, "Unkown command: %s\n", cmd[0]);
	return -1;
}
