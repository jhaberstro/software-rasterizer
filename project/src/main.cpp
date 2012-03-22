#include <GLUT/glut.h>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <tuple>
#include "Context.hpp"

StateContext context;
GLuint read_fbo;
GLuint framebuffer_tex;

enum : size_t
{
	WIDTH = 640,
	HEIGHT = 480
};

// Assumes counter clock-wise
// void DrawGeneralTriangle(Framebuffer& fb, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2) {
//     float minx = std::min(std::min(p0.x, p1.x), p2.x);
//     float miny = std::min(std::min(p0.y, p1.y), p2.y);
//     float maxx = std::max(std::max(p0.x, p1.x), p2.x);
//     float maxy = std::max(std::max(p0.y, p1.y), p2.y);

//     // All derived from:
//     //  float edge01 = (dx01 * (y - p0.y)) - (dy01 * (x - p0.x));
//     //  float edge12 = (dx12 * (y - p1.y)) - (dy12 * (x - p1.x));
//     //  float edge20 = (dx20 * (y - p2.y)) - (dy20 * (x - p2.x));
//     float dx01 = (p1.x - p0.x);
//     float dx12 = (p2.x - p1.x);
//     float dx20 = (p0.x - p2.x);
//     float dy01 = (p1.y - p0.y);
//     float dy12 = (p2.y - p1.y);
//     float dy20 = (p0.y - p2.y);
//     float c01 = (dx01 * -p0.y) + (dy01 * p0.x);
//     float c12 = (dx12 * -p1.y) + (dy12 * p1.x);
//     float c20 = (dx20 * -p2.y) + (dy20 * p2.x);
//     float cy01 = (dx01 * miny) + (dy01 * -minx) + c01;
//     float cy12 = (dx12 * miny) + (dy12 * -minx) + c12;
//     float cy20 = (dx20 * miny) + (dy20 * -minx) + c20;

//     // Correct for fill convention
//     if (dy01 < 0 || (dy01 == 0.0f && dx01 > 0)) c01 += 1;
//     if (dy12 < 0 || (dy12 == 0.0f && dx12 > 0)) c12 += 1;
//     if (dy20 < 0 || (dy20 == 0.0f && dx20 > 0)) c20 += 1;

//     uint32_t color = 0xff0000ff;
//     for (float y = miny; y < maxy; y += 1.0f) {
//         float cx01 = cy01;
//         float cx12 = cy12;
//         float cx20 = cy20;
//         for (float x = minx; x < maxx; x += 1.0f) {
//             if (cx01 >= 0.0f && cx12 >= 0.0f && cx20 >= 0.0f) {
//                 fb.set_pixel(x, y, reinterpret_cast< uint8_t* >(&color));
//             }

//             cx01 -= dy01;
//             cx12 -= dy12;
//             cx20 -= dy20;
//         }

//         cy01 += dx01;
//         cy12 += dx12;
//         cy20 += dx20;
//     }
// }

VaryingData vsh_func(size_t vindex, std::vector< VertexArray >& attributes, std::vector< ShaderVariable >& uniforms) {
    VertexArray& va = attributes[0];
    glm::vec3 position = reinterpret_cast< glm::vec3* >(va.vertices)[vindex];
    glm::mat4x4& modelview = uniforms[0].m4;
    glm::mat4x4& projection = uniforms[1].m4;
    VaryingData output;
    output.push_back(ShaderVariable(modelview * projection * glm::vec4(position.x, position.y, position.z, 1.0f)));
    return output;
}

glm::vec4 fsh_func(std::vector< ShaderVariable >& varyings, std::vector< ShaderVariable >& uniforms) {
    return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

void init(void) {
    context.set_framebuffer(WIDTH, HEIGHT, 4);
    context.set_viewport(0, 0, WIDTH, HEIGHT);
    context.set_depth_range(0.0f, 1.0f);

    Framebuffer& framebuffer = context.framebuffer();
    ShaderVariable modelview = ShaderVariable(glm::mat4x4());
    ShaderVariable projection(glm::perspective(60.0f, static_cast< float >(WIDTH) / static_cast< float >(HEIGHT), 0.1f, 100.0f));

    Shader vsh, fsh;
    vsh.uniforms.push_back(modelview);
    vsh.uniforms.push_back(projection);
    vsh.vfunc = vsh_func;
    fsh.ffunc = fsh_func;

	//DrawGeneralTriangle(*framebuffer, glm::vec2(100, 100), glm::vec2(150, 110), glm::vec2(130, 150));	


	glGenTextures(1, &framebuffer_tex);
	glBindTexture(GL_TEXTURE_2D, framebuffer_tex);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, framebuffer.width(), framebuffer.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.pixels());

	glGenFramebuffers(1, &read_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, read_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_tex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void display(void) {
    Framebuffer& framebuffer = context.framebuffer();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, framebuffer.width(), framebuffer.height(), 0, 0, framebuffer.width(), framebuffer.height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
}

void idle(void) {
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("GLUT Program");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    glm::vec2 p0(0.0, 10.0);
    glm::vec2 p1(0.0, 20.0);
    glm::vec2 p3(5.0, 15.0);
    glm::vec2 proj = glm::proj(p3, glm::normalize(p1 - p0));
    printf("projection: (%f, %f)\n", proj[0], proj[1]);

    init();
    
    glutMainLoop();
    return 0;
}