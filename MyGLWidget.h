// MyGLWidget.h
#include "BL2GLWidget.h"
#include "Model/model.h"
#include <math.h>

class MyGLWidget : public BL2GLWidget {
  Q_OBJECT

  public:
    MyGLWidget(QWidget *parent=0) : BL2GLWidget(parent) {}
    ~MyGLWidget();

    //virtual void resizeGL(int w, int h);
    virtual void initializeGL();
    virtual void carregaShaders();
    virtual void creaBuffers();
    virtual void paintGL();
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void modelTransformHomer();
    virtual void modelTransformBase();
    virtual void resizeGL(int amplada, int alçada);
    virtual void calcul_pmax_pmin();
    virtual void centre_escena();
    void radi_escena();
    float max(float p1, float p2);
    float min(float p1, float p2);


    virtual void pintaHomer();
    virtual void pintaBase();
    virtual void ini_camera();
    virtual void projectTransform(float FOV);
    virtual void viewTransform();

    //declarar variable para el modelo
    Model m;
    GLuint viewLoc;
    GLuint projLoc;
    GLuint VAO_Homer, VAO_Base;

    glm::vec3 PMAX, PMIN;
    glm::vec3 CENTRE;
    float radi, distancia_ps;


    float grados = 0.0f;
    glm::vec3 OBS;
    glm::vec3 VRP;
    glm::vec3 UP;
    float ra, znear, zfar, FOVopt;


  private:
    int printOglError(const char file[], int line, const char func[]);

};
