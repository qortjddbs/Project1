#include <iostream>
#include <vector>
#include <gl/glew.h> //--- 필요한 헤더파일 include
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <random>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid Mouse(int button, int state, int x, int y);
GLvoid Motion(int x, int y);
GLfloat bgColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 초기값 : 흰색 
bool left_button = false;
bool right_button = false;
int findIndex = -1;

std::random_device rd;  // 시드값을 얻기 위한 random_device 생성.
std::mt19937 gen(rd());	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
std::uniform_real_distribution<float> dis(0.0f, 1.0f); // 0.0f 부터 1.0f 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
std::uniform_real_distribution<float> dis_coord(-1.0f, 1.0f);
std::uniform_real_distribution<float> dis_center(-0.1f, 0.1f);
std::uniform_real_distribution<float> dis_size(0.05f, 0.1f);

class RECTANGLE {
public:
	float x, y; // 사각형의 중심 좌표
	float width, height; // 사각형의 너비와 높이
	float r, g, b; // 사각형의 색상 (RGB)

	RECTANGLE(float x, float y, float w, float h, float red, float green, float blue)
		: x(x), y(y), width(w), height(h), r(red), g(green), b(blue) {
	}

	void draw() {
		glColor3f(r, g, b);
		glRectf(x - width, y + height, x + width, y - height);
	}

	void randomColor() {
		r = dis(gen);
		g = dis(gen);
		b = dis(gen);
	}
};

bool is_combine(RECTANGLE& rect1, RECTANGLE& rect2) {
	// 두 사각형이 겹치는지 확인
	bool overlapX = (rect1.x - rect1.width < rect2.x + rect2.width) && (rect1.x + rect1.width > rect2.x - rect2.width);
	bool overlapY = (rect1.y - rect1.height < rect2.y + rect2.height) && (rect1.y + rect1.height > rect2.y - rect2.height);
	return overlapX && overlapY;
}

float mapToGLCoordX(int x) {
	return (static_cast<float>(x) / (WINDOW_WIDTH / 2)) - 1.0f;
}

float mapToGLCoordY(int y) {
	return 1.0f - (static_cast<float>(y) / (WINDOW_HEIGHT / 2));
}

std::vector<RECTANGLE> rectangles;

void initBackground() {
	rectangles.push_back(RECTANGLE(0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f)); // 중앙
	for (int i = 0; i < 5; ++i) {
		rectangles.push_back(RECTANGLE(dis_coord(gen), dis_coord(gen), dis_size(gen), dis_size(gen), dis(gen), dis(gen), dis(gen)));
	}
}

void addRectangles(float x, float y) {
	rectangles.push_back(RECTANGLE(x, y, 0.05f, 0.05f, dis(gen), dis(gen), dis(gen))); // 중앙
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("practice 5"); // 윈도우 생성 (윈도우 이름)

	//--- GLEW 초기화하기
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) // glew 초기화 
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	initBackground();
	glutDisplayFunc(drawScene); // 출력 콜백함수의 지정
	glutReshapeFunc(Reshape); // 다시 그리기 콜백함수 지정
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid drawScene() //--- 콜백 함수: 출력 콜백 함수 
{
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.
	rectangles[0].draw();
	for (int i = 1; i < rectangles.size(); ++i) {
		rectangles[i].draw();
	}
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
	float Mouse_x = mapToGLCoordX(x);
	float Mouse_y = mapToGLCoordY(y);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {	// 좌클릭
		left_button = true;
		rectangles[1].x = mapToGLCoordX(x);
		rectangles[1].y = mapToGLCoordY(y);
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		left_button = false;
		rectangles[1].r = 0.0f;
		rectangles[1].g = 0.0f;
		rectangles[1].b = 0.0f;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {	// 우클릭
		if (rectangles.size() < 30) {
			addRectangles(Mouse_x, Mouse_y);
			rectangles[1].width -= 0.02f;
			rectangles[1].height -= 0.02f;
		}
	}

	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

GLvoid Motion(int x, int y) {
	if (left_button == true) {
		rectangles[1].x = mapToGLCoordX(x);
		rectangles[1].y = mapToGLCoordY(y);
	}

	if (left_button == true) {
		for (int i = rectangles.size() - 1; i > 1; --i) {
			// 지우개(rectangles[1])와 다른 사각형(rectangles[i])이 겹치는지 체크
			if (is_combine(rectangles[1], rectangles[i])) {
				rectangles[1].r = rectangles[i].r;
				rectangles[1].g = rectangles[i].g;
				rectangles[1].b = rectangles[i].b;
				rectangles.erase(rectangles.begin() + i);
				rectangles[1].width += 0.02f;
				rectangles[1].height += 0.02f;
			}
		}
	}

	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}