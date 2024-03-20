#include <fstream>
#include <iostream>
#include "ray.h"
#include "sphere.h"
#include "plane.h"
#include "geometricObj.h"
#include "camera.h"
#include "mesh.h"
#include "transformation.cpp"
#include "light.h"
#include <map>

typedef vector<vector<double>> Matrix;
using namespace std;

#define PHONG 0.5, 0.5, 0.2, 0.5, 0.5, 5.0
#define REF_INDEX 1.5
#define EPSILON 0.01

bool shadow(const ray &r, vector<geometricObj*> &objects, point3 lightOrigin, point3 intPoint) {


    for (long long unsigned int i = 0; i < objects.size(); i++)
    { 
        geometricObj *obj = objects[i];
        double t;

        t = obj->intersect(r);

        if (t > 0 && t < 1) {
            return true;
        }


    }

    return false;
}

vec3 color(const ray &r, vector<geometricObj *> &objects, map<int, Matrix> &transf, vector<light> &lights, point3 camOrigin, int rec)
{
    vector<pair<double, int>> ts;
    vec3 normal;
    vector<vec3> normals(objects.size());

    for (long long unsigned int i = 0; i < objects.size(); i++)
    {
        geometricObj *obj = objects[i];
        double t;

        if (transf.count(i) > 0) {
            ray r2 = ray(point_transformation(transf[i], r.origin()), vector_transformation(transf[i], r.direction()));
            t = obj->intersect(r2);
            normal = obj->intNormal(r2, t);

            vector<double> a(4);
            Matrix transp(4, a);
            Transpose(transf[i], transp);

            normal = vector_transformation(transp, normal);

        } else {
            t = obj->intersect(r);
            normal = obj->intNormal(r, t);
        }
        
        // a interseção só é válida se ocorrer depois do plano
        if (t > EPSILON) {
            ts.push_back({t, i});
            normals[i] = normal;
        }
    }

    sort(ts.begin(), ts.end());
    if (ts.empty())
    {
        return vec3(0, 0, 0);
    }

    geometricObj *objf = objects[ts[0].second];
    normal = normals[ts[0].second];

    vec3 objColor = objf->color;

    vec3 ambient = vec3(0,0,0);
    //vec3 ambient = objColor;

    point3 intPoint = r.origin() + ts[0].first*r.direction();
    double t = ts[0].first;

    normal.make_unit_vector();

    vec3 phongColor = ambient*objf->ka;

    vec3 V = camOrigin - intPoint;
    V.make_unit_vector();

    for (light l : lights) {
        vec3 L = l.origin - intPoint;
        if (shadow(ray(intPoint, L), objects, l.origin, intPoint)) continue;

        L.make_unit_vector();

        

        for (auto obj : objects) {
            if (obj == objf) continue;
            if (objf->intersect(ray(intPoint, L)) > EPSILON) {
                return vec3(0,0,0);
            }
        }
        
        vec3 R = 2*normal*(dot(normal, L)) - L;
        R.make_unit_vector();

        double cosDiffuse = dot(normal,L);

        if (cosDiffuse > EPSILON) {
            vec3 diffuseColor = l.color * objf->kd * cosDiffuse*vec3(objColor.r()/255, objColor.g()/255, objColor.b()/255);
            vec3 specularColor = l.color * objf->ks * pow(max(dot(R, V), 0.0),objf->n);
            phongColor += diffuseColor + specularColor;
            
        }
        

        
    }
        
    vec3 R2 = 2*normal*(dot(normal, V)) - V;
    R2.make_unit_vector();

    double n = 1/REF_INDEX;
    double d = dot(normal, V);
    vec3 T = (n*d - sqrt(1-n*n*(1-d*d)))*normal + n*V;
    T.make_unit_vector();

    if (rec < 4) {
        phongColor += objf->kr*color(ray(intPoint, R2), objects, transf, lights, camOrigin, rec+1);
        phongColor += objf->kt*color(ray(intPoint, T), objects, transf, lights, camOrigin, rec+1);
    }

    /*
    0 0 0
    1 0 0
    0 1 0
    1
    400
    400
    sphere
    5 0 0
    1.5
    255 0 0
    light
    5 5 0
    255 255 255
    end
    */

   /*
    0 0 0
    1 0 0
    0 1 0
    1
    400
    400
    sphere
    5 2 0
    1.5
    255 0 0
    light
    5 0 4
    255 255 255
    sphere
    5 -2 0
    1.5
    0 255 0
    light 
    5 0 -4
    255 255 255
    end
    */

   /*
    0 0 0
    1 0 0
    0 1 0
    1
    400
    400
    sphere
    5 2 0
    1.5
    255 0 0
    light
    0 0 0
    255 255 255
    sphere
    15 0 0
    1.5
    0 255 0
    end
    */

   /*
    0 0 0
    1 0 0
    0 1 0
    1
    400
    400
    sphere
    2 -1 0
    0.7
    0 0 0
    light
    2 0 0
    255 255 255
    plane
    2 -5 0
    0 1 0
    0 255 0
    end
    */

   /*
   -1 0 0
    1 0 0
    0 1 0
    1
    400 
    400
    sphere
    2 -1 0
    1
    0 0 0
    plane
    0 -1 0
    0 1 0
    0 255 0
    light
    0 10 0
    255 255 255
    end
   */

  /*
   -1 0 0
    1 0 0
    0 1 0
    1
    400 
    400
    sphere
    2 -1 0
    1
    0 0 0
    plane
    0 -1 0
    0 1 0
    0 255 0
    light
    2 3 3
    255 255 255
    end
   */

    /*
    0 0 0
    1 0 0
    0 1 0
    1 
    500
    500
    mesh
    2
    4
    5 0 0
    5 1 0
    5 0 1
    5 1 1
    0 1 2
    1 2 3
    255 0 0
    light
    5 5 0
    255 255 255
    5 2 
    end
    */

   /*
   0 0 0
   1 0 0
   0 1 0
   1
   500
   500
   plane
   0 0.5 0
   0 1 0
   0 0 255
   sphere
   2 0 1
   0.8
   255 0 0
   sphere
   2 0 -1
   0.8
   0 255 0
   light
   0 -1 -2
   255 255 255
   end
   
   */

  /*
    -1 0 0
    1 0 0
    0 1 0
    1
    400 
    400
    sphere
    5 0 0
    1
    0 0 0
    plane
    0 -1 0
    0 1 0
    0 255 0
    light
    5 3 0
    255 255 255
    end
  */

 /*
    -1 0 0
    1 0 0
    0 1 0
    1
    400 
    400
    sphere
    5 0 2
    1
    0 0 0
    sphere
    5 0 -2
    1
    0 0 0
    plane
    0 -1 0
    0 1 0
    0 255 0
    light
    5 3 0
    255 255 255
    end
  */



    phongColor = vec3(min(phongColor.r(), 255.0), min(phongColor.g(), 255.0), min(phongColor.b(), 255.0));

    return phongColor;
    //return objColor;
}

