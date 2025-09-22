#include "SolarSystemSimulator.h"
#include <QMouseEvent>
#include <QTimer>
#include <cmath>
#include <iostream>

SolarSystemWidget::SolarSystemWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    QTimer* t = new QTimer(this);
    connect(t, &QTimer::timeout, this, QOverload<>::of(&SolarSystemWidget::update));
    t->start(16); // ~60 FPS
    timer.start();

    // Define planets
    planets = {
        {"Mercury", 3.0f, 0, 47.0f, 0, 100.0f, 0.4f, {0.7f, 0.7f, 0.6f}},
        {"Venus",   4.5f, 0, 35.0f, 0,  -5.0f, 0.95f, {0.9f, 0.7f, 0.3f}},
        {"Earth",   6.0f, 0, 20.0f, 0,  80.0f, 0.8f, {0.2f, 0.4f, 1.0f}},
        {"Mars",    8.0f, 0, 17.0f, 0,  60.0f, 0.6f, {1.0f, 0.3f, 0.3f}},
        {"Jupiter", 11.0f,0, 8.0f,  0, 150.0f, 1.5f, {0.9f, 0.6f, 0.3f}},
        {"Saturn",  14.0f,0, 6.0f,  0, 120.0f, 1.2f, {0.9f, 0.8f, 0.5f}},
        {"Uranus",  17.0f,0, 4.0f,  0, 100.0f, 1.0f, {0.5f, 0.8f, 1.0f}},
        {"Neptune", 20.0f,0, 3.0f,  0, 90.0f,  1.0f, {0.3f, 0.3f, 1.0f}}
    };
}

SolarSystemWidget::~SolarSystemWidget() {
    makeCurrent();
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    doneCurrent();
}

void SolarSystemWidget::setSimulationSpeedPercent(int percent) {
    if (percent < 0) percent = 0;
    simulationSpeed = percent / 100.0f;
    if (simulationSpeed < 0.01f) simulationSpeed = 0.0f;
}

void SolarSystemWidget::initializeGL() {
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    if (!compileShaders()) {
        std::cerr << "Shader compilation failed\n";
        return;
    }

    generateSphereMesh(latBands, longBands);
    createBuffers();

    uniModel = glGetUniformLocation(shaderProgram, "uModel");
    uniView = glGetUniformLocation(shaderProgram, "uView");
    uniProj = glGetUniformLocation(shaderProgram, "uProj");
    uniColor = glGetUniformLocation(shaderProgram, "uColor");

    glClearColor(0.02f, 0.02f, 0.06f, 1.0f);
}

void SolarSystemWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    proj.setToIdentity();
    proj.perspective(45.0f, float(w) / float(qMax(1, h)), 0.1f, 100.0f);
}

void SolarSystemWidget::paintGL() {
    updateAnimation();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    QMatrix4x4 view;
    view.translate(0, 0, -cameraDistance);
    view.rotate(pitch, 1, 0, 0);
    view.rotate(yaw, 0, 1, 0);

    glUniformMatrix4fv(uniView, 1, GL_FALSE, view.constData());
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, proj.constData());

    glBindVertexArray(vao);

    // Sun
    {
        QMatrix4x4 model;
        model.scale(2.0f);
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, model.constData());
        glUniform3f(uniColor, 1.0f, 0.9f, 0.2f);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    }

    // Planets
    for (auto& p : planets) {
        QMatrix4x4 model;
        model.rotate(p.orbitAngle, 0, 1, 0);
        model.translate(p.orbitRadius, 0, 0);
        model.rotate(p.rotation, 0, 1, 0);
        model.scale(p.size);

        glUniformMatrix4fv(uniModel, 1, GL_FALSE, model.constData());
        glUniform3f(uniColor, p.color.x(), p.color.y(), p.color.z());
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);

        // Saturn ring
        if (p.name == "Saturn") {
            QMatrix4x4 ring = model;
            ring.scale(1.6f, 1.0f, 1.6f);
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, ring.constData());
            glUniform3f(uniColor, 0.8f, 0.7f, 0.5f);
            glDrawElements(GL_LINE_LOOP, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        }

        // Earth Moon
        if (p.name == "Earth") {
            QMatrix4x4 moonModel = model;
            moonModel.rotate(50.0f * simulationSpeed, 0, 1, 0);
            moonModel.translate(1.6f, 0, 0);
            moonModel.scale(0.27f);
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, moonModel.constData());
            glUniform3f(uniColor, 0.8f, 0.8f, 0.8f);
            glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void SolarSystemWidget::mousePressEvent(QMouseEvent* e) {
    lastMousePos = e->pos();
    e->accept();
}

