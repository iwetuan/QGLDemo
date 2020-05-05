#include "paintingwidget.h"

PaintingWidget::PaintingWidget(QWidget* parent):
    QOpenGLWidget (parent), camera_pos(0.0f, 3.0f, 0.0f),
    m_vbo(nullptr), m_vao(nullptr), m_shader(nullptr), m_timer(nullptr), m_texture(nullptr){
    const GLfloat VERTEX_INIT_DATA[] = {
           //face 1
           -0.5f, 0.0f, -0.2887f,
           0.5f, 0.0f, -0.2887f,
           0.0f, 0.0f, 0.5774f,
           //face 2
           -0.5f, 0.0f, -0.2887f,
           0.5f, 0.0f, -0.2887f,
           0.0f, 0.8165f, 0.0f,
           //face 3
           -0.5f, 0.0f, -0.2887f,
           0.0f, 0.0f, 0.5774f,
           0.0f, 0.8165f, 0.0f,
           //face 4
           0.5f, 0.0f, -0.2887f,
           0.0f, 0.0f, 0.5774f,
           0.0f, 0.8165f, 0.0f,
       };
    const GLfloat COLOR_INIT_DATA[] = {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
       };
    const GLfloat UV_INIT_DATA[] = {
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
       };
    memcpy(this->vertexData, VERTEX_INIT_DATA, sizeof(this->vertexData));
    memcpy(this->colorBuffer, COLOR_INIT_DATA, sizeof(this->colorBuffer));
    memcpy(this->uvData, UV_INIT_DATA, sizeof(this->uvData));
    aspectRatio = (float) 800 / 600;
    m_timer = new QElapsedTimer;
    m_timer->start();
    setFocusPolicy(Qt::StrongFocus);
}
PaintingWidget::~PaintingWidget(){

}


void PaintingWidget::initializeGL()
{
    QOpenGLFunctions *f = this->context()->functions();
    f->glEnable(GL_DEPTH_TEST);
    m_texture = new QOpenGLTexture(QImage(":/imgs/lenna.png"));
    m_shader = new QOpenGLShaderProgram();
    m_shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertexShader.shader");
    m_shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragmentShader.shader");
    if (m_shader->link()) {
        qDebug("Shaders link success.");
    } else {
        qDebug("Shaders link failed!");
    }
    m_vao = new QOpenGLVertexArrayObject();
    m_vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_cbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_uvbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    m_vao->create();
    m_vao->bind();
    m_vbo->create();
    m_vbo->bind();
    m_vbo->allocate(this->vertexData, 4 * 3 * 3 * sizeof(GLfloat));
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,3*sizeof(GLfloat), 0);
    m_vbo->release();
    m_cbo->create();
    m_cbo->bind();
    m_cbo->allocate(this->colorBuffer, 4*3*3*sizeof(GLfloat));
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);
    m_cbo->release();
    m_uvbo->create();
    m_uvbo->bind();
    m_uvbo->allocate(this->uvData, 4 * 3 * 2 * sizeof(GLfloat));
    f->glEnableVertexAttribArray(2);
    f->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);
    m_uvbo->release();
    m_vao->release();
}

void PaintingWidget::paintGL()
{
    QOpenGLFunctions *f = this->context()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    f->glClearColor(0.0f, 0.2f, 0.0f, 1.0f);
    m_vao->bind();
    m_shader->bind();
    m_texture->bind();
    QMatrix4x4 mvp;
    mvp.perspective(45.0f, this->aspectRatio, 0.1f, 100.0f);
    QVector3D center(camera_pos);
    center.setY(0);
    mvp.lookAt(camera_pos, center, QVector3D(1.0f, 0.0f, 0.0f));

    float time_in_second = (float)m_timer->elapsed() / 1000;
    mvp.rotate(30.0f * time_in_second, QVector3D(0.7f, 0.5f, 0.2f));

    m_shader->setUniformValue(m_shader->uniformLocation("MVP"), mvp);
    f->glDrawArrays(GL_TRIANGLES, 0, 4 * 3);
    m_texture->release();
    m_shader->release();
    m_vao->release();
    this->update();
}

void PaintingWidget::resizeGL(int w, int h)
{
    aspectRatio = (float)w / h;
}

void PaintingWidget::keyPressEvent(QKeyEvent *keyEvent){
    switch (keyEvent->key()){
        case Qt::Key_Right:
            camera_pos.setZ(camera_pos.z() + 0.1f);
            break;
        case Qt::Key_Left:
            camera_pos.setZ(camera_pos.z() - 0.1f);
            break;
        case Qt::Key_Up:
            camera_pos.setX(camera_pos.x() + 0.1f);
            break;
        case Qt::Key_Down:
            camera_pos.setX(camera_pos.x() - 0.1f);
            break;
        case Qt::Key_Plus:
            camera_pos.setY(camera_pos.y() + 0.1f);
            break;
        case Qt::Key_Minus:
            camera_pos.setY(camera_pos.y() - 0.1f);
            break;
    }
    update();
}