int main()
{
    ofstream fOut;
    fOut.open("test5.ppm");

    double x, y, z;

    point3 origin;
    point3 target;
    vec3 up;

    cout << "Origem da camera (3 doubles): \n";
    cin >> x >> y >> z;
    origin = point3(x, y, z);
    cout << "Alvo (3 doubles): \n";
    cin >> x >> y >> z;
    target = point3(x, y, z);
    cout << "Up vector (3 doubles): \n";
    cin >> x >> y >> z;
    up = vec3(x, y, z);

    Camera cam(origin, target, up);
    double dist;
    int resh;
    int resv;

    cout << "Distancia (1 double): \n";
    cin >> dist;
    cout << "Resolucao v (1 int): \n";
    cin >> resh;
    cout << "Resolucao h (1 int): \n";
    cin >> resv;

    double sizex = 1;
    double sizey = 1;
    vec3 qx = (2 * sizex / (resh - 1)) * cam.u;
    vec3 qy = (2 * sizey / (resv - 1)) * cam.v;
    vec3 bottomLeftCorner = dist * cam.w - sizex * cam.u - sizey * cam.v;

    vector<geometricObj *> objects;
    map<int, Matrix> transf;
    vector<light> lights;

    cout << "\ndigite end para gerar a imagem | plane para adicionar um plano | sphere para adicionar uma esfera | mesh para uma malha de triangulos | matrix para uma matriz de transformação afim | light para adicionar luz\n";
    while (true)
    {
        string type = "";
        double x, y, z;
        point3 p;
        double r;
        vec3 v1, v2;

        cout << "\nTipo de objeto: \n";
        cin >> type;

        if (type == "sphere")
        {
            cout << "Centro (3 doubles): \n";
            cin >> x >> y >> z;
            p = point3(x, y, z);
            cout << "Raio (1 double): \n";
            cin >> r;
            cout << "Cor (3 doubles): \n";
            cin >> x >> y >> z;
            v1 = vec3(x, y, z);
            sphere *sp = new sphere(p, r, v1, PHONG);
            objects.push_back(sp);
        }
        else if (type == "plane")
        {
            cout << "Ponto do plano (3 doubles): \n";
            cin >> x >> y >> z;
            p = point3(x, y, z);
            cout << "Vetor normal (3 doubles): \n";
            cin >> x >> y >> z;
            v1 = vec3(x, y, z);
            cout << "Cor (3 doubles): \n";
            cin >> x >> y >> z;
            v2 = vec3(x, y, z);
            plane *pl = new plane(p, v1, v2, PHONG);
            objects.push_back(pl);
        }
        else if (type == "mesh")
        {
            int numTriangles, numVertices;

            cout << "Quantidade de triangulos (1 int): \n";
            cin >> numTriangles;
            cout << "Quantidade de vertices (1 int): \n";
            cin >> numVertices;
            
            vector<point3> listVertices(numVertices);
            vector<vector<int>> listTriangles(numTriangles);

            for(int i=0; i<numVertices; i++)
            {
                double x, y, z;
                cout << "Vertice " << i << " (3 doubles): \n";
                cin >> x >> y >> z;
                listVertices[i] = point3(x, y, z);
            }

            for(int i=0; i<numTriangles; i++)
            {
                int a, b, c;
                cout << "Indices dos vertices do triangulo " << i << " (3 ints): \n";
                cin >> a >> b >> c;

                listTriangles[i].push_back(a);
                listTriangles[i].push_back(b);
                listTriangles[i].push_back(c);
            }

            int r, g, b;

            cout << "Cor da malha RGB (3 ints): \n";
            cin >> r >> g >> b;
            vec3 triangleColor = vec3(r, g, b);

            Mesh *malha = new Mesh(numTriangles, numVertices, listVertices, listTriangles, triangleColor, PHONG);
            objects.push_back(malha);
            
        }
        else if (type == "matrix") {
            cout << "Digite: 'especifica' para inserir matrix especifica; digite 'translacao', 'rotacao', 'escalar', para inserir uma dessas\n";
            string type; cin >> type;
            Matrix finalMatrix(4, vector<double>(4,0));

            if (type == "especifica") {
                vector<double> intermediateMatrix(16);
                vector<double> matrix(16);
                cout << "Insira a Matriz Direta 4x4 (16 doubles):\n";
                for (int i = 0; i < 16; i++) {
                    cin >> matrix[i];
                }

                if (!InvertMatrix(matrix, intermediateMatrix)) {
                    cout << "Matriz Invalida!\n";
                } else {
                    for (int i = 0; i < 4; i++) {
                        for (int j = 0; j < 4; j++) {
                            finalMatrix[i][j] = intermediateMatrix[i*4 + j];
                        }
                    }
            }
            } else if (type == "translacao") {
                cout << "Digite os parametros de translacao dx, dy e dz (3 doubles)\n";
                double dx, dy, dz;
                cin >> dx >> dy >> dz;
                finalMatrix = {{1,0,0,-dx},{0,1,0,-dy},{0,0,1,-dz},{0,0,0,1}};

            } else if (type == "rotacao") {
                cout << "Em qual eixo deseja rotacionar? Digite x, y ou z (string)\n";
                string axis; cin >> axis;
                cout << "Qual ângulo de rotacao? Digite em graus (double)\n";
                double degree; cin >> degree;
                double pi = 355.0/113.0;
                double rad = pi / 180.0;
                degree = degree*(rad);

                if (axis == "x") {
                    finalMatrix = {{1,0,0,0},{0,cos(degree),sin(degree),0},{0,-sin(degree),cos(degree),0},{0,0,0,1}};
                } else if (axis == "y") {
                    finalMatrix = {{cos(degree), 0, -sin(degree), 0},{0,1,0,0},{sin(degree),0,cos(degree),0},{0,0,0,1}};
                } else if (axis == "z") {
                    finalMatrix = {{cos(degree),sin(degree),0,0},{-sin(degree),cos(degree),0,0},{0,0,1,0},{0,0,0,1}};
                }
                
            } else if (type == "escalar") {
                cout << "Digite os parametros a,b,c (3 doubles)\n";
                double a,b,c;
                cin >> a >> b >> c;
                finalMatrix = {{1/a, 0, 0, 0},{0,1/b, 0, 0},{0,0,1/c, 0},{0,0,0,1}};

            }

                cout << "Para qual objeto voce deseja aplicar essa transformacao?\n";
                int inx;
                cin >> inx;
                if (transf.count(inx) > 0) {
                    transf[inx] = MatrixMultiplication(transf[inx], finalMatrix);
                } else
                transf[inx] = finalMatrix;
            
        }
        else if (type == "light") {
            cout << "Digite a origem da luz (ponto, 3 doubles)\n";
            double x, y, z; cin >> x >> y >> z;
            point3 lightOrigin = point3(x,y,z);

            cout << "Digite a cor da luz (3 doubles)\n";
            cin >> x >> y >> z;
            vec3 lightColor = vec3(x,y,z);

            light l = light(lightOrigin, lightColor);
            lights.push_back(l);

        }
        else if (type == "end")
        {
            break;
        }
    }

    fOut << "P3\n";
    fOut << resh << " " << resv << "\n255\n";

    for (int y = resv - 1; y >= 0; y--)
    {
        for (int x = 0; x < resh; x++)
        {
            ray r(origin, bottomLeftCorner + (x * qx) + (y * qy));

            vec3 col = color(r, objects, transf, lights, origin, 0);
            int ir = int(col.r());
            int ig = int(col.g());
            int ib = int(col.b());
            fOut << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
    return 0;
}