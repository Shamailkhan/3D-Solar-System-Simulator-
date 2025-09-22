#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMatrix4x4>
#include <QElapsedTimer>
#include <QPoint>
#include <vector>

struct Planet {
    QString name;
    float orbitRadius;     // distance from Sun
    float orbitAngle = 0;  // updated over time
    float orbitSpeed;      // deg/sec
    float rotation = 0;    // self rotation
    float rotationSpeed;   // deg/sec
    float size;            // scale
    QVector3D color;       // planet color
};

class SolarSystemWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT
public:
    explicit SolarSystemWidget(QWidget* parent = nullptr);
    ~SolarSystemWidget() override;

public slots:
    void setSimulationSpeedPercent(int percent);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    bool compileShaders();
    GLuint compileSingleShader(GLenum type, const char* src);
    void generateSphereMesh(int latBands, int longBands);
    void createBuffers();
    void updateAnimation();

    // GL objects
    GLuint shaderProgram = 0;
    GLuint vao = 0, vbo = 0, ebo = 0;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Uniforms
    GLint uniModel = -1, uniView = -1, uniProj = -1, uniColor = -1;

    // Camera
    QMatrix4x4 proj;
    float cameraDistance = 25.0f;
    float pitch = 20.0f, yaw = -30.0f;
    QPoint lastMousePos;

    // Timing
    QElapsedTimer timer;
    float simulationSpeed = 1.0f;

    // Planets
    std::vector<Planet> planets;

    // Sphere resolution
    int latBands = 28, longBands = 28;
};
