// Source file for the OFF file viewer



////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////
#include <cctype>
#include "ogles.h"
#include "uglu.h"

////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
////////////////////////////////////////////////////////////
typedef struct Vertex
{
    float x, y, z;
} Vertex;

typedef struct Face
{
    Face(void) : nverts(0), verts(0) {};
    int nverts;
    Vertex **verts;
    float normal[3];
} Face;

typedef struct Mesh
{
    Mesh(void) : nverts(0), verts(0), nfaces(0), faces(0) {};
    int nverts;
    Vertex *verts;
    int nfaces;
    Face *faces;
} Mesh;




////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
////////////////////////////////////////////////////////////


// GLUT variables

static int GLUTwindow = 0;
static int GLUTwindow_height = 800;
static int GLUTwindow_width = 800;
static int GLUTmouse[2] = { 0, 0 };
static int GLUTbutton[3] = { 0, 0, 0 };
static int GLUTarrows[4] = { 0, 0, 0, 0 };
static int GLUTmodifiers = 0;



// Display variables

static int scaling = 0;
static int translating = 0;
static int rotating = 0;
static float scale = 0.0;
static float center[3] = { 0.0, 0.0, 0.0 };
static float rotation[3] = { 0.0, 0.0, 0.0 };
static float translation[3] = { 0.0, 0.0, -2.0 }; //for view03


// Mesh variables

static Mesh *mesh = NULL;



////////////////////////////////////////////////////////////
// OFF FILE READING CODE
////////////////////////////////////////////////////////////
Mesh * ReadOffFile(const char *filename)
{
    int i;

    // Open file
    FILE *fp;
    if (!(fp = fopen(filename, "r")))
    {
        fprintf(stderr, "Unable to open file %s\n", filename);
        return 0;
    }

    // Allocate mesh structure
    Mesh *mesh = new Mesh();
    if (!mesh)
    {
        fprintf(stderr, "Unable to allocate memory for file %s\n", filename);
        fclose(fp);
        return 0;
    }

    // Read file
    int nverts = 0;
    int nfaces = 0;
    int nedges = 0;
    int line_count = 0;
    char buffer[1024];
    while (fgets(buffer, 1023, fp))
    {
        // Increment line counter
        line_count++;

        // Skip white space
        char *bufferp = buffer;
        while (isspace(*bufferp)) bufferp++;

        // Skip blank lines and comments
        if (*bufferp == '#') continue;
        if (*bufferp == '\0') continue;

        // Check section
        if (nverts == 0)
        {
            // Read header
            if (!strstr(bufferp, "OFF"))
            {
                // Read mesh counts
                if ((sscanf(bufferp, "%d%d%d", &nverts, &nfaces, &nedges) != 3) || (nverts == 0))
                {
                    fprintf(stderr, "Syntax error reading header on line %d in file %s\n", line_count, filename);
                    fclose(fp);
                    return NULL;
                }

                // Allocate memory for mesh
                mesh->verts = new Vertex [nverts];
                assert(mesh->verts);
                mesh->faces = new Face [nfaces];
                assert(mesh->faces);
            }
        }
        else if (mesh->nverts < nverts)
        {
            // Read vertex coordinates
            Vertex& vert = mesh->verts[mesh->nverts++];
            if (sscanf(bufferp, "%f%f%f", &(vert.x), &(vert.y), &(vert.z)) != 3)
            {
                fprintf(stderr, "Syntax error with vertex coordinates on line %d in file %s\n", line_count, filename);
                fclose(fp);
                return NULL;
            }
        }
        else if (mesh->nfaces < nfaces)
        {
            // Get next face
            Face& face = mesh->faces[mesh->nfaces++];

            // Read number of vertices in face
            bufferp = strtok(bufferp, " \t");
            if (bufferp) face.nverts = atoi(bufferp);
            else
            {
                fprintf(stderr, "Syntax error with face on line %d in file %s\n", line_count, filename);
                fclose(fp);
                return NULL;
            }

            // Allocate memory for face vertices
            face.verts = new Vertex *[face.nverts];
            assert(face.verts);

            // Read vertex indices for face
            for (i = 0; i < face.nverts; i++)
            {
                bufferp = strtok(NULL, " \t");
                if (bufferp) face.verts[i] = &(mesh->verts[atoi(bufferp)]);
                else
                {
                    fprintf(stderr, "Syntax error with face on line %d in file %s\n", line_count, filename);
                    fclose(fp);
                    return NULL;
                }
            }

            // Compute normal for face
            face.normal[0] = face.normal[1] = face.normal[2] = 0;
            Vertex *v1 = face.verts[face.nverts-1];
            for (i = 0; i < face.nverts; i++)
            {
                Vertex *v2 = face.verts[i];
                face.normal[0] += (v1->y - v2->y) * (v1->z + v2->z);
                face.normal[1] += (v1->z - v2->z) * (v1->x + v2->x);
                face.normal[2] += (v1->x - v2->x) * (v1->y + v2->y);
                v1 = v2;
            }

            // Normalize normal for face
            float squared_normal_length = 0.0;
            squared_normal_length += face.normal[0]*face.normal[0];
            squared_normal_length += face.normal[1]*face.normal[1];
            squared_normal_length += face.normal[2]*face.normal[2];
            float normal_length = sqrt(squared_normal_length);
            if (normal_length > 1.0E-6)
            {
                face.normal[0] /= normal_length;
                face.normal[1] /= normal_length;
                face.normal[2] /= normal_length;
            }
        }
        else
        {
            // Should never get here
            fprintf(stderr, "Found extra text starting at line %d in file %s\n", line_count, filename);
            break;
        }
    }

    // Check whether read all faces
    if (nfaces != mesh->nfaces)
    {
        fprintf(stderr, "Expected %d faces, but read only %d faces in file %s\n", nfaces, mesh->nfaces, filename);
    }

    // Close file
    fclose(fp);

    // Return mesh
    return mesh;
}



