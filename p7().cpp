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
int findIndex = -1;
float initial_mouse_x, initial_mouse_y;
float initial_rect_x, initial_rect_y;

std::random_device rd;  // 시드값을 얻기 위한 random_device 생성.
std::mt19937 gen(rd());	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
std::uniform_real_distribution<float> dis(0.0f, 1.0f); // 0.0f 부터 1.0f 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
std::uniform_real_distribution<float> dis_coord(-1.0f, 1.0f);
std::uniform_real_distribution<float> dis_center(-0.1f, 0.1f);
std::uniform_real_distribution<float> dis_size(0.05f, 0.1f);
std::uniform_real_distribution<float> dis_right_x(0.0f, 1.0f); // 오른쪽 영역용 x 좌표 분포
std::uniform_real_distribution<float> dis_y_pos(-1.0f, 1.0f); // y 좌표 분포

class RECTANGLE {
public:
	float x, y; // 사각형의 중심 좌표
	float width, height; // 사각형의 너비와 높이
	float r, g, b; // 사각형의 색상 (RGB)
	bool fillMode;
	bool isCombined; // 결합된 사각형인지 나타내는 플래그

	RECTANGLE(float x, float y, float w, float h, float red, float green, float blue, bool fill = true)
		: x(x), y(y), width(w), height(h), r(red), g(green), b(blue), fillMode(fill), isCombined(false) {
	}

	void draw() {
		glColor3f(r, g, b);

		if (fillMode) {
			glRectf(x - width, y + height, x + width, y - height);
		}
		else {
			glBegin(GL_LINE_LOOP);
			glVertex2f(x - width, y + height);
			glVertex2f(x + width, y + height);
			glVertex2f(x + width, y - height);
			glVertex2f(x - width, y - height);
			glEnd();
		}
		
	}

