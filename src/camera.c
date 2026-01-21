//
// Created by User on 10/5/2025.
//

#include "camera.h"

void camera_update_vectors(Camera* camera) {
    camera->front[0] = cosf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
    camera->front[1] = sinf(glm_rad(camera->pitch));
    camera->front[2] = sinf(glm_rad(camera->yaw)) * cosf(glm_rad(camera->pitch));
    glm_normalize(camera->front);

    glm_cross(camera->front, camera->world_up, camera->right);
    glm_normalize(camera->right);

    glm_cross(camera->right, camera->front, camera->up);
    glm_normalize(camera->up);
}

Camera camera_init(float x, float y, float z) {
    Camera cam = {
        .position[0]=x, .position[1]=y, .position[2]=z,
        .world_up[0]=0.0f, .world_up[1]=1.0f, .world_up[2]=0.0f,
        .yaw=-90.0f, .pitch=0.0f, .roll=0.0f,
        .movement_speed=2.5f, .mouse_sensitivity=0.1f, .fov=45.0f
    };

    camera_update_vectors(&cam);

    return cam;
}

void camera_view_matrix(Camera camera, mat4 view) {
    vec3 center;
    glm_vec3_add(camera.position, camera.front, center);
    glm_lookat(camera.position, center, camera.up, view);
}

void camera_projection_matrix(const Camera camera, const float aspect_ratio, mat4 projection) {
    glm_perspective(glm_rad(camera.fov), aspect_ratio, 0.1f, 100.0f, projection);
}

void camera_to_shader(const Camera camera, const Shader shader, const float aspect_ratio) {
    mat4 tmp;
    camera_view_matrix(camera, tmp);
    shader_uMat4f(shader, "view", tmp);

    camera_projection_matrix(camera, aspect_ratio, tmp);
    shader_uMat4f(shader, "projection", tmp);
}

void camera_process_input(Camera *camera, GLFWwindow *window, float delta_time) {
    if (!camera->locked)
        return;

    const float camera_speed = camera->movement_speed * delta_time; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        float y = camera->position[1];
        glm_vec3_muladds(camera->front, camera_speed, camera->position);
        camera->position[1] = y;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        float y = camera->position[1];
        glm_vec3_mulsubs(camera->front, camera_speed, camera->position);
        camera->position[1] = y;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        float y = camera->position[1];
        vec3 temp = {0, 0, 0};
        glm_cross(camera->front, camera->up, temp);
        glm_normalize(temp);
        glm_vec3_mulsubs(temp, camera_speed, camera->position);
        camera->position[1] = y;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        float y = camera->position[1];
        vec3 temp = {0, 0, 0};
        glm_cross(camera->front, camera->up, temp);
        glm_normalize(temp);
        glm_vec3_muladds(temp, camera_speed, camera->position);
        camera->position[1] = y;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera->position[1] += camera_speed * 0.5f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        camera->position[1] -= camera_speed * 0.5f;
    }
}

void camera_process_mouse(Camera *camera, GLFWwindow *window, float xOffset, float yOffset) {
    if (!camera->locked)
        return;



    const float sensitivity = camera->mouse_sensitivity;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    camera->yaw += xOffset;
    camera->pitch += yOffset;

    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;

    camera_update_vectors(camera);
}

void camera_process_scroll(Camera *camera, GLFWwindow *window, double xOffset, double yOffset) {
    if (!camera->locked)
        return;

    camera->fov -= (float) yOffset;
    if (camera->fov < 1.0f)
        camera->fov = 1.0f;
    if (camera->fov > 45.0f)
        camera->fov = 45.0f;
}

void camera_cursor_lock(Camera* camera, GLFWwindow *window) {
    camera->locked = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void camera_cursor_unlock(Camera* camera, GLFWwindow *window) {
    camera->locked = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}