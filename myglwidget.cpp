#include "myglwidget.h"
#include <QKeyEvent>
#include <QDebug>
#include <QTimer>
#include <qmath.h>

#define ANGLE_STEP 4
#define MOVE_STEP 100
#define RADIAN 0.0174533   // magic number for translate degrees to radians
#define DEGREE 57.2958

#define TIMER_BASED
//#define USE_CROSSHAIR
//#define USE_GIZMO



MyGLWidget::MyGLWidget()
{

#ifdef TIMER_BASED
    QTimer *timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()) );
    timer->start(40);
#endif

    // SET UP OPENGL SPECIFIC VALUES
    this->setMouseTracking(true);

    // SET UP INITIAL VALUES
    wireframeMode = false;
    freeCamera = false;

    distaceFromCamera = 200;

    free_camera_pos.setZ(10);
}

void MyGLWidget::initializeGL()
{
    glViewport(0, 0, this->width(), this->height());                // setup viewport

    glEnable(GL_DEPTH_TEST);                        // enable Z-buffering
    //glEnable(GL_CULL_FACE);                       // backface culling (no much point, eats FPS in wireframe mode)


    glMatrixMode(GL_PROJECTION);                    // switch matrix to PROJECTION
    SetPerspective(50, 1, 100000,
                   ((float)(this->width()) /
                    (float)(this->height())) );     // setup frustum


    glClearColor(0.7, 0.7, 0.7, 1.);                // clear background with color

    initializeGLFunctions();
}

void MyGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

void MyGLWidget::handleKeys() {

    // MOVEMENT
    if (freeCamera) {

        float x = free_camera_pos.x();
        float y = free_camera_pos.y();
        float z = free_camera_pos.z();

        float forward = keyMap.value(Qt::Key_W, 0) - keyMap.value(Qt::Key_S, 0);
        float strafe = keyMap.value(Qt::Key_A, 0) - keyMap.value(Qt::Key_D, 0);
        float Shift = keyMap.value(Qt::Key_Shift, 0);

        x -= ( ( -forward * cos(camera_angles.z() * RADIAN)
              * cos( (camera_angles.y() - 90) * RADIAN) )
              - strafe * cos((camera_angles.z() + 90) * RADIAN) )
              * MOVE_STEP
              * (1 + 10 * Shift);

        y += ( ( -forward * sin(camera_angles.z() * RADIAN)
              * cos( (camera_angles.y() - 90) * RADIAN) )
              - strafe * sin((camera_angles.z() + 90) * RADIAN) )
              * MOVE_STEP
              * (1 + 10 * Shift);

        z += ( -forward * cos(camera_angles.y() * RADIAN)
              * MOVE_STEP )
              * (1 + 10 * Shift);

        free_camera_pos.setX(x);
        free_camera_pos.setY(y);
        free_camera_pos.setZ(z);

    } else {

        float north = keyMap.value(Qt::Key_Up, 0) - keyMap.value(Qt::Key_Down, 0);
        float east = keyMap.value(Qt::Key_Right, 0) - keyMap.value(Qt::Key_Left, 0);

        orb_camera_pos.setX( orb_camera_pos.x() + north * MOVE_STEP);
        orb_camera_pos.setY( orb_camera_pos.y() + east * MOVE_STEP);

    }

#ifndef TIMER_BASED
    updateGL();         // redraw scene if key pressed
#endif

}



void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear depth and color buffer

    // handle key presses
    handleKeys();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(-90, 0, 0, 1);        // Rotate system around X and Y axes (switch X and Y places)


#ifdef USE_CROSSHAIR
    glPushMatrix();
        glTranslatef(0, 0, -1);
        RenderCrosshair(0.02);
    glPopMatrix();
#endif

#ifdef USE_GIZMO
    glPushMatrix();
        glTranslatef(0.60, 0,  -2);
        glRotatef(camera_angles.y,   0, 1, 0);
        glRotatef(camera_angle_yaw,     0, 0, 1);
        RenderGizmo(0.15);
    glPopMatrix();
#endif


    // CAMERA TRANSFORMATIONS
    if (freeCamera) {
        glRotatef(camera_angles.y(), 0, 1, 0);          // pitch is now on Y axis! (instead of X axis by default)
        glRotatef(camera_angles.z(),   0, 0, 1);
        glTranslatef(-(free_camera_pos.x()), -(free_camera_pos.y()), -(free_camera_pos.z()));

        // RENDER OBJECTS
        glPushMatrix();
            qglColor(Qt::white);
            RenderObjectsArray();
        glPopMatrix();
        //


    } else {

        glTranslatef(0, 0, -distaceFromCamera);

        glRotatef(camera_angles.y(), 0, 1, 0);          // pitch is now on Y axis! (instead of X axis by default)
        glRotatef(camera_angles.z(),   0, 0, 1);

        // center of camera orbit
        glPushMatrix();
        RenderGizmo(30);
        glPopMatrix();

        glTranslatef(-orb_camera_pos.x(), -orb_camera_pos.y(), -orb_camera_pos.z());


        // RENDER OBJECTS
        glPushMatrix();
            qglColor(Qt::white);
            RenderObjectsArray();
        glPopMatrix();
    }



    // Fetch OpenGL errors if any present
    GLenum error = glGetError();
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
    free_camera_pos.setX(x);
    free_camera_pos.setY(y);
    free_camera_pos.setZ(z);

    orb_camera_pos.setX(x);
    orb_camera_pos.setY(y);
    orb_camera_pos.setZ(z);
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
    if ( e->key() == Qt::Key_F2 ) {
        colorModel+=1;
        if (colorModel >1 ) colorModel = 0;
        return;
    }
    if ( e->key() == Qt::Key_F3 ) {
        freeCamera = !freeCamera;
        return;
    }
    if ( e->key() == Qt::Key_F4 ) {
        camera_angles = QVector3D();
        return;
    }
    keyMap.remove(e->key());
}

void MyGLWidget::wheelEvent(QWheelEvent *e)
{
    if ( (e->delta() > 0) && (distaceFromCamera >= 0) ) {
        distaceFromCamera -= 1 + distaceFromCamera/2;
        distaceFromCamera < 0 ? distaceFromCamera=0 : 0;
    }
    if (e->delta() < 0) {
        distaceFromCamera += 1 + distaceFromCamera/2;
    }
    qDebug() <<distaceFromCamera;
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *e)
{

    int dx = e->x() - lastPos.x();
    int dy = e->y() - lastPos.y();

    if (e->buttons() & Qt::LeftButton) {
        if ( (camera_angles.y() >= 0) && (camera_angles.y() <= 180) ) {
            camera_angles.setY( camera_angles.y() - dy*0.2 );
            if (camera_angles.y() > 180)
                camera_angles.setY(180);

            if (camera_angles.y() < 0)
                camera_angles.setY(0);
        }
        camera_angles.setZ( camera_angles.z() - dx*0.2 );
        if (camera_angles.z() > 360)
            camera_angles.setZ(0);
        if (camera_angles.z() < 0)
            camera_angles.setZ(360);
    }

    if (e->buttons() & Qt::MiddleButton) {
        if (distaceFromCamera >= 0) {
            distaceFromCamera += dy/2;
            distaceFromCamera<0 ? distaceFromCamera=0 : 0;
        }
    }

    lastPos = e->pos();

#ifndef TIMER_BASED
    updateGL();         // redraw scene if key pressed
#endif

}


