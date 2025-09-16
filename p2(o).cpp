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
GLfloat bgColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 초기값 : 흰색 

std::random_device rd;  // 시드값을 얻기 위한 random_device 생성.
std::mt19937 gen(rd());	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
std::uniform_real_distribution<float> dis(0.0f, 1.0f); // 0.0f 부터 1.0f 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.

class RECTANGLE {
public:
	float x, y; // 사각형의 중심 좌표
	float width, height; // 사각형의 너비와 높이
	float r, g, b; // 사각형의 색상 (RGB)

	RECTANGLE(float x, float y, float w, float h, float red, float green, float blue)
		: x(x), y(y), width(w), height(h), r(red), g(green), b(blue) {}

	void draw() {
		glColor3f(r, g, b);
		glRectf(x - width, y + height, x + width, y - height);
	}

	bool is_contain(float mx, float my) const {
		return mx >= x - width && mx <= x + width &&
			my >= y - height && my <= y + height;
	}

	void randomColor() {
		r = dis(gen);
		g = dis(gen);
		b = dis(gen);
	}
};

std::vector<RECTANGLE> rectangles;

float mapToGLCoordX(int x) {
	return (static_cast<float>(x) / (WINDOW_WIDTH / 2)) - 1.0f;
}

float mapToGLCoordY(int y) {
	return 1.0f - (static_cast<float>(y) / (WINDOW_HEIGHT / 2));
}

void initRectangles() {
	rectangles.push_back(RECTANGLE(0.5f, 0.5f, 0.5f, 0.5f, dis(gen), dis(gen), dis(gen))); // 마지막 원소 뒤에 삽입 (push_back) - 1사분면
	rectangles.push_back(RECTANGLE(-0.5f, 0.5f, 0.5f, 0.5f, dis(gen), dis(gen), dis(gen))); // 2사분면
	rectangles.push_back(RECTANGLE(-0.5f, -0.5f, 0.5f, 0.5f, dis(gen), dis(gen), dis(gen))); // 3사분면
	rectangles.push_back(RECTANGLE(0.5f, -0.5f, 0.5f, 0.5f, dis(gen), dis(gen), dis(gen))); // 4사분면
	rectangles.push_back(RECTANGLE(0.0f, 0.0f, 1.0f, 1.0f, dis(gen), dis(gen), dis(gen))); // 중앙
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // 윈도우의 크기 지정
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
	rectangles[4].draw(); // 중앙 사각형이 가장 아래에 그려지도록
	rectangles[0].draw();
	rectangles[1].draw();
	rectangles[2].draw();
	rectangles[3].draw();
	// 그리기 부분 구현: 그리기 관련 부분이 여기에 포함된다.
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
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {	// 왼쪽 클릭
		if ((Mouse_x > (rectangles[0].x-rectangles[0].width) && Mouse_x < (rectangles[0].x + rectangles[0].width)) && (Mouse_y > (rectangles[0].y - rectangles[0].height) && Mouse_y < rectangles[0].y + rectangles[0].height)) {	// 1사분면
			rectangles[0].randomColor();
		} 
		else if ((Mouse_x > (rectangles[1].x - rectangles[1].width) && Mouse_x < (rectangles[1].x + rectangles[1].width)) && (Mouse_y > (rectangles[1].y - rectangles[1].height) && Mouse_y < rectangles[1].y + rectangles[1].height)) {	// 2사분면
			rectangles[1].randomColor();
		}
		else if ((Mouse_x > (rectangles[2].x - rectangles[2].width) && Mouse_x < (rectangles[2].x + rectangles[2].width)) && (Mouse_y > (rectangles[2].y - rectangles[2].height) && Mouse_y < rectangles[2].y + rectangles[2].height)) {	// 3사분면
			rectangles[2].randomColor();
		}
		else if ((Mouse_x > (rectangles[3].x - rectangles[3].width) && Mouse_x < (rectangles[3].x + rectangles[3].width)) && (Mouse_y > (rectangles[3].y - rectangles[3].height) && Mouse_y < rectangles[3].y + rectangles[3].height)) {	// 4사분면
			rectangles[3].randomColor();
		}
		else {
			rectangles[4].randomColor();
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {	// 오른쪽 클릭
		if ((Mouse_x > (rectangles[0].x - rectangles[0].width) && Mouse_x < (rectangles[0].x + rectangles[0].width)) && (Mouse_y > (rectangles[0].y - rectangles[0].height) && Mouse_y < rectangles[0].y + rectangles[0].height)) {	// 1사분면
			if (rectangles[0].width > 0.1f && rectangles[0].height > 0.1f) {
				rectangles[0].width -= 0.05f;
				rectangles[0].height -= 0.05f;
			}
		}
		else if ((Mouse_x > (rectangles[1].x - rectangles[1].width) && Mouse_x < (rectangles[1].x + rectangles[1].width)) && (Mouse_y > (rectangles[1].y - rectangles[1].height) && Mouse_y < rectangles[1].y + rectangles[1].height)) {	// 2사분면
			if (rectangles[1].width > 0.1f && rectangles[1].height > 0.1f) {
				rectangles[1].width -= 0.05f;
				rectangles[1].height -= 0.05f;
			}
		}
		else if ((Mouse_x > (rectangles[2].x - rectangles[2].width) && Mouse_x < (rectangles[2].x + rectangles[2].width)) && (Mouse_y > (rectangles[2].y - rectangles[2].height) && Mouse_y < rectangles[2].y + rectangles[2].height)) {	// 3사분면
				if (rectangles[2].width > 0.1f && rectangles[2].height > 0.1f) {
					rectangles[2].width -= 0.05f;
					rectangles[2].height -= 0.05f;
				} 
		}
		else if ((Mouse_x > (rectangles[3].x - rectangles[3].width) && Mouse_x < (rectangles[3].x + rectangles[3].width)) && (Mouse_y > (rectangles[3].y - rectangles[3].height) && Mouse_y < rectangles[3].y + rectangles[3].height)) {	// 4사분면
			if (rectangles[3].width > 0.1f && rectangles[3].height > 0.1f) {
				rectangles[3].width -= 0.05f;
				rectangles[3].height -= 0.05f;
			}
		}
		else if ((Mouse_x > 0 && Mouse_x < 1.0f) && (Mouse_y > 0 && Mouse_y < 1.0f)) {
			if (rectangles[0].width < 1.0f && rectangles[0].height < 1.0f) {
				rectangles[0].width += 0.05f;
				rectangles[0].height += 0.05f;
			}
		}
		else if ((Mouse_x > -1.0f && Mouse_x < 0) && (Mouse_y > 0 && Mouse_y < 1.0f)) {
			if (rectangles[1].width < 1.0f && rectangles[1].height < 1.0f) {
				rectangles[1].width += 0.05f;
				rectangles[1].height += 0.05f;
			}
		}
		else if ((Mouse_x > -1.0f && Mouse_x < 0) && (Mouse_y > -1.0f && Mouse_y < 0)) {
			if (rectangles[2].width < 1.0f && rectangles[2].height < 1.0f) {
				rectangles[2].width += 0.05f;
				rectangles[2].height += 0.05f;
			}
		}
		else if ((Mouse_x > 0 && Mouse_x < 1.0f) && (Mouse_y > -1.0f && Mouse_y < 0)) {
			if (rectangles[3].width < 1.0f && rectangles[3].height < 1.0f) {
				rectangles[3].width += 0.05f;
				rectangles[3].height += 0.05f;
			}
		}
	}

	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}