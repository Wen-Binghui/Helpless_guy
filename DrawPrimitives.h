#include <GLFW/glfw3.h>


#include <math.h>


/* PI */
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif


void drawSphere(double r, int lats, int longs) {
	int i, j;
	for(i = 0; i <= lats; i++) {
		double lat0 = M_PI * (-0.5 + (double) (i - 1) / lats);
		double z0  = r * sin(lat0);
		double zr0 = r *  cos(lat0);

		double lat1 = M_PI * (-0.5 + (double) i / lats);
		double z1  = r * sin(lat1);
		double zr1 = r * cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for(j = 0; j <= longs; j++) {
			double lng = 2 * M_PI * (double) (j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(x * zr0, y * zr0, z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(x * zr1, y * zr1, z1);
		}
		glEnd();
	}
}


void drawCone(GLdouble base, GLdouble height, GLint slices, GLint stacks)
{

	// draw the upper part of the cone
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, height);
	for (int angle = 0; angle < 360; angle++) {
		glVertex3f(sin((double)angle) * base, cos((double)angle) * base, 0.f);
	}
	glEnd();

	// draw the base of the cone
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);
	for (int angle = 0; angle < 360; angle++) {
		// normal is just pointing down
		glNormal3f(0, -1, 0);
		glVertex3f(sin((double)angle) * base, cos((double)angle) * base, 0.f);
	}
	glEnd();
}

void drawPOLYGO(void)
{
	glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
	float a = 0.3f;
	float b = 0.0f;
	float down_y = 0.05;
	float down_z = 0.2;
	glColor3f(0.0f, 1.0f, 0.0f);     // Green
	glVertex3f(1.0f * a, 1.0f * a, -1.0f * a * down_z);
	glVertex3f(-1.0f * a, 1.0f * a, -1.0f * a * down_z);
	glVertex3f(-1.0f * a, 1.0f * a, 1.0f * a);
	glVertex3f(1.0f * a, 1.0f * a, 1.0f * a);

	// Bottom face (y = -1.0f)
	glColor3f(1.0f, 0.5f, 0.0f);     // Orange
	glVertex3f(1.0f * a, -1.0f * a * down_y, 1.0f * a);
	glVertex3f(-1.0f * a, -1.0f * a * down_y, 1.0f * a);
	glVertex3f(-1.0f * a, -1.0f * a * down_y, -1.0f * a * down_z);
	glVertex3f(1.0f * a, -1.0f * a * down_y, -1.0f * a);

	// Front face  (z = 1.0f)
	glColor3f(1.0f, 0.0f, 0.0f);     // Red
	glVertex3f(1.0f * a, 1.0f * a, 1.0f * a);
	glVertex3f(-1.0f * a, 1.0f * a, 1.0f * a);
	glVertex3f(-1.0f * a, -1.0f * a * down_y, 1.0f * a);
	glVertex3f(1.0f * a, -1.0f * a * down_y, 1.0f * a);

	// Back face (z = -1.0f)
	glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
	glVertex3f(1.0f * a, -1.0f * a * down_y, -1.0f * a);
	glVertex3f(-1.0f * a, -1.0f * a * down_y, -1.0f * a * down_z);
	glVertex3f(-1.0f * a, 1.0f * a, -1.0f * a * down_z);
	glVertex3f(1.0f * a, 1.0f * a, -1.0f * a * down_z);

	// Left face (x = -1.0f)
	glColor3f(0.0f, 0.0f, 1.0f);     // Blue
	glVertex3f(-1.0f * a, 1.0f * a + b, 1.0f * a);
	glVertex3f(-1.0f * a, 1.0f * a, -1.0f * a * down_z);
	glVertex3f(-1.0f * a, -1.0f * a * down_y, -1.0f * a * down_z);
	glVertex3f(-1.0f * a, -1.0f * a * down_y, 1.0f * a);

	// Right face (x = 1.0f)
	glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
	glVertex3f(1.0f * a, 1.0f * a, -1.0f * a * down_z);
	glVertex3f(1.0f * a, 1.0f * a, 1.0f * a);
	glVertex3f(1.0f * a, -1.0f * a * down_y, 1.0f * a);
	glVertex3f(1.0f * a, -1.0f * a * down_y, -1.0f * a);
	glEnd();  // End of drawing color-cube


}
