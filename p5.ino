#define PI 3.1415

float x[5] = {0,0,0,0,0};
float y[5] = {0,0,0,0,0};

float l0 = 72.2;
float l1 = 125;
float l2 = 125;
float l3 = 60.7;

float a;
float b;
float c;

float C = PI/2;

void risolvi2(float pX, float pY);
void daAngoli();

void setup() {
  // put your setup code here, to run once:
  x[0] = 200;
  y[0] = 400;

  x[1] = x[0];
  y[1] = y[0]-l0;
  Serial.begin(9600);
  
}

float pX;
float pY;

void loop() {
  // put your main code here, to run repeatedly:
  pX = 250;
  pY = 200;
  risolvi2(pX,pY);
  Serial.print("a: ");
  Serial.print(180*a/PI);
  Serial.print(" b: ");
  Serial.print(180*b/PI);
  Serial.print(" c: ");
  Serial.println(180*c/PI);
  delay(100);
  
}

void risolvi2(float pX, float pY) {

  float x3 = pX - l3*cos(-C);
  float y3 = pY + l3*sin(-C);
  
  float Cx;
  if (y3 < y[1]) {
    Cx = acos((x3-x[1])/dist(x[1],y[1],x3,y3));
  }
  else {
    Cx = -acos((x3-x[1])/dist(x[1],y[1],x3,y3));
  }
  
  float lim = l1 + l2 + l3*cos(Cx+C);
  
  float A;
  if (pY < y[1]) {
    A = PI/2-acos((pX-x[1])/dist(x[1],y[1],pX,pY));
  }
  else {
    A = PI/2+acos((pX-x[1])/dist(x[1],y[1],pX,pY));
  }
  float d41 = dist(pX,pY,x[1],y[1]);
  
  if (d41 > l1 + l2 + l3) {
    
    a = A;
    b = 0;
    c = 0;
    daAngoli();
    
  }
  else {
    if (d41 > lim) {
      b = 0;

      float aa = acos(((l1+l2)*(l1+l2)+d41*d41-l3*l3)/(2*(l1+l2)*d41));
      float cc = acos(((l1+l2)*(l1+l2)+l3*l3-d41*d41)/(2*(l1+l2)*l3));

      a = A - aa;
      c = PI - cc;

      daAngoli();
    }
    else {
      float xm = 0;
      float ym = 0;

      if (x3 > x[1]) {
        xm = (x3 - x[1])/2 + x[1];
      }
      else {
        xm = (x[1] - x3)/2 + x3;
      }
      if (y3 > y[1]) {
        ym = (y3 - y[1])/2 + y[1];
      }
      else {
        ym = (y[1] - y3)/2 + y3;
      }

      //circle(xm,ym,5);
      float d13 = dist(x3,y3,x[1],y[1]);

      float h = sqrt(l1*l1 - (d13/2)*(d13/2));

      float x2 = xm + h*(y3-y[1])/d13;
      float y2 = ym - h*(x3-x[1])/d13;

      float d12 = dist(x[1],y[1],x2,y2);
      float d23 = dist(x2,y2,x3,y3);

      float aa = acos((d12*d12+d13*d13-d23*d23)/(2*d12*d13));
      float bb = acos((d12*d12+d23*d23-d13*d13)/(2*d12*d23));

      float cc = acos((d13*d13+d41*d41-l3*l3)/(2*d13*d41));

      if ((A > C-PI/2) && (A < C+PI/2)) {
        a = A-aa-cc;
      }
      else {
        a = A-aa+cc;
      }

      b = PI - bb;
      c = C+PI/2 - b - a;

      daAngoli();
    }
  }

}

void daAngoli() {
  if (a > radians(75)) {
    a = radians(75);
  }
  else if (a < radians(-70)) {
    a = radians(-70);
  }

  if (b > radians(220)) {
    b = radians(220);
  }
  else if (b < radians(-220)) {
    b = radians(-220);
  }

  if (c > radians(140)) {
    c = radians(140);
  }
  else if (c < radians(-140)) {
    c = radians(-140);
  }

  x[2] = x[1] + l1*cos(PI/2-a);
  y[2] = y[1] - l1*sin(PI/2-a);

  x[3] = x[2] + l2*cos(PI/2-a-b);
  y[3] = y[2] - l2*sin(PI/2-a-b);

  x[4] = x[3] + l3*cos(PI/2-a-b-c);
  y[4] = y[3] - l3*sin(PI/2-a-b-c);
  
}

float dist(float xa, float ya, float xb, float yb) {
  return sqrt((xa-xb)*(xa-xb)+(ya-yb)*(ya-yb));
}
