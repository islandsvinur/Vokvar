/*
 * Drag with the mouse to add smoke to the fluid. This will also move a "rotor" 
 * that disturbs the velocity field to the mouse location.
 * 'w' toggles wireframe rendering. 'v' toggles rendering of velocity vectors. 
 * 'T' and 't' increases and decreases the simulation time step, respectively.
 */

#include <math.h>
#include <rfftw.h>
#include <glut.h>
#include <stdio.h>


const int DIM = 50;				//size of simulation grid
int    winWidth, winHeight;
double dt = 0.4;				//simulation time step
int    color_dir = 0;				//use direction color-coding or not
float  vec_scale = 1000;			//scaling of hedgehogs
float  visc = 0.001;				//fluid viscosity
int    draw_smoke = 0;				//draw the smoke drift
int    draw_vecs = 1;				//draw the vector field
int    scalar_col = 0;				//method for scalar coloring
const int NUM_SCALAR_COL_METHODS = 3;
int    frozen = 0;				//toggles on/off the animation


fftw_real *u, *v;				//(u,v) = velocity field
fftw_real *u0, *v0;
fftw_real *u_u0, *u_v0;				// User-induced forces
fftw_real *rho, *rho0;				// Smoke density 


static rfftwnd_plan plan_rc, plan_cr;

float max(float x, float y)
{
  if (x >= y)
  {
    return x;
  }
  return y;
}


void cpalet(float value,float* R,float* G,float* B)
{
   const float dx=0.8; 

   if (value<0) value=0; if (value>1) value=1;
   value = (6-2*dx)*value+dx;
   *R = max(0.0,(3-fabs(value-4)-fabs(value-5))/2);
   *G = max(0.0,(4-fabs(value-2)-fabs(value-4))/2);
   *B = max(0.0,(3-fabs(value-1)-fabs(value-2))/2);
}


void set_cpalet(float v)
{
   float R,G,B; int NLEVELS = 7;
   switch(scalar_col)
   {
   default:	
   case 0:  R = G = B = v; break;
   case 1:  cpalet(v,&R,&G,&B); break;
   case 2:  v *= NLEVELS; v = (int)(v); v/= NLEVELS; 
	    cpalet(v,&R,&G,&B); break;
   }		
   glColor3f(R,G,B);
}

void init_FFT(int n)				//Initialize data structures dependent on grid size
{
	int i; size_t dim; 
	
	dim = n * 2*(n/2+1)*sizeof(fftw_real);
	u  = (fftw_real*) malloc(dim); v  = (fftw_real*) malloc(dim);
	u0 = (fftw_real*) malloc(dim); v0 = (fftw_real*) malloc(dim);
	dim = n * n * sizeof(fftw_real);
	u_u0 = (fftw_real*) malloc(dim); u_v0 = (fftw_real*) malloc(dim);
	rho = (fftw_real*) malloc(dim); rho0 = (fftw_real*) malloc(dim);

	plan_rc = rfftw2d_create_plan(n, n, FFTW_REAL_TO_COMPLEX, FFTW_IN_PLACE);
	plan_cr = rfftw2d_create_plan(n, n, FFTW_COMPLEX_TO_REAL, FFTW_IN_PLACE);
	
	for (i = 0; i < n * n; i++) 
	{ u[i] = v[i] = u0[i] = v0[i] = u_u0[i] = u_v0[i] = rho[i] = rho0[i] = 0.0f; }
}

#define FFT(s,u)\
	if(s==1) rfftwnd_one_real_to_complex(plan_rc,(fftw_real *)u,(fftw_complex*)u);\
	else rfftwnd_one_complex_to_real(plan_cr,(fftw_complex *)u,(fftw_real *)u)

#define floor(x) ((x)>=0.0?((int)(x)):(-((int)(1-(x)))))

