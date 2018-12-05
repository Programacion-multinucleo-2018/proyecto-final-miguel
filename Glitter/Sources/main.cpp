
#define STB_IMAGE_IMPLEMENTATION

#include <filesystem.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera2.h>
#include <model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);

// medidas
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camara
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	// Inicializar GLFW
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	// Usar OpenGL 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // para compilar en OSX

	// Crear ventana
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// obtener info de mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// inicializar glad
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// calcula la profundidad de lo que dibuja
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// shaders
	// -------------------------
	Shader skyboxShader("6.2.skybox.vs", "6.2.skybox.fs");
	Shader shader("1.model_loading.vs", "1.model_loading.fs");

	// load models
	// -----------
	Model ourModel(FileSystem::getPath("resources/objects/planet/planet.obj"));
	Model ourModel0(FileSystem::getPath("resources/objects/nanosuit/nanosuit.obj"));
	Model ourModel2(FileSystem::getPath("resources/objects/lowpolytree/Tree.obj"));
	Model ourModel3(FileSystem::getPath("resources/objects/lowpolytree/Tree.obj"));
	Model ourModel4(FileSystem::getPath("resources/objects/lowpolytree/Tree.obj"));
	Model ourModel5(FileSystem::getPath("resources/objects/rock/rock.obj"));
	Model ourModel6(FileSystem::getPath("resources/objects/rock/rock.obj"));
	Model ourModel7(FileSystem::getPath("resources/objects/rock/rock.obj"));
	Model ourModel8(FileSystem::getPath("resources/objects/rock/rock.obj"));

	// vertices de objetos
	// ------------------------------------------------------------------
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	// vertex array object de skybox

	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// cargar texturas de skybox
	vector<std::string> faces
	{
		FileSystem::getPath("resources/textures/skybox/Plants/posx.jpg"),
		FileSystem::getPath("resources/textures/skybox/Plants/negx.jpg"),
		FileSystem::getPath("resources/textures/skybox/Plants/posy.jpg"),
		FileSystem::getPath("resources/textures/skybox/Plants/negy.jpg"),
		FileSystem::getPath("resources/textures/skybox/Plants/posz.jpg"),
		FileSystem::getPath("resources/textures/skybox/Plants/negz.jpg"),
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	//configuración de shader

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// render loop
	while (!glfwWindowShouldClose(window))
	{

		//calcular tiempo transcurrido en cada frame
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		// limpiar pantalla con gris claro
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// datos de la camara
		shader.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		// nanosuit
		//glm::mat4 model0 = glm::mat4(1.0f);
		//model0 = glm::translate(model0, glm::vec3(0.0f, -1.75f, 0.0f)); 
		//model0 = glm::scale(model0, glm::vec3(100.2f, 0.02f, 100.2f));	
		//shader.setMat4("model", model0);
		//ourModel0.Draw(shader);

		// ground
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(100.2f, 0.02f, 100.2f));
		shader.setMat4("model", model);
		ourModel.Draw(shader);

		//tree
		glm::mat4 model2 = glm::mat4(1.0f);
		model2 = glm::translate(model2, glm::vec3(20.0f, -1.75f, 8.0f));
		model2 = glm::scale(model2, glm::vec3(2.0f, 2.0f, 2.0f));
		model2 = glm::rotate(model2, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		shader.setMat4("model", model2);
		ourModel2.Draw(shader);

		//tree
		glm::mat4 model3 = glm::mat4(1.0f);
		model3 = glm::translate(model3, glm::vec3(2.0f, -1.75f, 12.0f));
		model3 = glm::scale(model3, glm::vec3(5.0f, 5.0f, 5.0f));
		model3 = glm::rotate(model3, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		shader.setMat4("model", model3);
		ourModel3.Draw(shader);

		//tree
		glm::mat4 model4 = glm::mat4(1.0f);
		model4 = glm::translate(model4, glm::vec3(-6.0f, -1.75f, -8.0f));
		model4 = glm::scale(model4, glm::vec3(2.5f, 3.0f, 2.5f));
		model4 = glm::rotate(model4, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
		shader.setMat4("model", model4);
		ourModel4.Draw(shader);

		//rock
		glm::mat4 model5 = glm::mat4(1.0f);
		model5 = glm::translate(model5, glm::vec3(3.0f, -1.75f, -3.0f));
		model5 = glm::scale(model5, glm::vec3(1.0f, 1.0f, 1.0f));
		model4 = glm::rotate(model4, glm::radians(-40.0f), glm::vec3(0.2, 1.0, 0.3));
		shader.setMat4("model", model5);
		ourModel5.Draw(shader);

		//rock
		glm::mat4 model6 = glm::mat4(1.0f);
		model6 = glm::translate(model6, glm::vec3(10.0f, -1.75f, -2.0f));
		model6 = glm::scale(model6, glm::vec3(0.4f, 0.3f, 0.3f));
		model4 = glm::rotate(model4, glm::radians(10.0f), glm::vec3(0.0, 1.0, 0.2));
		shader.setMat4("model", model6);
		ourModel6.Draw(shader);

		//rock
		glm::mat4 model7 = glm::mat4(1.0f);
		model7 = glm::translate(model7, glm::vec3(-4.0f, -2.75f, 20.0f));
		model7 = glm::scale(model7, glm::vec3(1.0f, 2.2f, 0.5f));
		model4 = glm::rotate(model4, glm::radians(50.0f), glm::vec3(1.0, 1.0, 0.0));
		shader.setMat4("model", model7);
		ourModel7.Draw(shader);

		//rock
		glm::mat4 model8 = glm::mat4(1.0f);
		model8 = glm::translate(model8, glm::vec3(14.0f, -1.75f, -10.0f));
		model8 = glm::scale(model8, glm::vec3(0.5f, 0.5f, 0.5f));
		model4 = glm::rotate(model4, glm::radians(180.0f), glm::vec3(1.0, 0.0, 0.0));
		shader.setMat4("model", model8);
		ourModel8.Draw(shader);



		glDepthFunc(GL_LEQUAL);  // solo dibuja lo que sí debería ir más cerca de la cámara
		glEnable(GL_CULL_FACE); //culling, no dibuja la parte de "atrás" de los polígonos
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//desalocar info
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVAO);

	glfwTerminate();
	return 0;
}

// se llama cada iteración, maneja input
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(JUMP_UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
		camera.ProcessKeyboard(JUMP_DOWN, deltaTime);
}

// se llama cada que cambia el tamaño de la ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// maneja input del mouse, se llama automáticamente al mover el mouse
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// maneja input del mousewheel, se llama automáticamente al moverlo
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

//carga textura de un archivo
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// carga un skybox a partir de 6 imágenes en el orden +x (derecha), -x (izquierda), +y (arriba), -y (abajo), +z (frente), -z (atrás)
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrComponents;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}