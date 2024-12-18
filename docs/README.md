# MyShell
myshell project for Linux

MyShell은 C 언어로 구현된 간단한 shell 프로그램으로, 사용자로부터 명령어를 입력받아 실행합니다. 
외부 명령어 실행 기능과 함께 몇 가지 유용한 내장 명령어도 지원합니다.

GitHub에 업로드된 프로젝트에는 **실행 파일 및 컴파일된 결과물은 포함되지 않았으며**, 사용자가 직접 소스 코드를 컴파일하여 실행 파일을 생성해야 합니다.

---

## 주요 기능

1. **외부 명령어 실행**
   - 사용자가 입력한 명령어를 실행 파일로 찾아 실행합니다.
   - 예: `ls -l`, `gcc -o program program.c`, `./my_program`

2. **내장 명령어 지원**
   - `cd [경로]`: 현재 작업 디렉토리를 변경합니다.
   - `help`: 지원되는 내장 명령어 목록과 사용법을 출력합니다.
   - `exit`: shell을 종료합니다.
   - `pwd`: 현재 디렉토리 경로를 출력합니다.
   - `echo [메세지]`: 입력한 메세지를 출력합니다.

3. **환경 설정 로드**
   - `config/.myshell` 파일에서 사용자 정의 PATH 설정을 불러옵니다.
   - PATH 변수를 설정하여 명령어 실행 가능 범위를 확장합니다.

4. **Makefile 기반 빌드 자동화**
   - Makefile을 사용하여 소스 코드를 쉽게 컴파일하고, 결과 파일을 관리할 수 있습니다.

---

## 프로젝트 구조

```plaintext
MyShell/
├── bin/                     # 생성된 실행 파일 디렉토리
│   └── myshell              # (빌드 후 생성될 실행 파일)
├── src/                     # 소스 파일 디렉토리
│   ├── myshell.c            # 메인 프로그램 소스 파일
│   └── builtin_commands.c   # 내장 명령어 구현 파일
├── include/                 # 헤더 파일 디렉토리
│   └── builtin_commands.h   # 내장 명령어 선언 헤더 파일
├── config/                  # 설정 파일 디렉토리
│   └── .myshell             # 사용자 정의 PATH 설정 파일
├── Makefile                 # 빌드 및 클린업 자동화 파일
└── README.md                # 프로젝트 설명 파일
