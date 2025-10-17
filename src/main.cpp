#include <SDL2/SDL.h>
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <vector>

int gScreenWidth = 640;
int gScreenHeight = 480;
SDL_Window *gGraphicsApplicationWindow = nullptr;
SDL_GLContext gOpenGLContext = nullptr;

GLuint gVertexArrayObject = 0;
GLuint gVertexBufferObject = 0;
GLuint gGraphicsPipelineShaderProgram = 0;

bool gQuit = false;

std::string LoadShaderAsString(const std::string &filename) {
  std::string result = "";
  std::string line = "";
  std::ifstream myFile(filename.c_str());

  if (myFile.is_open()) {
    while (std::getline(myFile, line)) {
      result += line + '\n';
    }
    myFile.close();
  };
  return result;
}

const std::string gVertexShaderSource =
    "#version 410 core\n"
    "in vec4 position;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(position.x, position.y, position.z, position.w);\n"
    "}\n";

const std::string gFragmentShaderSource =
    "#version 410 core\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "    color = vec4(0.772549f, 0.188235f, 0.188235f, 1.f);\n"
    "}\n";

GLuint CompileShader(GLuint type, const std::string &source) {
  GLuint shaderObject;
  if (type == GL_VERTEX_SHADER) {
    shaderObject = glCreateShader(GL_VERTEX_SHADER);
  } else if (type == GL_FRAGMENT_SHADER) {
    shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
  }

  const char *src = source.c_str();
  glShaderSource(shaderObject, 1, &src, nullptr);
  glCompileShader(shaderObject);

  return shaderObject;
}

GLuint CreateShaderProgram(const std::string &vertexShaderSource,
                           const std::string &fragmentShaderSource) {
  GLuint programObject = glCreateProgram();

  GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
  GLuint myFragmentShader =
      CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

  glAttachShader(programObject, myVertexShader);
  glAttachShader(programObject, myFragmentShader);
  glLinkProgram(programObject);

  glValidateProgram(programObject);

  return programObject;
}

void CreateGraphicsPipeline() {
  std::string vertexShaderSource = LoadShaderAsString("./shaders/vert.glsl");
  std::string fragmentShaderSource = LoadShaderAsString("./shaders/frag.glsl");

  gGraphicsPipelineShaderProgram =
      CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void GetOpenGLVersionInfo() {
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Shading Language Version: "
            << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void VertexSpecification() {
  const std::vector<GLfloat> vertexData{-0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f,
                                        0.5f,  -0.5f, 0.0f, 0.6f, 0.0f,  0.8f,
                                        -0.5f, 0.5f,  0.0f, 1.0f, 0.75f, 0.8f};

  glGenVertexArrays(1, &gVertexArrayObject);
  glBindVertexArray(gVertexArrayObject);

  glGenBuffers(1, &gVertexBufferObject);
  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat),
               vertexData.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6,
                        (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6,
                        (GLvoid *)(sizeof(GLfloat) * 3));

  glBindVertexArray(0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
};

void InitializeProgram() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "SDL2 could not initialize video subsystem" << std::endl;
    exit(1);
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  gGraphicsApplicationWindow = SDL_CreateWindow(
      "OpenGL Window", 0, 0, gScreenWidth, gScreenHeight, SDL_WINDOW_OPENGL);
  if (gGraphicsApplicationWindow == nullptr) {
    std::cout << "SDl_Window was not able to be created" << std::endl;
    exit(1);
  }
  gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);

  if (gOpenGLContext == nullptr) {
    std::cout << "Could not create openGL context" << std::endl;
  }

  if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
    std::cout << "Glad was not initialized" << std::endl;
    exit(1);
  }

  GetOpenGLVersionInfo();
}

void Input() {
  SDL_Event e;

  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      std::cout << "Goodbye!" << std::endl;
      gQuit = true;
    }
  }
}

void PreDraw() {
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glViewport(0, 0, gScreenWidth, gScreenHeight);

  glClearColor(0.0705882f, 0.0705882f, 0.0705882f, 1.f);

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  glUseProgram(gGraphicsPipelineShaderProgram);
}

void Draw() {
  glBindVertexArray(gVertexArrayObject);
  glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

void MainLoop() {
  while (!gQuit) {
    Input();

    PreDraw();

    Draw();

    SDL_GL_SwapWindow(gGraphicsApplicationWindow);
  }
}

void CleanUp() {
  SDL_DestroyWindow(gGraphicsApplicationWindow);
  SDL_Quit();
}

int main() {

  InitializeProgram();

  VertexSpecification();

  CreateGraphicsPipeline();

  MainLoop();

  CleanUp();

  return 0;
}
