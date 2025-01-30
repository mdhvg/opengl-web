#include "Core.h"
#include "Debug.h"
#include "Shader.h"
#include "Utils.h"
#include "Renderer.h"

#include <emscripten/html5.h>

/*
NOTE:
To use NVIDIA GPU, run the following command:
export __GLX_VENDOR_LIBRARY_NAME=nvidia
before running the program.
*/

EM_JS(void, test, (), {
	console.log("Hello from C++!");
	});

EM_JS(int, canvasWidth, (), {
	return document.getElementById("canvas").width;
	});

EM_JS(int, canvasHeight, (), {
	return document.getElementById("canvas").height;
	});

char text[100] = { 0 };

struct UserData {
	GLFWwindow* window;
	std::vector<unsigned int> indices;
	unsigned int renderProgram;
	unsigned int texture;
	float startTime = 0;
	RenderBuffer renderBuffers;
};

static EM_BOOL loop(double time, void* data) {
	UserData* userData = (UserData*)data;

	glfwPollEvents();
	GLCall(glClearColor(0.1f, 0.4f, 0.5f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	/*
	Render Loop here
	*/
	glBindTexture(GL_TEXTURE_2D, userData->texture);
	GLCall(glUseProgram(userData->renderProgram));
	glUniform1f(glGetUniformLocation(userData->renderProgram, "time"), time);
	glUniform2f(glGetUniformLocation(userData->renderProgram, "resolution"), canvasWidth(), canvasHeight());
	GLCall(glBindVertexArray(userData->renderBuffers.vao));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->renderBuffers.ebo));
	GLCall(glDrawElements(GL_TRIANGLES, userData->indices.size(), GL_UNSIGNED_INT, nullptr));

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	{
		ImGui::Begin("FPS");
		float frameTime = time - userData->startTime;
		ImGui::Text("FPS: %.2f", 1000.0f / frameTime);
		ImGui::Text("Frametime: %.1fms", frameTime);
		ImGui::InputText("Text", text, 10);
		userData->startTime = time;
		ImGui::End();
	}

	ImGui::Render();

	glfwMakeContextCurrent(userData->window);
	glViewport(0, 0, canvasWidth(), canvasHeight());
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwMakeContextCurrent(userData->window);

	emscripten_webgl_commit_frame();
	return EM_TRUE;
}

static EM_BOOL EmscriptenMouseCallback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseButtonEvent(mouseEvent->button, true);
	return 1;
}

static EM_BOOL EmscriptenMouseMoveCallback(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
{
	ImGuiIO& io = ImGui::GetIO();
	glUniform2f(glGetUniformLocation(((UserData*)userData)->renderProgram, "mouse"), (float)mouseEvent->clientX, (float)(950 - mouseEvent->clientY));
	io.AddMousePosEvent(mouseEvent->clientX, mouseEvent->clientY);

	return 1; // The event was consumed by the callback handler
}

extern "C" int main(int argc, char** argv) {
	if (!glfwInit()) {
		return 0;
	}

	GLFWwindow* window = glfwCreateWindow(canvasWidth(), canvasHeight(), "canvas", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	std::vector<float> vertices = {
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f
	};

	std::vector<int> widths = { 3, 2 };

	std::vector<unsigned int> indices = {
		0, 1, 2,
		1, 2, 3
	};

	RenderBuffer renderBuffers = generateBuffers(vertices, indices, widths);

	unsigned int vertexShader = compileShader(argv[0], GL_VERTEX_SHADER);
	unsigned int fragmentShader = compileShader(argv[1], GL_FRAGMENT_SHADER);

	unsigned int renderProgram = createRenderProgram(vertexShader, fragmentShader);

	GLCall(glDeleteShader(vertexShader));
	GLCall(glDeleteShader(fragmentShader));

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// // Nearest filtering for pixelated look
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	io.BackendPlatformName = "imgui_impl_glfw";

	const char* glsl_version = "#version 300 es";

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	UserData* userData = new UserData{ window, indices, renderProgram, texture, 0, renderBuffers };

	/* Render Loop */
	emscripten_set_click_callback("#canvas", NULL, 1, EmscriptenMouseCallback);
	emscripten_set_mousemove_callback("#canvas", userData, 1, EmscriptenMouseMoveCallback);
	emscripten_request_animation_frame_loop(loop, userData);

	return 0;
}
