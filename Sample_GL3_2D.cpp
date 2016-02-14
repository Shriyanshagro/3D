#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;

GLuint programID;

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
                          0,                  // attribute 0. Vertices
                          3,                  // size (x,y,z)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
                          1,                  // attribute 1. Color
                          3,                  // size (r,g,b)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Enable Vertex Attribute 1 - Color
    glEnableVertexAttribArray(1);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}


 // all variables defined here
float camera_rotation_angle = 90;
float rectangle_rotation = 180;
float triangle_rotation = 0;
float obstacle_rotation[51] ;
float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
bool triangle_rot_status = false;
bool rectangle_rot_status = false;
float eyefrom[4] = {0,1.2f,1.4f,1.2f} ;
float targetto[4] = {0,0,0,0};
float botpos[4] = {0,-0.97f,1.1f,-0.97f};
float posz=0;
float posx=0;
float panx =0 ;
float panz =0;
int num_obs = 6;
float obsx[51];
float obsz[51] ;
int no_cam = 4 ;
float camfrom[4] ;
float camlook[4] ;
int campos=0;
double mov[51] ;
float dir[51] ;
int visibility[51];
int appear[51] ;
float mouposx;
float mouposy ;
float mousez;
double lastx = 0;
double lasty =0 ;
double theta;
double phi=0;
double zoom =0 ;
int appear_time = 1500;


void reshapeWindow(int width,int height);

/* Executed when a regular key is pressed */
void keyboardDown (unsigned char key, int x, int y)
{
    switch (key) {
        case 'Q':
        case 'q':
        case 27: //ESC
            exit (0);
        case 'd':
        case 'D':
        if(posx>0.0f)
            posx-=0.05f;
        break;
        case 'a':
        case 'A':
            if(posx<1.95f)
                posx+=0.05f;
        break;
        case 'w':
        case 'W':
            if(posz<1.95f)
                posz+=0.05f;
        break;
        case 's':
        case 'S':
        if(posz>0.0f)
            posz-=0.05f;
        break;
        default:
            break;
    }
}

/* Executed when a regular key is released */
void keyboardUp (unsigned char key, int x, int y)
{
    switch (key) {
        case 'c':
        case 'C':
            rectangle_rot_status = !rectangle_rot_status;
            break;
        case 'p':
        case 'P':
            triangle_rot_status = !triangle_rot_status;
            break;
        case 'x':
            // do something
            break;
        default:
            break;
    }
}

/* Executed when a special key is pressed */
void keyboardSpecialDown (int key, int x, int y)
{
    switch(key){
            case GLUT_KEY_UP:
                // if(panx<=1)
                // {
                    panx-=0.1f;
                // }
            break;
            case GLUT_KEY_DOWN:
            // if(panx>=-1)
            // {
                panx+=0.1f;
            // }
            break;
            case GLUT_KEY_RIGHT:
                // if(panz<=35)
                // {
                    panz-=0.1f;
                // }
            break;
            case GLUT_KEY_LEFT:
                // if(panz>=-35)
                // {
                    panz+=0.1f;
                // }
            break;

    }
}

/* Executed when a special key is released */
void keyboardSpecialUp (int key, int x, int y)
{
}

/* Executed when a mouse button 'button' is put into state 'state'
 at screen position ('x', 'y')
 */
void mouseClick (int button, int state, int x, int y)
{
    switch (button) {
        case GLUT_LEFT_BUTTON:
            if (state == GLUT_UP){
                campos++;
                campos=campos%no_cam;
            }
            break;
        case GLUT_RIGHT_BUTTON:
            if (state == GLUT_UP) {
            }
            break;
        case 3:
            if(zoom<=300)
            {
                zoom+=20;
                reshapeWindow(600,600);
            }
        break;
        case 4:
            if(zoom>=-300)
            {
                zoom-= 20;
                reshapeWindow(600,600);
            }
        break;
        default:
            break;
    }
}

