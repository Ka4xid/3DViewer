#include "myglwidget.h"
#include <QKeyEvent>
#include <QDebug>
#include <QTimer>
#include <qmath.h>

#define ANGLE_STEP 4
#define MOVE_STEP 10
#define RADIAN 0.0174533   // magic number for translate degrees to radians
#define DEGREE 57.2958

#define TIMER_BASED



MyGLWidget::MyGLWidget()
{

#ifdef TIMER_BASED
    QTimer *timer = new QTimer(this);
    timer->setSingleShot(0);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()) );
    timer->start(40);
#endif

    // SET UP OPENGL SPECIFIC VALUES
    QGLFormat f;
    f.setDoubleBuffer(true);
    f.setSampleBuffers(false);
    f.setStencil(false);
    f.setStereo(false);
    f.setSwapInterval(0);
    this->setFormat(f);
    this->setMouseTracking(true);



    // SET UP INITIAL VALUES

    wireframeMode = false;
    colorModel = 0;
    freeCamera = true;

    camera_angle_pitch = 0;
    camera_angle_yaw = 0;
    camera_angle_roll = 0;

    camera_translate_x = 0;
    camera_translate_y = 0;
    camera_translate_z = 10;//50000;
}

void MyGLWidget::initializeGL()
{
    glViewport(0, 0, this->width(), this->height());                // setup viewport

    glEnable(GL_DEPTH_TEST);                        // enable Z-buffering
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);
    glEnable(GL_NORMALIZE);
    //glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);                    // switch matrix to PROJECTION

    SetPerspective(50, 1, 100000, ((float)(this->width())/(float)(this->height())) );     // setup frustum

    glClearColor(0.3, 0.3, 0.3, 1.);                // clear background with color

    initializeGLFunctions();
}

void MyGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    //updateGL();
}

void MyGLWidget::handleKeys() {

    // MOVEMENT
    if (freeCamera) {

        float forward = keyMap.value(Qt::Key_W, 0) - keyMap.value(Qt::Key_S, 0);
        float strafe = keyMap.value(Qt::Key_A, 0) - keyMap.value(Qt::Key_D, 0);
        float Shift = keyMap.value(Qt::Key_Shift, 0);


        camera_translate_x -= ( ( -forward * cos(camera_angle_yaw * RADIAN)
                                  * cos( (camera_angle_pitch-90) * RADIAN) )
                                  - strafe * cos((camera_angle_yaw+90) * RADIAN) )
                                  * MOVE_STEP
                                  * (1 + 10 * Shift);

        camera_translate_y += ( ( -forward * sin(camera_angle_yaw * RADIAN)
                                  * cos( (camera_angle_pitch-90) * RADIAN) )
                                  - strafe * sin((camera_angle_yaw+90) * RADIAN) )
                                  * MOVE_STEP
                                  * (1 + 10 * Shift);

        camera_translate_z += ( -forward * cos(camera_angle_pitch * RADIAN)
                                * MOVE_STEP )
                                * (1 + 10 * Shift);
    }

#ifndef TIMER_BASED
    updateGL();         // redraw scene if key pressed
#endif

}



void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear depth and color buffer

    handleKeys();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //glScalef(1, -1, -1);            // Inverse Y and Z axes
    glRotatef(-90, 0, 0, 1);        // Rotate system around X and Y axes (switch X and Y places)


    // CROSSHAIR
    glPushMatrix();
        glTranslatef(0, 0, -1);
        RenderCrosshair(0.02);
    glPopMatrix();
    //


    // NAVIGATION COMPASS
    glPushMatrix();
        glTranslatef(0.60, 0,  -2);
        glRotatef(camera_angle_pitch,   0, 1, 0);
        glRotatef(camera_angle_yaw,     0, 0, 1);
        RenderGizmo(0.15);
    glPopMatrix();
    //


    // CAMERA TRANSFORMATIONS
    glRotatef(camera_angle_pitch, 0, 1, 0);                             // pitch is now on Y axis! (instead of X axis by default)
    glRotatef(camera_angle_yaw,   0, 0, 1);
    glTranslatef(-camera_translate_x, -camera_translate_y, -camera_translate_z);
                                                                        // sign flipped because:
                                                                        // objects translated to X points to right =>
                                                                        // to follow it we need to
                                                                        // translate it X points back to the left <=
    glPushMatrix();
    RenderGizmo(30);
    glPopMatrix();



    // RENDER OBJECTS
    //RenderOcta(5);
    glPushMatrix();
        qglColor(Qt::white);
        RenderObjectsArray();
    glPopMatrix();
    //

    GLenum error;
    error = glGetError();
    if (error != 0) {
        qDebug() << "OpenGL Error:" << error;
    }

    swapBuffers();
}



