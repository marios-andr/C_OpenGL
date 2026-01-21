#include <stdbool.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "mesh.h"
#include "shader.h"
#include "texture_helper.h"
#include "camera.h"

#define INITIAL_WIDTH 1700
#define INITIAL_HEIGHT 1000

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void key_input(GLFWwindow *window);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void focus_callback(GLFWwindow *window, int focused);

unsigned int WIN_WIDTH = INITIAL_WIDTH;
unsigned int WIN_HEIGHT = INITIAL_HEIGHT;
float ASPECT_RATIO = (float) INITIAL_WIDTH / (float) INITIAL_HEIGHT;
bool is_fullscreen = false;

Camera camera;
double delta_time = 0.0f;
double last_frame = 0.0f;

bool first_mouse = true;
float last_mouse_x = (float) INITIAL_WIDTH / 2;
float last_mouse_y = (float) INITIAL_HEIGHT / 2;

int main() {
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Black Hole Sim", NULL, NULL);
    if (!window) {
        printf("Failed to create window\n");
        glfwTerminate();
        return -1;
    }


    glfwSwapInterval(1);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowFocusCallback(window, focus_callback);


    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    unsigned int skybox_tex = gen_skybox_texture("../resources/starmap_2020_8k_gal.hdr");

    Shader shader = create_shader("../shaders/simple.vert", "../shaders/blackhole/black_hole.frag");

    float cam_angle = 0;
    float cam_dist = 8.0f;
    camera = camera_init(cam_dist * sinf(cam_angle), 0.0f, cam_dist * cosf(cam_angle));
    camera.fov = 90.0f;
    camera_cursor_lock(&camera, window);

    Mesh skybox = shape_skybox();

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        double current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        key_input(window);

        cam_angle += - 0.1f * delta_time;
        camera.position[0] = cam_dist * sinf(cam_angle);
        camera.position[2] = cam_dist * cosf(cam_angle);
        camera.yaw = - cam_angle * 180 / GLM_PI - 90.0f;
        camera.pitch = 0;
        camera_update_vectors(&camera);

        //glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
        glClearColor(26.0f/255.0f, 26.0f/255.0f, 30.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        shader_use(shader);
        shader_u2f(shader, "resolution", WIN_WIDTH, WIN_HEIGHT);
        shader_u3f(shader, "cam_pos", camera.position);
        shader_u3f(shader, "cam_x", camera.right);
        shader_u3f(shader, "cam_y", camera.up);
        shader_u3f(shader, "cam_z", camera.front);
        shader_u1f(shader, "fov", camera.fov);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, skybox_tex);
        shader_u1i(shader, "equirectangularMap", 0);

        mesh_bind(skybox);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shader_delete(&shader);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void key_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        if (is_fullscreen) {
            int xpos = (mode->width - INITIAL_WIDTH) / 2;
            int ypos = (mode->height - INITIAL_HEIGHT) / 2;
            glfwSetWindowMonitor(window, NULL, xpos, ypos, INITIAL_WIDTH, INITIAL_HEIGHT, GLFW_DONT_CARE);
            is_fullscreen = false;
        } else {
            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            is_fullscreen = true;
        }
    }


    camera_process_input(&camera, window, delta_time);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (first_mouse) // initially set to true
    {
        last_mouse_x = xpos;
        last_mouse_y = ypos;
        first_mouse = false;
    }

    float xoffset = xpos - last_mouse_x;
    float yoffset = last_mouse_y - ypos; // reversed since y-coordinates range from bottom to top
    last_mouse_x = xpos;
    last_mouse_y = ypos;

    camera_process_mouse(&camera, window, xoffset, yoffset);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    WIN_WIDTH = width;
    WIN_HEIGHT = height;
    ASPECT_RATIO = (float) width / (float) height;
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera_process_scroll(&camera, window, xoffset, yoffset);
}

void focus_callback(GLFWwindow *window, int focused) {
    if (focused == GLFW_FALSE) {
        first_mouse = true;
    }
}