/* Executed when the mouse moves to position ('x', 'y') */
void mouseMotion (int x, int y)
{
                // cout<<x<<" "<<y<<endl;
    theta += (lastx-x) / 100.0;
   phi += (lasty-y) / 50.0;
   lastx = x/1000;
   lasty = y/1000;
   mouposx = x ;
    mouposx /=1000;
    mouposy = y;
    mouposy /=1000;

}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (int width, int height)
{
	GLfloat fov = 90.0f;

	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) width, (GLsizei) height);

	// set the projection matrix as perspective/ortho
	// Store the projection matrix in a variable for future use

    // Matrices.projection = glm::ortho(x, y, x, y, 0.1f, 500.0f);
    // Perspective projection for 3D views
    Matrices.projection = glm::perspective (fov, (GLfloat) (width - zoom) / (GLfloat) (height +zoom ) , 0.1f, 500.0f);

    // Ortho projection for 2D views
    // Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}

VAO *triangle, *rectangle,*obstacle[50];

// Creates the triangle object used in this sample code
void createground ()
{
  /* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

  /* Define vertex array as used in glBegin (GL_TRIANGLES) */
  static const GLfloat vertex_buffer_data [] = {
      -1.0f,-1.0f,-1.0f, // triangle 1 : begin

       -1.0f,-1.0f, 1.0f,

       -1.0f, 1.0f, 1.0f, // triangle 1 : end

       1.0f, 1.0f,-1.0f, // triangle 2 : begin

       -1.0f,-1.0f,-1.0f,

       -1.0f, 1.0f,-1.0f, // triangle 2 : end

       1.0f,-1.0f, 1.0f,

       -1.0f,-1.0f,-1.0f,

       1.0f,-1.0f,-1.0f,

       1.0f, 1.0f,-1.0f,

       1.0f,-1.0f,-1.0f,

       -1.0f,-1.0f,-1.0f,

       -1.0f,-1.0f,-1.0f,

       -1.0f, 1.0f, 1.0f,

       -1.0f, 1.0f,-1.0f,

       1.0f,-1.0f, 1.0f,

       -1.0f,-1.0f, 1.0f,

       -1.0f,-1.0f,-1.0f,

       -1.0f, 1.0f, 1.0f,

       -1.0f,-1.0f, 1.0f,

       1.0f,-1.0f, 1.0f,

       1.0f, 1.0f, 1.0f,

       1.0f,-1.0f,-1.0f,

       1.0f, 1.0f,-1.0f,

       1.0f,-1.0f,-1.0f,

       1.0f, 1.0f, 1.0f,

       1.0f,-1.0f, 1.0f,

       1.0f, 1.0f, 1.0f,

       1.0f, 1.0f,-1.0f,

       -1.0f, 1.0f,-1.0f,

       1.0f, 1.0f, 1.0f,

       -1.0f, 1.0f,-1.0f,

       -1.0f, 1.0f, 1.0f,

       1.0f, 1.0f, 1.0f,

       -1.0f, 1.0f, 1.0f,

       1.0f,-1.0f, 1.0f
  };

  static const GLfloat color_buffer_data [] = {
      0.583f,  0.771f,  0.014f,

       0.609f,  0.115f,  0.436f,

       0.327f,  0.483f,  0.844f,

       0.822f,  0.569f,  0.201f,

       0.435f,  0.602f,  0.223f,

       0.310f,  0.747f,  0.185f,

       0.597f,  0.770f,  0.761f,

       0.559f,  0.436f,  0.730f,

       0.359f,  0.583f,  0.152f,

       0.483f,  0.596f,  0.789f,

       0.559f,  0.861f,  0.639f,

       0.195f,  0.548f,  0.859f,

       0.014f,  0.184f,  0.576f,

       0.771f,  0.328f,  0.970f,

       0.406f,  0.615f,  0.116f,

       0.676f,  0.977f,  0.133f,

       0.971f,  0.572f,  0.833f,

       0.140f,  0.616f,  0.489f,

       0.997f,  0.513f,  0.064f,

       0.945f,  0.719f,  0.592f,

       0.543f,  0.021f,  0.978f,

       0.279f,  0.317f,  0.505f,

       0.167f,  0.620f,  0.077f,

       0.347f,  0.857f,  0.137f,

       0.055f,  0.953f,  0.042f,

       0.714f,  0.505f,  0.345f,

       0.783f,  0.290f,  0.734f,

       0.722f,  0.645f,  0.174f,

       0.302f,  0.455f,  0.848f,

       0.225f,  0.587f,  0.040f,

       0.517f,  0.713f,  0.338f,

       0.053f,  0.959f,  0.120f,

       0.393f,  0.621f,  0.362f,

       0.673f,  0.211f,  0.457f,

       0.820f,  0.883f,  0.371f,

       0.982f,  0.099f,  0.879f
  };

  // create3DObject creates and returns a handle to a VAO that can be used later
  triangle = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createbot ()
{
  // GL3 accepts only Triangles. Quads are not supported static
  static const GLfloat vertex_buffer_data [] = {
      -0.05f,-0.05f,-0.05f, // triangle 1 : begin

       -0.05f,-0.05f, 0.05f,

       -0.05f, 0.05f, 0.05f, // triangle 1 : end

       0.05f, 0.05f,-0.05f, // triangle 2 : begin

       -0.05f,-0.05f,-0.05f,

       -0.05f, 0.05f,-0.05f, // triangle 2 : end

       0.05f,-0.05f, 0.05f,

       -0.05f,-0.05f,-0.05f,

       0.05f,-0.05f,-0.05f,

       0.05f, 0.05f,-0.05f,

       0.05f,-0.05f,-0.05f,

       -0.05f,-0.05f,-0.05f,

       -0.05f,-0.05f,-0.05f,

       -0.05f, 0.05f, 0.05f,

       -0.05f, 0.05f,-0.05f,

       0.05f,-0.05f, 0.05f,

       -0.05f,-0.05f, 0.05f,

       -0.05f,-0.05f,-0.05f,

       -0.05f, 0.05f, 0.05f,

       -0.05f,-0.05f, 0.05f,

       0.05f,-0.05f, 0.05f,

       0.05f, 0.05f, 0.05f,

       0.05f,-0.05f,-0.05f,

       0.05f, 0.05f,-0.05f,

       0.05f,-0.05f,-0.05f,

       0.05f, 0.05f, 0.05f,

       0.05f,-0.05f, 0.05f,

       0.05f, 0.05f, 0.05f,

       0.05f, 0.05f,-0.05f,

       -0.05f, 0.05f,-0.05f,

       0.05f, 0.05f, 0.05f,

       -0.05f, 0.05f,-0.05f,

       -0.05f, 0.05f, 0.05f,

       0.05f, 0.05f, 0.05f,

       -0.05f, 0.05f, 0.05f,

       0.05f,-0.05f, 0.05f
  };

  static const GLfloat color_buffer_data [] = {
      0.583f,  0.771f,  0.014f,

       0.609f,  0.115f,  0.436f,

       0.327f,  0.483f,  0.844f,

       0.822f,  0.569f,  0.201f,

       0.435f,  0.602f,  0.223f,

       0.310f,  0.747f,  0.185f,

       0.597f,  0.770f,  0.761f,

       0.559f,  0.436f,  0.730f,

       0.359f,  0.583f,  0.152f,

       0.483f,  0.596f,  0.789f,

       0.559f,  0.861f,  0.639f,

       0.195f,  0.548f,  0.859f,

       0.014f,  0.184f,  0.576f,

       0.771f,  0.328f,  0.970f,

       0.406f,  0.615f,  0.116f,

       0.676f,  0.977f,  0.133f,

       0.971f,  0.572f,  0.833f,

       0.140f,  0.616f,  0.489f,

       0.997f,  0.513f,  0.064f,

       0.945f,  0.719f,  0.592f,

       0.543f,  0.021f,  0.978f,

       0.279f,  0.317f,  0.505f,

       0.167f,  0.620f,  0.077f,

       0.347f,  0.857f,  0.137f,

       0.055f,  0.953f,  0.042f,

       0.714f,  0.505f,  0.345f,

       0.783f,  0.290f,  0.734f,

       0.722f,  0.645f,  0.174f,

       0.302f,  0.455f,  0.848f,

       0.225f,  0.587f,  0.040f,

       0.517f,  0.713f,  0.338f,

       0.053f,  0.959f,  0.120f,

       0.393f,  0.621f,  0.362f,

       0.673f,  0.211f,  0.457f,

       0.820f,  0.883f,  0.371f,

       0.982f,  0.099f,  0.879f
  };


  // create3DObject creates and returns a handle to a VAO that can be used later
  rectangle = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}


void createobstacle ()
{
  // GL3 accepts only Triangles. Quads are not supported static
  static const GLfloat vertex_buffer_data [] = {
       -0.05f,-0.05f,-0.05f, // triangle 1 : begin

       -0.05f,-0.05f, 0.05f,

       -0.05f, 0.05f, 0.05f, // triangle 1 : end

       0.05f, 0.05f,-0.05f, // triangle 2 : begin

       -0.05f,-0.05f,-0.05f,

       -0.05f, 0.05f,-0.05f, // triangle 2 : end

       0.05f,-0.05f, 0.05f,

       -0.05f,-0.05f,-0.05f,

       0.05f,-0.05f,-0.05f,

       0.05f, 0.05f,-0.05f,

       0.05f,-0.05f,-0.05f,

       -0.05f,-0.05f,-0.05f,

       -0.05f,-0.05f,-0.05f,

       -0.05f, 0.05f, 0.05f,

       -0.05f, 0.05f,-0.05f,

       0.05f,-0.05f, 0.05f,

       -0.05f,-0.05f, 0.05f,

       -0.05f,-0.05f,-0.05f,

       -0.05f, 0.05f, 0.05f,

       -0.05f,-0.05f, 0.05f,

       0.05f,-0.05f, 0.05f,

       0.05f, 0.05f, 0.05f,

       0.05f,-0.05f,-0.05f,

       0.05f, 0.05f,-0.05f,

       0.05f,-0.05f,-0.05f,

       0.05f, 0.05f, 0.05f,

       0.05f,-0.05f, 0.05f,

       0.05f, 0.05f, 0.05f,

       0.05f, 0.05f,-0.05f,

       -0.05f, 0.05f,-0.05f,

       0.05f, 0.05f, 0.05f,

       -0.05f, 0.05f,-0.05f,

       -0.05f, 0.05f, 0.05f,

       0.05f, 0.05f, 0.05f,

       -0.05f, 0.05f, 0.05f,

       0.05f,-0.05f, 0.05f
  };

  static const GLfloat color_buffer_data [] = {
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,
      0,0,0,

  };

  // create3DObject creates and returns a handle to a VAO that can be used later
  srand((unsigned)time(0));
  int temp,temp2,temp3;
  for(int r=1;r<=num_obs;r++)
  {
      obstacle[r] = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
      mov[r] = rand()%5;
      mov[r] /=100;
      temp = rand()%2;
      temp2 = rand()%num_obs;
      temp3 = rand()%appear_time;
      if(temp==0)
      {
          temp++;
      }
      else{
          temp*=-1;
      }
      obsx[r] = rand()%10;
      obsx[r] /=10;
      obsx[r] *= temp;
      obsz[r] = rand()%10;
      obsz[r] /=10;
      obsz[r] *=temp;
      dir[r] = temp;
      appear[r] = temp2;
      visibility[r]=temp3;
  }
}

void cameraposition(){
    if(campos==0)
    {
      // tower view
        camfrom[1]= eyefrom[1]+panx ;
        camfrom[2]= eyefrom[2];
        camfrom[3] = eyefrom[3]+panz ;
        camlook[1]= targetto[1] ;
        camlook[2]= targetto[2];
        camlook[3] = targetto[3];
    }

    else if(campos==1)
    {
      //   bot's eye
        camfrom[1]= botpos[1]+posx ;
        camfrom[2]= botpos[2];
        camfrom[3] = botpos[3] + posz;
        camlook[1]= botpos[1]+posx ;
        camlook[2]= botpos[2];
        camlook[3] = botpos[3] + posz -0.2f;
        rectangle_rotation = theta;
    }
    else if(campos==2)
    {
      //   bot's head
        camfrom[1]= botpos[1]+posx ;
        camfrom[2]= botpos[2]+0.2f;
        camfrom[3] = botpos[3] + posz + 0.4f;
        camlook[1]= botpos[1]+posx ;
        camlook[2]= botpos[2];
        camlook[3] = botpos[3] + posz ;
        rectangle_rotation = theta;
    }
    else if(campos==3)
    {
      // tower view
        camfrom[1]= botpos[1] ;
        camfrom[2]= botpos[2] + 1 ;
        camfrom[3] =botpos[3] ;
        camlook[1]= targetto[1] ;
        camlook[2]= targetto[2];
        camlook[3] = targetto[3];
    }

}

void fall_down(){
    for(int r=1;r<=num_obs;r++){
        if(botpos[1]+posx<=(obsx[r]+0.05f) && botpos[1]+posx>=(obsx[r]-0.05f) && botpos[3]+posz<=(obsz[r]+0.05) && botpos[3]+posz>=(obsz[r]-0.05) &&
            visibility[r]<appear_time*2/3) {
            cout<<"You Lose!!"<<endl;
            exit(0);
        }
    }
    // botpos[1]+posx,botpos[2]-0.09f,botpos[3]+posz
    // obsx[r],botpos[2]-0.12f+mov[r],obsz[r]
}
/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw ()
{
  // clear the color and depth in the frame buffer
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // use the loaded shader program
  // Don't change unless you know what you are doing
  glUseProgram (programID);

  // Eye - Location of camera. Don't change unless you are sure!!
  glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
  // Target - Where is the camera looking at.  Don't change unless you are sure!!
  glm::vec3 target (0, 0, 0);
  // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
  glm::vec3 up (0, 1, 0);

  // Compute Camera matrix (view)
  // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
  //  Don't change unless you are sure!!

  cameraposition();

  // TO-DO = camera roattion with bot rotation , for man's eye
  Matrices.view = glm::lookAt(glm::vec3(camfrom[1],camfrom[2],camfrom[3]), glm::vec3(mouposx,camlook[2],mouposy), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

  // Compute ViewProject matrix as view/camera might not beappear changed for this frame (basic scenario)
  //  Don't change unless you are sure!!
  glm::mat4 VP = Matrices.projection * Matrices.view;

  // Send our transformation to the currently bound shader, in the "MVP" uniform
  // For each model you render, since the MVP will be different (at least the M part)
  //  Don't change unless you are sure!!
  glm::mat4 MVP;	// MVP = Projection * View * Model

  // Load identity to model matrix
  Matrices.model = glm::mat4(1.0f);

  /* Render your scene */

  glm::mat4 translateTriangle = glm::translate (glm::vec3(0, -0.03f, 0.0f)); // glTranslatef
  glm::mat4 rotateTriangle = glm::rotate((float)(triangle_rotation*M_PI/180.0f), glm::vec3(0,0,1));  // rotate about vector (1,0,0)
  glm::mat4 triangleTransform = rotateTriangle * translateTriangle;
  Matrices.model *= triangleTransform;
  MVP = VP * Matrices.model; // MVP = p * V * M

  //  Don't change unless you are sure!!
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(triangle);

  Matrices.model = glm::mat4(1.0f);
  // bot
  glm::mat4 translateRectangle = glm::translate (glm::vec3(botpos[1],botpos[2]-0.09f,botpos[3]));        // glTranslatef
  glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,1,0)); // rotate about vector (-1,1,1)
  Matrices.model *= ( rotateRectangle *   translateRectangle  );
  glm::mat4 translateRectangle2 = glm::translate (glm::vec3(posx,0,posz));        // glTranslatef
  // glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,1,0)); // rotate about vector (-1,1,1)
  Matrices.model *= (  translateRectangle2 );
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(rectangle);


  glm::mat4 translateobstacle[num_obs+1],obstacle_rotation[num_obs+1],rotateobstacle[num_obs+1];
  for(int r=1;r<=num_obs;r++)
  {
      Matrices.model = glm::mat4(1.0f);

    //   some tiles appear disappear
    if(appear[r]%(num_obs/2)==0){
        visibility[r]++;
        if(visibility[r]>appear_time)
            visibility[r]=1;
    }
    else{
    if(mov[r]>0.05 || mov[r]<-0.1 ){
        dir[r]*=-1;
    }
        mov[r]+=0.001*dir[r];
    }
      translateobstacle[r] = glm::translate (glm::vec3(obsx[r],botpos[2]-0.12f+mov[r],obsz[r]));        // glTranslatef
      rotateobstacle[r] = glm::rotate((float)(180*M_PI/180.0f), glm::vec3(0,1,0)); // rotate about vector (-1,1,1)
      Matrices.model *= (rotateobstacle[r]*translateobstacle[r] );
      MVP = VP * Matrices.model;
      glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

      // draw3DObject draws the VAO given to it using current MVP matrix
      if(visibility[r]<(appear_time*2/3))
      draw3DObject(obstacle[r]);

    //   obstacle_rotation[r] = obstacle_rotation[r] + obstacle_rot_dir[r]*obstacle_rot_status[r];

  }

  fall_down();

  // Swap the frame buffers
  glutSwapBuffers ();

  // Increment angles
  float increments = 1;

  //camera_rotation_angle++; // Simulating camera rotation
  triangle_rotation = triangle_rotation + increments*triangle_rot_dir*triangle_rot_status;
  rectangle_rotation = rectangle_rotation + increments*rectangle_rot_dir*rectangle_rot_status;

}

