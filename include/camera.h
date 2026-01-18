//
// Created by User on 10/5/2025.
//

#ifndef CAMERA_H
#define CAMERA_H
#include <cglm/cglm.h>

#include "shader.h"
#include "GLFW/glfw3.h"

typedef struct {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;
    float yaw;
    float pitch;
    float roll;
    float movement_speed;
    float mouse_sensitivity;
    float fov;
    bool locked;
} Camera;

Camera camera_init(float x, float y, float z);

void camera_view_matrix(Camera camera, mat4 view);

void camera_projection_matrix(Camera camera, float aspect_ratio, mat4 projection);

void camera_to_shader(Camera camera, Shader shader, float aspect_ratio);

void camera_process_input(Camera* camera, GLFWwindow* window, float delta_time);

void camera_process_mouse(Camera* camera, GLFWwindow* window, float xOffset, float yOffset);

void camera_process_scroll(Camera* camera, GLFWwindow* window, double xOffset, double yOffset);

void camera_cursor_lock(Camera* camera, GLFWwindow* window);

void camera_cursor_unlock(Camera* camera, GLFWwindow* window);

#endif //CAMERA_H