void SolarSystemWidget::mouseMoveEvent(QMouseEvent* e) {
    QPoint delta = e->pos() - lastMousePos;
    lastMousePos = e->pos();

    if (e->buttons() & Qt::LeftButton) {
        yaw += delta.x() * 0.5f;
        pitch += delta.y() * 0.5f;
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
        update();
    }
    e->accept();
}

void SolarSystemWidget::wheelEvent(QWheelEvent* e) {
    cameraDistance -= e->angleDelta().y() / 240.0f;
    if (cameraDistance < 4.0f) cameraDistance = 4.0f;
    if (cameraDistance > 80.0f) cameraDistance = 80.0f;
    update();
    e->accept();
}

GLuint SolarSystemWidget::compileSingleShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[1024];
        glGetShaderInfoLog(s, 1024, nullptr, buf);
        std::cerr << "Shader compile error: " << buf << "\n";
        glDeleteShader(s);
        return 0;
    }
    return s;
}

bool SolarSystemWidget::compileShaders() {
    const char* vertSrc = R"(#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vNormal;

void main() {
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
})";

    const char* fragSrc = R"(#version 330 core
in vec3 vNormal;
uniform vec3 uColor;
out vec4 fragColor;

void main() {
    vec3 lightDir = normalize(vec3(0.3, 1.0, 0.4));
    float diff = max(dot(normalize(vNormal), lightDir), 0.0);
    vec3 col = uColor * (0.3 + 0.7 * diff);
    fragColor = vec4(col, 1.0);
})";

    GLuint vs = compileSingleShader(GL_VERTEX_SHADER, vertSrc);
    GLuint fs = compileSingleShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!vs || !fs) return false;

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    GLint ok;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
    if (!ok) {
        char buf[1024];
        glGetProgramInfoLog(shaderProgram, 1024, nullptr, buf);
        std::cerr << "Program link error: " << buf << "\n";
        return false;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return true;
}

void SolarSystemWidget::generateSphereMesh(int latBands, int longBands) {
    vertices.clear();
    indices.clear();

    for (int lat = 0; lat <= latBands; ++lat) {
        float theta = lat * M_PI / latBands;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= longBands; ++lon) {
            float phi = lon * 2.0f * M_PI / longBands;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            // pos
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normal
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for (int lat = 0; lat < latBands; ++lat) {
        for (int lon = 0; lon < longBands; ++lon) {
            unsigned int first = (lat * (longBands + 1)) + lon;
            unsigned int second = first + longBands + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

void SolarSystemWidget::createBuffers() {
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (ebo) glDeleteBuffers(1, &ebo);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    GLsizei stride = 6 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
}

void SolarSystemWidget::updateAnimation() {
    static qint64 last = timer.elapsed();
    qint64 now = timer.elapsed();
    float dt = (now - last) / 1000.0f;
    last = now;

    if (simulationSpeed <= 0.0f) return;

    for (auto& p : planets) {
        p.orbitAngle += p.orbitSpeed * simulationSpeed * dt;
        p.rotation += p.rotationSpeed * simulationSpeed * dt;
        if (p.orbitAngle > 360.0f) p.orbitAngle -= 360.0f;
        if (p.rotation > 360.0f) p.rotation -= 360.0f;
    }
}