	bool is_contain(float mx, float my) const {
		return mx >= x - width && mx <= x + width && my >= y - height && my <= y + height;
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
	// 검은 배경 생성
	rectangles.push_back(RECTANGLE(0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f)); // 검은 배경

	// 이미지와 같은 복잡한 기하학적 패턴 생성
	// 모든 사각형들은 빈 사각형(테두리만)으로 생성
	
	// === 상단 작은 사각형들 (빈 사각형) ===
	rectangles.push_back(RECTANGLE(-0.7f, 0.75f, 0.1f, 0.1f, 0.7f, 0.3f, 1.0f, false));    // 연한 파랑
	rectangles.push_back(RECTANGLE(dis_right_x(gen) + 0.1f, dis_y_pos(gen), 0.1f, 0.1f, 0.7f, 0.3f, 1.0f, true));    // 연한 파랑 - 채워진

	rectangles.push_back(RECTANGLE(-0.5f, 0.8f, 0.1f, 0.15f, 0.0f, 1.0f, 0.0f, false)); 
	rectangles.push_back(RECTANGLE(dis_right_x(gen) + 0.1f, dis_y_pos(gen), 0.1f, 0.15f, 0.0f, 1.0f, 0.0f, true));   // 초록 - 채워진

	rectangles.push_back(RECTANGLE(-0.3f, 0.75f, 0.1f, 0.1f, 0.1f, 0.5f, 0.0f, false)); 
	rectangles.push_back(RECTANGLE(dis_right_x(gen) + 0.1f, dis_y_pos(gen), 0.1f, 0.1f, 0.1f, 0.5f, 0.0f, true));    // 주황 - 채워진

	rectangles.push_back(RECTANGLE(-0.1f, 0.8f, 0.1f, 0.15f, 0.4f, 0.1f, 1.0f, false));
	rectangles.push_back(RECTANGLE(dis_right_x(gen) + 0.1f, dis_y_pos(gen), 0.1f, 0.15f, 0.4f, 0.1f, 1.0f, true));   // 파랑 - 채워진

	// === 중앙의 큰 가로 사각형 (빈 사각형) ===
	rectangles.push_back(RECTANGLE(-0.4f, 0.6f, 0.35f, 0.05f, 0.7f, 0.7f, 1.0f, false));  //긴 가로 사각형
	rectangles.push_back(RECTANGLE(dis_right_x(gen) + 0.35f, dis_y_pos(gen), 0.35f, 0.05f, 0.7f, 0.7f, 1.0f, true)); // 연한 파랑 - 긴 가로 채워진

	// === 중앙 세로 사각형들 (빈 사각형) ===
	rectangles.push_back(RECTANGLE(-0.6f, 0.2f, 0.03f, 0.35f, 1.0f, 0.5f, 1.0f, false));  //좁은 세로
	rectangles.push_back(RECTANGLE(dis_right_x(gen) + 0.03f, dis_y_pos(gen), 0.03f, 0.35f, 1.0f, 0.5f, 1.0f, true)); // 파랑 - 좁은 세로 채워진

	rectangles.push_back(RECTANGLE(-0.5f, 0.2f, 0.03f, 0.35f, 0.2f, 0.5f, 0.2f, false));  // 좁은 세로
	rectangles.push_back(RECTANGLE(dis_right_x(gen) + 0.03f, dis_y_pos(gen), 0.03f, 0.35f, 0.2f, 0.5f, 0.2f, true)); // 파랑 - 좁은 세로 채워진

	rectangles.push_back(RECTANGLE(-0.4f, 0.2f, 0.03f, 0.35f, 0.7f, 0.5f, 0.8f, false));  // 좁은 세로
	rectangles.push_back(RECTANGLE(dis_right_x(gen) + 0.03f, dis_y_pos(gen), 0.03f, 0.35f, 0.7f, 0.5f, 0.8f, true)); // 파랑 - 좁은 세로 채워진

	rectangles.push_back(RECTANGLE(-0.3f, 0.2f, 0.03f, 0.35f, 0.5f, 0.5f, 0.8f, false));  // 좁은 세로
	rectangles.push_back(RECTANGLE(dis_right_x(gen) + 0.03f, dis_y_pos(gen), 0.03f, 0.35f, 0.5f, 0.5f, 0.8f, true)); // 파랑 - 좁은 세로 채워진

	// === 왼쪽 큰 사각형 (빈 사각형) ===
	rectangles.push_back(RECTANGLE(-0.78f, 0.0f, 0.15f, 0.25f, 1.0f, 0.5f, 0.0f, false));  // 주황 - 큰 사각형
	rectangles.push_back(RECTANGLE(dis_right_x(gen) + 0.15f, dis_y_pos(gen), 0.15f, 0.25f, 1.0f, 0.5f, 0.0f, true)); // 주황 - 큰 사각형 채워진

	// === 추가적인 장식용 작은 사각형들 (빈 사각형) ===
	rectangles.push_back(RECTANGLE(-0.1f, 0.47f, 0.06f, 0.08f, 1.0f, 1.0f, 0.0f, false));  // 노랑 - 작은 장식
	rectangles.push_back(RECTANGLE(dis_right_x(gen) + 0.06f, dis_y_pos(gen), 0.06f, 0.08f, 1.0f, 1.0f, 0.0f, true)); // 노랑 - 작은 장식 채워진

}

void addRectangles(float x, float y, float width, float height, float r, float g, float b) {
	RECTANGLE combinedRect(x, y, width, height, r, g, b);
	combinedRect.isCombined = true; // 결합된 사각형으로 표시
	rectangles.push_back(combinedRect);
	std::cout << "결합 ! " << std::endl;
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정 
{ //--- 윈도우 생성하기
	glutInit(&argc, argv); // glut 초기화
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 디스플레이 모드 설정
	glutInitWindowPosition(0, 0); // 윈도우의 위치 지정
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // 윈도우의 크기 지정
	glutCreateWindow("Practice 7"); // 윈도우 생성 (윈도우 이름)

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
	case 'r':
		rectangles.clear();
		initBackground();
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
		left_button = true;
		findIndex = -1; // 초기화
		for (int i = rectangles.size() - 1; i > 0; --i) {
			if (rectangles[i].is_contain(mapToGLCoordX(x), mapToGLCoordY(y)) && rectangles[i].fillMode && !rectangles[i].isCombined) {
				findIndex = i;
				// 초기 위치들 저장
				initial_mouse_x = Mouse_x;
				initial_mouse_y = Mouse_y;
				initial_rect_x = rectangles[i].x;
				initial_rect_y = rectangles[i].y;
				break;
			}
		}
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		left_button = false;
		if (findIndex > 0) {
			if (is_combine(rectangles[findIndex], rectangles[findIndex - 1])) {
				addRectangles(rectangles[findIndex - 1].x, rectangles[findIndex - 1].y, rectangles[findIndex - 1].width, rectangles[findIndex - 1].height, rectangles[findIndex - 1].r, rectangles[findIndex - 1].g, rectangles[findIndex - 1].b);
				rectangles.erase(rectangles.begin() + findIndex);
				rectangles.erase(rectangles.begin() + findIndex - 1);
			}
		}
		findIndex = -1;
	}

	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}

GLvoid Motion(int x, int y) {
	if (left_button == true && findIndex != -1) {
		// 마우스 이동 거리 계산
		float mouse_x = mapToGLCoordX(x);
		float mouse_y = mapToGLCoordY(y);
		float delta_x = mouse_x - initial_mouse_x;
		float delta_y = mouse_y - initial_mouse_y;
		
		// 초기 사각형 위치에서 마우스 이동 거리만큼 이동
		rectangles[findIndex].x = initial_rect_x + delta_x;
		rectangles[findIndex].y = initial_rect_y + delta_y;
	}

	glutPostRedisplay(); //--- 배경색이 바뀔 때마다 출력 콜백 함수를 호출하여 화면을 refresh 한다
}
