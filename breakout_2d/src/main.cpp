#include "pch.hpp"

#include "fast_random.hpp"
#include "game.hpp"
#include "path_manager.hpp"
#include "render_config.hpp"
#include "shader_observer.hpp"
#include "window.hpp"

#include <opengl_4_5/glad/glad.h> // 1
#include <GLFW/glfw3.h> // 2

#define FIXED_FRAMETIME (1.0 / 100.0) // 100 Hz fixed update loop
#define MAX_FRAMETIME 0.25

bool DEBUG_WIREFRAME = false;

Game game(Window::getWidth(), Window::getHeight(), 3);

void key_callback(GLFWwindow *window, int key, int scanCode, int action, int mods) {
	if (action == GLFW_PRESS) {
		game.Keys[key] = true;
	}
	else if (action == GLFW_RELEASE) {
		game.Keys[key] = false;
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		DEBUG_WIREFRAME = !DEBUG_WIREFRAME;
		if (DEBUG_WIREFRAME) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		game.spawnBall();
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

int main() {
	glfwInit();
	glfwWindowHint(GLFW_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_VERSION_MINOR, 5);
	GLFWwindow *window = glfwCreateWindow(Window::getWidth(), Window::getHeight(), "Breakout2D", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		return -1;
	}
	glViewport(0, 0, Window::getWidth(), Window::getHeight());
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glfwSwapInterval(1); // v-sync 

	RenderConfig::setupDefaultAlphaBlending();

	_fr::initRandomEngine();
	PathManager::init();
	ShaderObserver::Get().startObserving();
	game.init();

	double frameTime = 0.0;
	double lastTime = 0.0;
	double accumulation = 0.0;
	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		frameTime = currentTime - lastTime;
		lastTime = currentTime;
		if (frameTime > MAX_FRAMETIME)
			frameTime = MAX_FRAMETIME;
		accumulation += frameTime;
		glfwPollEvents();
		game.processInput(static_cast<float>(frameTime));
		while (accumulation >= FIXED_FRAMETIME) {
			game.fixedUpdate(static_cast<float>(FIXED_FRAMETIME));
			accumulation -= FIXED_FRAMETIME;
		}
		game.update(static_cast<float>(frameTime));

		glClear(GL_COLOR_BUFFER_BIT);
		float alpha = static_cast<float>(accumulation / FIXED_FRAMETIME);
		game.render(alpha);
		glfwSwapBuffers(window);
		ShaderObserver::Get().update();
	}
	ShaderObserver::Get().stopObserving();
	glfwTerminate();
	return 0;
}