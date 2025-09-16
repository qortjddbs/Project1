#include <iostream>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>
#include <vector>

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLfloat bgColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 초기값 : 흰색 

std::random_device rd;  // 시드값을 얻기 위한 random_device 생성.
std::mt19937 gen(rd());	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
std::uniform_real_distribution<float> dis(0.0f, 1.0f); // 0.0f 부터 1.0f 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.

class Rectangle {
public:
	float x, y; // 사각형의 중심 좌표
	float width, height; // 사각형의 너비와 높이
	float r, g, b; // 사각형의 색상 (RGB)

	Rectangle(float x, float y, float w, float h, float red, float green, float blue)
		: x(x), y(y), width(w), height(h), r(red), g(green), b(blue) {}

	void draw() {
		glColor3f(r, g, b);
		glRectf(x - width / 2, y + height / 2, x + width / 2, y - height / 2);
	}

	bool is_contain(float mx, float my) const {
		return mx >= x - width / 2 && mx <= x + width / 2 &&
			my >= y - height / 2 && my <= y + height / 2;
	}

	void randomColor() {
		r = dis(gen);
		g = dis(gen);
		b = dis(gen);
	}
};

std::vector<Rectangle> rectangles;

void initRectangles() {
	rectangles.push_back(Rectangle(0.5f, 0.5f, 1.0f, 1.0f, dis(gen), dis(gen), dis(gen))); // 마지막 원소 뒤에 삽입 (push_back)
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(800, 600); // 윈도우의 크기 지정
	glutCreateWindow("Example1"); // 윈도우 생성 (윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화 
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	std::cout << "GLEW Initialized\n";
	initRectangles();
	glutDisplayFunc(drawScene); // 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 콜백함수 지정
	glutMouseFunc(Mouse);
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid drawScene() //--- 콜백 함수: 출력 콜백 함수 
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 바탕색을 ‘white’로 지정
	glClear(GL_COLOR_BUFFER_BIT); // 설정된 색으로 전체를 칠하기
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.
	bgColor[0] = dis(gen);
	bgColor[1] = dis(gen);
	bgColor[2] = dis(gen);
	glColor3f(bgColor[0], bgColor[1], bgColor[2]);
	glRectf(0, 1.0, 1.0, 0); // 1사분면 사각형 그리기

	bgColor[0] = dis(gen);
	bgColor[1] = dis(gen);
	bgColor[2] = dis(gen);
	glColor3f(bgColor[0], bgColor[1], bgColor[2]);
	glRectf(-1.0, 1.0, 0, 0); // 2사분면 사각형 그리기
	
	bgColor[0] = dis(gen);
	bgColor[1] = dis(gen);
	bgColor[2] = dis(gen);
	glColor3f(bgColor[0], bgColor[1], bgColor[2]);
	glRectf(-1.0, -1.0, 0, 0); // 3사분면사각형 그리기
	
	bgColor[0] = dis(gen);
	bgColor[1] = dis(gen);
	bgColor[2] = dis(gen);
	glColor3f(bgColor[0], bgColor[1], bgColor[2]);
	glRectf(0, 0, 1.0, -1.0); // 4사분면 사각형 그리기

	glutSwapBuffers(); // 화면에 출력하기
}

GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수 
{
	glViewport(0, 0, w, h);
}

GLvoid Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

GLvoid Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {	// 왼쪽 클릭
		if ((x > 0 && x < 1.0) && (y > 0 && y < 1.0)) {	// 1사분면
			bgColor[0] = dis(gen);
			bgColor[1] = dis(gen);
			bgColor[2] = dis(gen);
			glColor3f(bgColor[0], bgColor[1], bgColor[2]);
			glRectf(0, 1.0, 1.0, 0); // 1사분면 사각형 그리기
		}
	}

	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}