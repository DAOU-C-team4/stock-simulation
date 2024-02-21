##### stock-simulation

<hr/>

## project setting
1. [openssl 설치](https://slproweb.com/download/Win64OpenSSL-3_2_1.exe)
2. sqlite3 헤더 세팅 (완료)

+ 추가
1. db browser tool : sqlite3 db qui툴로 작업 권장

<hr/>

## project architecture
1. 서버 : C기반으로 작성한 콘솔로 소켓통신(클라이언트) + DB연동
2. DB : sqlite3로 서버와 연동
3. 클라이언트 : C기반으로 작성한 콘솔로 입출력 기반으로 서버와 통신