////////////////////////////////////////////////////////////
// GLUT USER INTERFACE CODE
////////////////////////////////////////////////////////////

void RenderObject(void)
{
    // Setup viewing transformation
    glLoadIdentity();
    glScalef(scale, scale, scale);
    glTranslatef(translation[0], translation[1], 0.0);

    // Set projection transformation
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ugluPerspectivef(45.0, (GLfloat) 640 /(GLfloat) 480, 0.1, 100.0);

    // Set camera transformation
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(translation[0], translation[1], translation[2]);
    glScalef(scale, scale, scale);
    glRotatef(rotation[0], 1.0, 0.0, 0.0);
    glRotatef(rotation[1], 0.0, 1.0, 0.0);
    glRotatef(rotation[2], 0.0, 0.0, 1.0);
    glTranslatef(-center[0], -center[1], -center[2]);

    // Set lights
    static GLfloat light0_position[] = { 0.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    // Set material
    static GLfloat material[] = { 1.0, 0.5, 0.5, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material);

    // Clear window
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat vertex[9];
    GLfloat normal[9];

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    // Draw faces
    for (int i = 0; i < mesh->nfaces; i++)
    {
        Face& face = mesh->faces[i];

        normal[0] = face.normal[0];
        normal[1] = face.normal[1];
        normal[2] = face.normal[2];
        normal[3] = face.normal[0];
        normal[4] = face.normal[1];
        normal[5] = face.normal[2];
        normal[6] = face.normal[0];
        normal[7] = face.normal[1];
        normal[8] = face.normal[2];

        glNormalPointer(GL_FLOAT, 0, normal);
        for (int j = 0; j < face.nverts; j++)
        {
            Vertex *vert = face.verts[j];
            //printf("%d, %f %f %f\n", i,vert->x, vert->y, vert->z);
            vertex[j * 3 + 0] = vert->x;
            vertex[j * 3 + 1] = vert->y;
            vertex[j * 3 + 2] = vert->z;
        }
        glVertexPointer(3, GL_FLOAT, 0, vertex);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

}

void GLinit()
{
    // Initialize lights
    static GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    static GLfloat light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glEnable(GL_LIGHT0);

    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
}

void MainLoop(void)
{
    // Compute bounding box
    float bbox[2][3] = { { 1.0E30F, 1.0E30F, 1.0E30F }, { -1.0E30F, -1.0E30F, -1.0E30F } };
    for (int i = 0; i < mesh->nverts; i++)
    {
        Vertex& vert = mesh->verts[i];
        if (vert.x < bbox[0][0]) bbox[0][0] = vert.x;
        else if (vert.x > bbox[1][0]) bbox[1][0] = vert.x;
        if (vert.y < bbox[0][1]) bbox[0][1] = vert.y;
        else if (vert.y > bbox[1][1]) bbox[1][1] = vert.y;
        if (vert.z < bbox[0][2]) bbox[0][2] = vert.z;
        else if (vert.z > bbox[1][2]) bbox[1][2] = vert.z;
    }

//    // Setup initial viewing scale
    float dx = bbox[1][0] - bbox[0][0];
    float dy = bbox[1][1] - bbox[0][1];
    float dz = bbox[1][2] - bbox[0][2];
    scale = 2.0 / sqrt(dx*dx + dy*dy + dz*dz);

    // Setup initial viewing center
    center[0] = 0.5 * (bbox[1][0] + bbox[0][0]);
    center[1] = 0.5 * (bbox[1][1] + bbox[0][1]);
    center[2] = 0.5 * (bbox[1][2] + bbox[0][2]);

    fprintf(stderr, "BBox : xmin=%f , ymin=%f, zmin=%f\n", bbox[0][0], bbox[0][1], bbox[0][2]);
    fprintf(stderr, "BBox : xmax=%f , ymax=%f, zmax=%f\n", bbox[1][0], bbox[1][1], bbox[1][2]);

    fprintf(stderr, "Init View Scale : %f\n", scale);

    fprintf(stderr, "Init View Center in x axis : %f\n", center[0]);
    fprintf(stderr, "Init View Center in y axis : %f\n", center[1]);
    fprintf(stderr, "Init View Center in z axis : %f\n", center[2]);

    // Run main loop -- never returns
    RenderObject();

}

////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////

void DrawOFF(const char * filename)
{
    GLinit();

    // Read file
    mesh = ReadOffFile(filename);
    if (!mesh)
    {
        printf("mesh = NULL\n");
        exit(1);
    }

    MainLoop();
}
