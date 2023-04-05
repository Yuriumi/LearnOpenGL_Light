#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "../ref/stb_image.h"

#include "../ref/imgui/imgui.h"
#include "../ref/imgui/imgui_impl_glfw.h"
#include "../ref/imgui/imgui_impl_opengl3.h"

#include "../ref/Camera.h"
#include "../ref/Shader.h"

// Function Declaration
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
ImGuiIO& init_imgui(GLFWwindow* window);
void update_gui_frame();
void draw_imgui_window(const char* title, ImGuiIO& io);
void render_gui();
void destory_gui_data();
void load_texture(const char* texturePath);

float screen_Width = 800.0f;
float screen_Height = 600.0f;

Camera mainCamera(glm::vec3(0.0f, 0.0f, 3.0f));
bool cameraMove = false;
bool firstMouse = true;
float lastX = screen_Width / 2.0f;
float lastY = screen_Height / 2.0f;

float currentTime = 0.0f;
float deltaTime = 0.0f;
float lastTime = 0.0f;

// Light 
glm::vec3 dirlightDirection(0.0f, 1.0f, 0.0f);
glm::vec3 pointlightPosition(0.0f,1.0f,2.0f);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(screen_Width, screen_Height, "OpenGL_Light", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return -1;
	}
	glfwMakeContextCurrent(window);

	// Window Event Callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to init GLAD" << std::endl;

		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader basicNormal("./shader/vNormal.shader", "./shader/fNormal.shader");
	Shader basicLight("./shader/vLight.shader", "./shader/fLight.shader");

	// Texture 1
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	load_texture("./Texture/container2.png");

	// Texture 2
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	load_texture("./Texture/container2_specular.png");

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	unsigned VAO, VBO;

	// Normal Cube
	glGenVertexArrays(1, &VAO);

	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Light Cube
	unsigned int lightVAO;

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	glm::mat4 model;
	glm::mat4 view(1.0f);
	glm::mat4 projection(1.0f);

	ImGuiIO& io = init_imgui(window);

	while (!glfwWindowShouldClose(window))
	{
		currentTime = (float)glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		process_input(window);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update_gui_frame();

		// Renderer Order
		basicNormal.use();

		view = mainCamera.GetViewMatrix();
		projection = glm::perspective(glm::radians(mainCamera.fov), screen_Width / screen_Height, 0.1f, 100.0f);

		glUniformMatrix4fv(glGetUniformLocation(basicNormal.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(basicNormal.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Apply Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glUniform1i(glGetUniformLocation(basicNormal.ID, "material.diffuse"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glUniform1i(glGetUniformLocation(basicNormal.ID, "material.specular"), 1);
		glUniform1f(glGetUniformLocation(basicNormal.ID, "material.shininess"), 32.0f);

		glUniform3fv(glGetUniformLocation(basicNormal.ID, "viewPosition"), 1, glm::value_ptr(mainCamera.position));

		glUniform3fv(glGetUniformLocation(basicNormal.ID, "dirLight.ambient"), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
		glUniform3fv(glGetUniformLocation(basicNormal.ID, "dirLight.diffuse"), 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
		glUniform3fv(glGetUniformLocation(basicNormal.ID, "dirLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		
		glUniform3fv(glGetUniformLocation(basicNormal.ID, "pointLight.ambient"), 1, glm::value_ptr(glm::vec3(0.2f, 0.2f, 0.2f)));
		glUniform3fv(glGetUniformLocation(basicNormal.ID, "pointLight.diffuse"), 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
		glUniform3fv(glGetUniformLocation(basicNormal.ID, "pointLight.specular"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
		glUniform1f(glGetUniformLocation(basicNormal.ID, "pointLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(basicNormal.ID, "pointLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(basicNormal.ID, "pointLight.quadratic"), 0.032f);

		glUniform3fv(glGetUniformLocation(basicNormal.ID, "pointLight.position"), 1, glm::value_ptr(pointlightPosition));
		glUniform3fv(glGetUniformLocation(basicNormal.ID, "dirLight.direction"), 1, glm::value_ptr(dirlightDirection));

		glBindVertexArray(VAO);

		for (unsigned int i = 0; i < 10; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

			// ¾ØÕóÓ¦ÓÃ
			glUniformMatrix4fv(glGetUniformLocation(basicNormal.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// Light Cube Matrix Transform
		basicLight.use();

		model = glm::mat4(1.0f);
		model = glm::translate(model, pointlightPosition);
		model = glm::scale(model, glm::vec3(0.2f));

		glUniformMatrix4fv(glGetUniformLocation(basicLight.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(basicLight.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(basicLight.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		draw_imgui_window("OpenGL_Light_Settings", io);
		render_gui();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	destory_gui_data();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f * deltaTime;
	if (cameraMove)
	{
		// Mouse Setting
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			mainCamera.processKeyBoard(Camera_Movement::FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			mainCamera.processKeyBoard(Camera_Movement::BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			mainCamera.processKeyBoard(Camera_Movement::LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			mainCamera.processKeyBoard(Camera_Movement::RIGHT, deltaTime);
	}
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	if (cameraMove)
		mainCamera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	mainCamera.processMouseScroll(yoffset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (action == GLFW_PRESS)
			cameraMove = true;
		else if (action == GLFW_RELEASE)
			cameraMove = false;
	}
}

ImGuiIO& init_imgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");

	return io;
}

void update_gui_frame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void draw_imgui_window(const char* title, ImGuiIO& io)
{
	ImGui::Begin(title);

	// Draw window

	ImGui::End();
}

void render_gui()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void destory_gui_data()
{
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}

void load_texture(const char* texturePath)
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture1" << std::endl;
	}

	stbi_image_free(data);
}