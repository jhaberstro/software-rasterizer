#include <GLUT/glut.h>
#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <tuple>
#include "Renderer.hpp"
#include "DefaultRasteriser.hpp"
#include "stb_image.h"

Renderer renderer;
GLuint read_fbo;
GLuint framebuffer_tex;
struct {
    void* data;
    int width, height, numChannels;
} texture;
struct {
    std::vector< glm::vec3 > positions;
    std::vector< glm::vec2 > texcoords;
} cylinder;

void CreateCylinder(float height, float radius, int subdiv, std::vector< glm::vec3 >& vertices, std::vector< glm::vec2 >& texcoords) {
    float ybottom = -height * 0.5f;
    float dangle = (2.0f * 3.14f) / float(subdiv);
    for (float angle = 0; angle <= 2.0f * 3.14f; angle += dangle) {
        glm::vec3 bottom(std::cosf(angle) * radius, ybottom, std::sinf(angle));
        glm::vec3 top(bottom.x, -bottom.y, bottom.z);
        float t = angle / (2.0f * 3.14f);
        vertices.push_back(bottom);
        vertices.push_back(top);
        texcoords.push_back(glm::vec2(t, 0.0f));
        texcoords.push_back(glm::vec2(t, 1.0f));
    }
}

enum : size_t
{
	WIDTH = 640,
	HEIGHT = 480
};

void calculateFPS()
{
    static int frameCount = 0, previousTime = 0;
    //  Increase frame count
    frameCount++;

    //  Get the number of milliseconds since glutInit called
    //  (or first callr to glutGet(GLUT ELAPSED TIME)).
    int currentTime = glutGet(GLUT_ELAPSED_TIME);

    //  Calculate time passed
    int timeInterval = currentTime - previousTime;

    if(timeInterval > 1000)
    {
        //  calculate the number of frames per second
        float fps = frameCount / (timeInterval / 1000.0f);
        printf("fps: %f\n", fps);

        //  Set time
        previousTime = currentTime;

        //  Reset frame count
        frameCount = 0;
    }
}

VaryingData vsh_func(size_t vindex, VertexArray* attributes, std::vector< ShaderVariable > const& uniforms) {
    auto& position   = *reinterpret_cast< glm::vec3* >(attributes[0].index(vindex));
    auto& uv         = *reinterpret_cast< glm::vec2* >(attributes[1].index(vindex));
    glm::mat4x4 const& modelview  = uniforms[0].m4;
    glm::mat4x4 const& projection = uniforms[1].m4;

    VaryingData output;
    output.push_back(projection * modelview * glm::vec4(position.x, position.y, position.z, 1.0f));
    output.push_back(uv);
    return output;
}

glm::vec4 fsh_func(ShaderVariable* varyings, std::vector< ShaderVariable > const& uniforms) {
    struct rgb8 { uint8_t r, g, b; };
    glm::ivec2 st(varyings[0].v2.x * texture.width, varyings[0].v2.y * texture.height);
    rgb8* pixels = reinterpret_cast< rgb8* >(texture.data);
    size_t index = std::min((st.y * texture.width) + st.x, (texture.width * texture.height) - 1);
    rgb8 pcolor = pixels[index];
    return glm::vec4(pcolor.r / 255.0f, pcolor.g / 255.0f, pcolor.b / 255.0f, 255.0f);
}

void init(void) {
    texture.data = nullptr;
    texture.data = stbi_load("Nehe.png", &texture.width, &texture.height, &texture.numChannels, 3);
    std::printf("Num channels: %i\n", texture.numChannels);
    assert(texture.data != nullptr);

    CreateCylinder(1.0f, 1.0f, 100, cylinder.positions, cylinder.texcoords);

    renderer.set_framebuffer(WIDTH, HEIGHT, 4);
    renderer.set_viewport(0, 0, WIDTH, HEIGHT);
    renderer.set_depth_range(0.0f, 1.0f);

    Framebuffer& framebuffer = renderer.framebuffer();
    glGenTextures(1, &framebuffer_tex);
    glBindTexture(GL_TEXTURE_2D, framebuffer_tex);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, framebuffer.width(), framebuffer.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.pixels());
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, framebuffer.width(), framebuffer.height(), 0, GL_RED, GL_FLOAT, renderer.depth_buffer().pixels());

    glGenFramebuffers(1, &read_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, read_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_tex, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void draw_quad() {
    glm::vec3 positions[] = {glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f) };
    glm::vec2 uvs[] = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f) };
    renderer.set_attribute(0, 3, 0, positions);
    renderer.set_attribute(1, 2, 0, uvs);

    static int angle = 0;
    angle = (angle + 1) % 360;
    auto modelview = glm::translate(glm::mat4x4(), glm::vec3(0.0f, 0.0f, -3.5f)) * glm::rotate(glm::mat4x4(), float(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    auto projection = glm::perspective(60.0f, static_cast< float >(WIDTH) / static_cast< float >(HEIGHT), 0.1f, 100.0f);
    Shader vsh(vsh_func), fsh(fsh_func);
    vsh.uniforms.push_back(modelview);
    vsh.uniforms.push_back(projection);

    renderer.set_vertex_shader(vsh);
    renderer.set_fragment_shader(fsh);

    renderer.set_polygon_winding(PolygonWinding::CounterClockwise);
    renderer.set_primitive_topology(PrimitiveTopology::TriangleStrip);
    renderer.draw(0, 4);
}

void draw_cylinder() {
    renderer.set_attribute(0, 3, 0, &cylinder.positions[0].x);
    renderer.set_attribute(1, 2, 0, &cylinder.texcoords[0].x);

    static int angle = 0;
    angle = (angle + 1) % 360;
    auto modelview = glm::translate(glm::mat4x4(), glm::vec3(0.0f, 0.0f, -3.5f)) * glm::rotate(glm::mat4x4(), float(angle), glm::vec3(1.0f, 0.0f, 0.0f));
    auto projection = glm::perspective(60.0f, static_cast< float >(WIDTH) / static_cast< float >(HEIGHT), 0.1f, 100.0f);
    Shader vsh(vsh_func), fsh(fsh_func);
    vsh.uniforms.push_back(modelview);
    vsh.uniforms.push_back(projection);

    renderer.set_vertex_shader(vsh);
    renderer.set_fragment_shader(fsh);
    renderer.set_primitive_topology(PrimitiveTopology::TriangleStrip);
    renderer.set_polygon_winding(PolygonWinding::CounterClockwise);
    renderer.draw(0, cylinder.positions.size());
    renderer.set_polygon_winding(PolygonWinding::Clockwise);
    renderer.draw(0, cylinder.positions.size());
}

void draw() {
    uint32_t clearColor = 0x00000000;
    float depthClear = INFINITY;
    renderer.depth_buffer().clear(&depthClear);
    Framebuffer& framebuffer = renderer.framebuffer();
    framebuffer.clear(&clearColor);

    //draw_quad();
    draw_cylinder();

    glBindTexture(GL_TEXTURE_2D, framebuffer_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, framebuffer.width(), framebuffer.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.pixels());
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, framebuffer.width(), framebuffer.height(), 0, GL_RED, GL_FLOAT, renderer.depth_buffer().pixels());
}

void display(void) {
    Framebuffer& framebuffer = renderer.framebuffer();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // GLEquivalient();

    draw();
    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_fbo);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, framebuffer.width(), framebuffer.height(), 0, 0, framebuffer.width(), framebuffer.height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glFinish();
    glutSwapBuffers();

    calculateFPS();
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

    init();
    
    glutMainLoop();
    return 0;
}
