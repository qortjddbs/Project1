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
GLvoid TimerFunction(int value);
GLfloat bgColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; // 초기값 : 흰색 
bool left_button = false;
int findIndex = -1;
bool animationRunning = false;

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
	float dx, dy; // 이동 방향
	bool isMoving; // 움직이는 상태인지
	float shrinkRate; // 축소 비율

	RECTANGLE(float x, float y, float w, float h, float red, float green, float blue)
		: x(x), y(y), width(w), height(h), r(red), g(green), b(blue), dx(0), dy(0), isMoving(false), shrinkRate(0.001f) {
	}

	RECTANGLE(float x, float y, float w, float h, float red, float green, float blue, float dx, float dy)
		: x(x), y(y), width(w), height(h), r(red), g(green), b(blue), dx(dx), dy(dy), isMoving(true), shrinkRate(0.001f) {
	}

	void draw() {
		glColor3f(r, g, b);
		glRectf(x - width, y + height, x + width, y - height);
	}

	bool is_contain(float mx, float my) const {
		return mx >= x - width && mx <= x + width && my >= y - height && my <= y + height;
	}

	void randomColor() {
		r = dis(gen);
		g = dis(gen);
		b = dis(gen);
	}

	void update() {
		if (isMoving) {
			x += dx;
			y += dy;
			width -= shrinkRate;
			height -= shrinkRate;
			
			// 화면 경계에서 반사
			// if (x - width <= -1.0f || x + width >= 1.0f) {
			// 	dx = -dx;
			// }
			// if (y - height <= -1.0f || y + height >= 1.0f) {
			// 	dy = -dy;
			// }
		}
	}

	bool shouldRemove() const {
		return isMoving && (width <= 0.0001f || height <= 0.0001f);
	}
};

float mapToGLCoordX(int x) {
	return (static_cast<float>(x) / (WINDOW_WIDTH / 2)) - 1.0f;
}

float mapToGLCoordY(int y) {
	return 1.0f - (static_cast<float>(y) / (WINDOW_HEIGHT / 2));
}

std::vector<RECTANGLE> rectangles;

void initBackground() {
	rectangles.push_back(RECTANGLE(0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f)); // 배경 background 백그라운드
	for (int i = 0; i < 10; ++i) {
		rectangles.push_back(RECTANGLE(dis_coord(gen), dis_coord(gen), dis_size(gen), dis_size(gen), dis(gen), dis(gen), dis(gen)));
	}
}

void addRectangles(float x, float y, float width, float height, float r, float g, float b) {
	rectangles.push_back(RECTANGLE(x, y, width, height, r, g, b)); // 중앙
}

void addMovingRectangles(float centerX, float centerY, float width, float height, float r, float g, float b) {
	float speed = 0.01f;
	float newWidth = width * 0.3f;
	float newHeight = height * 0.3f;
	
	// 8방향으로 작은 사각형들 생성 (상하좌우 + 대각선)
	rectangles.push_back(RECTANGLE(centerX, centerY + height * 0.3f, newWidth, newHeight, r, g, b, 0, speed));        // 위
	rectangles.push_back(RECTANGLE(centerX, centerY - height * 0.3f, newWidth, newHeight, r, g, b, 0, -speed));       // 아래
	rectangles.push_back(RECTANGLE(centerX - width * 0.3f, centerY, newWidth, newHeight, r, g, b, -speed, 0));        // 왼쪽
	rectangles.push_back(RECTANGLE(centerX + width * 0.3f, centerY, newWidth, newHeight, r, g, b, speed, 0));         // 오른쪽
	rectangles.push_back(RECTANGLE(centerX - width * 0.2f, centerY + height * 0.2f, newWidth, newHeight, r, g, b, -speed * 0.7f, speed * 0.7f)); // 왼쪽 위
	rectangles.push_back(RECTANGLE(centerX + width * 0.2f, centerY + height * 0.2f, newWidth, newHeight, r, g, b, speed * 0.7f, speed * 0.7f));  // 오른쪽 위
	rectangles.push_back(RECTANGLE(centerX - width * 0.2f, centerY - height * 0.2f, newWidth, newHeight, r, g, b, -speed * 0.7f, -speed * 0.7f)); // 왼쪽 아래
	rectangles.push_back(RECTANGLE(centerX + width * 0.2f, centerY - height * 0.2f, newWidth, newHeight, r, g, b, speed * 0.7f, -speed * 0.7f));  // 오른쪽 아래
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("practice 6"); // 윈도우 생성 (윈도우 이름)

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
	glutTimerFunc(16, TimerFunction, 1); // 타이머 함수 설정 (약 60FPS)
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
		for (int i = 1; i < rectangles.size(); ++i) {
			if (rectangles[i].is_contain(Mouse_x, Mouse_y) && !rectangles[i].isMoving) {
				// 클릭한 사각형을 8개의 작은 사각형으로 분할하고 퍼뜨리기
				addMovingRectangles(rectangles[i].x, rectangles[i].y, rectangles[i].width, rectangles[i].height, rectangles[i].r, rectangles[i].g, rectangles[i].b);
				rectangles.erase(rectangles.begin() + i);
				animationRunning = true;
				break;
			}
		}
	}

	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

GLvoid Motion(int x, int y) {

	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

GLvoid TimerFunction(int value) {
	if (animationRunning) {
		// 모든 사각형 업데이트
		for (auto& rect : rectangles) {
			rect.update();
		}
		
		// 너무 작아진 사각형들 제거
		rectangles.erase(
			std::remove_if(rectangles.begin() + 1, rectangles.end(), 
				[](const RECTANGLE& rect) { return rect.shouldRemove(); }),
			rectangles.end()
		);
		
		// 움직이는 사각형이 없으면 애니메이션 중지
		bool hasMovingRect = false;
		for (const auto& rect : rectangles) {
			if (rect.isMoving) {
				hasMovingRect = true;
				break;
			}
		}
		animationRunning = hasMovingRect;
	}
	
	glutPostRedisplay();
	glutTimerFunc(16, TimerFunction, 1); // 다음 프레임을 위한 타이머 설정
}