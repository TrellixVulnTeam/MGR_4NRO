#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "mesh.hpp"
#include "utils.hpp"


constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 800;

int main() {
	
	// WINDOW SETUP
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "Motion Blur", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, WindowWidth, WindowHeight);
	glClearColor(0, 0, 0, 1);

	double time = glfwGetTime();

	// OBJECT CREATION
	
	Mesh map = loadMesh("assets/float.obj");
	Mesh quad = makeQuad();
	Mesh sky = makeCube();
	GLuint firstProgram = loadShaders("assets/first.vert", "assets/first.frag");
	GLuint skyProgram = loadShaders("assets/sky.vert", "assets/sky.frag");
	GLuint quadProgram = loadShaders("assets/quad.vert", "assets/quad.frag");
	// TODO 2.1: Utworzenie oraz wczytanie programu odpowiedzialnego za rozmycie obrazu
	GLuint blurProgram = loadShaders("assets/blur.vert", "assets/blur.frag");
	GLuint texture = loadTexture("assets/texture.jpg");
	GLuint skyTexture = loadTexture("assets/canyon.jpg");
	
	glm::vec3 cameraPos = {0, 0, -5};
	glm::vec2 cameraRotationDegrees = {0,0};
	double cursorX, cursorY;
	glfwGetCursorPos(window, &cursorX, &cursorY);
	int spaceState = GLFW_RELEASE;
	bool running = true;
	
	glm::mat4 modelMatrix = glm::mat4(1);
	glm::mat4 oldModelMatrix = glm::mat4(1);
	glm::mat4 viewMatrix = glm::mat4(1);
	glm::mat4 oldViewMatrix = glm::mat4(1);
	glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(45.f), WindowWidth / (float) WindowHeight, 0.1f, 40.f);
	float modelAngle = 0;
	
	// FRAMEBUFFER CREATION
	
	GLuint gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	GLuint gPos, gNorm, gAlb, gEmit, gDepth, gVel;

	glGenTextures(1, &gPos);
	glBindTexture(GL_TEXTURE_2D, gPos);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPos, 0);

	glGenTextures(1, &gNorm);
	glBindTexture(GL_TEXTURE_2D, gNorm);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNorm, 0);

	glGenTextures(1, &gAlb);
	glBindTexture(GL_TEXTURE_2D, gAlb);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlb, 0);

	glGenTextures(1, &gEmit);
	glBindTexture(GL_TEXTURE_2D, gEmit);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gEmit, 0);

	// TODO 3: Utworzenie oraz podpiecie tekstury z wektorami predkosci
	glGenTextures(1, &gVel);
	glBindTexture(GL_TEXTURE_2D, gVel);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, WindowWidth, WindowHeight, 0, GL_RG, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gVel, 0);

	glGenRenderbuffers(1, &gDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, gDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WindowWidth, WindowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, gDepth);

	// TODO 4: Dodanie tekstury predkosci do listy attachmentow
	unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
	glDrawBuffers(5, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer incomplete" << '\n';
	}

	// TODO 1: Utworzenie framebuffera na rozmywany obraz
	GLuint gBlurBuffer;
	glGenFramebuffers(1, &gBlurBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBlurBuffer);

	GLuint gCol;
	glGenTextures(1, &gCol);
	glBindTexture(GL_TEXTURE_2D, gCol);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WindowWidth, WindowHeight, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gCol, 0);
	glDrawBuffers(1, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer incomplete" << '\n';
	}
	// MAIN LOOP

	while(!glfwWindowShouldClose(window))
	{

		glfwPollEvents();
		double newTime = glfwGetTime();
		float dt = static_cast<float>(newTime - time);
		time = newTime;

		int newSpaceState = glfwGetKey(window, GLFW_KEY_SPACE);
		if (newSpaceState == GLFW_PRESS && spaceState == GLFW_RELEASE) running = !running;
		spaceState = newSpaceState;

		
		// OBJECT POSITION UPDATE
		
		if (running) {
			// TODO 5: Zapisywanie poprzedniej macierzy modelu
			oldModelMatrix = modelMatrix;
			static float speed = 0;
			speed += dt / 2;
			float step = dt * glm::sin(speed);
			modelAngle += step * 300;
			while (modelAngle > 360) modelAngle -= 360;
			modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, {0, glm::sin(glm::radians(modelAngle)), 0 });
			modelMatrix = glm::rotate(modelMatrix, glm::radians(modelAngle), { 0, 1, 0 });
		}

		// CAMERA UPDATE

		double newCursorX, newCursorY;
		glfwGetCursorPos(window, &newCursorX, &newCursorY);
		double cursorDX = newCursorX - cursorX;
		double cursorDY = newCursorY - cursorY;
		cursorX = newCursorX;
		cursorY = newCursorY;

		bool rotate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
		bool goForward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
		bool goBack = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
		bool goLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
		bool goRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
		bool goUp = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
		bool goDown = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
		bool moveCamera = rotate || goForward || goBack || goLeft || goRight || goUp || goDown;

		if (rotate) {
			cameraRotationDegrees.x -= static_cast<float>(cursorDY) / 5;
			cameraRotationDegrees.y -= static_cast<float>(cursorDX) / 5;
			if (cameraRotationDegrees.x > 90) cameraRotationDegrees.x = 90;
			if (cameraRotationDegrees.x < -90) cameraRotationDegrees.x = -90;
			while (cameraRotationDegrees.y > 360) cameraRotationDegrees.y -= 360;
			while (cameraRotationDegrees.y < 0) cameraRotationDegrees.y += 360;
		}

		if (running || moveCamera) {
		    // TODO 6: Zapisywanie poprzedniej macierzy widoku
			oldViewMatrix = viewMatrix;
		}

		viewMatrix = glm::mat4(1);
		viewMatrix = glm::rotate(viewMatrix, glm::radians(cameraRotationDegrees.x), { 1, 0, 0 });
		viewMatrix = glm::rotate(viewMatrix, glm::radians(cameraRotationDegrees.y), { 0, 1, 0 });

		glm::vec4 cameraFront = { 0, 0, 1, 0 };
		glm::vec4 cameraUp = { 0, 1, 0, 0 };
		glm::vec4 cameraSide = { 1, 0, 0, 0 };

		cameraFront = cameraFront * viewMatrix;
		//cameraUp = cameraUp * viewMatrix;
		cameraSide = cameraSide * viewMatrix;

		const float movementSpeed = 5.0f * dt;
		if (goForward) cameraPos += glm::vec3(cameraFront) * movementSpeed;
		if (goBack) cameraPos -= glm::vec3(cameraFront) * movementSpeed;
		if (goLeft) cameraPos += glm::vec3(cameraSide) * movementSpeed;
		if (goRight) cameraPos -= glm::vec3(cameraSide) * movementSpeed;
		if (goUp) cameraPos -= glm::vec3(cameraUp) * movementSpeed;
		if (goDown) cameraPos += glm::vec3(cameraUp) * movementSpeed;

		viewMatrix = glm::translate(viewMatrix, cameraPos);

		// RENDERING STAGE 1
		
		// STAGE 1 - object
		
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(firstProgram);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glUniformMatrix4fv(10, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		glUniformMatrix4fv(11, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(12, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
		glUniformMatrix4fv(13, 1, GL_FALSE, glm::value_ptr(oldModelMatrix));
		glUniformMatrix4fv(14, 1, GL_FALSE, glm::value_ptr(oldViewMatrix));
		// TODO 7: Przeslanie poprzednich macierzy oraz modyfikacja skaderow obiektu
		glUniform1i(20, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(map.vao);
		glDrawArrays(GL_TRIANGLES, 0, map.count);

		// STAGE 1 - skybox
		
		glUseProgram(skyProgram);
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_CULL_FACE);
		glUniformMatrix4fv(11, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(12, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
		glUniformMatrix4fv(13, 1, GL_FALSE, glm::value_ptr(oldViewMatrix));
		// TODO 8: Przeslanie poprzednich macierzy oraz modyfikacja shadera skyboxa
		glUniform1i(20, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, skyTexture);
		glBindVertexArray(sky.vao);
		glDrawArrays(GL_TRIANGLES, 0, sky.count);

		// STAGE 2

		// TODO 2.3: Podmiana docelowego bufora
		glBindFramebuffer(GL_FRAMEBUFFER, gBlurBuffer);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(quadProgram);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPos);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNorm);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlb);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gEmit);
		glUniform1i(20, 0);
		glUniform1i(21, 1);
		glUniform1i(22, 2);
		glUniform1i(23, 3);
		glBindVertexArray(quad.vao);
		glDrawArrays(GL_TRIANGLES, 0, quad.count);

		// TODO 2.2: Wyswietlenie obrazu z uzyciem programu rozmycia
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(blurProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gCol);

		// TODO 9: Podpiecie tekstury wektorow predkosci i modyfikacja shadera rozmycia
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gVel);

		glBindVertexArray(quad.vao);
		glDrawArrays(GL_TRIANGLES, 0, quad.count);
		

		// SWAP
		
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
