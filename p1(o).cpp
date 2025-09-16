#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid TimerFunction(int value);
bool timer = false;

std::random_device rd;  // 시드값을 얻기 위한 random_device 생성.
std::mt19937 gen(rd());	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
std::uniform_real_distribution<float> dis(0.0f, 1.0f); // 0.0f 부터 1.0f 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
		//--- 윈도우 생성하기
		glutInit(&argc, argv); //--- glut 초기화
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); //--- 디스플레이 모드 설정
		glutInitWindowPosition(0, 0); //--- 윈도우의 위치 지정
		glutInitWindowSize(800, 600); //--- 윈도우의 크기 지정
		glutCreateWindow("Practice 1"); //--- 윈도우 생성 (윈도우 이름)
		//--- GLEW 초기화하기
		glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) //--- glew 초기화 
	{
	std::cerr << "Unable to initialize GLEW" << std::endl;
	exit(EXIT_FAILURE);
	}
else
	std::cout << "GLEW Initialized\n";
	glutDisplayFunc(drawScene); //--- 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); //--- 다시 그리기 콜백함수 지정
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutMainLoop(); //--- 이벤트 처리 시작
}

// 전역 변수로 배경색 지정
GLfloat bgColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 초기값 : 흰색 

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	//--- 변경된 배경색 설정
	glClearColor(bgColor[0], bgColor[1], bgColor[2], bgColor[3]); //--- 바탕색을 변경
	glClear(GL_COLOR_BUFFER_BIT); //--- 설정된 색으로 전체를 칠하기
	glutSwapBuffers(); //--- 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

GLvoid TimerFunction(int value) // 타이머 콜백 함수
{
	if (!timer) return; // 타이머가 비활성화 상태이면 함수 종료
	bgColor[0] = dis(gen);
	bgColor[1] = dis(gen);
	bgColor[2] = dis(gen);
	bgColor[3] = 1.0f;
	glutPostRedisplay(); // 화면 갱신 요청
	glutTimerFunc(100, TimerFunction, 1); // 100밀리초 후에 다시 타이머 함수 호출
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'c': //--- 배경색을 청록색으로 설정
		bgColor[0] = 0.0f;
		bgColor[1] = 1.0f;
		bgColor[2] = 1.0f;
		bgColor[3] = 1.0f;
		break; 

	case 'm':
		bgColor[0] = 1.0f;
		bgColor[1] = 0.0f;
		bgColor[2] = 1.0f;
		bgColor[3] = 1.0f;
		break; //--- 배경색을 자홍색으로 설정

	case 'y':
		bgColor[0] = 1.0f;
		bgColor[1] = 1.0f;
		bgColor[2] = 0.0f;
		bgColor[3] = 1.0f;
		break; //--- 배경색을 노랑색으로 설정

	case 'a':
		bgColor[0] = dis(gen);
		bgColor[1] = dis(gen);
		bgColor[2] = dis(gen);
		bgColor[3] = 1.0f;
		break;

	case 'w':
		bgColor[0] = 1.0f;
		bgColor[1] = 1.0f;
		bgColor[2] = 1.0f;
		bgColor[3] = 1.0f;
		break;

	case 'k':
		bgColor[0] = 0.0f;
		bgColor[1] = 0.0f;
		bgColor[2] = 0.0f;
		bgColor[3] = 1.0f;
		break;
		
	case 't':		// 근데 누를때마다 타이머가 빨라짐.
		timer = true;
		glutTimerFunc(100, TimerFunction, 1); // 타이머 함수 설정
		break;

	case 's':
		timer = false;
		break;

	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}