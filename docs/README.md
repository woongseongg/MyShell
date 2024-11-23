# MyShell
myshell project for Linux

'myshell'은 C 언어로 구현된 쉘 프로그램으로, 기본적인 명령어 실행과 몇 가지 내장 명령어를 지원합니다.


## 주요 기능
- 외부 명령어 실행: 사용자가 입력한 명령어를 실행 파일로 찾아 실행합니다.
- 내장 명령어:
  - 'cd <directory>': 디렉토리 변경
  - 'help': 쉘 명령어 목록과 설명을 출력
  - 'exit': 쉘을 종료
  - 'pwd': 현재 디렉토리 출력
  - 'echo <message>': 입력한 메세지 출력
- 환경 설정 로드: 'config/.myshell' 파일에서 사용자 정의 경로 정보를 불러옵니다.


## 프로젝트 구조
- 'src/': 소스 코드가 위치한 디렉토리
  - 'myshell.c': 쉘 프로그램의 메인 소스 파일
  - 'builtin_commands.c': 내장 명령어 정의 파일
- 'include/': 헤더 파일이 위치한 디렉토리
  - 'builtin_commands.h': 내장 명령어 함수 선언
- 'config/': 설정 파일 디렉토리
  - '.myshell': 사용자 정의 경로 설정 파일
- 'Makefile': 프로그램 빌드 작업 자동화 파일


## 설치 및 실행 방법

### 요구 사항
  - GCC 컴파일러가 필요합니다.
    
### 컴파일
프로젝트 디렉토리에서 다음 명령어를 실행하여 컴파일합니다.
```bash
make
