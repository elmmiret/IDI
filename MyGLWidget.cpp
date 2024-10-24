// MyGLWidget.cpp
#include "MyGLWidget.h"
#include <iostream>
#include <stdio.h>

#define printOpenGLError() printOglError(__FILE__, __LINE__)
#define CHECK() printOglError(__FILE__, __LINE__,__FUNCTION__)
#define DEBUG() std::cout << __FILE__ << " " << __LINE__ << " " << __FUNCTION__ << std::endl;

int MyGLWidget::printOglError(const char file[], int line, const char func[]) 
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    const char * error = 0;
    switch (glErr)
    {
        case 0x0500:
            error = "GL_INVALID_ENUM";
            break;
        case 0x501:
            error = "GL_INVALID_VALUE";
            break;
        case 0x502: 
            error = "GL_INVALID_OPERATION";
            break;
        case 0x503:
            error = "GL_STACK_OVERFLOW";
            break;
        case 0x504:
            error = "GL_STACK_UNDERFLOW";
            break;
        case 0x505:
            error = "GL_OUT_OF_MEMORY";
            break;
        default:
            error = "unknown error!";
    }
    if (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s function: %s\n",
                             file, line, error, func);
        retCode = 1;
    }
    return retCode;
}

MyGLWidget::~MyGLWidget() {
}

//--------------------------------------------

void MyGLWidget::initializeGL (){
    BL2GLWidget::initializeGL();
    glEnable(GL_DEPTH_TEST);
    ini_camera();

}

void MyGLWidget::ini_camera(){
    calcul_pmax_pmin();
    radi_escena();

    /*
        PERSPECTIVA = angle (FOV/2), ra, znear, zfar
        ORTOGONAL = left, right, top, bottom, znear, zfar
    */

    OBS = glm::vec3(0,0,3);
    //VRP = glm::vec3(0,0,0);
    VRP = CENTRE;
    UP = glm::vec3(0,1,0);
    //FOV = float(M_PI/2);
    FOVopt = float(2*asin(radi/distancia_ps));
    //ra = 1.0f;
    //znear = 0.1f;
    znear = float(PMAX[2]);
    //zfar = 10.0f;
    zfar = float(PMIN[2]);

    projectTransform(FOVopt);
    viewTransform();
}

void MyGLWidget::paintGL(){
    // Esborrem el frame-buffer
    glClear (GL_COLOR_BUFFER_BIT);

    // pintar el homer
    pintaHomer();

    // pintar la base
    pintaBase();
}

void MyGLWidget::pintaHomer(){
    //pintem el homer
    modelTransformHomer();
    glBindVertexArray(VAO_Homer);
    glDrawArrays(GL_TRIANGLES, 0, m.faces().size()*3);
    glBindVertexArray (0);
}

void MyGLWidget::modelTransformHomer(){
    // Matriu de transformació del homer
    glm::mat4 transform(1.0f);
    transform = glm::rotate(transform, glm::radians(grados), glm::vec3(0,1,0));
    transform = glm::scale(transform, glm::vec3(escala));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

void MyGLWidget::pintaBase(){
    //pintem la base
    modelTransformBase();
    glBindVertexArray(VAO_Base);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray (0);
}

void MyGLWidget::modelTransformBase(){
    // Matriu de transformació de la base
    glm::mat4 transform(1.0f);
    transform = glm::scale(transform, glm::vec3(escala));
    transform = glm::translate(transform, glm::vec3(0, -1, 0));
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);
}

// CALCULA LA TRANSFORMACIO DE PROJECCIO I ENVIA EL UNIFORM AMB LA MATRIU
// CAP AL VERTEX SHADER (parametres han de ser floats)

void MyGLWidget::projectTransform (float FOV){
    glm::mat4 Proj = glm::perspective (FOV, ra, znear, zfar);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
}

// CALCULA LA TRANSFORMACIO DE PUNT DE VISTA (VIEW) I ENVIA EL UNIFORM AMB
// LA MATRIU CAP AL VERTEX SHADER

void MyGLWidget::viewTransform (){
    glm::mat4 View = glm::lookAt(OBS, VRP, UP);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &View[0][0]);
}

//crea los buffers para el modeloglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