void MyGLWidget::RenderObjectsArray() {

    foreach (Object_class* currentObject, ObjectsArray) {

        // OBJECT TRANSFORMATIONS
        glPushMatrix();                     //individual matrix

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);


        glEnable(GL_TEXTURE_2D);

        currentObject->Draw();

        glDisable(GL_TEXTURE_2D);

        glPopMatrix();
    }
}



void MyGLWidget::RenderOcta(float m) {
    //OCTAHEDRON

    glBegin(GL_TRIANGLES);

        qglColor(Qt::red);

        glVertex3f(0,-1*m,0);
        glVertex3f(1*m,0,0);
        glVertex3f(0,0,1*m);

        qglColor(Qt::blue);

        glVertex3f(-1*m,0,0);
        glVertex3f(0,-1*m,0);
        glVertex3f(0,0,1*m);

        qglColor(Qt::red);

        glVertex3f(0,1*m,0);
        glVertex3f(-1*m,0,0);
        glVertex3f(0,0,1*m);

        qglColor(Qt::blue);

        glVertex3f(1*m,0,0);
        glVertex3f(0,1*m,0);
        glVertex3f(0,0,1*m);

        qglColor(Qt::green);

        glVertex3f(1*m,0,0);
        glVertex3f(0,-1*m,0);
        glVertex3f(0,0,-1*m);

        qglColor(Qt::red);

        glVertex3f(0,-1*m,0);
        glVertex3f(-1*m,0,0);
        glVertex3f(0,0,-1*m);

        qglColor(Qt::green);

        glVertex3f(-1*m,0,0);
        glVertex3f(0,1*m,0);
        glVertex3f(0,0,-1*m);

        qglColor(Qt::red);

        glVertex3f(0,1*m,0);
        glVertex3f(1*m,0,0);
        glVertex3f(0,0,-1*m);

    glEnd();
}
void MyGLWidget::RenderGizmo(float m) {
    //COMPASS

    glScalef(m,m,m);

    glBegin(GL_LINES);

        qglColor(Qt::red);      // X
        glVertex3f(0,0,0);
        glVertex3f(1,0,0);

        qglColor(Qt::green);    // Y
        glVertex3f(0,0,0);
        glVertex3f(0,1,0);

        qglColor(Qt::blue);     // Z
        glVertex3f(0,0,0);
        glVertex3f(0,0,1);

    glEnd();
}
void MyGLWidget::RenderCrosshair(float m) {
    //CROSSHAIR

    glScalef(m,m,m);

    glBegin(GL_LINES);

        qglColor(Qt::red);
        glVertex3f(1,0,0);
        glVertex3f(-1,0,0);

        glVertex3f(0,1,0);
        glVertex3f(0,-1,0);

    glEnd();
}



void MyGLWidget::SetPerspective(float fov, float Znear, float Zfar, float aspect)
{

    float radians_fov,
          tangent,
          VPlane,
          HPlane;

    radians_fov = tan( fov * 3.1415 / 180);
    radians_fov = 2. * atan( tan( radians_fov * 0.5 ) / aspect ) ;
    tangent = tan(radians_fov / 2);
    VPlane = Znear * tangent;
    HPlane = VPlane * aspect;

    glFrustum(-HPlane, HPlane, VPlane, -VPlane, Znear, Zfar);
}

void MyGLWidget::SetCameraPosition(float x, float y, float z)
{
    camera_translate_x = x;
    camera_translate_y = y;
    camera_translate_z = z;
}

void MyGLWidget::keyPressEvent(QKeyEvent *e)
{
    keyMap.insert(e->key(), 1);
}

void MyGLWidget::keyReleaseEvent(QKeyEvent *e)
{
    if ( e->key() == Qt::Key_F1 ) {
        wireframeMode = !wireframeMode;
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        return;
    }
    if (  e->key() == Qt::Key_F2 ) {
        colorModel+=1;
        if (colorModel >1 ) colorModel = 0;
        return;
    }
    if (  e->key() == Qt::Key_F3 ) {
        freeCamera = !freeCamera;
        return;
    }

    if (  e->key() == Qt::Key_F4 ) {
        camera_angle_pitch = 0;
        camera_angle_yaw = 0;
        camera_angle_roll = 0;
        return;
    }
    keyMap.remove(e->key());
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{

    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        if ( (camera_angle_pitch >= 0) && (camera_angle_pitch <= 180) ) {
            camera_angle_pitch -= dy*0.2;
            camera_angle_pitch > 180 ? camera_angle_pitch = 180 : 0;
            camera_angle_pitch < 0 ? camera_angle_pitch = 0 : 0;
        }
        camera_angle_yaw -= dx*0.2;
        camera_angle_yaw > 360 ? camera_angle_yaw = 0 : 0;
        camera_angle_yaw < 0 ? camera_angle_yaw = 360 : 0;
    }

    if (event->buttons() & Qt::RightButton) {

    }

    lastPos = event->pos();

#ifndef TIMER_BASED
    updateGL();         // redraw scene if key pressed
#endif

}


