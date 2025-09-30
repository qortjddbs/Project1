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
// GLfloat bgColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // 초기값 : 흰색 
bool button_1 = false;
bool button_2 = false;
bool button_3 = false;
bool button_4 = false;
bool button_5 = false;
bool runningTimer = false;
int train_head = -1;

std::random_device rd;  // 시드값을 얻기 위한 random_device 생성.
std::mt19937 gen(rd());	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
std::uniform_real_distribution<float> dis(0.0f, 1.0f); // 0.0f 부터 1.0f 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
std::uniform_real_distribution<float> dis_random(-1.0f, 1.0f);
std::uniform_int_distribution<int> dis_number(1, 5);

class RECTANGLE {
public:
	float x, y; // 사각형의 중심 좌표
	float width, height; // 사각형의 너비와 높이
	float r, g, b; // 사각형의 색상 (RGB)
	bool growing = dis(gen) > 0.5f ? true : false;
	float dx = dis_random(gen) > 0.0f ? 0.01f : -0.01f;
	float dy = dis_random(gen) > 0.0f ? 0.01f : -0.01f;
	float first_x, first_y;

	RECTANGLE(float x, float y, float w, float h, float red, float green, float blue, float first_x, float first_y)
		: x(x), y(y), width(w), height(h), r(red), g(green), b(blue), first_x(first_x), first_y(first_y) {
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

float mapToGLCoordX(int x) {
	return (static_cast<float>(x) / (WINDOW_WIDTH / 2)) - 1.0f;
}

float mapToGLCoordY(int y) {
	return 1.0f - (static_cast<float>(y) / (WINDOW_HEIGHT / 2));
}

std::vector<RECTANGLE> rectangles;

void MoveToCorner() {
	for (int i = 1; i < rectangles.size(); ++i) {
		if (rectangles[i].x - rectangles[i].width <= -1.0f) {
			rectangles[i].dx = 0.01f;
		} else if (rectangles[i].x + rectangles[i].width >= 1.0f) {
			rectangles[i].dx = -0.01f;
		} 

		if (rectangles[i].y - rectangles[i].height <= -1.0f) {
			rectangles[i].dy = 0.01f;
		} else if (rectangles[i].y + rectangles[i].height >= 1.0f) {
			rectangles[i].dy = -0.01f;
		}
		rectangles[i].x += rectangles[i].dx;
		rectangles[i].y += rectangles[i].dy;
	}
}

void MoveToSide() {
	for (int i = 1; i < rectangles.size(); ++i) {
		if (rectangles[i].x - rectangles[i].width <= -1.0f) {
			rectangles[i].dx = 0.01f;
		}
		else if (rectangles[i].x + rectangles[i].width >= 1.0f) {
			rectangles[i].dx = -0.01f;
		}

		if (rectangles[i].y - rectangles[i].height <= -1.0f) {
			rectangles[i].dy = 0.01f;
		}
		else if (rectangles[i].y + rectangles[i].height >= 1.0f) {
			rectangles[i].dy = -0.01f;
		}
		rectangles[i].x += rectangles[i].dx * 5;
		rectangles[i].y += rectangles[i].dy / 8;
	}
}

void changeSize() {
	for (int i = 1; i < rectangles.size(); ++i) {
		if (rectangles[i].growing) {
			rectangles[i].width += 0.01f;
			rectangles[i].height += 0.01f;
			if (rectangles[i].width >= 0.2f) rectangles[i].growing = false;
		}
		else {
			rectangles[i].width -= 0.01f;
			rectangles[i].height -= 0.01f;
			if (rectangles[i].width <= 0.05f) rectangles[i].growing = true;
		}
	}
}

void changeColor() {
	for (int i = 1; i < rectangles.size(); ++i) {
		rectangles[i].randomColor();
	}
}

void MoveTrain() {
	if (rectangles.size() <= 1 || train_head < 1) return;

	RECTANGLE& head = rectangles[train_head];
	static std::vector<std::pair<float, float>> trail; // 머리의 궤적을 저장
	const float SPACING = 0.25f; // 사각형들 사이의 간격

	// 머리 사각형의 움직임 (1번, 2번 키 입력에 따라)
	if (button_1) {
		// 1번: 대각선으로 움직이는 패턴
		if (head.x - head.width <= -1.0f) {
			head.dx = 0.01f;
		}
		else if (head.x + head.width >= 1.0f) {
			head.dx = -0.01f;
		}
		if (head.y - head.height <= -1.0f) {
			head.dy = 0.01f;
		}
		else if (head.y + head.height >= 1.0f) {
			head.dy = -0.01f;
		}
		head.x += head.dx;
		head.y += head.dy;
	}
	else if (button_2) {
		// 2번: 가로로 빠르게, 세로로 천천히 움직이는 패턴
		if (head.x - head.width <= -1.0f) {
			head.dx = 0.01f;
		}
		else if (head.x + head.width >= 1.0f) {
			head.dx = -0.01f;
		}
		if (head.y - head.height <= -1.0f) {
			head.dy = 0.01f;
		}
		else if (head.y + head.height >= 1.0f) {
			head.dy = -0.01f;
		}
		head.x += head.dx * 5;
		head.y += head.dy / 8;
	}

	// 머리의 현재 위치를 궤적에 추가
	trail.push_back({head.x, head.y});
	
	// 궤적이 너무 길어지면 앞쪽을 제거
	if (trail.size() > 100) {
		trail.erase(trail.begin());
	}

	// 나머지 사각형들을 궤적을 따라 배치
	int follower_count = 0;
	for (int i = 1; i < rectangles.size(); ++i) {
		if (i != train_head) {
			follower_count++;
			// 간격에 따른 궤적 인덱스 계산
			int trail_index = trail.size() - 1 - (int)(follower_count * SPACING / 0.02f);
			
			if (trail_index >= 0 && trail_index < trail.size()) {
				rectangles[i].x = trail[trail_index].first;
				rectangles[i].y = trail[trail_index].second;
			}
		}
	}
}

void initBackground() {
	rectangles.push_back(RECTANGLE(0.0f, 0.0f, 1.0f, 1.0f, 0.2f, 0.2f, 0.2f, 0.0f, 0.0f)); // 배경 background 백그라운드
}

void addRectangles(float x, float y) {
	rectangles.push_back(RECTANGLE(x, y, 0.1f, 0.1f, dis(gen), dis(gen), dis(gen), x, y)); // 중앙
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("Practice 4"); // 윈도우 생성 (윈도우 이름)

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
	glutKeyboardFunc(Keyboard); //--- 키보드 입력 콜백함수 지정
	glutMainLoop(); // 이벤트 처리 시작
}

GLvoid TimerFunction(int value)
{
	if (button_1) MoveToCorner();
	if (button_2) MoveToSide();
	if (button_3) changeSize();
	if (button_4) changeColor();
	if (button_5 && train_head > 0) MoveTrain();
	glutPostRedisplay(); // 화면 재 출력
	glutTimerFunc(10, TimerFunction, 1); // 타이머 함수 설정
}

GLvoid drawScene() //--- 콜백 함수: 출력 콜백 함수 
{
	rectangles[0].draw();
	for (int i = 1; i < rectangles.size(); ++i) {
		rectangles[i].draw();
	}
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
	case '1':
		if (!button_1) {
			button_1 = true;
			if (!runningTimer) {
				glutTimerFunc(10, TimerFunction, 1); // 타이머 함수 설정
				runningTimer = true;
			}
		}
		else {
			button_1 = false;
		}
		break;

	case '2':
		if (!button_2) {
			button_2 = true;
			if (!runningTimer) {
				glutTimerFunc(10, TimerFunction, 1); // 타이머 함수 설정
				runningTimer = true;
			}
		}
		else button_2 = false;
		break;

	case '3':
		if (!button_3) {
			button_3 = true;
			for (int i = 1; i < rectangles.size(); ++i) {
				rectangles[i].growing = dis(gen) > 0.5f ? true : false;
			}
			if (!runningTimer) {
				glutTimerFunc(10, TimerFunction, 1); // 타이머 함수 설정
				runningTimer = true;
			}
		}
		else button_3 = false;
		break;

	case '4':
		if (!button_4) {
			button_4 = true;
			if (!runningTimer) {
				glutTimerFunc(10, TimerFunction, 1); // 타이머 함수 설정
				runningTimer = true;
			}
		}
		else button_4 = false;
		break;

	case '5':
		if (!button_5) {
			button_5 = true;
			if (rectangles.size() > 1) {
				// 1부터 rectangles.size()-1까지의 범위로 수정
				std::uniform_int_distribution<int> train_dis(1, rectangles.size() - 1);
				train_head = train_dis(gen);
			}
			if (!runningTimer) {
				glutTimerFunc(10, TimerFunction, 1); // 타이머 함수 설정
				runningTimer = true;
			}
		}
		else {
			button_5 = false;
			train_head = -1;
		}
		break;

	case 's':
		for (int i = 1; i < 6; ++i) {
			if (button_1 || button_2 || button_3 || button_4 || button_5) {
				button_1 = false;
				button_2 = false;
				button_3 = false;
				button_4 = false;
				button_5 = false;
			}
		}
		break;

	case 'r':
		rectangles.clear();
		initBackground();
		for (int i = 1; i < 6; ++i) {
			if (button_1 || button_2 || button_3 || button_4 || button_5) {
				button_1 = false;
				button_2 = false;
				button_3 = false;
				button_4 = false;
				button_5 = false;
			}
		}
		break;

	case 'm':
		for (int i = 1; i < rectangles.size(); ++i) {
			rectangles[i].x = rectangles[i].first_x;
			rectangles[i].y = rectangles[i].first_y;
		}
		break;

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
		if (rectangles.size() < 6) addRectangles(Mouse_x, Mouse_y);
	}

	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}