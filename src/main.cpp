#include "main.h"
#include "timer.h"
#include "cuboid.h"
#include "boat.h"

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/

Cuboid water,cannon;
Boat boat;
Cuboid rocks[1000000];
float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;
float camera_rotation_angle = 0;
int num_rocks = 200;
float acc = 0.01f,upsped = -0.15f;
Timer t60(1.0 / 60);
int flagw = 1;
float random(float a,float b)
{
   return (a + (((float) rand()) / (float) RAND_MAX)*(b-a));
}

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    glm::vec3 eye (boat.position.x, boat.position.y + 5,boat.position.z +  20 );
//    glm::vec3 eye ( 1, 5,0 );

    // Target - Where is the camera looking at.  Don't change unless you are sure!!
//    glm::vec3 target (0, 10, 10);
    glm::vec3 target (boat.position.x,boat.position.y,boat.position.z);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    // Don't change unless you are sure!!
    // Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render
    for(int i=0;i<num_rocks;i++)
        rocks[i].draw(VP);
    boat.draw(VP);
    cannon.draw((VP));
    water.draw(VP);
}

void tick_input(GLFWwindow *window) {
    int left  = glfwGetKey(window, GLFW_KEY_LEFT);
    int right = glfwGetKey(window, GLFW_KEY_RIGHT);
    int up = glfwGetKey(window, GLFW_KEY_UP);
    int down = glfwGetKey(window, GLFW_KEY_DOWN);
    int space = glfwGetKey(window, GLFW_KEY_SPACE);

    if (left) {
//        boat.set_position(boat.position.x + 0.1,boat.position.y,boat.position.z);
        boat.rotation-=1;
    }
    if (right) {
//        boat.set_position(boat.position.x - 0.1,boat.position.y,boat.position.z);
        boat.rotation+=1;

    }
    if (up) {
        boat.set_position(boat.position.x + 0.1*sin(boat.rotation*M_PI/180),boat.position.y,boat.position.z + 0.1*cos(boat.rotation*M_PI/180));
    }
    if (down) {
        boat.set_position(boat.position.x - 0.1*sin(boat.rotation*M_PI/180),boat.position.y,boat.position.z - 0.1*cos(boat.rotation*M_PI/180));
    }
    if (space && boat.position.y <=0) {
        boat.speed.y = upsped;
    }
}

void tick_elements() {
    boat.tick();
//    camera_rotation_angle += 1;

    if(flagw)
    {
        water.position.y+=0.007;
        if(water.position.y + 9.5f > 0.3f)
            flagw = 0;
    }
    else
    {
        water.position.y-=0.007;
        if(water.position.y + 9.5f < 0.0f)
            flagw = 1;
    }
    if(boat.speed.y < 0)
        boat.speed.y+=acc;
    if(boat.position.y < 0)
    {
        boat.position.y = 0;
        boat.speed.y = 0;
    }
    cout << boat.position.x << " " << boat.position.y << " " << boat.position.z << " " << endl;
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models

    water = Cuboid(0,-9,0,2000,2000,20,COLOR_BLUE);
    boat = Boat(0,0,0,COLOR_GREEN);
    for(int i=0;i<num_rocks;i++)
        rocks[i] = Cuboid(random(-500,500),1,random(-500,500),2,2,2,COLOR_RED);
    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("Sample_GL.vert", "Sample_GL.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 600;
    int height = 600;

    window = initGLFW(width, height);

    initGL (window, width, height);

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        // Process timers

        if (t60.processTick()) {
            // 60 fps
            // OpenGL Draw commands
            draw();
            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);

            tick_elements();
            tick_input(window);
        }

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) * 2 < (a.width + b.width)) &&
           (abs(a.y - b.y) * 2 < (a.height + b.height));
}

void reset_screen() {
    /*float top    = screen_center_y + 5 / screen_zoom;
    float bottom = screen_center_y - 5 / screen_zoom;
    float left   = screen_center_x - 5 / screen_zoom;
    float right  = screen_center_x + 5 / screen_zoom;
    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);*/
    Matrices.projection = glm::perspective(45.0f, 1.0f, 10.0f, 500.0f);
}