void stable_solve ( int n, fftw_real * u, fftw_real * v, fftw_real * u0, fftw_real * v0, fftw_real visc, fftw_real dt ) 
{
	fftw_real x, y, x0, y0, f, r, U[2], V[2], s, t;
	int i, j, i0, j0, i1, j1;

	for (i=0;i<n*n;i++) 
	{ u[i] += dt*u0[i]; u0[i] = u[i]; v[i] += dt*v0[i]; v0[i] = v[i]; }    

	for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n ) 
	   for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n ) 
	   {
	      x0 = n*(x-dt*u0[i+n*j])-0.5f; 
	      y0 = n*(y-dt*v0[i+n*j])-0.5f;
	      i0 = floor(x0); s = x0-i0;
	      i0 = (n+(i0%n))%n;
	      i1 = (i0+1)%n;
	      j0 = floor(y0); t = y0-j0;
	      j0 = (n+(j0%n))%n;
	      j1 = (j0+1)%n;
	      u[i+n*j] = (1-s)*((1-t)*u0[i0+n*j0]+t*u0[i0+n*j1])+                        
			  s *((1-t)*u0[i1+n*j0]+t*u0[i1+n*j1]);
	      v[i+n*j] = (1-s)*((1-t)*v0[i0+n*j0]+t*v0[i0+n*j1])+
			  s *((1-t)*v0[i1+n*j0]+t*v0[i1+n*j1]);
	   }     
	
	for(i=0; i<n; i++)
	  for(j=0; j<n; j++) 
	  {  u0[i+(n+2)*j] = u[i+n*j]; v0[i+(n+2)*j] = v[i+n*j]; }

	FFT(1,u0);
	FFT(1,v0);

	for (i=0;i<=n;i+=2) 
	{
	   x = 0.5f*i;
	   for (j=0;j<n;j++) 
	   {
	      y = j<=n/2 ? (fftw_real)j : (fftw_real)j-n;
	      r = x*x+y*y;
	      if ( r==0.0f ) continue;
	      f = (fftw_real)exp(-r*dt*visc);
	      U[0] = u0[i  +(n+2)*j]; V[0] = v0[i  +(n+2)*j];
	      U[1] = u0[i+1+(n+2)*j]; V[1] = v0[i+1+(n+2)*j];

	      u0[i  +(n+2)*j] = f*( (1-x*x/r)*U[0]     -x*y/r *V[0] );
	      u0[i+1+(n+2)*j] = f*( (1-x*x/r)*U[1]     -x*y/r *V[1] );
	      v0[i+  (n+2)*j] = f*(   -y*x/r *U[0] + (1-y*y/r)*V[0] );
	      v0[i+1+(n+2)*j] = f*(   -y*x/r *U[1] + (1-y*y/r)*V[1] );
	   }    
	}

	FFT(-1,u0); 
	FFT(-1,v0);

	f = 1.0/(n*n);
 	for (i=0;i<n;i++)
	   for (j=0;j<n;j++) 
	   { u[i+n*j] = f*u0[i+(n+2)*j]; v[i+n*j] = f*v0[i+(n+2)*j]; }
} 


/*
 * This function diffuses matter that has been placed
 * in the velocity field. It's almost identical to the
 * velocity diffusion step in the function above. The
 * input matter densities are in rho0 and the result
 * is written into rho.
 *
 */
void diffuse_matter(int n, fftw_real *u, fftw_real *v, fftw_real *rho, fftw_real *rho0, fftw_real dt) {
	fftw_real x, y, x0, y0, s, t;
	int i, j, i0, j0, i1, j1;

	for ( x=0.5f/n,i=0 ; i<n ; i++,x+=1.0f/n ) {
		for ( y=0.5f/n,j=0 ; j<n ; j++,y+=1.0f/n ) {
			x0 = n*(x-dt*u[i+n*j])-0.5f; 
			y0 = n*(y-dt*v[i+n*j])-0.5f;
			i0 = floor(x0);
			s = x0-i0;
			i0 = (n+(i0%n))%n;
			i1 = (i0+1)%n;
			j0 = floor(y0);
			t = y0-j0;
			j0 = (n+(j0%n))%n;
			j1 = (j0+1)%n;
			rho[i+n*j] = (1-s)*((1-t)*rho0[i0+n*j0]+t*rho0[i0+n*j1])+                        
				s *((1-t)*rho0[i1+n*j0]+t*rho0[i1+n*j1]);
		}    
	} 
}



void set_color(float x, float y, int col)
{
	float r,g,b,f;
	if (col)
	{
	  f = atan2(y,x) / 3.1415927 + 1;
	  r = f;
	  if(r > 1) r = 2 - r;
	  g = f + .66667;
          if(g > 2) g -= 2;
	  if(g > 1) g = 2 - g;
	  b = f + 2 * .66667;
	  if(b > 2) b -= 2;
	  if(b > 1) b = 2 - b;
	} 
	else
	{ r = g = b = 1; }
	glColor3f(r,g,b);
}

void drawField(void)					//Draw the fluid 
{
	int        i, j, idx; double px,py; 
	fftw_real  wn = (fftw_real)winWidth / (fftw_real)(DIM + 1);   /* Grid element width */
	fftw_real  hn = (fftw_real)winHeight / (fftw_real)(DIM + 1);  /* Grid element height */
	
	if (draw_smoke)
	{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for (j = 0; j < DIM - 1; j++)			//draw smoke
	{
		glBegin(GL_TRIANGLE_STRIP);

		i = 0;
		px = wn + (fftw_real)i * wn;
		py = hn + (fftw_real)j * hn;
		idx = (j * DIM) + i;
		glColor3f(rho[idx], rho[idx], rho[idx]);
		glVertex2f(px, py);
						
		for (i = 0; i < DIM - 1; i++) 
		{
			px = wn + (fftw_real)i * wn;
			py = hn + (fftw_real)(j + 1) * hn;
			idx = ((j + 1) * DIM) + i;
			set_cpalet(rho[idx]);
			glVertex2f(px, py);
			px = wn + (fftw_real)(i + 1) * wn;
			py = hn + (fftw_real)j * hn;
			idx = (j * DIM) + (i + 1);
			set_cpalet(rho[idx]);
			glVertex2f(px, py);
		}

		px = wn + (fftw_real)(DIM - 1) * wn;
		py = hn + (fftw_real)(j + 1) * hn;
		idx = ((j + 1) * DIM) + (DIM - 1);
		set_cpalet(rho[idx]);
		glVertex2f(px, py);
		glEnd();
	}
	}

	if (draw_vecs)
	{
	glBegin(GL_LINES);				//draw velocities
	for (i = 0; i < DIM; i++) 
	  for (j = 0; j < DIM; j++) 
	  {
		idx = (j * DIM) + i;
		set_color(u[idx], v[idx],color_dir);
		glVertex2f(wn + (fftw_real)i * wn, hn + (fftw_real)j * hn);
		glVertex2f((wn + (fftw_real)i * wn) + vec_scale * u[idx], (hn + (fftw_real)j * hn) + vec_scale * v[idx]);
	  }
	glEnd();
	}
}