/* Executed when the program is idle (no I/O activity) */
void idle () {
    // OpenGL should never stop drawing
    // can draw the same scene or a modified scene
    draw (); // drawing same scene
}


/* Initialise glut window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
void initGLUT (int& argc, char** argv, int width, int height)
{
    // Init glut
    glutInit (&argc, argv);

    // Init glut window
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitContextVersion (3, 3); // Init GL 3.3
    glutInitContextFlags (GLUT_CORE_PROFILE); // Use Core profile - older functions are deprecated
    glutInitWindowSize (width, height);
    glutCreateWindow ("D.N.A.H.B Games");

    // Initialize GLEW, Needed in Core profile
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        cout << "Error: Failed to initialise GLEW : "<< glewGetErrorString(err) << endl;
        exit (1);
    }

    // register glut callbacks
    glutKeyboardFunc (keyboardDown);
    glutKeyboardUpFunc (keyboardUp);

    glutSpecialFunc (keyboardSpecialDown);
    glutSpecialUpFunc (keyboardSpecialUp);

    glutMouseFunc (mouseClick);
    glutPassiveMotionFunc (mouseMotion);

    glutReshapeFunc (reshapeWindow);

    glutDisplayFunc (draw); // function to draw when active
    glutIdleFunc (idle); // function to draw when idle (no I/O activity)

    glutIgnoreKeyRepeat (true); // Ignore keys held down
}

/* Process menu option 'op' */
void menu(int op)
{
    switch(op)
    {
        case 'Q':
        case 'q':
            exit(0);
    }
}

void addGLUTMenus ()
{
    // create sub menus
    int subMenu = glutCreateMenu (menu);
    glutAddMenuEntry ("Do Nothing", 0);
    glutAddMenuEntry ("Really Quit", 'q');

    // create main "middle click" menu
    glutCreateMenu (menu);
    glutAddSubMenu ("Sub Menu", subMenu);
    glutAddMenuEntry ("Quit", 'q');
    glutAttachMenu (GLUT_MIDDLE_BUTTON);
}


/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (int width, int height)
{
	// Create the models
	createground (); // Generate the VAO, VBOs, vertices data & copy into the array buffer
    createobstacle();
	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (width, height);

	// Background color of the scene
	glClearColor (0.3f, 0.3f, 0.3f, 0.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	createbot ();

	cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
	int width = 600;
	int height = 600;

    initGLUT (argc, argv, width, height);

    addGLUTMenus ();

	initGL (width, height);

    glutMainLoop ();

    return 0;
}