void MyGLWidget::creaBuffers (){
    m.load("./models/HomerProves.obj");
    // creacio del vertex array object per pintar
    glGenVertexArrays(1, &VAO_Homer);
    glBindVertexArray(VAO_Homer);

    GLuint VBO_Homer[2];
    glGenBuffers(2, VBO_Homer);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_vertices(), GL_STATIC_DRAW);

    // activar atribut de vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Homer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m.faces().size()*3*3, m.VBO_matdiff(), GL_STATIC_DRAW);

    // activar atribut de colorLoc
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    glBindVertexArray(0);

    //---------------------------------------------------------------------------

    glm::vec3 Vertices[6];
    Vertices[0] = glm::vec3(-2.0, 0.0, -2.0);
    Vertices[1] = glm::vec3(2.0, 0.0, -2.0);
    Vertices[2] = glm::vec3(-2.0, 0.0, 2.0);
    Vertices[3] = glm::vec3(2.0, 0.0, 2.0);
    Vertices[4] = glm::vec3(-2.0, 0.0, 2.0);
    Vertices[5] = glm::vec3(2.0, 0.0, -2.0);

    // creacio del VAO que usarem per pintar
    glGenVertexArrays(1, &VAO_Base);
    glBindVertexArray(VAO_Base);

    // creacio dels buffers de vertexs i colors
    GLuint VBO_Base;
    glGenBuffers(1, &VBO_Base);

    // buffer vertexs
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Base);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    // activar atribut de vertexLoc
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexLoc);

    glm::vec3 Colores[6];
    Colores[0] = glm::vec3(1.0, 0.0, 1.0);
    Colores[1] = glm::vec3(1.0, 0.0, 1.0);
    Colores[2] = glm::vec3(1.0, 0.0, 1.0);
    Colores[3] = glm::vec3(1.0, 0.0, 1.0);
    Colores[4] = glm::vec3(1.0, 0.0, 1.0);
    Colores[5] = glm::vec3(1.0, 0.0, 1.0);

    // buffer colors
    GLuint VBO_color_terra;
    glGenBuffers(1, &VBO_color_terra);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_color_terra);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colores), Colores, GL_STATIC_DRAW);

    // activar atribut de colorLoc
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(colorLoc);

    glBindVertexArray (0);
}

float MyGLWidget::max(float p1, float p2){
    if(p1 > p2) return p1;
    return p2;
}

float MyGLWidget::min(float p1, float p2){
    if(p1 > p2) return p2;
    return p1;
}   

void MyGLWidget::calcul_pmax_pmin(){
    for(unsigned int i = 0; i < m.faces().size()/*numero de vertices*/; i+=3/*de tres en tres*/){
        //calcul del punt maxim i minim
        if(i == 0)
        {
            //inicializar
            PMAX[0] = m.vertices()[0];
            PMAX[1] = m.vertices()[1];
            PMAX[2] = m.vertices()[2];
            PMIN[0] = m.vertices()[0];
            PMIN[1] = m.vertices()[1];
            PMIN[2] = m.vertices()[2];

        }
        else
        {
            //comparar
            PMAX[0] = max(PMAX[0], m.vertices()[i]);
            PMAX[1] = max(PMAX[1], m.vertices()[i+1]);
            PMAX[2] = max(PMAX[2], m.vertices()[i+2]);
            PMIN[0] = min(PMIN[0], m.vertices()[i]);
            PMIN[1] = min(PMIN[1], m.vertices()[i+1]);
            PMIN[2] = min(PMIN[2], m.vertices()[i+2]);
        }
    }
    //recalcular el centre de l'escena cada vegada que els punts maxims i minims variin
    centre_escena();
}

void MyGLWidget::centre_escena(){
    CENTRE[0] = (PMAX[0] + PMIN[0]) / 2;
    CENTRE[1] = (PMAX[1] + PMIN[1]) / 2;
    CENTRE[2] = (PMAX[2] + PMIN[2]) / 2;
}

void MyGLWidget::radi_escena(){
    // distancia entre els punts
    float xSqr = (PMAX[0] - PMIN[0]) * (PMAX[0] - PMIN[0]);
    float ySqr = (PMAX[1] - PMIN[1]) * (PMAX[1] - PMIN[1]);
    float zSqr = (PMAX[2] - PMIN[2]) * (PMAX[2] - PMIN[2]);

    float mySqr = xSqr + ySqr + zSqr;

    distancia_ps = sqrt(mySqr);

    // radi
    radi = distancia_ps / 2;

}

void MyGLWidget::resizeGL(int amplada, int alçada){
    // para tener las variables ample i alt
    BL2GLWidget::resizeGL(amplada, alçada);
    ra = float(ample) / float(alt);
    
    //cuando es mas ancho que alto
    if(ra > 1)
    {
        glm::mat4 Proj = glm::perspective (FOVopt, ra, znear, zfar);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
    }

    //cuando es mas alto que ancho
    else if(ra < 1)
    {
        float FOV = 2*atanl(tan(FOVopt/2)/ra);
        glm::mat4 Proj = glm::perspective (FOV, ra, znear, zfar);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &Proj[0][0]);
    }

}

//SOBREESCRIBIR FUNCIONES EN BL2GLWidget.cpp PARA AÑADIR LOCALIZADORES

void MyGLWidget::carregaShaders (){
    BL2GLWidget::carregaShaders();
    projLoc = glGetUniformLocation(program->programId(), "PRJ");
    viewLoc = glGetUniformLocation(program->programId(), "VM");
    
}

void MyGLWidget::keyPressEvent (QKeyEvent *event){
    makeCurrent();
  switch (event->key()) {
    case Qt::Key_S :
        escala += 0.05;
        break;

    case Qt::Key_D :
        escala -= 0.05;
        break;

    case Qt::Key_R : // escalar a més gran
      grados += 45.0f;
      break;

    case Qt::Key_E : // escalar a més petit
        grados -= 45.0f;
        break;
    
    default: event->ignore(); break;
  }
  update();
}


//hola