void display(void) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	drawField(); glFlush(); glutSwapBuffers();
}

void reshape(int w, int h) 
{
 	glViewport(0.0f, 0.0f, (GLfloat)w, (GLfloat)h);
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
	winWidth = w; winHeight = h;
}

void keyboard(unsigned char key, int x, int y) 
{
	switch (key) 
	{
	  case 't': dt -= 0.001; break;
	  case 'T': dt += 0.001; break;
	  case 'c': color_dir = 1 - color_dir; break;
	  case 'S': vec_scale *= 1.2; break;
	  case 's': vec_scale *= 0.8; break;
	  case 'V': visc *= 5; break;
	  case 'v': visc *= 0.2; break;
	  case 'x': draw_smoke = 1 - draw_smoke; 
		    if (draw_smoke==0) draw_vecs = 1; break;
	  case 'y': draw_vecs = 1 - draw_vecs; 
		    if (draw_vecs==0) draw_smoke = 1; break;
	  case 'm': scalar_col++; if (scalar_col==NUM_SCALAR_COL_METHODS) scalar_col=0; break;
	  case 'a': frozen = 1-frozen; break;
	  case 'q': exit(0);
	}
}



//
// When the user drags with the mouse, add a force that corresponds to the direction of the mouse
// cursor movement. Also inject some new matter into the field at the mouse location.
void drag(int mx, int my) 
{
	int xi,yi,X,Y; double  dx, dy, len;
	static int lmx=0,lmy=0;				//remembers last mouse location

	// Compute the array index that corresponds to the cursor location 
	xi = (int)floor((double)(DIM + 1) * ((double)mx / (double)winWidth));
	yi = (int)floor((double)(DIM + 1) * ((double)(winHeight - my) / (double)winHeight));

	X = xi; Y = yi;

	if (X > (DIM - 1))  X = DIM - 1; if (Y > (DIM - 1))  Y = DIM - 1;
	if (X < 0) X = 0; if (Y < 0) Y = 0;

	// Add force at the cursor location 
	my = winHeight - my;
	dx = mx - lmx; dy = my - lmy;
	len = sqrt(dx * dx + dy * dy);
	if (len != 0.0) {  dx *= 0.1 / len; dy *= 0.1 / len; }
	u_u0[Y * DIM + X] += dx; u_v0[Y * DIM + X] += dy;
	rho[Y * DIM + X] = 10.0f;
	lmx = mx; lmy = my;
}


//Copy user-induced forces to the force vectors that is sent to the solver. Also dampen forces and matter density. 
void setForces(void) 
{
	int i;
	for (i = 0; i < DIM * DIM; i++) 
	{
		rho0[i] = 0.995 * rho[i];
		u_u0[i] *= 0.85; u_v0[i] *= 0.85;
		u0[i] = u_u0[i]; v0[i] = u_v0[i];
	}
}


// Update the simulation when we're not drawing.
void idle(void) 
{
	if (!frozen)
	{
	  setForces();
	  stable_solve(DIM, u, v, u0, v0, visc, dt);
	  diffuse_matter(DIM, u, v, rho, rho0, dt);
	  glutPostRedisplay();
	}
	
}


int main(int argc, char **argv) 
{
	printf("Fluid Flow Simulation and Visualization\n");
	printf("=======================================\n");
	printf("Click and drag the mouse to steer the flow!\n");
	printf("T/t:   increase/decrease simulation timestep\n");
	printf("S/s:   increase/decrease hedgehog scaling\n");
	printf("c:     toggle direction coloring on/off\n");
	printf("V/v:   increase decrease fluid viscosity\n");
	printf("x:     toggle drawing matter on/off\n");
	printf("y:     toggle drawing hedgehogs on/off\n");
	printf("m:     toggle thru scalar coloring\n");
	printf("a:     toggle the animation on/off\n");
	printf("q:     quit\n\n");


	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Real-time wispy smoke");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(drag);
	
	init_FFT(DIM);			//initialize FFT
	
	glutMainLoop();			//calls idle, keyboard, display, drag, reshape
	return 0;
}
